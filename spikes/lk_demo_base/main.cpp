// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "drivers/BufferedSerial.h"
#include "events/EventQueue.h"
#include "rtos/EventFlags.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "BatteryUtils.h"
#include "Flags.h"
#include "HelloWorld.h"
#include "LogKit.h"
#include "WatchdogUtils.h"

using namespace leka;
using namespace std::chrono_literals;

auto thread_event_queue				  = rtos::Thread {};
auto event_queue					  = events::EventQueue {};
auto event_flags_external_interaction = rtos::EventFlags {};

auto hello = HelloWorld {};

auto battery_utils = BatteryUtils {};

auto main() -> int
{
	startWatchdog();

	static auto serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	leka::logger::set_print_function([](const char *str, size_t size) { serial.write(str, size); });

	auto start = rtos::Kernel::Clock::now();

	thread_event_queue.start({&event_queue, &events::EventQueue::dispatch_forever});

	log_info("Hello, World!\n\n");
	hello.start();

	battery_utils.registerEventQueue(event_queue);

	rtos::ThisThread::sleep_for(1s);

	while (true) {
		auto t = rtos::Kernel::Clock::now() - start;
		log_info("A message from your board %s --> \"%s\" at %i s\n", MBED_CONF_APP_TARGET_NAME, hello.world,
				 int(t.count() / 1000));

		rtos::ThisThread::sleep_for(1s);
	}
}
