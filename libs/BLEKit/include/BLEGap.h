// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_BLE_GAP_H_
#define _LEKA_OS_LIB_BLE_GAP_H_

#include "ble/Gap.h"

namespace leka {

class BLEGap : public ble::Gap::EventHandler
{
  public:
	explicit BLEGap() = default;

	~BLEGap() = default;

  private:
};

}	// namespace leka

#endif	 // _LEKA_OS_LIB_BLE_GAP_H_