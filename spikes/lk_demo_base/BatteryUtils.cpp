#include "BatteryUtils.h"

#include "rtos/ThisThread.h"

using namespace leka;
using namespace std::chrono_literals;

void BatteryUtils::registerEventQueue(events::EventQueue &event_queue)
{
	while (battery.getVoltage() < leka::CoreBattery::Capacity::empty) {
		rtos::ThisThread::sleep_for(1s);
	}

	event_queue.call_every(1s, this, &BatteryUtils::checkReboot);
}

void BatteryUtils::checkReboot()
{
	if (battery.getVoltage() < leka::CoreBattery::Capacity::empty) {
		NVIC_SystemReset();
	}

	if (checkRaiseAndDrop()) {
		NVIC_SystemReset();
	}
}

auto BatteryUtils::checkRaiseAndDrop() -> bool
{
	if (last_charge_status != battery.isCharging()) {
		raise_and_drop_counter++;
		loop_since_charge = 0;
	} else if (battery.isCharging() and ++loop_since_charge > 60) {
		raise_and_drop_counter = 0;
	}

	last_charge_status = battery.isCharging();

	return raise_and_drop_counter > 10;
}
