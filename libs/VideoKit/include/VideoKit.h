#ifndef _LEKA_OS_LIB_VIDEOKIT_H_
#define _LEKA_OS_LIB_VIDEOKIT_H_

#include <chrono>
#include <cstdint>

#include "rtos/ThisThread.h"

#include "CGFont.hpp"
#include "CoreDMA2D.hpp"
#include "CoreDSI.hpp"
#include "CoreJPEG.hpp"
#include "CoreLCD.hpp"
#include "CoreLCDDriverOTM8009A.hpp"
#include "CoreLTDC.hpp"
#include "Graphics.h"
#include "LKCoreSTM32Hal.h"

using namespace std::chrono_literals;

namespace leka {

class VideoKit
{
  public:
	VideoKit(LKCoreSTM32HalBase &);

	auto getDSI() -> CoreDSI &;
	auto getLTDC() -> CoreLTDC &;
	auto getDMA2D() -> CoreDMA2D &;
	auto getJPEG() -> CoreJPEG &;

	void initialize();

	void setFrameRateLimit(uint32_t framerate);

	void fillConfig(LKCoreFatFs &file, JPEGConfig *config);

	void clear(gfx::Color color = gfx::Color::White);

	void draw(gfx::Drawable &drawable);

	void drawRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, gfx::Color color);

	void drawText(const char *text, uint32_t x, uint32_t y, gfx::Color color, gfx::Color bg = gfx::Color::Transparent);

	auto drawImage(LKCoreFatFs &file) -> uint32_t;

	auto drawImage(LKCoreFatFs &file, JPEGConfig &config) -> uint32_t;

	void display();

  private:
	void refresh();
	void tick();

	LKCoreSTM32HalBase &_hal;

	// peripherals
	CoreJPEGModeDMA _corejpegmode;
	CoreJPEG _corejpeg;
	CoreDMA2D _coredma2d;

	// ltdc + dsi + lcd screen
	CoreLTDC _coreltdc;
	CoreDSI _coredsi;
	CoreLCDDriverOTM8009A _coreotm;
	CoreLCD _corelcd;

	rtos::Kernel::Clock::time_point _last_time {};
	std::chrono::milliseconds _frametime = 40ms;
};

}	// namespace leka

#endif	 // _LEKA_OS_LIB_VIDEOKIT_H_
