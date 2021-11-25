#include "LedsUtils.h"

#include "rtos/ThisThread.h"

using namespace leka;
using namespace std::chrono_literals;

void LedsUtils::initialize()
{
	leds_controller.addLeds<SK9822, LED_BELT_SPI_MOSI, LED_BELT_SPI_SCK, BGR>(leds_belt_control.data(),
																			  leds_belt_output, leds_belt_count);
	leds_controller.addLeds<SK9822, LED_EARS_SPI_MOSI, LED_EARS_SPI_SCK, BGR>(leds_ears_control.data(),
																			  leds_ears_output, leds_ears_count);

	turnOffAll();
}

void LedsUtils::initializationAnimation()
{
	setBrightness(0x40);
	for (auto &led: leds_belt_control) {
		for (uint16_t intensity = 0; intensity < 0x100; intensity += 0x10) {
			_last_color = CRGB(intensity, intensity, intensity);
			led			= _last_color;
			leds_controller.show();
			rtos::ThisThread::sleep_for(10ms);
		}
	}
	for (int i = 0; i < 3; i++) {
		turnOffBelt();
		rtos::ThisThread::sleep_for(400ms);
		turnOnBelt(CRGB::White);
		rtos::ThisThread::sleep_for(400ms);
	}
	rtos::ThisThread::sleep_for(800ms);

	turnOffAll();
}

void LedsUtils::setBrightness(uint8_t brightness)
{
	if (brightness != 0) {
		_brightness = brightness;
	}
	leds_controller.setBrightness(brightness);
}

void LedsUtils::turnOffAll()
{
	leds_controller.setBrightness(0x00);
	leds_controller.clear();
	_last_color = 0;

	leds_controller.show();
}

void LedsUtils::turnOffBelt()
{
	for (auto &led: leds_belt_control) {
		led = 0;
	}
	_last_color = 0;
	leds_controller.show();
}

void LedsUtils::turnOffEars()
{
	for (auto &led: leds_ears_control) {
		led = 0;
	}
	_last_color = 0;
	leds_controller.show();
}

void LedsUtils::turnOnAll(CRGB color)
{
	setBrightness(_brightness);
	turnOnBelt(color);
	turnOnEars(color);
	_last_color = color;
}

void LedsUtils::turnOnBelt(CRGB color)
{
	for (auto &led: leds_belt_control) {
		led = color;
	}
	_last_color = color;
	leds_controller.show();
}

void LedsUtils::turnOnEars(CRGB color)
{
	for (auto &led: leds_ears_control) {
		led = color;
	}
	_last_color = color;
	leds_controller.show();
}

auto LedsUtils::wheelFire(uint8_t wheel_position) -> uint32_t
{
	if (wheel_position < 85) {
		// return beltLeds.Color(255, wheelPosition * 3, 0); //MODEL
		// return (0xFF << 16) & ((wheel_position * 3) << 8);
		return 255 * 16 + (wheel_position * 3) * 8;
	}

	if (wheel_position < 170) {
		wheel_position -= 85;
		// return beltLeds.Color(255, wheelPosition, 0); //MODEL
		// return (0xFF << 16) & (wheel_position << 8);
		return 255 * 16 + wheel_position * 8;
	}

	wheel_position -= 170;
	// return beltLeds.Color(255, wheelPosition, 0); //MODEL
	// return (0xFF << 16) & (wheel_position << 8);
	return 255 * 16 + wheel_position * 8;
}

auto LedsUtils::offsetColor(int16_t red, int16_t green, int16_t blue) -> CRGB
{
	if (auto new_red = _last_color.red + red; new_red < 0x00) {
		_last_color.red = 0x00;
	} else if (new_red > 0xFF) {
		_last_color.red = 0xFF;
	} else {
		_last_color.red = new_red;
	}

	if (auto new_green = _last_color.green + green; new_green < 0x00) {
		_last_color.green = 0x00;
	} else if (new_green > 0xFF) {
		_last_color.green = 0xFF;
	} else {
		_last_color.green = new_green;
	}

	if (auto new_blue = _last_color.blue + blue; new_blue < 0x00) {
		_last_color.blue = 0x00;
	} else if (new_blue > 0xFF) {
		_last_color.blue = 0xFF;
	} else {
		_last_color.blue = new_blue;
	}

	return _last_color;
}

void LedsUtils::fillFromPaletteColors(uint8_t index_palette)
{
	for (auto &led: leds_belt_control) {
		led = ColorFromPalette(_palette, index_palette, _brightness, _blending);
		index_palette += 3;
	}
}

void LedsUtils::runMotivationFire()
{
	for (int j = 0; j < 256 * 3; j++) {
		for (int i = 0; i < leds_belt_count; i++) {
			leds_belt_control[i] = wheelFire(((i * 256 / 20) + j) & 255);
		}
		leds_controller.show();
		rtos::ThisThread::sleep_for(10ms);
	}
}

void LedsUtils::runRainbowColor()
{
	runRainbowColorStopLess();
	turnOffAll();
}

void LedsUtils::runRainbowColorStopLess(int repetition)
{
	setBrightness(_brightness);
	_palette = RainbowColors_p;

	for (int rep = 0; rep < repetition; rep++) {
		int start_index_palette = 0;

		while (start_index_palette < 400) {
			start_index_palette++;

			fillFromPaletteColors(start_index_palette);

			FastLED.show();
			rtos::ThisThread::sleep_for(10ms);
		}
	}
}
