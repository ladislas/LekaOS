// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "CoreDMA2D.hpp"

#include "LogKit.h"
#include "internal/corevideo_config.h"

using namespace leka;

CoreDMA2D::CoreDMA2D(LKCoreSTM32HalBase &hal) : _hal(hal)
{
	// MARK: Configure DMA2D mode, color mode and output offset
	_handle.Init.Mode		   = DMA2D_M2M_PFC;
	_handle.Init.ColorMode	   = DMA2D_OUTPUT_ARGB8888;
	_handle.Init.OutputOffset  = 0;
	_handle.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;	// No Output Alpha Inversion
	_handle.Init.RedBlueSwap   = DMA2D_RB_REGULAR;		// No Output Red & Blue swap

	// MARK: Configure DMA2D background
	_handle.LayerCfg[0].AlphaMode	   = DMA2D_REPLACE_ALPHA;
	_handle.LayerCfg[0].InputAlpha	   = 0xFF;
	_handle.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;

	_handle.LayerCfg[0].InputOffset	  = 0;
	_handle.LayerCfg[0].RedBlueSwap	  = DMA2D_RB_REGULAR;	   // No Background Red/Blue swap
	_handle.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;   // No Background Alpha inversion

	// MARK: Configure DMA2D foreground
	_handle.LayerCfg[1].AlphaMode	   = DMA2D_REPLACE_ALPHA;
	_handle.LayerCfg[1].InputAlpha	   = 0xFF;
	_handle.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;

	_handle.LayerCfg[1].InputOffset	  = 0;
	_handle.LayerCfg[1].RedBlueSwap	  = DMA2D_RB_REGULAR;	   // No ForeGround Red/Blue swap
	_handle.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;   // No ForeGround Alpha inversion

	// MARK: Set DMA2D instance
	_handle.Instance = DMA2D;
}

void CoreDMA2D::initialize()
{
	// MARK: Initializer DMA2D
	// This part **must not** be moved to the constructor as LCD
	// initialization must be performed in a very specific order
	_hal.HAL_DMA2D_Init(&_handle);
	_hal.HAL_DMA2D_ConfigLayer(&_handle, 0);
	_hal.HAL_DMA2D_ConfigLayer(&_handle, 1);
}

void CoreDMA2D::setFrameBufferAddress(uintptr_t address)
{
	_frame_buffer_address = address;
}

void CoreDMA2D::transferData(uintptr_t input, uintptr_t output, uint32_t width, uint32_t height)
{
	if (_hal.HAL_DMA2D_Init(&_handle) != HAL_OK) {
		log_error("DMA2D Init error");
		return;
	}
	if (_hal.HAL_DMA2D_ConfigLayer(&_handle, 1) != HAL_OK) {
		log_error("DMA2D config layer error");
		return;
	}
	if (HAL_DMA2D_Start_IT(&_handle, input, output, width, height) != HAL_OK) {
		log_error("DMA2D Start IT error");
	}
}

void CoreDMA2D::transferImage(uint32_t width, uint32_t height, uint32_t width_offset)
{
	_handle.Init.Mode				= DMA2D_M2M_PFC;
	_handle.LayerCfg[1].InputOffset = width_offset;
	_handle.Init.OutputOffset		= lcd::dimension.width - width;	  // TODO(@yann): Check if needed

	transferData(jpeg::decoded_buffer_address, _frame_buffer_address, width, height);
}

void CoreDMA2D::transferDrawing(uintptr_t first_pixel_address, uint32_t width, uint32_t height, uint32_t color)
{
	_handle.Init.Mode		  = DMA2D_R2M;
	_handle.Init.OutputOffset = lcd::dimension.width - width;

	transferData(color, first_pixel_address, width, height);
}
