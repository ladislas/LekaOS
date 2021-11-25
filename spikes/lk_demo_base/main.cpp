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

#include "Activities.h"
#include "BatteryUtils.h"
#include "CoreSDRAM.hpp"
#include "DisplayUtils.h"
#include "FATFileSystem.h"
#include "Flags.h"
#include "HelloWorld.h"
#include "LKCoreFatFs.h"
#include "LedsUtils.h"
#include "LogKit.h"
#include "RFIDUtils.h"
#include "SDBlockDevice.h"
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

void deepSleepClock()
{
	while (true) {
		if (event_flags_external_interaction.get() != KICK_SLEEP_FLAG) {
			leds_utils.turnOffAll();
			display_utils.setOff();
			event_flags_external_interaction.set(STOP_VIDEO_FLAG);

			event_flags_external_interaction.wait_any(KICK_SLEEP_FLAG);
		}
		event_flags_external_interaction.clear(KICK_SLEEP_FLAG);
		rtos::ThisThread::sleep_for(5min);
	}
}

void initializeSD()
{
	sd_blockdevice.init();
	sd_blockdevice.frequency(25'000'000);

	fatfs.mount(&sd_blockdevice);
}

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

	rtos::Thread thread_deep_sleep;
	thread_deep_sleep.start(deepSleepClock);

	battery_utils.registerEventQueue(event_queue);

	ble_utils.setDeviceName("Leka_DemoBase");
	ble_thread.start({&ble_utils, &BLEUtils::startAdvertising});

	leds_utils.initialize();
	rtos::ThisThread::sleep_for(100ms);
	leds_utils.initializationAnimation();

	initializeSD();
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

		if ((event_flags_external_interaction.get() & NEW_LCD_INTENSITY_FLAG) == NEW_LCD_INTENSITY_FLAG) {
			auto new_value = (float(ble_utils.getLCDIntensity()) / 255.F);
			display_utils.setBrightness(new_value);
			log_info("BRITGHTNESS SET %.2f", new_value);
		}
		if ((event_flags_external_interaction.get() & NEW_LEDS_INTENSITY_FLAG) == NEW_LEDS_INTENSITY_FLAG) {
			auto new_value = ble_utils.getMonitoringData();
			leds_utils.setBrightness(new_value);
			log_info("BRITGHTNESS SET %x", new_value);
		}

		display_utils.setOff();
		leds_utils.turnOffAll();
		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		auto tag_value = rfid_utils.getTag();
		switch (tag_value) {
			case Tag::number_1_one:
				displayTags(event_flags_external_interaction, display_utils, leds_utils, rfid_utils);
				break;
			case Tag::number_2_two:
				activityRecognitionEmotions(event_flags_external_interaction, display_utils, leds_utils, rfid_utils);
				break;
			case Tag::number_3_three:
				activityRecognitionColor(event_flags_external_interaction, display_utils, leds_utils, rfid_utils);
				break;
			case Tag::number_4_four:
				activityColorLeka2(event_flags_external_interaction, display_utils, leds_utils, rfid_utils);
				break;
			case Tag::number_5_five:
				activityColorLeka3(event_flags_external_interaction, display_utils, leds_utils, rfid_utils);
				break;
			default:
				break;
		}
	}
}
