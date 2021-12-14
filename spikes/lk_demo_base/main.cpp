// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "drivers/BufferedSerial.h"
#include "events/EventQueue.h"
#include "rtos/EventFlags.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "BLEUtils.h"

#include "BatteryUtils.h"
#include "DisplayUtils.h"
#include "Flags.h"
#include "HelloWorld.h"
#include "LogKit.h"
#include "RFIDUtils.h"
#include "WatchdogUtils.h"

using namespace leka;
using namespace std::chrono_literals;

auto thread_watchdog		  = rtos::Thread {osPriorityNormal};
auto thread_event_queue		  = rtos::Thread {osPriorityNormal};
auto thread_deep_sleep		  = rtos::Thread {osPriorityNormal};
auto thread_ble				  = rtos::Thread {osPriorityNormal};
auto thread_ble_notifications = rtos::Thread {osPriorityNormal};
auto thread_video			  = rtos::Thread {osPriorityNormal};

auto event_queue					  = events::EventQueue {};
auto event_flags_external_interaction = rtos::EventFlags {};

auto hello = HelloWorld {};

auto battery_utils = BatteryUtils {};

auto ble_utils = BLEUtils {event_flags_external_interaction};

auto hal	   = LKCoreSTM32Hal {};
auto coresdram = CoreSDRAM {hal};
auto display   = VideoKit {hal};
VideoKit_DeclareIRQHandlers(display);
auto display_utils = DisplayUtils {thread_video, event_flags_external_interaction, hal, coresdram, display};

auto rfid_utils = RFIDUtils {event_flags_external_interaction};

void deepSleepLoop()
{
	auto is_kick_turn_off_flag = []() {
		return (event_flags_external_interaction.get() & KICK_TURN_OFF_FLAG) == KICK_TURN_OFF_FLAG;
	};

	while (true) {
		if (!is_kick_turn_off_flag()) {
			event_flags_external_interaction.set(STOP_VIDEO_FLAG);
			rtos::ThisThread::sleep_for(20s);
			display_utils.setOff();

			event_flags_external_interaction.wait_any(KICK_TURN_OFF_FLAG);
		}
		event_flags_external_interaction.clear(KICK_TURN_OFF_FLAG);
		rtos::ThisThread::sleep_for(5min);
	}
}

void useRFID()
{
	event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
	log_info("Data: %x", rfid_utils.getTag());
}

void updateBLENotifications()
{
	auto reboot_instruction_flag_is_set = [&]() {
		return (event_flags_external_interaction.get() & BLE_REBOOT_INSTRUCTION_FLAG) == BLE_REBOOT_INSTRUCTION_FLAG;
	};
	auto lcd_intensity_flag_is_set = [&]() {
		return (event_flags_external_interaction.get() & BLE_LCD_INTENSITY_FLAG) == BLE_LCD_INTENSITY_FLAG;
	};

	auto update_battery_level = [&]() { ble_utils.setBatteryLevel(battery_utils.getBatteryLevel()); };
	event_queue.call_every(1s, update_battery_level);

	while (true) {
		event_flags_external_interaction.clear(BLE_REBOOT_INSTRUCTION_FLAG | BLE_LCD_INTENSITY_FLAG);
		event_flags_external_interaction.wait_any(NEW_BLE_MESSAGE_FLAG);

		if (reboot_instruction_flag_is_set()) {
			if (auto has_to_reboot = ble_utils.getRebootInstruction(); has_to_reboot) {
				event_flags_external_interaction.set(STOP_VIDEO_FLAG);
				display_utils.setOff();

				rtos::ThisThread::sleep_for(500ms);

				NVIC_SystemReset();
			}
		}
		if (lcd_intensity_flag_is_set()) {
			auto new_value = (float(ble_utils.getLCDIntensity()) / 255.F);
			display_utils.setBrightness(new_value);
		}
	}
}

auto main() -> int
{
	startWatchdog(thread_watchdog);

	static auto serial = mbed::BufferedSerial(USBTX, USBRX, 115200);
	logger::set_print_function([](const char *str, size_t size) { serial.write(str, size); });

	auto start = rtos::Kernel::Clock::now();

	thread_event_queue.start({&event_queue, &events::EventQueue::dispatch_forever});

	log_info("Hello, World!\n\n");
	hello.start();

	battery_utils.registerEventQueue(event_queue);

	event_flags_external_interaction.set(KICK_TURN_OFF_FLAG);
	thread_deep_sleep.start(deepSleepLoop);

	ble_utils.setDeviceName("LekaDemo");
	thread_ble.start({&ble_utils, &BLEUtils::startAdvertising});
	thread_ble_notifications.start(updateBLENotifications);

	display_utils.initializeSD();
	display_utils.initializeScreen();

	rfid_utils.initialize();
	rfid_utils.registerEventQueue(event_queue);

	event_flags_external_interaction.set(KICK_TURN_OFF_FLAG);

	while (true) {
		auto t = rtos::Kernel::Clock::now() - start;
		log_info("A message from your board %s --> \"%s\" at %i s\n", MBED_CONF_APP_TARGET_NAME, hello.world,
				 int(t.count() / 1000));

		rtos::ThisThread::sleep_for(1s);

		// useLeds();
		// useDisplay();
		// useRFID();

		display_utils.displayImage("demo-main-menu");

		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		event_flags_external_interaction.set(KICK_TURN_OFF_FLAG);
		auto tag_value = rfid_utils.getTag();
		switch (tag_value) {
			case Tag::number_1_one:
				// Video
				break;
			case Tag::number_2_two:
				// Video
				break;
			case Tag::number_3_three:
				// Video
				break;
			case Tag::number_4_four:
				// Video
				break;
			case Tag::number_5_five:
				// Video
				break;
			case Tag::number_6_six:
				// Video
				break;
			case Tag::number_7_seven:
				// Video
				break;
			case Tag::number_8_eight:
				// Video
				break;
			case Tag::number_9_nine:
				// Video
				break;
			case Tag::number_10_ten:
				// Video
				break;
			default:
				break;
		}
	}
}
