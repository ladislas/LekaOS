// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_BLE_SERVICE_MONITORING_H_
#define _LEKA_OS_LIB_BLE_SERVICE_MONITORING_H_

#include "rtos/EventFlags.h"

#include "Flags.h"
#include "interface/drivers/BLEService.h"

namespace leka {

class BLEServiceMonitoring : public interface::BLEService
{
	const static uint16_t SERVICE_UUID				   = 0xA000;
	const static uint16_t WRITABLE_CHARACTERISTIC_UUID = 0xA001;

	const static uint8_t CHARACTERITICS_COUNT = 1;

  public:
	BLEServiceMonitoring(rtos::EventFlags &event_flags)
		: interface::BLEService(SERVICE_UUID, _characteristic_table.data(), CHARACTERITICS_COUNT),
		  _event_flags(event_flags) {};

	auto getValue() const -> GattAttribute::Handle_t { return _characteristic_value; }

	void onDataUpdated(const GattWriteCallbackParams &params) final
	{
		std::fill_n(&_characteristic_value, 1, '\0');
		std::copy(params.data, params.data + 1, &_characteristic_value);

		_event_flags.set(NEW_BLE_MESSAGE_FLAG);
	};

  private:
	rtos::EventFlags &_event_flags;

	uint8_t _characteristic_value {};
	WriteOnlyGattCharacteristic<uint8_t> _writable_characteristic {WRITABLE_CHARACTERISTIC_UUID,
																   &_characteristic_value};

	std::array<GattCharacteristic *, CHARACTERITICS_COUNT> _characteristic_table {&_writable_characteristic};
};

}	// namespace leka

#endif	 // _LEKA_OS_LIB_BLE_SERVICE_MONITORING_H_
