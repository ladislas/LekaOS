#include "Activities.h"

#include "rtos/ThisThread.h"

using namespace leka;
using namespace std::chrono_literals;

void leka::displayTags(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
					   LedsUtils &leds_utils, RFIDUtils &rfid_utils)
{
	display_utils.displayImage(0);

	while (true) {
		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		event_flags_external_interaction.set(KICK_SLEEP_FLAG);
		display_utils.setOn();
		if (auto tag_value = rfid_utils.getTag(); tag_value == Tag::reinforcer_5_rainbow_static) {
			leds_utils.turnOffEars();
			leds_utils.runRainbowColor();
		} else if (tag_value != Tag::number_0_zero) {
			display_utils.displayImage(static_cast<uint8_t>(tag_value));
		} else {
			return;
		}
	}
}

void leka::activityRecognitionColor(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
									LedsUtils &leds_utils, RFIDUtils &rfid_utils)
{
	auto start = rtos::Kernel::Clock::now();

	display_utils.displayImage(0);
	auto expected_tag_color = Tag::color_black;

	auto random_color  = 0;
	auto random_family = 0;

	while (true) {
		random_color = int((rtos::Kernel::Clock::now() - start).count()) % 6;
		rtos::ThisThread::sleep_for(17ms);
		random_family = int((rtos::Kernel::Clock::now() - start).count()) % 4;

		switch (random_color) {
			case 0:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-red");
				expected_tag_color = Tag::color_red;
				break;
			case 1:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-blue");
				expected_tag_color = Tag::color_blue;
				break;
			case 2:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-green");
				expected_tag_color = Tag::color_green;
				break;
			case 3:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-yellow");
				expected_tag_color = Tag::color_yellow;
				break;
			case 4:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-black");
				expected_tag_color = Tag::color_black;
				break;
			case 5:
				display_utils.displayImage(object_colored_family_table.at(random_family) + "-white");
				expected_tag_color = Tag::color_white;
				break;
			default:
				break;
		}

		do {
			event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
			event_flags_external_interaction.set(KICK_SLEEP_FLAG);
			display_utils.setOn();
		} while ((rfid_utils.getTag() != expected_tag_color) && (rfid_utils.getTag() != Tag::number_0_zero));

		if (rfid_utils.getTag() == Tag::number_0_zero) {
			return;
		}

		display_utils.displayVideo("animation-joy");
		leds_utils.runRainbowColor();
		rtos::ThisThread::sleep_for(2s);

		leds_utils.turnOffAll();
		display_utils.setOff();
		rtos::ThisThread::sleep_for(1s);
	}
}

void leka::activityRecognitionEmotions(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
									   LedsUtils &leds_utils, RFIDUtils &rfid_utils)
{
	auto start = rtos::Kernel::Clock::now();

	leds_utils.setBrightness(0x40);
	auto expected_tag_emotion_child = Tag::emotion_happiness_child;
	auto expected_tag_emotion_leka	= Tag::emotion_happiness_leka;

	auto random_value {0};

	while (true) {
		random_value = int((rtos::Kernel::Clock::now() - start).count()) % (emotion_table.size() - 1);
		display_utils.displayImage(emotion_table.at(random_value));

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
			event_flags_external_interaction.set(KICK_SLEEP_FLAG);
			display_utils.setOn();
		} while ((rfid_utils.getTag() != expected_tag_emotion_child) &&
				 (rfid_utils.getTag() != expected_tag_emotion_leka) && (rfid_utils.getTag() != Tag::number_0_zero));

		if (rfid_utils.getTag() == Tag::number_0_zero) {
			return;
		}

		display_utils.displayVideo("animation-joy");
		leds_utils.runRainbowColor();
		rtos::ThisThread::sleep_for(2s);

		leds_utils.turnOffAll();
		display_utils.setOff();
		rtos::ThisThread::sleep_for(1s);
	}
}

void leka::activityColorLeka2(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
							  LedsUtils &leds_utils, RFIDUtils &rfid_utils)
{
	auto is_happy = true;
	display_utils.displayImage("robot-emotion-neutral");

	// leds_utils.setBrightness(0x80);
	auto new_color = CRGB {};

	while (true) {
		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		event_flags_external_interaction.set(KICK_SLEEP_FLAG);

		if (is_happy) {
			display_utils.displayImage("robot-emotion-happy");
		} else {
			display_utils.displayImage("robot-emotion-neutral");
		}
		is_happy = !is_happy;

		auto tag_value = rfid_utils.getTag();
		if (tag_value == Tag::number_0_zero) {
			return;
		}
		switch (tag_value) {
			case Tag::color_red:
				leds_utils.turnOnAll(CRGB::Red);
				break;
			case Tag::color_green:
				leds_utils.turnOnAll(CRGB::Green);
				break;
			case Tag::color_blue:
				leds_utils.turnOnAll(CRGB::Blue);
				break;
			case Tag::color_yellow:
				leds_utils.turnOnAll(CRGB::Yellow);
				break;
			case Tag::color_white:
				leds_utils.turnOnAll(CRGB::White);
				break;
			case Tag::color_black:
				leds_utils.turnOnAll(CRGB::Black);
				break;
			case Tag::reinforcer_5_rainbow_static:
				leds_utils.turnOffEars();
				leds_utils.runRainbowColor();
				break;
			default:
				rtos::ThisThread::sleep_for(10ms);
		}
		rtos::ThisThread::sleep_for(2s);
		event_flags_external_interaction.clear(NEW_RFID_TAG_FLAG);
	}
}

void leka::activityColorLeka3(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
							  LedsUtils &leds_utils, RFIDUtils &rfid_utils)
{
	display_utils.displayImage("robot-emotion-happy");

	// leds_utils.setBrightness(0x80);
	auto new_color	   = CRGB {};
	uint8_t color_step = 0x08;

	while (true) {
		event_flags_external_interaction.wait_any(NEW_RFID_TAG_FLAG);
		event_flags_external_interaction.set(KICK_SLEEP_FLAG);

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
