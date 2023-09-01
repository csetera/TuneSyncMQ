#pragma once

#include <lvgl.h>

namespace InMemoryFS {
	static char DRIVE_LETTER = 'M';

	lv_fs_drv_t *registerInMemoryDriver();
	void registerFile(const char *path, void *buffer, uint32_t size);
}
