// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "CoreJPEG.hpp"

#include "internal/corevideo_config.h"

using namespace leka;

CoreJPEG::CoreJPEG(LKCoreSTM32HalBase &hal, interface::JPEGMode &mode) : _hal(hal), _mode(mode)
{
	_handle.Instance = JPEG;
}

void CoreJPEG::initialize()
{
	registerCallbacks();

	JPEG_InitColorTables();
	_hal.HAL_RCC_JPEG_CLK_ENABLE();
	_hal.HAL_JPEG_Init(&_handle);

	// need to be called again because JPEG_Init resets the callbacks
	registerCallbacks();

	// enable JPEG IRQ request
	_hal.HAL_NVIC_SetPriority(JPEG_IRQn, 0x06, 0x0F);
	_hal.HAL_NVIC_EnableIRQ(JPEG_IRQn);
}

auto CoreJPEG::getConfig() -> JPEGConfig
{
	JPEGConfig config;
	_hal.HAL_JPEG_GetInfo(&_handle, &config);

	config.initialized = true;
	return config;
}

void CoreJPEG::registerCallbacks()
{
	static CoreJPEG *self;
	self = this;

	auto info_ready_cb = [](JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *info) {
		self->_mode.onInfoReadyCallback(hjpeg, info);
	};

	auto get_data_cb = [](JPEG_HandleTypeDef *hjpeg, uint32_t decoded_datasize) {
		self->_mode.onGetDataCallback(hjpeg, decoded_datasize);
	};

	auto data_ready_cb = [](JPEG_HandleTypeDef *hjpeg, uint8_t *output_data, uint32_t datasize) {
		self->_mode.onDataReadyCallback(hjpeg, output_data, datasize);
	};

	auto decode_cmplt_cb = [](JPEG_HandleTypeDef *hjpeg) { self->_mode.onDecodeCompleteCallback(hjpeg); };

	auto error_cb = [](JPEG_HandleTypeDef *hjpeg) { self->_mode.onErrorCallback(hjpeg); };

	auto mspinit_cb = [](JPEG_HandleTypeDef *hjpeg) { self->_mode.onMspInitCallback(hjpeg); };

	_hal.HAL_JPEG_RegisterInfoReadyCallback(&_handle, info_ready_cb);
	_hal.HAL_JPEG_RegisterGetDataCallback(&_handle, get_data_cb);
	_hal.HAL_JPEG_RegisterDataReadyCallback(&_handle, data_ready_cb);
	_hal.HAL_JPEG_RegisterCallback(&_handle, HAL_JPEG_DECODE_CPLT_CB_ID, decode_cmplt_cb);
	_hal.HAL_JPEG_RegisterCallback(&_handle, HAL_JPEG_ERROR_CB_ID, error_cb);
	_hal.HAL_JPEG_RegisterCallback(&_handle, HAL_JPEG_MSPINIT_CB_ID, mspinit_cb);
}

auto CoreJPEG::decodeImage(interface::File &file) -> std::uint32_t
{
	return _mode.decodeImage(&_handle, file);
}

auto CoreJPEG::findFrameOffset(interface::File &file, uint32_t offset) -> uint32_t
{
	std::array<uint8_t, 512> pattern_search_buffer;

	size_t file_size   = file.size();
	uint32_t index	   = offset;
	uint32_t read_size = 0;

	do {
		if (file_size <= (index + 1)) {
			return 0;
		}
		file.seek(index);
		read_size = file.read(pattern_search_buffer.data(), pattern_search_buffer.size());

		if (read_size != 0) {
			for (uint32_t i = 0; i < (read_size - 1); i++) {
				if ((pattern_search_buffer[i] == jpeg::JPEG_SOI_MARKER_BYTE1) &&
					(pattern_search_buffer[i + 1] == jpeg::JPEG_SOI_MARKER_BYTE0)) {
					return index + i;
				}
			}
			index += (read_size - 1);
		}
	} while (read_size != 0);

	return 0;
}

auto JPEGConfig::getWidthOffset() -> uint32_t
{
	uint32_t width_offset = 0;

	switch (ChromaSubsampling) {
		case JPEG_420_SUBSAMPLING:
			if ((ImageWidth % 16) != 0) {
				width_offset = 16 - (ImageWidth % 16);
			}
			break;
		case JPEG_422_SUBSAMPLING:
			if ((ImageWidth % 16) != 0) {
				width_offset = 16 - (ImageWidth % 16);
			}
			break;
		case JPEG_444_SUBSAMPLING:
			if ((ImageWidth % 8) != 0) {
				width_offset = (ImageWidth % 8);
			}
			break;
		default:
			width_offset = 0;
			break;
	}

	return width_offset;
}
