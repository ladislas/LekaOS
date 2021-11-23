// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DISPLAY_UTILS_H_
#define _LEKA_OS_DISPLAY_UTILS_H_

#include "rtos/EventFlags.h"
#include "rtos/Thread.h"

#include "CoreSDRAM.hpp"
#include "FATFileSystem.h"
#include "Flags.h"
#include "LKCoreFatFs.h"
#include "LKCoreSTM32Hal.h"
#include "SDBlockDevice.h"
#include "VideoKit.h"

namespace leka {

constexpr std::array<const char *, 51> image_table = {
	"leka-logo",
	"shape-circle-plain_line",
	"shape-square-plain_line",
	"shape-triangle-plain_line",
	"shape-star-plain_line",
	"shape-circle-dotted_line",
	"shape-square-dotted_line",
	"shape-triangle-dotted_line",
	"shape-star-dotted_line",
	"number-0_zero",
	"number-1_one",
	"number-2_two",
	"number-3_three",
	"number-4_four",
	"number-5_five",
	"number-6_six",
	"number-7_seven",
	"number-8_eight",
	"number-9_nine",
	"number-10_ten",
	"color-red",
	"color-blue",
	"color-green",
	"color-yellow",
	"color-black",
	"color-white",
	"activity-magic_objects",
	"activity-dance_freeze",
	"activity-hide_and_seek",
	"activity-music_colored_board",
	"activity-music_quest",
	"activity-color_quest",
	"activity-colors_and_sounds",
	"activity-super_simon",
	"remote-colored_arrows",
	"remote-standard",
	"reinforcer-1-green-spin",
	"reinforcer-2-violet_green_blink-spin",
	"reinforcer-3-fire-static",
	"reinforcer-4-glitters-static",
	"reinforcer-5-rainbow-static",
	"emotion-happiness-child",
	"emotion-sadness-child",
	"emotion-anger-child",
	"emotion-fear-child",
	"emotion-disgust-child",
	"robot-emotion-happy",
	"robot-emotion-sad",
	"robot-emotion-angry",
	"robot-emotion-affraid",
	"robot-emotion-disgusted",
};

constexpr std::array<const char *, 9> emotion_table = {
	"robot-emotion-happy",		 "robot-emotion-angry",		"robot-emotion-affraid",
	"robot-emotion-disgusted",	 "robot-emotion-sad_tears", "robot-emotion-sad",
	"robot-emotion-tired_tears", "robot-emotion-tired",		"robot-emotion-neutral"};

const std::array<std::string, 4> object_colored_family_table = {
	"animal-fish",
	"object-balloon",
	"object-beach_bucket",
	"plant-flower-5_petals",

};

constexpr std::array<const char *, 3> video_table = {"animation-idle", "animation-joy", "animation-perplexity"};

class DisplayUtils
{
  public:
	explicit DisplayUtils(rtos::Thread &video_thread, rtos::EventFlags &event_flags, LKCoreSTM32Hal &hal,
						  CoreSDRAM &sdram, VideoKit &display)
		: _video_thread(video_thread), _event_flags(event_flags), _hal(hal), _coresdram(sdram), _display(display) {};

	void initializeSD();
	void initializeScreen();

	void setOn();
	void setOff();
	void setBrightness(float value);

	auto fileExists(lstd::span<const char> full_path);

	void displayImage(std::string filename);
	void displayImage(uint8_t image_table_index);
	void displayVideo(std::string filename);
	void displayVideo(uint8_t video_table_index);

  private:
	void runVideo();

	rtos::Thread &_video_thread;
	rtos::EventFlags &_event_flags;

	// SDBlockDevice _sd_blockdevice {SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK};
	// FATFileSystem _fatfs {"fs"};
	float _brightness_on_default {1.F};

	LKCoreSTM32Hal &_hal;

	CoreSDRAM &_coresdram;
	VideoKit &_display;

	std::string _full_path_video {};
};

}	// namespace leka

#endif	 // _LEKA_OS_DISPLAY_UTILS_H_
