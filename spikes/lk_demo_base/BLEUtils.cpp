#include "BLEUtils.h"

using namespace leka;

void BLEUtils::setDeviceName(const lstd::span<const char> name)
{
	_ble_gap.setDeviceName(name);
}

auto BLEUtils::getMonitoringData() -> uint8_t
{
	return _ble_service_monitoring.getValue();
}

auto BLEUtils::getLCDIntensity() -> uint8_t
{
	return _ble_service_monitoring.getLCDIntensity();
}

void BLEUtils::startAdvertising()
{
	std::array<interface::BLEService *, 1> services = {&_ble_service_monitoring};
	_ble_server.addServices(services);

	_ble_gap.onInit(mbed::Callback(&_ble_server, &BLEGattServer::start));

	_ble_gap.start();
}
