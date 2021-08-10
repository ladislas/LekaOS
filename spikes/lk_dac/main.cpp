// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "mbed.h"
#include <sys/types.h>

#include "CoreDAC.h"
#include "CoreDACTimer.h"
#include "LKCoreSTM32Hal.h"

using namespace leka;

mbed::DigitalOut audio_enable(SOUND_ENABLE, 1);
static auto serial = mbed::BufferedSerial(USBTX, USBRX, 115200);

LKCoreSTM32Hal hal;

CoreDACTimer coreTimer_6(hal, CoreDACTimer::HardWareBasicTimer::BasicTimer6);
CoreDACTimer coreTimer_7(hal, CoreDACTimer::HardWareBasicTimer::BasicTimer7);
CoreDAC coreDac(hal);

uint32_t _countSamples						 = 0;
bool _ended									 = false;
CoreDACTimer::HardWareBasicTimer _currentTim = CoreDACTimer::HardWareBasicTimer::BasicTimer6;

/**
 * @brief This function handles DMA1 stream5 global interrupt.
 */
extern "C" {
void DMA1_Stream5_IRQHandler()
{
	HAL_DMA_IRQHandler(coreDac.getHandle().DMA_Handle1);
}
}

void fillBufferWithSinWave(uint16_t *buffer, uint32_t bufferSize, uint32_t frequency, uint32_t samplingRate,
						   uint16_t maxValue, uint16_t minValue)
{
	uint32_t samplesPerPeriod = (samplingRate / frequency);

	for (uint32_t i = 0; i < bufferSize; ++i) {
		float tmp = 0.5 * sin(i * 2.0 * M_PI / samplesPerPeriod) + 0.5;
		tmp *= maxValue - minValue;
		buffer[i] = tmp + minValue;
	}
}

void callbackTest(DAC_HandleTypeDef *)
{
	static const int numSecs		 = 2;
	static const uint32_t maxSamples = numSecs * 44100 / 256;	// Num of calls to callback in numSecs seconds
	if (_countSamples <= maxSamples) {
		_countSamples++;
	}
	if (_countSamples == maxSamples) {
		_ended = true;

		switch (_currentTim) {
			case CoreDACTimer::HardWareBasicTimer::BasicTimer6:
				coreTimer_6.stop();
				break;
			case CoreDACTimer::HardWareBasicTimer::BasicTimer7:
				coreTimer_7.stop();
				break;
		}
	}
}

void startSound(CoreDACTimer::HardWareBasicTimer tim)
{
	_countSamples = 0;
	_ended		  = false;
	if (tim == CoreDACTimer::HardWareBasicTimer::BasicTimer6) {
		coreTimer_6.start();
		_currentTim = CoreDACTimer::HardWareBasicTimer::BasicTimer6;
	} else if (tim == CoreDACTimer::HardWareBasicTimer::BasicTimer7) {
		coreTimer_7.start();
		_currentTim = CoreDACTimer::HardWareBasicTimer::BasicTimer7;
	}
}

auto main() -> int
{
	std::array<uint16_t, 512> outBuff {};
	fillBufferWithSinWave(outBuff.data(), 512, 440, 44100, 0x999, 0x666);
	auto outSpan = lstd::span {outBuff.data(), outBuff.size()};

	printf("\n\nHello, investigation day!\n\n");

	coreDac.setOnHalfBufferReadPtr(&callbackTest);
	coreDac.setOnFullBufferReadPtr(&callbackTest);

	// Init
	coreTimer_6.initialize(44100);
	coreTimer_7.initialize(44100);

	coreDac.initialize(coreTimer_6);
	coreDac.start(outSpan);

	// start
	printf("First timer\n");
	startSound(CoreDACTimer::HardWareBasicTimer::BasicTimer6);

	while (!_ended) {
		rtos::ThisThread::sleep_for(50ms);
	}
	rtos::ThisThread::sleep_for(2s);

	coreDac.configTimer(coreTimer_7);	// change timer associated to DAC

	printf("Second timer\n");
	startSound(CoreDACTimer::HardWareBasicTimer::BasicTimer7);

	while (!_ended) {
		rtos::ThisThread::sleep_for(50ms);
	}

	printf("End of DAC and Timer test!\n\n");
}