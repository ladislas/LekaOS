// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_JPEG_H_
#define _LEKA_OS_DRIVER_JPEG_H_

#include <cstdint>
#include <memory>

#include "LKCoreFatFsBase.h"
#include "LKCoreJPEGMode.h"
#include "LKCoreSTM32HalBase.h"
#include "interface/LKCoreJPEG.h"
#include "st_jpeg_utils.h"

namespace leka {

class LKCoreJPEG : public interface::LKCoreJPEG
{
  public:
	LKCoreJPEG(LKCoreSTM32HalBase &hal, std::unique_ptr<LKCoreJPEGMode> mode);

	void initialize() final;

	auto getConfig() -> JPEG_ConfTypeDef final;

	void registerCallbacks() final;

	auto decodeImage(LKCoreFatFsBase &file) -> uint32_t final;

	static auto getWidthOffset(JPEG_ConfTypeDef &config) -> uint32_t;
	static auto findFrameOffset(LKCoreFatFsBase &file, uint32_t offset) -> uint32_t;

  private:
	LKCoreSTM32HalBase &_hal;

	std::unique_ptr<LKCoreJPEGMode> _mode;
};

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_JPEG_H_
