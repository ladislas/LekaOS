// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_SPIKE_QSPI_UTILS_H_
#define _LEKA_OS_SPIKE_QSPI_UTILS_H_

#include "drivers/QSPI.h"

namespace qspi_ns {

constexpr uint8_t CMD_READ	= 0x03;
constexpr uint8_t CMD_WRITE = 0x02;
constexpr uint8_t CMD_ERASE = 0x20;
constexpr uint8_t CMD_RDSR	= 0x5;
constexpr uint8_t CMD_WREN	= 0x6;
constexpr uint8_t CMD_RSTEN = 0x66;
constexpr uint8_t CMD_RST	= 0x99;

constexpr uint8_t STATUS_REG_SIZE = 2;
constexpr uint8_t BIT_WIP		  = 0x01;
constexpr uint8_t BIT_WEL		  = 0x02;

constexpr int ExtFlashSize = 0x00200000;

typedef enum ext_flash_status
{
	EXTERNAL_FLASH_STATUS_ERROR				= 1, /**< Generic error >*/
	EXTERNAL_FLASH_STATUS_INVALID_PARAMETER = 2, /**< The parameter is invalid >*/
	EXTERNAL_FLASH_STATUS_OK				= 0, /**< Function executed sucessfully  >*/
} ext_flash_status_t;

}	// namespace qspi_ns

bool memoryIsReady(mbed::QSPI *qspi)
{
	char status_value[qspi_ns::STATUS_REG_SIZE] = {0xFF};
	int retries									= 10000;
	bool mem_ready								= true;

	using namespace std::chrono;
	do {
		retries--;
		if (QSPI_STATUS_OK !=
			qspi->command_transfer(qspi_ns::CMD_RDSR, -1, NULL, 0, status_value, qspi_ns::STATUS_REG_SIZE)) {
			printf("Reading Status Register failed \n");
		}
		rtos::ThisThread::sleep_for(1ms);
	} while ((status_value[0] & qspi_ns::BIT_WIP) != 0 && retries);

	if ((status_value[0] & qspi_ns::BIT_WIP) != 0) {
		printf("mem_ready FALSE: status value = 0x%x\n", (int)status_value[0]);
		mem_ready = false;
	}
	return mem_ready;
}

void init(mbed::QSPI *qspi)
{
	qspi_status_t status						 = QSPI_STATUS_OK;
	qspi_ns::ext_flash_status_t ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_OK;
	char status_value[qspi_ns::STATUS_REG_SIZE]	 = {0};

	/* Set QSPI interface configuration */
	status = qspi->configure_format(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE, QSPI_CFG_ADDR_SIZE_24,
									QSPI_CFG_BUS_SINGLE, QSPI_CFG_ALT_SIZE_8, QSPI_CFG_BUS_SINGLE, 0);
	if (status != QSPI_STATUS_OK) {
		printf("Config format failed\n");
		return;
	}

	/* Read the Status Register from device */
	status = qspi->command_transfer(qspi_ns::CMD_RDSR, -1, NULL, 0, status_value, qspi_ns::STATUS_REG_SIZE);
	if (status != QSPI_STATUS_OK) {
		printf("Reading Status Register failed: value = 0x%x\n", (int)status_value[0]);
		ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_ERROR;
	}

	/* Send Reset Enable */
	status = qspi->command_transfer(qspi_ns::CMD_RSTEN, -1, NULL, 0, NULL, 0);
	if (status != QSPI_STATUS_OK) {
		printf("Sending RSTEN failed \n");
		ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (false == memoryIsReady(qspi)) {
		printf("Device not ready \n");
		ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_ERROR;
	}

	/* Send Reset */
	status = qspi->command_transfer(qspi_ns::CMD_RST, -1, NULL, 0, NULL, 0);
	if (status != QSPI_STATUS_OK) {
		printf("Sending RST failed \n");
		ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (false == memoryIsReady(qspi)) {
		printf("Device not ready \n");
		ext_flash_status = qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (ext_flash_status != qspi_ns::ext_flash_status::EXTERNAL_FLASH_STATUS_OK) {
		printf("Init failed\n");
	}

	qspi->set_frequency(133'000'000);
}

qspi_ns::ext_flash_status_t writeEnable(mbed::QSPI *qspi)
{
	char status_value[qspi_ns::STATUS_REG_SIZE] = {0};
	qspi_ns::ext_flash_status_t status			= qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;

	if (QSPI_STATUS_OK != qspi->command_transfer(qspi_ns::CMD_WREN, -1, NULL, 0, NULL, 0)) {
		printf("Sending WREN command FAILED \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (false == memoryIsReady(qspi)) {
		printf("Device not ready \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (QSPI_STATUS_OK !=
		qspi->command_transfer(qspi_ns::CMD_RDSR, -1, NULL, 0, status_value, qspi_ns::STATUS_REG_SIZE)) {
		printf("Reading Status Register failed \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if ((status_value[0] & qspi_ns::BIT_WEL)) {
		status = qspi_ns::EXTERNAL_FLASH_STATUS_OK;
	}
	return status;
}

qspi_ns::ext_flash_status_t eraseSector(mbed::QSPI *qspi, uint8_t sector_address)
{
	if (0 != writeEnable(qspi)) {
		printf("Write Enable failed \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (QSPI_STATUS_OK !=
		qspi->command_transfer(qspi_ns::CMD_ERASE, (((int)sector_address) & 0x00FFF000), NULL, 0, NULL, 0)) {
		printf("Erase failed\n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (false == memoryIsReady(qspi)) {
		printf("Device not ready \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	return qspi_ns::EXTERNAL_FLASH_STATUS_OK;
}

qspi_ns::ext_flash_status_t writeQSPI(mbed::QSPI *qspi, uint32_t address, const char *buffer, size_t buffer_length,
									  bool priorErase)
{
	if (priorErase && eraseSector(qspi, address) != qspi_ns::EXTERNAL_FLASH_STATUS_ERROR) {
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	if (0 != writeEnable(qspi)) {
		printf("Write Enable failed at %lx\n", address);
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	qspi_status_t result = qspi->write(qspi_ns::CMD_WRITE, -1, address, buffer, &buffer_length);
	if (result != QSPI_STATUS_OK) {
		printf("Write failed\n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	return qspi_ns::EXTERNAL_FLASH_STATUS_OK;
}

qspi_ns::ext_flash_status_t readQSPI(mbed::QSPI *qspi, uint8_t address, char *buffer, size_t buffer_length)
{
	if (memoryIsReady(qspi) == false) {
		printf("Device not ready \n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	qspi_status_t result = qspi->read(qspi_ns::CMD_READ, -1, address, buffer, &buffer_length);
	if (result != QSPI_STATUS_OK) {
		printf("Read failed\n");
		return qspi_ns::EXTERNAL_FLASH_STATUS_ERROR;
	}

	return qspi_ns::EXTERNAL_FLASH_STATUS_OK;
}

void runQSPI(mbed::QSPI *qspi)
{
	const uint16_t message_size			= 0x100;
	char message_to_write[message_size] = {0};
	char buffer[message_size]			= {0};

	for (uint16_t content = 0x00; content < message_size; content += 0x01) {
		message_to_write[content] = (uint8_t)content;
	}

	for (uint32_t address = 0x0; address < qspi_ns::ExtFlashSize; address += message_size) {
		/* Write on flash */
		writeQSPI(qspi, address, message_to_write, message_size,
				  false);	// Do not erase (0x1000) before write

		/* Read to check good writing */
		for (uint16_t i = 0; i < message_size; i++) {
			buffer[i] = 0x00;
		}

		readQSPI(qspi, address, buffer, message_size);
		if (memcmp(message_to_write, buffer, message_size) != 0) {
			printf("Error on external memory at address 0x%lX\n", address);
			return;
		}
		if (address != 0x0 && address % 0x10000 == 0x0000) {
			printf("Block(64kB) from 0x%lX to 0x%lX on external memory is fine\n", address - 0x10000, address);
		}
	}

	printf("External memory is OK!\n");
}

#endif	 // _LEKA_OS_SPIKE_QSPI_UTILS_H_
