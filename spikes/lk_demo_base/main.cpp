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

void demoThree()
{
	auto start = rtos::Kernel::Clock::now();

	leds_utils.setBrightness(0x40);
	auto expected_tag_emotion_child = Tag::emotion_happiness_child;
	auto expected_tag_emotion_leka	= Tag::emotion_happiness_leka;

	auto random_value {0};

	while (true) {
		random_value = int((rtos::Kernel::Clock::now() - start).count()) % (emotion_table.size() - 1);
		display_utils.displayImage(emotion_table.at(random_value));
		rtos::ThisThread::sleep_for(100ms);
		display_utils.setBrightness(1.F);

		switch (random_value) {
			case 0:
				expected_tag_emotion_child = Tag::emotion_happiness_child;
				expected_tag_emotion_leka  = Tag::emotion_happiness_leka;
				break;
			case 1:
				expected_tag_emotion_child = Tag::emotion_anger_child;
				expected_tag_emotion_leka  = Tag::emotion_anger_leka;
				break;
			case 2:
				expected_tag_emotion_child = Tag::emotion_fear_child;
				expected_tag_emotion_leka  = Tag::emotion_fear_leka;
				break;
			case 3:
				expected_tag_emotion_child = Tag::emotion_disgust_child;
				expected_tag_emotion_leka  = Tag::emotion_disgust_leka;
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				expected_tag_emotion_child = Tag::emotion_sadness_child;
				expected_tag_emotion_leka  = Tag::emotion_sadness_leka;
				break;
			default:
				break;
		}

		do {
			event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		} while ((rfid_utils.getTag() != expected_tag_emotion_child) &&
				 (rfid_utils.getTag() != expected_tag_emotion_leka) && (rfid_utils.getTag() != Tag::number_0_zero));

		if (rfid_utils.getTag() == Tag::number_0_zero) {
			return;
		}

		display_utils.displayVideo("animation-joy");
		leds_utils.runRainbowColor();
		rtos::ThisThread::sleep_for(2s);

		leds_utils.turnOffAll();
		display_utils.setBrightness(0.F);
		rtos::ThisThread::sleep_for(1s);
	}
}

void demoFour()
{
	display_utils.displayImage("emotion-happy");
	display_utils.setBrightness(1.F);

	leds_utils.setBrightness(0x80);
	auto new_color	   = CRGB {};
	uint8_t color_step = 0x08;

	while (true) {
		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);

		auto tag_value = rfid_utils.getTag();
		if (tag_value == Tag::number_0_zero) {
			return;
		}
		switch (tag_value) {
			case Tag::color_red:
				new_color = leds_utils.offsetColor(color_step, 0, 0);
				break;
			case Tag::color_green:
				new_color = leds_utils.offsetColor(0, color_step, 0);
				break;
			case Tag::color_blue:
				new_color = leds_utils.offsetColor(0, 0, color_step);
				break;
			case Tag::color_yellow:
				new_color = leds_utils.offsetColor(color_step, color_step, 0);
				break;
			case Tag::color_white:
				new_color = leds_utils.offsetColor(color_step, color_step, color_step);
				break;
			case Tag::color_black:
				new_color = leds_utils.offsetColor(-color_step, -color_step, -color_step);
				break;
			default:
				rtos::ThisThread::sleep_for(10ms);
		}
		leds_utils.turnOnAll(new_color);
		rtos::ThisThread::sleep_for(50ms);
	}
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
			case Tag::number_3_three:
				demoThree();
				break;
			case Tag::number_4_four:
				demoFour();
				break;
			default:
				break;
		}
	}
}
