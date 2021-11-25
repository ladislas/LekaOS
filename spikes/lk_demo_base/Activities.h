// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_ACTIVITIES_H_
#define _LEKA_OS_ACTIVITIES_H_

#include "rtos/EventFlags.h"

#include "DisplayUtils.h"
#include "LedsUtils.h"
#include "RFIDUtils.h"

namespace leka {

void displayTags(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils, LedsUtils &leds_utils,
				 RFIDUtils &rfid_utils);

void activityRecognitionColor(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
							  LedsUtils &leds_utils, RFIDUtils &rfid_utils);

void activityRecognitionEmotions(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
								 LedsUtils &leds_utils, RFIDUtils &rfid_utils);

void activityColorLeka2(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
						LedsUtils &leds_utils, RFIDUtils &rfid_utils);

void activityColorLeka3(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
						LedsUtils &leds_utils, RFIDUtils &rfid_utils);

void rainbowAndJoy(rtos::EventFlags &event_flags_external_interaction, DisplayUtils &display_utils,
				   LedsUtils &leds_utils, RFIDUtils &rfid_utils);

}	// namespace leka

#endif	 // _LEKA_OS_ACTIVITIES_H_
