// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVERS_FLASH_MEMORY_MOCK_H_
#define _LEKA_OS_DRIVERS_FLASH_MEMORY_MOCK_H_

#include <span>

#include "gmock/gmock.h"
#include "interface/drivers/FlashMemory.h"

namespace leka::mock {

class FlashMemory : public interface::FlashMemory
{
  public:
	MOCK_METHOD(size_t, getSize, (), (override));

	MOCK_METHOD(size_t, read, (uint32_t, std::span<uint8_t>, size_t), (override));
	MOCK_METHOD(size_t, write, (uint32_t, std::span<uint8_t>, size_t), (override));

	MOCK_METHOD(void, erase, (), (override));
};

}	// namespace leka::mock

#endif	 // _LEKA_OS_DRIVERS_FLASH_MEMORY_MOCK_H_
