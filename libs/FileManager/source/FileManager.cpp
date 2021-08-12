// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "FileManager.h"

namespace leka {

using namespace mbed;

FileManager::FileManager() : _bd(SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK), _fs("fs")
{
	_bd.init();
	_bd.frequency(25'000'000);

	_fs.mount(&_bd);
}

bool FileManager::open(const char *filename)
{
	bool file_opened = false;

	if (f_open(&_file, filename, FA_READ) == FR_OK) {
		file_opened = true;
	}

	return file_opened;
}

bool FileManager::close()
{
	bool file_closed = false;

	if (f_close(&_file) == FR_OK) {
		file_closed = true;
	}

	return file_closed;
}

bool FileManager::write(const char *data, const uint32_t size)
{
	UINT uint_size = static_cast<UINT>(size);
	if (f_write(&_file, data, uint_size, nullptr) == FR_OK) {
		return true;
	}
	return false;
}

uint32_t FileManager::getFileSize()
{
	uint32_t file_size = 0;

	if (_file.obj.fs != nullptr) {
		file_size = f_size(&_file);
	}

	return file_size;
}

void FileManager::testWorkingToRemove()
{
	DIR *dir = opendir("/fs");

	if (dir != nullptr) {
		struct dirent *p;
		printf("At root of SD card:\n");
		while ((p = readdir(dir)) != nullptr) {
			printf(" - %s\n", p->d_name);
		}
	}
	else {
		printf("Could not open directory!\n");
	}
	closedir(dir);

	return;
}

}	// namespace leka
