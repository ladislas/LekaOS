// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_SPIKE_LED_UTILS_H_
#define _LEKA_OS_SPIKE_LED_UTILS_H_

#include "FastLED.h"
#include "LekaPinNames.h"

// SPI spi(D11, NC, D13);
SPI spi(LED_BELT_SPI_MOSI, NC, LED_BELT_SPI_SCK);

#define NUM_LEDS 20
#define BRIGHTNESS 255

CRGB leds[NUM_LEDS];

// static BufferedSerial serial(USBTX, USBRX, 9600);

// constexpr uint8_t buff_size = 128;
// char buff[buff_size] {};

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p;

uint32_t mainMillis()
{
	HighResClock::lock();
	auto t = HighResClock::now();
	HighResClock::unlock();
	std::chrono::microseconds tms = t.time_since_epoch();
	return static_cast<uint32_t>(tms.count() / 1000);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
	uint8_t brightness = 255;

	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
		colorIndex += 3;
	}
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
	for (int i = 0; i < 16; i++) {
		currentPalette[i] = CHSV(random8(), 255, random8());
	}
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid(currentPalette, 16, CRGB::Black);
	// and set every fourth one to white.
	currentPalette[0]  = CRGB::White;
	currentPalette[4]  = CRGB::White;
	currentPalette[8]  = CRGB::White;
	currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
	CRGB purple = CHSV(HUE_PURPLE, 255, 255);
	CRGB green	= CHSV(HUE_GREEN, 255, 255);

	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(green, green, black, black, purple, purple, black, black, green, green, black, black,
								   purple, purple, black, black);
}

// This function sets up a palette of purple and green stripes.
void turnOff()
{
	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(black, black, black, black, black, black, black, black, black, black, black, black,
								   black, black, black, black);
}

void ChangePalettePeriodically()
{
	uint8_t secondHand		  = (mainMillis() / 1000) % 60;
	static uint8_t lastSecond = 99;

	if (lastSecond != secondHand) {
		lastSecond = secondHand;

		if (secondHand == 0) {
			currentPalette	= RainbowColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 10) {
			currentPalette	= RainbowStripeColors_p;
			currentBlending = NOBLEND;
		}
		if (secondHand == 15) {
			currentPalette	= RainbowStripeColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 20) {
			SetupPurpleAndGreenPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 25) {
			SetupTotallyRandomPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 30) {
			SetupBlackAndWhiteStripedPalette();
			currentBlending = NOBLEND;
		}
		if (secondHand == 35) {
			SetupBlackAndWhiteStripedPalette();
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 40) {
			currentPalette	= CloudColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 45) {
			currentPalette	= PartyColors_p;
			currentBlending = LINEARBLEND;
		}
		if (secondHand == 50) {
			currentPalette	= myRedWhiteBluePalette_p;
			currentBlending = NOBLEND;
		}
		if (secondHand == 55) {
			currentPalette	= myRedWhiteBluePalette_p;
			currentBlending = LINEARBLEND;
		}
	}
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p = {CRGB::Red,
												   CRGB::Gray,	 // 'white' is too bright compared to red and blue
												   CRGB::Blue,	 CRGB::Black,

												   CRGB::Green,	 CRGB::Purple, CRGB::Green,	 CRGB::Black,

												   CRGB::Yellow, CRGB::Gray,   CRGB::Yellow, CRGB::Black,

												   CRGB::Orange, CRGB::Gray,   CRGB::Orange, CRGB::Black};

void initLed()
{
	FastLED.addLeds<SK9822, LED_BELT_SPI_MOSI, LED_BELT_SPI_SCK, RGB>(leds, spi, NUM_LEDS);
	FastLED.setBrightness(BRIGHTNESS);

	leds[0] = CRGB::White;
	leds[1] = CRGB::Black;
	leds[2] = CRGB::White;
	leds[3] = CRGB::Black;
	leds[4] = CRGB::White;
	FastLED.show();

	rtos::ThisThread::sleep_for(2s);

	currentPalette	= RainbowColors_p;
	currentBlending = LINEARBLEND;
}

void led_thread()
{
	static uint8_t startIndex = 0;

	while (true) {
		ChangePalettePeriodically();
		startIndex = startIndex + 1; /* motion speed */
		FillLEDsFromPaletteColors(startIndex);
		FastLED.show();
		rtos::ThisThread::sleep_for(10ms);
	}
}

#endif	 // _LEKA_OS_SPIKE_LED_UTILS_H_