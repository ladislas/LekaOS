// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "mbed.h"

#include "drivers/BufferedSerial.h"
#include "drivers/InterruptIn.h"
#include "rtos/Kernel.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "FATFileSystem.h"
#include "FlashIAP/FlashIAPBlockDevice.h"
#include "HelloWorld.h"
#include "QSPIFBlockDevice.h"
#include "QSPIUtils.h"
#include "SDBlockDevice.h"
#include "bootutil/bootutil.h"
// #include "bootutil/image.h"

#define TRACE_GROUP "main"
#include "mbed-trace/mbed_trace.h"

using namespace leka;
using namespace std::chrono;

mbed::BlockDevice *get_secondary_bd(void)
{
	static QSPIFBlockDevice _bd;

	static mbed::SlicingBlockDevice sliced_bd(&_bd, 0x0, MCUBOOT_SLOT_SIZE);
	return &sliced_bd;
}

HelloWorld hello;

// static BufferedSerial serial(USBTX, USBRX, 9600);

constexpr uint8_t buff_size = 128;
char buff[buff_size] {};

static mbed::BufferedSerial serial(USBTX, USBRX, 115200);
mbed::QSPI coreqspi(QSPI_FLASH_IO0, QSPI_FLASH_IO1, QSPI_FLASH_IO2, QSPI_FLASH_IO3, QSPI_FLASH_CLK, QSPI_FLASH_nCS);

SDBlockDevice sd_blockdevice(SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK);
FATFileSystem fatfs("fs");

void initializeSD()
{
	sd_blockdevice.init();
	sd_blockdevice.frequency(25'000'000);

	fatfs.mount(&sd_blockdevice);
}

void setup_application()
{
	initializeSD();

	int err = -1;

	// Open the numbers file
	printf("Opening \"/fs/dummy.bin\"... ");
	fflush(stdout);
	FILE *f = fopen("/fs/dummy.bin", "r+");
	printf("%s\n", (!f ? "Fail :(" : "OK"));
	if (!f) {
		// Create the numbers file if it doesn't exist
		printf("No file found, creating a new file... ");
		fflush(stdout);
		f = fopen("/fs/dummy.bin", "w+");
		printf("%s\n", (!f ? "Fail :(" : "OK"));
		if (!f) {
			error("error: %s (%d)\n", strerror(errno), -errno);
		}

		printf("\rWriting data... ");
		for (uint8_t i = 0x2A; i < 0x2E; i++) {
			fflush(stdout);
			err = fprintf(f, "%x", i);
			if (err < 0) {
				printf("Fail :(\n");
				error("error: %s (%d)\n", strerror(errno), -errno);
			}
		}

		printf("\rWriting data... OK\n");

		printf("Seeking file... ");
		fflush(stdout);
		err = fseek(f, 0, SEEK_SET);
		printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
		if (err < 0) {
			error("error: %s (%d)\n", strerror(errno), -errno);
		}
	}

	printf("Result!: ");
	while (!feof(f)) {
		int c = fgetc(f);
		printf("%x", c);
	}

	// Close the file which also flushes any cached writes
	printf("Closing \"/fs/dummy.bin\"... ");
	fflush(stdout);
	err = fclose(f);
	printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
	if (err < 0) {
		error("error: %s (%d)\n", strerror(errno), -errno);
	}
}

void current_application()
{
	serial.write("Hello, Application!\n", 20);

	initializeSD();
	init(&coreqspi);

	fflush(stdout);
	FILE *f = fopen("/fs/LekaOS_update-signed.bin", "r+");
	printf("%s\n", (!f ? "Fail :(" : "OK"));
	uint32_t address		   = 0x0;
	const uint16_t packet_size = 0x100;
	char buffer[packet_size];

	while (!feof(f)) {
		for (uint16_t i = 0; i < packet_size; i++) {
			buffer[i] = fgetc(f);
		}
		writeQSPI(&coreqspi, address, buffer, packet_size, false);
		address += packet_size;
		rtos::ThisThread::sleep_for(1ms);	// Delay to let time for QSPI to write
	}

	// Close the file which also flushes any cached writes
	printf("Closing \"/fs/LekaOS_update-signed.bin\"... ");
	fflush(stdout);
	int err = fclose(f);
	printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
	if (err < 0) {
		error("error: %s (%d)\n", strerror(errno), -errno);
	}

	int ret = boot_set_pending(0);
	if (ret == 0) {
		tr_info("> Secondary image pending, reboot to update");
	} else {
		tr_error("Failed to set secondary image pending: %d", ret);
	}
}

void updated_application()
{
	serial.write("Hello, Update!\n", 15);

	//    run_self_test();
	int ret = boot_set_confirmed();
	if (ret == 0) {
		tr_info("Boot confirmed");
	} else {
		tr_error("Failed to confirm boot: %d", ret);
	}

	sd_blockdevice.init();
	sd_blockdevice.frequency(25'000'000);

	fatfs.mount(&sd_blockdevice);

	FIL info_file;
	if (f_open(&info_file, "LK_FW.txt", FA_READ | FA_WRITE) != FR_OK) {
		serial.write("Fail to open file...\n", 21);
		ThisThread::sleep_for(10ms);
		return;	  // TODO: Handle this case
	}

	auto file_size = f_size(&info_file);

	char buffer[128];
	UINT number_of_bytes_read = 0;
	f_read(&info_file, buffer, file_size, &number_of_bytes_read);

	std::string content_file = std::string(buffer);

	auto pos = content_file.find('0');
	content_file.replace(pos, 1, "1");

	f_lseek(&info_file, 0);
	f_truncate(&info_file);

	UINT number_of_bytes_wrote = 0;
	f_write(&info_file, content_file.data(), file_size, &number_of_bytes_wrote);

	f_close(&info_file);

	serial.write("Data wrote, you can reboot\n", 27);
}

void factory_application()
{
	serial.write("Hello, Factory!\n", 15);
}

int main()
{
	// Enable traces from relevant trace groups
	mbed_trace_init();
	mbed_trace_include_filters_set("main,MCUb,BL");

	// setup_application();

	// factory_application();
	// current_application();
	// updated_application();
}
