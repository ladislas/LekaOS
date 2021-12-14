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
	 "animation-face-action-yawning", "animation-face-state-affraid", "animation-face-action-wink"});
}	// namespace leka

#endif	 // _LEKA_OS_ASSETS_H_
