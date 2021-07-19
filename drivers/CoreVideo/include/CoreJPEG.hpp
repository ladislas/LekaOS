// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_JPEG_H_
#define _LEKA_OS_DRIVER_JPEG_H_

#include <array>
#include <cstdint>
#include <memory>

#include "CoreJPEGMode.h"
#include "LKCoreFatFsBase.h"
#include "LKCoreSTM32HalBase.h"
#include "external/st_jpeg_utils.h"
#include "interface/JPEG.hpp"

namespace leka {

class CoreJPEG : public interface::JPEGBase
{
  public:
	explicit CoreJPEG(LKCoreSTM32HalBase &hal);

	void initialize() final;

	auto getConfig() -> JPEG_ConfTypeDef final;

	void registerCallbacks() final;

	auto decodeImage(LKCoreFatFsBase &file) -> uint32_t final;

	static auto getWidthOffset(JPEG_ConfTypeDef &config) -> uint32_t;
	static auto findFrameOffset(LKCoreFatFsBase &file, uint32_t offset) -> uint32_t;

  private:
	LKCoreSTM32HalBase &_hal;

	CoreJPEGModeDMA _mode;
};

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_JPEG_H_
