// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include <queue>

#include "drivers/BufferedSerial.h"
#include "drivers/InterruptIn.h"
#include "drivers/LowPowerTimeout.h"
#include "events/EventQueue.h"
#include "rtos/Kernel.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "Controller.h"
#include "HelloWorld.h"
#include "LogKit.h"
#include "boost/sml.hpp"

using namespace leka;
using namespace std::chrono;

auto hello = HelloWorld {};

auto fsm  = boost::sml::sm<StateMachine> {};
auto ctrl = Controller {fsm};

auto main() -> int
{
	logger::init();

	log_debug("Hello, world");

	hello.start();

	ctrl.init();

	while (true) {
		log_debug("Running main thread");
		rtos::ThisThread::sleep_for(1s);
	}
}
