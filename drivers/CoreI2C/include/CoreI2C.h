// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_LK_CORE_I2C_H_
#define _LEKA_OS_DRIVER_LK_CORE_I2C_H_

#include "drivers/I2C.h"

namespace leka::interface {

class I2C
{
  public:
	virtual ~I2C() = default;

	virtual auto read(int address, uint8_t *data, int length, bool repeated = false) -> int		   = 0;
	virtual auto write(int address, const uint8_t *data, int length, bool repeated = false) -> int = 0;
};

}	// namespace leka::interface

namespace leka {

class CoreI2C : public interface::I2C
{
  public:
	explicit CoreI2C(mbed::I2C &i2c) : _i2c {i2c} {};

	auto read(int address, uint8_t *data, int length, bool repeated) -> int final;
	auto write(int address, const uint8_t *data, int length, bool repeated) -> int final;

  private:
	mbed::I2C &_i2c;
};

}	// namespace leka

#endif	 //_LEKA_OS_DRIVER_LK_CORE_I2C_H_
