// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_CORE_FLASH_MEMORY_IS25LP016D_H_
#define _LEKA_OS_DRIVER_CORE_FLASH_MEMORY_IS25LP016D_H_

#include <cstdint>
#include <span>

#include "interface/drivers/FlashManager.h"
#include "interface/drivers/FlashMemory.h"
#include "interface/drivers/QSPI.h"

namespace leka {

class CoreFlashIS25LP016D : public interface::FlashMemory
{
  public:
	explicit CoreFlashIS25LP016D(interface::QSPI &qspi, interface::FlashManager &flash_manager)
		: _qspi(qspi), _flash_manager(flash_manager) {};

	auto getSize() -> size_t final;

	void reset();

	auto read(uint32_t address, std::span<uint8_t> rx_buffer, size_t rx_buffer_size) -> size_t final;
	auto write(uint32_t address, std::span<uint8_t> tx_buffer, size_t tx_buffer_size) -> size_t final;

	void erase() final;

  private:
	interface::QSPI &_qspi;
	interface::FlashManager &_flash_manager;
};

}	// namespace leka

namespace flash::is25lp016d {

constexpr size_t size					 = 0x00200000;
constexpr auto max_clock_frequency_in_hz = 133'000'000;

namespace command {
	constexpr uint8_t write = 0x02;
	constexpr uint8_t read	= 0x03;
}	// namespace command

// }	// namespace status

}	// namespace flash::is25lp016d

#endif	 //_LEKA_OS_DRIVER_CORE_FLASH_MEMORY_IS25LP016D_H_
