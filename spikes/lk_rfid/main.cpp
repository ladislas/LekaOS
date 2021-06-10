// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

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

auto main() -> int
{
	static auto log_serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	leka::logger::set_print_function([](const char *str, size_t size) { log_serial.write(str, size); });

	static auto mbed_serial = mbed::BufferedSerial(RFID_UART_TX, RFID_UART_RX, 57600);
	static auto rfid_serial = CoreBufferedSerial(mbed_serial);
	static auto rfid_reader = CoreCR95HF(rfid_serial);
	static auto core_rfid	= RFIDKit(rfid_reader);

	core_rfid.init();
	std::array<uint8_t, 16> tag_data {};

	auto start = rtos::Kernel::Clock::now();

	log_info("Hello, World!\n\n");

	rtos::ThisThread::sleep_for(2s);

	HelloWorld hello;
	hello.start();

	while (true) {
		auto t = rtos::Kernel::Clock::now() - start;
		core_rfid.getTagData(tag_data);

		printf("TagData : ");
		for (int i = 0; i < tag_data.size(); ++i) {
			printf("%i ,", tag_data.data()[i]);
		}
		printf("\n");

		rtos::ThisThread::sleep_for(1s);
	}
}
