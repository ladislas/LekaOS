#include "DisplayUtils.h"

#include "FileSystemKit.h"

using namespace leka;
using namespace std::chrono_literals;

void DisplayUtils::initializeScreen()
{
	_coresdram.initialize();

	_display.initialize();
	_display.setBrightness(0.F);
	_display.setFrameRateLimit(15);

	_display.clear();

	_video_thread.start(mbed::Callback(this, &DisplayUtils::runVideo));
}

void DisplayUtils::setOn()
{
	setBrightness(_brightness_on_default);
}

void DisplayUtils::setOff()
{
	setBrightness(0.F);
}

void DisplayUtils::setBrightness(float value)
{
	if (value != 0.F) {
		_brightness_on_default = value;
	}
	_display.setBrightness(value);
}

auto DisplayUtils::fileExists(lstd::span<const char> full_path)
{
	FileSystemKit::File file {full_path.data()};

	if (file.is_open()) {
		file.close();
		return true;
	}
	return false;
}

void DisplayUtils::displayImage(std::string filename)
{
	std::string full_path = "/fs/images/" + filename + ".jpg";

	rtos::ThisThread::sleep_for(100ms);
	_event_flags.set(STOP_VIDEO_FLAG);
	rtos::ThisThread::sleep_for(100ms);

	if (fileExists(full_path)) {
		_display.clear();
		rtos::ThisThread::sleep_for(100ms);

		gfx::Image image(full_path.data());

		_display.draw(image);
		_display.display();

		setOn();
	}
}

void DisplayUtils::displayImage(const uint8_t image_table_index)
{
	if (image_table_index < image_table.size()) {
		displayImage(image_table.at(image_table_index));
	}
}

void DisplayUtils::displayVideo(std::string filename)
{
	std::string full_path = "/fs/videos/" + filename + ".avi";

	rtos::ThisThread::sleep_for(100ms);
	_event_flags.set(STOP_VIDEO_FLAG);
	rtos::ThisThread::sleep_for(100ms);

	if (fileExists(full_path)) {
		_full_path_video = "/fs/videos/" + filename + ".avi";
		_event_flags.set(START_VIDEO_FLAG);
	}
}

void DisplayUtils::displayVideo(const uint8_t video_table_index)
{
	if (video_table_index < video_table.size()) {
		displayVideo(video_table.at(video_table_index));
	}
}

void DisplayUtils::runVideo()
{
	auto hasToStopVideo = [&]() { return (_event_flags.get() & STOP_VIDEO_FLAG) == STOP_VIDEO_FLAG; };

	while (true) {
		_event_flags.wait_any(START_VIDEO_FLAG);

		gfx::Video video(_full_path_video.data());

		video.restart();
		_event_flags.clear(STOP_VIDEO_FLAG);
		setOn();
		while (!hasToStopVideo() && !video.hasEnded()) {
			_display.draw(video);
			video.nextFrame();
			_display.display();
		}
		if (!hasToStopVideo()) {
			_event_flags.set(START_VIDEO_FLAG);
		}
		_event_flags.clear(STOP_VIDEO_FLAG);
	}
}
