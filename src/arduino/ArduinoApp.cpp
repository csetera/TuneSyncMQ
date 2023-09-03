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

String ALBUM_ART_UPDATE_MESSAGE = "tunesyncmq/update/albumart";
String STATUS_UPDATE_MESSAGE = "tunesyncmq/update/playbackstatus";

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

/**
 * The loop function of the ArduinoApp class.
 *
 * This function is responsible for executing the main loop of the Arduino application. It calls the loop function of the ESP32Terminal class and the loop function of the networkManager object.
 */
void ArduinoApp::loop() {
  ESP32Terminal::get().loop();
  networkManager.loop();
}

/**
 * Initializes the Arduino application after the LVGL library has been initialized.
 *
 * @throws None
 */
void ArduinoApp::afterLvglInit() {
  ESP32Terminal::get().setup();

  DisplayManager::get().setRefreshDisplayHandler([]() {
    ESP32Terminal::get().loop();
  });

	playbackScreen.createWidgets();
	playbackScreen.onNextClick([&](lv_event_t *event) {
		this->networkManager.publishToMQTT(TUNESYNCMQ_COMMAND, "next");
	});

	playbackScreen.onPlayClick([&](lv_event_t *event) {
		this->networkManager.publishToMQTT(TUNESYNCMQ_COMMAND, "play");
	});

	playbackScreen.onPreviousClick([&](lv_event_t *event) {
		this->networkManager.publishToMQTT(TUNESYNCMQ_COMMAND, "previous");
	});

	DisplayManager::get().setCurrentScreen(&playbackScreen);

	networkManager.onTopicMessageReceived(ALBUM_ART_UPDATE_MESSAGE, [&](char* topic, byte* payload, unsigned int length) {
		this->albumArtUpdated(payload, length);
	});

	networkManager.onTopicMessageReceived(STATUS_UPDATE_MESSAGE, [&](char* topic, byte* payload, unsigned int length) {
		this->playbackStatusUpdated(payload, length);
	});

	networkManager.start();
  Logger::get().println("Setup done");
}

/**
 * @brief Handler for messages that update the album art.
 *
 * @param payload
 * @param length
 */
void ArduinoApp::albumArtUpdated(byte *payload, unsigned int length) {
	Logger::get().println("Album art update message received");
}

/**
 * Initializes the Arduino application before the LVGL library is initialized.
 *
 * @return void
 */
void ArduinoApp::beforeLvglInit()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("Starting setup");

#if LV_USE_LOG
    lv_log_register_print_cb(log_to_serial);
#endif

    char build_timestamp[FORMATTED_BUILD_TIMESTAMP_LENGTH];
    Utils::formatBuildTimestamp(build_timestamp);
    Serial.printf("Starting version built: %s\n", build_timestamp);

    if (!LittleFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
}

/**
 * @brief Handler for messages that update the status of playback.
 *
 * @param payload
 * @param length
 */
void ArduinoApp::playbackStatusUpdated(byte *payload, unsigned int length) {
	Logger::get().println("Playback status update message received");
}
