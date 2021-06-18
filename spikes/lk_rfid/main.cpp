// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

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

static auto mbed_serial = mbed::BufferedSerial(RFID_UART_TX, RFID_UART_RX, 57600);
static auto rfid_serial = CoreBufferedSerial(mbed_serial);
static auto rfid_reader = CoreCR95HF(rfid_serial);
static auto core_rfid	= RFIDKit(rfid_reader);

template <size_t size>
void printarray(std::array<uint8_t, size> array)
{
	printf("Data : ");
	for (int i = 0; i < size; ++i) {
		printf("%i, ", array.data()[i]);
	}
	printf("\n");
}

void getData(void)
{
	std::array<uint8_t, 16> tag_data {};

	if (core_rfid.getTagData(tag_data)) {
		printarray(tag_data);
	}
	core_rfid.setReaderForTagDetection();
}

void onCallback(void)
{
	eventQueue.call(getData);
}

void test()
{
	static auto log_serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	leka::logger::set_print_function([](const char *str, size_t size) { log_serial.write(str, size); });

	auto start = rtos::Kernel::Clock::now();

	log_info("Hello, World!\n\n");

	rtos::ThisThread::sleep_for(2s);

	HelloWorld hello;
	hello.start();

	core_rfid.setReaderForTagDetection();	// not necessary

	while (true) {
		rtos::ThisThread::sleep_for(10ms);
	}
}

auto main() -> int
{
	rtos::Thread thread1;
	rtos::Thread thread2;

	thread1.start(test);
	thread2.start(mbed::callback(&eventQueue, &EventQueue::dispatch_forever));
	core_rfid.setInterrupt(onCallback);

	while (1) {
		rtos::ThisThread::sleep_for(10ms);
	}
}
