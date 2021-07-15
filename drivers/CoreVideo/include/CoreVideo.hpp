// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_LKCOREVIDEO_H_
#define _LEKA_OS_DRIVER_LKCOREVIDEO_H_

#include "CoreJPEG.hpp"
#include "LKCoreFatFs.h"
#include "LKCoreSTM32HalBase.h"
#include "interface/DMA2D.hpp"
#include "interface/DSI.hpp"
#include "interface/JPEG.hpp"
#include "interface/LCD.hpp"
#include "interface/LTDC.hpp"
#include "interface/SDRAM.hpp"

namespace leka {

class CoreVideo
{
  public:
	CoreVideo(LKCoreSTM32HalBase &hal, interface::LCD &corelcd, interface::DSIBase &coredsi,
			  interface::LTDCBase &coreltdc, interface::DMA2DBase &coredma2d, interface::JPEGBase &corejpeg,
			  interface::SDRAM &coresdram);

	void initialize();

	void turnOff();
	void turnOn();

	void setBrightness(float value);

	void displayImage(LKCoreFatFs &file);
	void displayVideo(LKCoreFatFs &file);
	void display();

  private:
	LKCoreSTM32HalBase &_hal;

	interface::LCD &_corelcd;
	interface::DSIBase &_coredsi;
	interface::LTDCBase &_coreltdc;
	interface::DMA2DBase &_coredma2d;
	interface::JPEGBase &_corejpeg;
	interface::SDRAM &_coresdram;
};

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_LKCOREVIDEO_H_
