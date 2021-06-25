// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_RFID_MOCK_H_
#define _LEKA_OS_RFID_MOCK_H_

#include <cstddef>

#include "gmock/gmock.h"
#include "interface/drivers/RFID.h"

namespace leka {

class CoreRFIDMock : public interface::RFID
{
  public:
	MOCK_METHOD(interface::BufferedSerial &, getSerial, (), (override));
	MOCK_METHOD(void, enableTagDetection, (), (override));
	MOCK_METHOD(bool, setup, (), (override));
	MOCK_METHOD(void, send, (const lstd::span<uint8_t> &), (override));
	MOCK_METHOD(size_t, receiveTagData, (const lstd::span<uint8_t> &), (override));
	MOCK_METHOD(bool, receiveCallback, (), (override));
};

}	// namespace leka

#endif	 // _LEKA_OS_RFID_MOCK_H_
