#include "Controller.h"

#include "LogKit.h"

void Controller::init()
{
	thread.start(callback(&event_queue, &events::EventQueue::dispatch_forever));

	auto on_button_pressed = [this]() { event_queue.call(this, &Controller::onButtonPressed); };

	button.rise(on_button_pressed);
}

void Controller::onButtonPressed()
{
	_fsm.process_event(system_event::button_pressed {});
}

// void Controller::shutdown()
// {
// 	_fsm.process_event(system_event::timeout_5s {});
// }
