/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "EmulatorApp.h"

#include <filesystem>
#include <emulator/SDLEmulator/SDLEmulator.h>
#include <InMemoryFS.h>
#include <shared/ui/DisplayManager.h>

#if LV_USE_LOG == 1
#include <stdlib.h>
#include <fstream>
#include <string>

/**
 * @brief Simple logging callback for LVGL logging to a file.
 *
 * @param buf
 */
void lvgl_log_to_file(const char *buf) {
  std::ofstream outfile;

  std::string filename(getenv("HOME"));
  filename += "/lvgl.log";

  outfile.open(filename, std::ios_base::app); // append instead of overwrite
  outfile << buf;

  outfile.close();
}
#endif

static const char *FILENAME = "ajr.png";
static const char *FILENAME_WITH_DRIVE = "M:ajr.png";

void testCoverImage(PlaybackScreen *playbackScreen) {
	auto cwd = std::filesystem::current_path();
	auto testImagesFolder = cwd / "test/assets/images";
	auto imagePath = testImagesFolder / FILENAME;

	auto fileSize = std::filesystem::file_size(imagePath);
	uint8_t *imageData = new uint8_t[fileSize];
	auto file = std::fopen(imagePath.c_str(), "rb");
	std::fread(imageData, 1, fileSize, file);
	InMemoryFS::registerFile(FILENAME, imageData, fileSize);
	fclose(file);

	playbackScreen->setCoverImage(FILENAME_WITH_DRIVE);
	delete [] imageData;
}

void EmulatorApp::afterLvglInit() {
  SDLEmulator::get().setup();

  DisplayManager::get().setRefreshDisplayHandler([]() {
    lv_task_handler();
  });

	playbackScreen.createWidgets();
	playbackScreen.onPlayClick([&](lv_event_t *event) {
		testCoverImage(&this->playbackScreen);
		this->playbackScreen.setArtist("AJR");
		this->playbackScreen.setTitle("Bang!");
	});

	DisplayManager::get().setCurrentScreen(&playbackScreen);
}

void EmulatorApp::beforeLvglInit() {
  #if LV_USE_LOG == 1
  lv_log_register_print_cb(lvgl_log_to_file);
  #endif
}

void EmulatorApp::loop() {
  SDLEmulator::get().loop();
}
