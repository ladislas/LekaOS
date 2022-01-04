#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "drivers/BufferedSerial.h"
#include "drivers/DigitalOut.h"
#include "drivers/InterruptIn.h"
#include "drivers/LowPowerTimeout.h"
#include "events/EventQueue.h"
#include "rtos/Kernel.h"
#include "rtos/ThisThread.h"
#include "rtos/Thread.h"

#include "ControllerBase.h"
#include "StateMachine.h"

class Controller
{
  public:
	explicit Controller(boost::sml::sm<StateMachine> &fsm) : _fsm(fsm) {}

	void onButtonPressed();

	void init();

  private:
	boost::sml::sm<StateMachine> &_fsm;

	mbed::InterruptIn button {USER_BUTTON};
	mbed::DigitalOut led1 {LED1};
	mbed::DigitalOut led2 {LED2};

	rtos::Thread thread {osPriorityLow};
	events::EventQueue event_queue {32 * EVENTS_EVENT_SIZE};

	mbed::LowPowerTimeout timeout {};
};

#endif	 // CONTROLLER_H
