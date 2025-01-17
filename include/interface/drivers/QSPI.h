// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_INTERFACE_QSPI_H_
#define _LEKA_OS_DRIVER_INTERFACE_QSPI_H_

#include <cstdint>
#include <span>
#include <tuple>

namespace leka::interface {

class QSPI
{
  public:
	virtual ~QSPI() = default;

	virtual void setDataTransmissionFormat()	  = 0;
	virtual void setFrequency(int hz = 1'000'000) = 0;

	virtual auto read(uint8_t command, uint32_t address, std::span<uint8_t> rx_buffer, size_t rx_buffer_size)
		-> size_t = 0;
	virtual auto write(uint8_t command, uint32_t address, const std::span<uint8_t> tx_buffer, size_t tx_buffer_size)
		-> size_t = 0;

	virtual auto sendCommand(uint8_t command, uint32_t address, const std::span<uint8_t> tx_buffer,
							 size_t tx_buffer_size, std::span<uint8_t> rx_buffer, size_t rx_buffer_size)
		-> std::tuple<size_t, size_t> = 0;
};

}	// namespace leka::interface

#endif	 // _LEKA_OS_DRIVER_INTERFACE_QSPI_H_
