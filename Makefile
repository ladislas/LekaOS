# Leka - LekaOS
# Copyright 2020 APF France handicap
# SPDX-License-Identifier: Apache-2.0

#
# MARK: - Constants
#

ROOT_DIR             := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CMAKE_DIR            := $(ROOT_DIR)/cmake
MBED_OS_DIR          := $(ROOT_DIR)/extern/mbed-os
PROJECT_BUILD_DIR    := $(ROOT_DIR)/build

#
# MARK:- Arguments
#

PORT         ?= /dev/tty.usbmodem14303
BRANCH       ?= master
TARGET       ?=
VERSION      ?= mbed-os-6.3.0
BAUDRATE     ?= 115200
BIN_PATH     ?= $(PROJECT_BUILD_DIR)/src/LekaOS.bin
BUILD_TYPE   ?= Release
TARGET_BOARD ?= -x LEKA_V1_0_DEV

#
# MARK:- Build targets
#

all:
	@echo ""
	@echo "🏗️  Building everything! 🌈"
	cmake --build $(PROJECT_BUILD_DIR)

os:
	@echo ""
	@echo "🏗️  Building LekaOS 🤖"
	cmake --build $(PROJECT_BUILD_DIR) -t LekaOS

lekaos:
	@echo ""
	@echo "🏗️  Building spikes 🍱"
	cmake --build $(PROJECT_BUILD_DIR) -t spikes

tests_functional:
	@echo ""
	@echo "🏗️  Building functional tests ⚗️"
	cmake --build $(PROJECT_BUILD_DIR) -t tests_functional

#
# MARK:- Config targets
#

config_leka_disco:
	@$(MAKE) config TARGET_BOARD="-x LEKA_DISCO"

config:
	@$(MAKE) deep_clean
	@echo ""
	@$(MAKE) config_target
	@echo ""
	@$(MAKE) config_cmake

config_target:
	@echo ""
	@echo "🏃 Running target configuration script 📝"
	python3 $(CMAKE_DIR)/scripts/configure_cmake_for_target.py $(TARGET_BOARD) -p $(CMAKE_DIR)/config -a $(ROOT_DIR)/mbed_app.json

config_cmake: mkdir_build
	@echo ""
	@echo "🏃 Running cmake configuration script 📝"
	@cd $(PROJECT_BUILD_DIR); cmake -GNinja -DTARGET_BOARD="$(TARGET_BOARD)" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

#
# MARK:- Tools targets
#

clang_format:
	@echo ""
	@echo "🕵️ Running clang-format 🔍"
	python3 tools/run-clang-format.py -r --extension=h,c,cpp --color=always --style=file ./src ./drivers ./libs ./spikes ./tests

clang_format_fix:
	@echo ""
	@echo "🕵️ Running clang-format & fixing files ♻️"
	python3 tools/run-clang-format.py -r -i --extension=h,c,cpp --color=always --style=file ./src ./drivers ./libs ./spikes ./tests

#
# MARK:- Mbed targets
#

clone_mbed:
	@echo ""
	@echo "🧬 Cloning Mbed OS ⚗️"
	@rm -rf $(MBED_OS_DIR)
	git clone --depth=1 --branch=$(BRANCH) https://github.com/ARMmbed/mbed-os $(MBED_OS_DIR)
	@echo ""
	@echo "🔗 Symlinking templates to Mbed OS directory 🗂️"
	ln -srf $(CMAKE_DIR)/templates/Template_MbedOS_CMakelists.txt $(MBED_OS_DIR)/CMakeLists.txt
	ln -srf $(CMAKE_DIR)/templates/Template_MbedOS_mbedignore.txt $(MBED_OS_DIR)/.mbedignore

curl_mbed:
	@echo ""
	@echo "🧬 Curling Mbed OS ⚗️"
	@rm -rf $(MBED_OS_DIR)
	@mkdir -p $(MBED_OS_DIR)
	curl -O -L https://github.com/ARMmbed/mbed-os/archive/$(VERSION).tar.gz
	tar -xzf $(VERSION).tar.gz --strip-components=1 -C extern/mbed-os
	rm -rf $(VERSION).tar.gz
	@echo ""
	@echo "🔗 Symlinking templates to Mbed OS directory 🗂️"
	ln -srf $(CMAKE_DIR)/templates/Template_MbedOS_CMakelists.txt $(MBED_OS_DIR)/CMakeLists.txt
	ln -srf $(CMAKE_DIR)/templates/Template_MbedOS_mbedignore.txt $(MBED_OS_DIR)/.mbedignore

#
# MARK:- Utils targets
#

mkdir_build:
	@mkdir -p $(PROJECT_BUILD_DIR)

clean:
	@echo ""
	@echo "⚠️  Cleaning up build directories 🧹"
	rm -rf $(PROJECT_BUILD_DIR)

deep_clean:
	@$(MAKE) clean
	@echo ""
	@echo "⚠️  Cleaning up cmake/config directories 🧹"
	rm -rf $(CMAKE_DIR)/config

flash:
	openocd -f interface/stlink.cfg -c 'transport select hla_swd' -f target/stm32f7x.cfg -c 'program $(BIN_PATH) 0x08000000' -c exit
	sleep 1
	@$(MAKE) reset

reset:
	openocd -f interface/stlink.cfg -c 'transport select hla_swd' -f target/stm32f7x.cfg -c init -c 'reset run' -c exit

term:
	mbed sterm -b $(BAUDRATE) -p $(PORT)
