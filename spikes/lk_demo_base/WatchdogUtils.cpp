#include "WatchdogUtils.h"

#include "drivers/Watchdog.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

constexpr uint32_t timeout_ms	  = 30000;
constexpr uint32_t min_stack_size = 72;
rtos::Thread watchdog_thread;
mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();

void startWatchdog()
{
	watchdog.start(timeout_ms);
	watchdog_thread.start(watchdogLoop);
}

void watchdogLoop()
{
	while (true) {
		watchdog.kick();
		rtos::ThisThread::sleep_for(
			std::chrono::milliseconds(std::chrono::duration<int, std::milli>(timeout_ms - 10000)));
	}
}
