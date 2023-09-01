#include "InMemoryFS.h"

#include <string>
#include <unordered_map>

class Buffer {
public:
	Buffer() : data(nullptr), size(0) {}
	Buffer(const Buffer& b) : data(b.data), size(b.size) {}

	uint8_t *data;
	uint32_t size;
};

class BufferCursor {
public:
	BufferCursor(): bufferOffset(0), buffer(nullptr) {}
	BufferCursor(const BufferCursor& b) : bufferOffset(b.bufferOffset), buffer(b.buffer) {}

	uint32_t bufferOffset;
	Buffer *buffer;
};

static bool initialized = false;
static lv_fs_drv_t drv;

static std::unordered_map<std::string, Buffer*> buffersByName;

/**
 * Close an opened file
 *
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 *
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
lv_fs_res_t mem_fs_close(struct _lv_fs_drv_t *drv, void *file_p) {
	BufferCursor *cursor = (BufferCursor *) file_p;
	delete cursor;
	return LV_FS_RES_OK;
}

/**
 * Open a file
 *
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 *
 * @return          a file descriptor or NULL on error
 */
void *mem_fs_open(struct _lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
	BufferCursor *cursor = nullptr;

	if (mode == LV_FS_MODE_RD) {
		std::string key = path;
		Buffer *buffer = buffersByName[key];

		cursor = new BufferCursor();
		cursor->buffer = buffer;
	}

	return cursor;
}

/**
 * Read data from an opened file
 *
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 *
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
lv_fs_res_t mem_fs_read(struct _lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
	*br = 0;

	BufferCursor *cursor = (BufferCursor *) file_p;
	if (cursor->bufferOffset < cursor->buffer->size) {
		uint32_t bytesAvailable = cursor->buffer->size - cursor->bufferOffset;
		if (bytesAvailable > 0) {
			*br = (btr < bytesAvailable) ? btr : bytesAvailable;

			if (*br > 0) {
				memcpy(buf, cursor->buffer->data + cursor->bufferOffset, *br);
				cursor->bufferOffset += *br;
			}
		}
	}

	return LV_FS_RES_OK;
}

/**
 * Checks if the memory filesystem is ready.
 *
 * @param drv pointer to the _lv_fs_drv_t structure
 *
 * @return true if the memory filesystem is ready, false otherwise
 */
bool mem_fs_ready(struct _lv_fs_drv_t *drv) {
	return true;
}

/**
 * Set the read pointer.
 *
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 *
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t mem_fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
	BufferCursor *cursor = (BufferCursor *) file_p;

	switch (whence) {
		case LV_FS_SEEK_SET:
			cursor->bufferOffset = pos;
			break;
		case LV_FS_SEEK_CUR:
			cursor->bufferOffset = cursor->bufferOffset + pos;
			break;
		case LV_FS_SEEK_END:
			cursor->bufferOffset = cursor->buffer->size + pos;
			break;
	}

	return LV_FS_RES_OK;
}

/**
 * Return the position of the read pointer
 *
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 *
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t mem_fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
	BufferCursor *cursor = (BufferCursor *)file_p;
	*pos_p = cursor->bufferOffset;

	return LV_FS_RES_OK;
}

/**
 * Adds file data to the InMemoryFS.
 *
 * @param path The path of the file.
 * @param data A pointer to the file data.
 * @param size The size of the file data.
 */
void InMemoryFS::registerFile(const char *path, void *data, uint32_t size) {
	std::string key = path;

	Buffer *buffer = new Buffer();
	buffer->data = (uint8_t* ) malloc(size);
	buffer->size = size;

	memcpy(buffer->data, data, size);
	buffersByName.insert({ key, buffer });
}

/**
 * Registers the in-memory driver for the InMemoryFS.
 *
 * @return A pointer to the registered file system driver.
 */
lv_fs_drv_t *InMemoryFS::registerInMemoryDriver() {
	if (!initialized) {
		lv_fs_drv_init(&drv);

		drv.letter = InMemoryFS::DRIVE_LETTER;
		drv.cache_size = 0;

		drv.ready_cb = mem_fs_ready;
		drv.open_cb = mem_fs_open;
		drv.close_cb = mem_fs_close;
		drv.read_cb = mem_fs_read;
		drv.write_cb = nullptr;
		drv.seek_cb = mem_fs_seek;
		drv.tell_cb = mem_fs_tell;

		drv.dir_open_cb = nullptr;
		drv.dir_read_cb = nullptr;
		drv.dir_close_cb = nullptr;

		drv.user_data = nullptr;

		lv_fs_drv_register(&drv);
		initialized = true;
	}

  return &drv;
}
