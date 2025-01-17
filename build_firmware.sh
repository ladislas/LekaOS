#!/bin/bash

mkdir -p _tmp
mkdir -p _release

# Variables
RECOMPILE_BOOTLOADER="false"
while getopts r flag
do
    case "${flag}" in
        r) RECOMPILE_BOOTLOADER="true";;
    esac
done

BOOTLOADER_HEX="_tmp/bootloader.hex"

APPLICATION_HEX_SOURCE="_build/LEKA_V1_2_DEV/spikes/lk_update_process_app_base/spike_lk_update_process_app_base.hex"
APPLICATION_VERSION="1.2.3+4"
APPLICATION_SIGNED_HEX="_tmp/application-signed.hex"

FIRMWARE_HEX="_tmp/firmware.hex"
FIRMWARE_RELEASE="_release/firmware.bin"

# Get bootloader
if [ "$RECOMPILE_BOOTLOADER" = "true" ];
then
	echo "Build bootloader"
	./build_bootloader.sh
fi;

# Get application
echo "Build application"
./build_os.sh $RECOMPILE_BOOTLOADER $APPLICATION_HEX_SOURCE $APPLICATION_VERSION $APPLICATION_SIGNED_HEX

# Merge bootloader and application
echo "Merge bootloader & applications"
hexmerge.py -o $FIRMWARE_HEX --no-start-addr $BOOTLOADER_HEX $APPLICATION_SIGNED_HEX
arm-none-eabi-objcopy -I ihex -O binary $FIRMWARE_HEX $FIRMWARE_RELEASE
