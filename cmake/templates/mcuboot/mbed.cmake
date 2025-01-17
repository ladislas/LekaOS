# vim: set filetype=cmake:
# Leka
# Copyright 2021 APF France handicap
# SPDX-License-Identifier: Apache-2.0

target_compile_definitions(bootutil
	PUBLIC
		MCUBOOT_BOOTLOADER_SIZE=0x40000

		MCUBOOT_SCRATCH_SIZE=0x40000
		MCUBOOT_SCRATCH_START_ADDR=0x081C0000

		MCUBOOT_CRYPTO_BACKEND=MBEDTLS
		MCUBOOT_SIGNATURE_ALGORITHM=SIGNATURE_TYPE_RSA
		MCUBOOT_RSA_SIGNATURE_LENGTH=2048

		MCUBOOT_LOG_BOOTLOADER_ONLY=true
		MCUBOOT_LOG_LEVEL=MCUBOOT_LOG_LEVEL_DEBUG

		MCUBOOT_PRIMARY_SLOT_START_ADDR=0x08040000
		MCUBOOT_SLOT_SIZE=0x200000-MCUBOOT_BOOTLOADER_SIZE-MCUBOOT_SCRATCH_SIZE
		MCUBOOT_VALIDATE_PRIMARY_SLOT=true

		MCUBOOT_READ_GRANULARITY=1
		MCUBOOT_MAX_IMG_SECTORS=512
)

target_include_directories(bootutil
	PUBLIC
		./
		include
		include/flash_map_backend
		include/mcuboot_config
		include/os
		include/sysflash
)

target_sources(bootutil
	PRIVATE
		src/flash_map_backend.cpp
		src/secondary_bd.cpp
		app_enc_keys.c
		mcuboot_main.cpp
)

target_link_libraries(bootutil
	mbed-os
)
