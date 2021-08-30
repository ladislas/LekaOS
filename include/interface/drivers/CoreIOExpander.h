// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_INTERFACE_DRIVER_CORE_IO_EXPANDER_H_
#define _LEKA_OS_INTERFACE_DRIVER_CORE_IO_EXPANDER_H_

#include <cstdint>

#include "PlatformMutex.h"

namespace leka::interface {

class CoreIOExpander
{
  public:
	virtual ~CoreIOExpander() = default;

	virtual void setRegisterMappingUnified() = 0;
	virtual void setRegisterMappingSeparated() = 0;
	virtual void init(uint16_t input_pins)							   = 0;
	virtual void setInputPins(uint16_t pins)						   = 0;
	virtual void setOutputPins(uint16_t pins)						   = 0;
	virtual void writeOutputs(uint16_t values)						   = 0;
	virtual auto readOutputs() -> uint16_t							   = 0;
	virtual auto readInputs() -> uint16_t							   = 0;
	virtual void setInputPolarity(uint16_t values)					   = 0;
	virtual auto getInputPolarity() -> uint16_t						   = 0;
	virtual void setPullups(uint16_t values)						   = 0;
	virtual auto getPullups() -> uint16_t							   = 0;
	virtual void interruptOnChanges(uint16_t pins)					   = 0;
	virtual void disableInterrupts(uint16_t pins)					   = 0;
	virtual void acknowledgeInterrupt(uint16_t &pin, uint16_t &values) = 0;

	PlatformMutex mutex;
};

}	// namespace leka::interface

#endif	 // _LEKA_OS_INTERFACE_DRIVER_CORE_IO_EXPANDER_H_
