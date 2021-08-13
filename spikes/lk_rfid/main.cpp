// // Leka - LekaOS
// // Copyright 2020 APF France handicap
// // SPDX-License-Identifier: Apache-2.0

// #include "mbed.h"
// #include <cstddef>
// #include <cstdint>

// #include "drivers/BufferedSerial.h"
// #include "rtos/ThisThread.h"
// #include "rtos/Thread.h"

// #include "CoreBufferedSerial.h"
// #include "CoreCR95HF.h"
// #include "HelloWorld.h"
// #include "LogKit.h"
// #include "RFIDKit.h"

// using namespace leka;
// using namespace std::chrono;

// static auto mbed_serial = mbed::BufferedSerial(RFID_UART_TX, RFID_UART_RX, 57600);
// static auto rfid_serial = CoreBufferedSerial(mbed_serial);
// static auto rfid_reader = CoreCR95HF(rfid_serial);
// static auto core_rfid	= RFIDKit(rfid_reader);

// template <size_t size>
// void printarray(std::array<uint8_t, size> array)
// {
// 	printf("Data : ");
// 	for (int i = 0; i < size; ++i) {
// 		printf("%c, ", array.data()[i]);
// 	}
// 	printf("\n");
// }

// void test()
// {
// 	rfid_reader.onTagAvailable();
// }

// auto main() -> int
// {
// 	static auto serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
// 	leka::logger::set_print_function([](const char *str, size_t size) { serial.write(str, size); });

// 	log_info("Hello, World!\n\n");
// 	rtos::ThisThread::sleep_for(2s);

// 	HelloWorld hello;
// 	hello.start();

// 	printf("before Sigio\n");
// 	rfid_serial.sigio(test);
// 	printf("Sigio succeed\n");

// 	rfid_reader.setModeTagDetection();
// 	printf("SetModeDetection\n");

// 	while (true) {
// 		printf("While true\n");
// 		rtos::ThisThread::sleep_for(1s);
// 	}
// }

#include "mbed.h"
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

EventQueue eventQueue;
Thread thread;

template <size_t size>
void printArrayAsChar(std::array<uint8_t, size> array)
{
	printf("Data : ");
	for (int i = 0; i < size; ++i) {
		printf("%c, ", array.data()[i]);
	}
	printf("\n");
}

template <size_t size>
void printArrayAsUint(std::array<uint8_t, size> array)
{
	printf("Data : ");
	for (int i = 0; i < size; ++i) {
		printf("%i, ", array.data()[i]);
	}
	printf("\n");
}

// void getData(void)
// {
// 	rfid_reader.onDataAvailable();
// 	// printArrayAsUint(core_rfid.getTag().data);
// }

// void onCallback(void)
// {
// 	eventQueue.call(getData);
// }

auto main() -> int
{
	static auto log_serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	leka::logger::set_print_function([](const char *str, size_t size) { log_serial.write(str, size); });

	auto start = rtos::Kernel::Clock::now();

	log_info("Hello, World!\n\n");

	rtos::ThisThread::sleep_for(2s);

	auto mbed_serial = mbed::BufferedSerial(RFID_UART_TX, RFID_UART_RX, 57600);
	auto rfid_serial = CoreBufferedSerial(mbed_serial);
	auto rfid_reader = CoreCR95HF(rfid_serial, thread, eventQueue);
	auto core_rfid	 = RFIDKit(rfid_reader);

	core_rfid.init();

	HelloWorld hello;
	hello.start();

	while (true) {
		rtos::ThisThread::sleep_for(10ms);
	}
}
