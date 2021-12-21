// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_ASSETS_H_
#define _LEKA_OS_ASSETS_H_

#include <array>
#include <lstd_array>
#include <string>

namespace leka {

// constexpr auto video_table =
// 	lstd::to_array<const char *>({"animation-idle", "animation-joy", "animation-perplexity",
// 								  "animation-face-state-waiting", "animation-face-action-wink"});

constexpr auto video_table = lstd::to_array<const char *>(
	{"animation-face-state-waiting", "animation-face-state-sad", "animation-face-state-disgusted",
	 "animation-face-state-happy", "animation-face-state-amazed", "animation-face-state-angry",
	 "animation-face-action-yawning", "animation-face-state-affraid", "animation-face-action-wink",

	 "2021_12_21-animation-face-state-sad-without-eyebrows", "2021_12_21-animation-face-state-disgusted",
	 "2021_12_21-animation-face-state-happy", "2021_12_21-animation-face-state-angry",
	 "2021_12_21-animation-face-action-yawning-sleeping", "2021_12_21-animation-face-action-winking"});
}	// namespace leka

#endif	 // _LEKA_OS_ASSETS_H_
