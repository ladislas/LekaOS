// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "MCP23017.h"
#include <array>
#include <cstdint>

using namespace leka;

MCP23017::ExpandedIO::ExpandedIO(MCP23017 &parent, Pin pin) : _parent(parent), _pin(pin) {}

auto MCP23017::ExpandedIO::internalRead() -> int
{
	return (_parent.readInputs() & _pin);
}

void MCP23017::ExpandedIO::internalMode(PinMode pull)
{
	if (pull != PullDown) {	  // MCP23017 may not support PullDown mode
		_parent.mutex.lock();
		uint8_t pullups = _parent.getPullups();

		if (pull == PullNone) {
			pullups &= ~_pin;
		} else if (pull == PullUp) {
			pullups |= _pin;
		}

		_parent.setPullups(pullups);
		_parent.mutex.unlock();
	}
}

void MCP23017::ExpandedIO::internalWrite(int value)
{
	uint16_t outputs = _parent.readOutputs();
	if (value) {
		_parent.writeOutputs((outputs | _pin));
	} else {
		_parent.writeOutputs((outputs & (~_pin)));
	}
}

void MCP23017::ExpandedIO::internalOutput()
{
	_parent.setOutputPins(_pin);
}

void MCP23017::ExpandedIO::internalInput()
{
	_parent.setInputPins(_pin);
}

void MCP23017::init()
{
	reset();
}

void MCP23017::setRegisterMapping(bool separated)
{
	writeRegister(mcp23017::registers::IOCON, static_cast<uint8_t>(separated));
}

void MCP23017::reset()
{
	//
	// First make sure that the device is in BANK=0 mode
	//
	writeRegister(0x05, (unsigned char)0x00);
	//
	// set direction registers to inputs
	//
	writeRegister(mcp23017::registers::IODIR, (unsigned short)0xFFFF);
	//
	// set all other registers to zero (last of 10 registers is OLAT)
	//
	for (int reg_addr = 2; reg_addr <= mcp23017::registers::OLAT; reg_addr += 2) {
		writeRegister(reg_addr, (unsigned short)0x0000);
	}
	//
	// Set the shadow registers to power-on state
	//
	shadow_IODIR   = 0xFFFF;
	shadow_GPIO	   = 0;
	shadow_GPPU	   = 0;
	shadow_IPOL	   = 0;
	shadow_GPINTEN = 0;
}

void MCP23017::writeRegister(uint8_t reg, uint16_t value)
{
	std::array<uint8_t, 3> buffer {};

	buffer[0] = reg;
	buffer[1] = (0x00FF & value);
	buffer[2] = (0xFF00 & value) >> 8;

	_i2c.write(_I2C_ADDRESS, buffer.data(), buffer.size());
}

auto MCP23017::readRegister(uint8_t reg) -> uint16_t
{
	std::array<uint8_t, 2> buffer {reg, 0};

	mutex.lock();

	_i2c.write(_I2C_ADDRESS, buffer.data(), 1);
	_i2c.read(_I2C_ADDRESS, buffer.data(), 2);

	mutex.unlock();

	return (buffer[0] + (buffer[1] << 8));
}

void MCP23017::setInputPins(uint8_t pins)
{
	auto value = readRegister(mcp23017::registers::IODIR);

	writeRegister(mcp23017::registers::IODIR, value | pins);
}

void MCP23017::setOutputPins(uint8_t pins)
{
	auto value = readRegister(mcp23017::registers::IODIR);

	writeRegister(mcp23017::registers::IODIR, value & ~pins);
}

void MCP23017::writeOutputs(uint16_t values)
{
	writeRegister(mcp23017::registers::GPIO, values);
}

auto MCP23017::readOutputs() -> uint16_t
{
	return readRegister(mcp23017::registers::OLAT);
}

uint16_t MCP23017::readInputs()
{
	return readRegister(mcp23017::registers::GPIO);
}

void MCP23017::setInputPolarity(uint16_t values)
{
	writeRegister(mcp23017::registers::IPOL, values);
}

auto MCP23017::getInputPolarity() -> uint16_t
{
	return readRegister(mcp23017::registers::IPOL);
}

void MCP23017::setPullups(uint16_t values)
{
	writeRegister(mcp23017::registers::GPPU, values);
}

auto MCP23017::getPullups() -> uint16_t
{
	return readRegister(mcp23017::registers::GPPU);
}

void MCP23017::interruptOnChanges(uint16_t pins)
{
	uint16_t value = readRegister(mcp23017::registers::INTCON);
	value &= ~pins;
	writeRegister(mcp23017::registers::INTCON, value);

	value = readRegister(mcp23017::registers::GPINTEN);
	value |= pins;
	writeRegister(mcp23017::registers::GPINTEN, value);
}

void MCP23017::disableInterrupts(uint16_t pins)
{
	uint16_t value = readRegister(mcp23017::registers::GPINTEN);
	value &= ~pins;
	writeRegister(mcp23017::registers::GPINTEN, value);
}

void MCP23017::acknowledgeInterrupt(uint16_t &pin, uint16_t &values)
{
	pin	   = readRegister(mcp23017::registers::INTF);
	values = readRegister(mcp23017::registers::INTCAP);
}
