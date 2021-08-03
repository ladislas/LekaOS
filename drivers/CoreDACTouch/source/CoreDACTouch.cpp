// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX - License - Identifier : Apache - 2.0

#include "CoreDACTouch.h"
#include <array>

using namespace leka;

void CoreDACTouch::setVoltageReference(uint8_t voltageReference)
{
	uint8_t command = dac_touch::command::setVoltageReference | voltageReference;
	_i2c.write(_address, &command, 1, false);
}

void CoreDACTouch::setPowerMode(uint8_t powerMode)
{
	std::array<uint8_t, 2> command;
	command[0] = dac_touch::command::powerMode | ((0xf0 & powerMode) >> 4);
	command[1] = (0x0f & powerMode) << 4;

	_i2c.write(_address, command.data(), command.size(), false);
}
