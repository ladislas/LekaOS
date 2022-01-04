# Leka - LekaOS
# Copyright 2021 APF France handicap
# SPDX-License-Identifier: Apache-2.0

add_mbed_executable(boost_sml_simple)

target_include_directories(boost_sml_simple
	PRIVATE
		.
		include
)

target_sources(boost_sml_simple
	PRIVATE
		main.cpp
		src/Controller.cpp
)

target_link_custom_leka_targets(boost_sml_simple)
