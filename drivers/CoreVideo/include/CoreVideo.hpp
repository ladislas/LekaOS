// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_LKCOREVIDEO_H_
#define _LEKA_OS_DRIVER_LKCOREVIDEO_H_

#include "LKCoreSTM32HalBase.h"
#include "interface/CoreGraphics.hpp"
#include "interface/CoreJPEG.hpp"
#include "interface/CoreSDRAM.hpp"
#include "interface/DMA2D.hpp"
#include "interface/DSI.hpp"
#include "interface/Font.hpp"
#include "interface/LCD.hpp"
#include "interface/LTDC.hpp"

namespace leka {

class CoreVideo
{
  public:
	CoreVideo(LKCoreSTM32HalBase &hal, interface::CoreSDRAM &coresdram, interface::DMA2DBase &coredma2d,
			  interface::DSIBase &coredsi, interface::LTDCBase &coreltdc, interface::LCD &corelcd,
			  interface::CoreGraphics &coregraphics, interface::Font &corefont, interface::CoreJPEG &corejpeg);

	void initialize();

	void turnOff();
	void turnOn();

	void setBrightness(float value);

	void clearScreen(CGColor color = CGColor::white);
	void displayRectangle(interface::CoreGraphics::FilledRectangle rectangle, CGColor color);
	void displayImage(FIL *file);
	void displayText(const char *text, uint32_t size, uint32_t starting_line, CGColor foreground = CGColor::black,
					 CGColor background = CGColor::white);

  private:
	LKCoreSTM32HalBase &_hal;
	interface::CoreSDRAM &_coresdram;
	interface::DMA2DBase &_coredma2d;
	interface::DSIBase &_coredsi;
	interface::LTDCBase &_coreltdc;
	interface::LCD &_corelcd;
	interface::CoreGraphics &_coregraphics;
	interface::Font &_corefont;
	interface::CoreJPEG &_corejpeg;
};

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_LKCOREVIDEO_H_
