// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_MBED_MOCK_DIGITAL_OUT_H_
#define _LEKA_OS_MBED_MOCK_DIGITAL_OUT_H_

#include "PinNames.h"

#include "drivers/interfaces/InterfaceDigitalOut.h"

#include "gmock/gmock.h"

namespace mbed::mock {

class DigitalOut : public mbed::interface::DigitalOut
{
  public:
	MOCK_METHOD(void, write, (int value), (override));
	MOCK_METHOD(int, read, (), (override));
	MOCK_METHOD(int, is_connected, (), (override));
};

}	// namespace mbed::mock

#endif	 // _LEKA_OS_MBED_MOCK_DIGITAL_OUT_H_
