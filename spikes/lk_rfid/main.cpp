// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>

#include "drivers/BufferedSerial.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "CoreBufferedSerial.h"
#include "CoreCR95HF.h"
#include "HelloWorld.h"
#include "LogKit.h"
#include "RFIDKit.h"

using namespace leka;
using namespace std::chrono;

template <size_t size>
void printarray(std::array<uint8_t, size> array)
{
	printf("Data : ");
	for (int i = 0; i < size; ++i) {
		printf("%i, ", array.data()[i]);
	}
	printf("\n");
}

auto main() -> int
{
	static auto log_serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	leka::logger::set_print_function([](const char *str, size_t size) { log_serial.write(str, size); });

	static auto mbed_serial = mbed::BufferedSerial(RFID_UART_TX, RFID_UART_RX, 57600);
	static auto rfid_serial = CoreBufferedSerial(mbed_serial);
	static auto rfid_reader = CoreCR95HF(rfid_serial);
	static auto core_rfid	= RFIDKit(rfid_reader);

	std::array<uint8_t, 16> enable_wake_up_on_tag {0x07, 0x0E, 0x02, 0x21, 0x00, 0x79, 0x01, 0x18,
												   0x00, 0x20, 0x60, 0x60, 0x70, 0x80, 0x3F, 0x01};

	std::array<uint8_t, 3> set_up_answer {};

	std::array<uint8_t, 16> tag_data {};

	auto start = rtos::Kernel::Clock::now();

	log_info("Hello, World!\n\n");

	rtos::ThisThread::sleep_for(2s);

	HelloWorld hello;
	hello.start();

	int count		 = 0;
	bool status		 = false;
	uint8_t DacDataH = 0xFC;

	while (true) {
		auto t = rtos::Kernel::Clock::now() - start;

		mbed_serial.write(enable_wake_up_on_tag.data(), enable_wake_up_on_tag.size());
		rtos::ThisThread::sleep_for(10ms);

		while (!mbed_serial.readable()) {
			rtos::ThisThread::sleep_for(1ms);
		}
		mbed_serial.read(set_up_answer.data(), set_up_answer.size());

		if (set_up_answer[2] == 0x02) {
			core_rfid.init();
			core_rfid.getTagData(tag_data);
			printarray(tag_data);
		}
	}
}
