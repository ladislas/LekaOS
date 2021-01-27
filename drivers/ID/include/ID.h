// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_ID_H_
#define _LEKA_OS_DRIVER_ID_H_

#include <stdint.h>

#define MMIO32(addr) (*(volatile uint32_t *)(addr))
#define U_ID_REG	 0x1ff0f420

namespace leka {

typedef struct {
	uint32_t front;
	uint32_t middle;
	uint32_t back;
} mcu_id;	// ID is 96 bits length

mcu_id get_id();

}	// namespace leka

#endif	 // _LEKA_OS_DRIVER_ID_H_
