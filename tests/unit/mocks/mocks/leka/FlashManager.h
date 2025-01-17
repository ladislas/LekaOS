// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVERS_FLASH_MANAGER_MOCK_H_
#define _LEKA_OS_DRIVERS_FLASH_MANAGER_MOCK_H_

#include "gmock/gmock.h"
#include "interface/drivers/FlashManager.h"

namespace leka::mock {

class FlashManager : public interface::FlashManager
{
  public:
	MOCK_METHOD(uint8_t, getStatusRegister, (), (override));

	MOCK_METHOD(void, waitForChipAvailable, (), (override));
	MOCK_METHOD(bool, chipIsNotAvailable, (), (override));

	MOCK_METHOD(void, enableWrite, (), (override));
	MOCK_METHOD(bool, writeIsNotEnabled, (), (override));

	MOCK_METHOD(void, reset, (), (override));
	MOCK_METHOD(void, erase, (), (override));
};

}	// namespace leka::mock

#endif	 // _LEKA_OS_DRIVERS_FLASH_MANAGER_MOCK_H_
