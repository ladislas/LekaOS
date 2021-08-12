// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVERS_PWMOUT_H_
#define _LEKA_OS_DRIVERS_PWMOUT_H_

#include "interface/drivers/PwmOut.h"

#include "gmock/gmock.h"

namespace leka::mock {

class PwmOut : public interface::PwmOut
{
  public:
	MOCK_METHOD(float, read, (), (override));
	MOCK_METHOD(void, write, (float), (override));
};

}	// namespace leka::mock

#endif	 // _LEKA_OS_DRIVERS_PWMOUT_H_
