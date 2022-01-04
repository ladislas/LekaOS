#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string>

#include "ControllerBase.h"
#include "LogKit.h"
#include "boost/sml.hpp"

namespace system_event {

struct button_pressed {
	static inline const std::string name = {"Event: button pressed"};
	static inline auto action			 = []() { log_debug("%s", name.c_str()); };
};

struct timeout_5s {
	static inline const std::string name = {"Event: timeout 5s"};
	static inline auto action			 = []() { log_debug("%s", name.c_str()); };
};

}	// namespace system_event

struct StateMachine {
	auto operator()() const noexcept
	{
		using namespace boost::sml;
		// clang-format off
        return make_transition_table(
                 * "Idle"_s      +  event<system_event::button_pressed>  / system_event::button_pressed::action  =  "Blinking"_s
                ,  "Blinking"_s  +  event<system_event::timeout_5s>      / system_event::timeout_5s::action      =  "Idle"_s
        );
		// clang-format on
	}
};

#endif	 // STATE_MACHINE_H
