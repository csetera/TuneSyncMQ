/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include <Arduino.h>
#include <LittleFS.h>

#include <arduino/ESP32Terminal/ESP32Terminal.h>
#include <arduino/logging/Logger.h>
#include <arduino/settings/SettingsManager.h>
#include <shared/ui/DisplayManager.h>
#include <shared/misc/Utils.h>

#include "ArduinoApp.h"

#if LV_USE_LOG
/**
 * @brief Simple logging callback for LVGL logging to Serial output.
 *
 * @param buf
 */
void log_to_serial(const char *buf) {
  Serial.print(buf);
}
#endif

void ArduinoApp::loop() {
  ESP32Terminal::get().loop();
  networkManager.loop();
}

void ArduinoApp::afterLvglInit() {
  ESP32Terminal::get().setup();

  DisplayManager::get().setRefreshDisplayHandler([]() {
    ESP32Terminal::get().loop();
  });

	playbackScreen.createWidgets();
	playbackScreen.onNextClick([&](lv_event_t *event) {
		Logger::get().println("Next clicked");
	});

	playbackScreen.onPlayClick([&](lv_event_t *event) {
		Logger::get().println("Play clicked");
	});

	playbackScreen.onPreviousClick([&](lv_event_t *event) {
		Logger::get().println("Previous clicked");
	});

	DisplayManager::get().setCurrentScreen(&playbackScreen);

	networkManager.start();
  Logger::get().println("Setup done");
}

void ArduinoApp::beforeLvglInit() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting setup");

  #if LV_USE_LOG
  lv_log_register_print_cb(log_to_serial);
  #endif

  char build_timestamp[FORMATTED_BUILD_TIMESTAMP_LENGTH];
  Utils::formatBuildTimestamp(build_timestamp);
  Serial.printf("Starting version built: %s\n", build_timestamp);

  if (!LittleFS.begin(true)) {
      Serial.println("An Error has occurred while mounting LittleFS");
      return;
  }
}
