// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_DSI_H_
#define _LEKA_OS_DRIVER_DSI_H_

#include <array>

#include "LKCoreSTM32HalBase.h"
#include "interface/DSI.hpp"
#include "interface/LTDC.hpp"
#include "internal/corevideo_config.h"

namespace leka {

class CoreDSI : public interface::DSIBase
{
  public:
	explicit CoreDSI(LKCoreSTM32HalBase &hal, interface::LTDCBase &ltdc);

	void initialize() final;
	void reset() final;
	void refresh() final;

	void enableLPCmd() final;
	void disableLPCmd() final;

	void enableTearingEffectReporting() final;

	auto refreshDone() -> bool final;

	void write(const uint8_t *data, uint32_t size) final;

  private:
	LKCoreSTM32HalBase &_hal;
	interface::LTDCBase &_ltdc;

	DSI_CmdCfgTypeDef _cmdconf;
	DSI_LPCmdTypeDef _lpcmd;

	std::array<std::array<uint8_t, 4>, dsi::refresh_columns_count> _columns;
	int _current_column = 0;

	bool _sync_on_TE   = false;
	bool _refresh_done = true;
};

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_DSI_H_
