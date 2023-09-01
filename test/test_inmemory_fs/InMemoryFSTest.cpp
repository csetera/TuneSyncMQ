/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "unity.h"
#include <InMemoryFS.h>
#include <filesystem>

static std::filesystem::path CWD = std::filesystem::current_path();
static std::filesystem::path TEST_ASSETS_FOLDER = CWD / "test/assets";

static uint8_t *getAssetData(const char *relativePath, uint32_t *bytesRead) {
  auto fullPath = TEST_ASSETS_FOLDER / relativePath;

  *bytesRead = std::filesystem::file_size(fullPath);
	uint8_t *fileData = new uint8_t[*bytesRead];
	auto file = std::fopen(fullPath.c_str(), "rb");
	std::fread(fileData, 1, *bytesRead, file);
	fclose(file);

  return fileData;
}

static void loadFileIntoFileSystem(const char *relativePath) {
  uint32_t bytesRead;
  uint8_t *data = getAssetData(relativePath, &bytesRead);
  InMemoryFS::registerFile(relativePath, data, bytesRead);
  delete [] data;
}

void setUp() {
  lv_init();
	InMemoryFS::registerInMemoryDriver();

  loadFileIntoFileSystem("binary_test.bin");
}

void tearDown() {
}

void test_asset_retrieval()  {
  uint32_t bytesRead;
  uint8_t *data = getAssetData("binary_test.bin", &bytesRead);
  TEST_ASSERT_EQUAL_UINT32(2560, bytesRead);
  TEST_ASSERT_NOT_NULL(data);
  delete [] data;
}

void test_filesystem_open() {
  lv_fs_file_t file;
  lv_fs_res_t result = lv_fs_open(&file, "M:binary_test.bin", LV_FS_MODE_RD);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_close(&file);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
}

void test_filesystem_read() {
  lv_fs_file_t file;
  lv_fs_res_t result = lv_fs_open(&file, "M:binary_test.bin", LV_FS_MODE_RD);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  uint8_t buffer[256];
  uint32_t bytesRead;
  for (int i = 0; i < 10; i++) {
    result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
    TEST_ASSERT_EQUAL(sizeof(buffer), bytesRead);

    for (int j = 0; j < bytesRead; j++) {
      TEST_ASSERT_EQUAL(buffer[j], j);
    }
  }

  result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(0, bytesRead);

  result = lv_fs_close(&file);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
}

void test_seek_and_read() {
  lv_fs_file_t file;
  uint8_t buffer[256];
  uint32_t bytesRead;

  lv_fs_res_t result = lv_fs_open(&file, "M:binary_test.bin", LV_FS_MODE_RD);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_seek(&file, 0, LV_FS_SEEK_END);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  uint32_t pos;
  result = lv_fs_tell(&file, &pos);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(2560, pos);

  result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(0, bytesRead);

  result = lv_fs_seek(&file, 5, LV_FS_SEEK_END);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_tell(&file, &pos);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(2565, pos);

  result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(0, bytesRead);

  result = lv_fs_seek(&file, -5, LV_FS_SEEK_END);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_tell(&file, &pos);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(2555, pos);

  result = lv_fs_seek(&file, 5, LV_FS_SEEK_SET);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_tell(&file, &pos);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(5, pos);

  result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(256, bytesRead);
  TEST_ASSERT_EQUAL(5, buffer[0]);

  result = lv_fs_seek(&file, 5, LV_FS_SEEK_CUR);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);

  result = lv_fs_tell(&file, &pos);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(266, pos);

  result = lv_fs_read(&file, buffer, sizeof(buffer), &bytesRead);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
  TEST_ASSERT_EQUAL(256, bytesRead);
  TEST_ASSERT_EQUAL(10, buffer[0]);

  result = lv_fs_close(&file);
  TEST_ASSERT_EQUAL(LV_FS_RES_OK, result);
}

int runUnityTests(void) {
  UNITY_BEGIN();

  RUN_TEST(test_asset_retrieval);
  RUN_TEST(test_filesystem_open);
  RUN_TEST(test_filesystem_read);
  RUN_TEST(test_seek_and_read);

  return UNITY_END();
}

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) {
  return runUnityTests();
}
