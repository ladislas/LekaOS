// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "drivers/BufferedSerial.h"
#include "events/EventQueue.h"
#include "platform/Callback.h"
#include "rtos/EventFlags.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "BLEUtils.h"

#include "BatteryUtils.h"
#include "CoreSDRAM.hpp"
#include "DisplayUtils.h"
#include "Flags.h"
#include "HelloWorld.h"
#include "LedsUtils.h"
#include "LogKit.h"
#include "RFIDUtils.h"
#include "WatchdogUtils.h"

using namespace leka;
using namespace std::chrono_literals;

auto thread_event_queue				  = rtos::Thread {};
auto event_queue					  = events::EventQueue {};
auto event_flags_external_interaction = rtos::EventFlags {};

auto hello = HelloWorld {};

auto battery_utils = BatteryUtils {};

auto ble_utils	= BLEUtils {event_flags_external_interaction};
auto ble_thread = rtos::Thread {};

auto leds_utils = LedsUtils {};

SDBlockDevice sd_blockdevice(SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK);
FATFileSystem fatfs("fs");

LKCoreSTM32Hal hal;
CoreSDRAM coresdram(hal);
VideoKit display(hal);
VideoKit_DeclareIRQHandlers(display);
auto video_thread  = rtos::Thread {};
auto display_utils = DisplayUtils {video_thread, event_flags_external_interaction, hal, coresdram, display};

auto rfid_utils = RFIDUtils {event_flags_external_interaction};

void useLeds()
{
	leds_utils.setBrightness(0x08);

	// leds_utils.runMotivationFire();
	leds_utils.turnOnAll(0xFF0000);
	leds_utils.turnOffEars();
	leds_utils.runRainbowColor();
	leds_utils.turnOffAll();
}

void useDisplay()
{
	display_utils.setOn();

	display_utils.displayImage(0);
	rtos::ThisThread::sleep_for(5s);

	display_utils.displayVideo("animation-idle");
	rtos::ThisThread::sleep_for(10s);
}

void useRFID()
{
	event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
	log_info("Data: %x", rfid_utils.getTag());
	rtos::ThisThread::sleep_for(100ms);
}

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

	ble_utils.setDeviceName("Leka_DemoBase");
	ble_thread.start({&ble_utils, &BLEUtils::startAdvertising});

	leds_utils.initialize();
	rtos::ThisThread::sleep_for(100ms);
	leds_utils.initializationAnimation();

	display_utils.initializeSD();
	display_utils.initializeScreen();

	rfid_utils.initialize();
	rfid_utils.registerEventQueue(event_queue);

	rtos::ThisThread::sleep_for(1s);

	while (true) {
		auto t = rtos::Kernel::Clock::now() - start;
		log_info("A message from your board %s --> \"%s\" at %i s\n", MBED_CONF_APP_TARGET_NAME, hello.world,
				 int(t.count() / 1000));

		rtos::ThisThread::sleep_for(1s);

		// useLeds();
		// useDisplay();
		// useRFID();

		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		auto tag_value = rfid_utils.getTag();
		switch (tag_value) {
			default:
				break;
		}
	}
}
