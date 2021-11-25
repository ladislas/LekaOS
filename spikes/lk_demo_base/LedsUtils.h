// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LEDS_UTILS_H_
#define _LEKA_OS_LEDS_UTILS_H_

#include "FastLED.h"

namespace leka {

class LedsUtils
{
  public:
	explicit LedsUtils() = default;

	void initialize();
	void initializationAnimation();

	void setBrightness(uint8_t brightness);

	void turnOffAll();
	void turnOffBelt();
	void turnOffEars();

	void turnOnAll(CRGB color);
	void turnOnBelt(CRGB color);
	void turnOnEars(CRGB color);

	auto offsetColor(int16_t red, int16_t green, int16_t blue) -> CRGB;

	void fillFromPaletteColors(uint8_t index_palette);

	void runMotivationFire();
	void runRainbowColorStopLess(int repetition = 1);
	void runRainbowColor();

  private:
	auto wheelFire(uint8_t wheel_position) -> uint32_t;

	uint8_t _brightness = 0;
	CRGB _last_color {};

	static const int leds_belt_count {20};
	static const int leds_ears_count {2};

	mbed::SPI leds_belt_output {LED_BELT_SPI_MOSI, NC, LED_BELT_SPI_SCK};
	mbed::SPI leds_ears_output {LED_EARS_SPI_MOSI, NC, LED_EARS_SPI_SCK};

	std::array<CRGB, leds_belt_count> leds_belt_control;
	std::array<CRGB, leds_ears_count> leds_ears_control;

	CFastLED leds_controller;

	CRGBPalette16 _palette {};
	TBlendType _blending {LINEARBLEND};
};

}	// namespace leka

#endif	 // _LEKA_OS_LEDS_UTILS_H_
