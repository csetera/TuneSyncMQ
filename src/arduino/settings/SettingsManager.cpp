/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "SettingsManager.h"

#include <LittleFS.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <arduino/logging/Logger.h>
#include <shared/misc/Utils.h>

SettingsManager& SettingsManager::get() {
    static SettingsManager singleton;
    return singleton;
}

SettingsManager::SettingsManager() {
}

/**
 * @brief Read all settings in from file system.
 *
 * @return true
 * @return false
 */
bool SettingsManager::readSettings() {
    bool exists = LittleFS.exists(SETTINGS_FILE);
    if (exists) {
        StaticJsonDocument<256> doc;

        // Read in the settings JSON file
        File file = LittleFS.open(SETTINGS_FILE, "r", false);
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        // Check for deserialization errors
        if (error.code() != DeserializationError::Code::Ok) {
            const char *errorString = error.c_str();
            Logger::get().printf("Failed to read file (%s), using default configuration\n", errorString);
            return false;
        }

        // Destructure the setting values
        if (doc.containsKey(WIFI_SETTINGS)) {
            setWifiSSID(doc[WIFI_SETTINGS][WIFI_SSID_KEY]);
            setWifiPassword(doc[WIFI_SETTINGS][WIFI_PASS_KEY]);
        }
    }

    return exists;
}

/**
 * @brief Clear all settings from EEPROM.
 */
void SettingsManager::reset() {
    if (wifiPassword != nullptr) {
        free(wifiPassword);
        wifiPassword = nullptr;
    }

    if (wifiSSID != nullptr) {
        free(wifiSSID);
        wifiSSID = nullptr;
    }

    LittleFS.remove(SETTINGS_FILE);
}

/**
 * @brief Add a new setting definition to the specified category array.
 *
 * @param array
 * @param name
 * @param type
 * @param value
 */
void addDefinition(JsonArray &array, const char *name, const char *type, const char *value) {
    auto def = array.createNestedObject();
    def["Name"] = name;
    def["Type"] = type;
    def["Value"] = value;
}

/**
 * @brief Send the settings JSON reponse.
 *
 * @param root
 */
void SettingsManager::sendSettingsResponse(JsonVariant &root) {
    JsonObject obj = root.to<JsonObject>();

    if ((wifiSSID != nullptr) || (wifiPassword != nullptr)) {
        auto wifiSettings = obj.createNestedArray(WIFI_SETTINGS);
        addDefinition(wifiSettings, "SSID", "string", wifiSSID);
        addDefinition(wifiSettings, "Password", "password", wifiPassword);
    }
}

/**
 * @brief Set the Wifi password
 *
 * @param value
 */
void SettingsManager::setWifiPassword(const char *value) {
    if (wifiPassword != nullptr) {
        free(wifiPassword);
    }

    wifiPassword = (char *) malloc(MAX_WIFI_PASS_LENGTH);
    strlcpy(wifiPassword, value, MAX_WIFI_PASS_LENGTH);
}

/**
 * @brief Set the Wifi SSID
 *
 * @param value
 */
void SettingsManager::setWifiSSID(const char *value) {
    if (wifiSSID != nullptr) {
        free(wifiSSID);
    }

    wifiSSID = (char *) malloc(MAX_WIFI_SSID_LENGTH);
    strlcpy(wifiSSID, value, MAX_WIFI_SSID_LENGTH);
}

/**
 * @brief Update the settings based on the specified JSON object.
 *
 * @param obj
 */
void SettingsManager::updateSettings(JsonObject &obj) {
    bool updated = false;

    if (obj.containsKey(WIFI_SETTINGS)) {
        JsonArray settings = obj[WIFI_SETTINGS];
        updated = updateWifiSettings(settings);
    }

    if (updated) {
        writeSettings();
    }
}

/**
 * @brief Update the wifi settings based on the JsonArray
 * information.
 *
 * @param settings
 */
bool SettingsManager::updateWifiSettings(JsonArray &settings) {
    bool updated = false;

    for (int i = 0; i < settings.size(); i++) {
        auto obj = settings[i];
        String name = obj["Name"];

        if (name == "SSID") {
            String value = obj["Value"];
            setWifiSSID(value.c_str());
            updated = true;
        } else if (name == "Password") {
            String value = obj["Value"];
            setWifiPassword(value.c_str());
            updated = true;
        }
    }

    return updated;
}

/**
 * @brief Write out the current settings to the file system
 */
void SettingsManager::writeSettings() {
    StaticJsonDocument<256> doc;

    if ((wifiSSID != nullptr) || (wifiPassword != nullptr)) {
        auto wifi = doc.createNestedObject(WIFI_SETTINGS);
        wifi[WIFI_SSID_KEY] = wifiSSID;
        wifi[WIFI_PASS_KEY] = wifiPassword;
    }

    File file = LittleFS.open(SETTINGS_FILE, "w", true);
    serializeJson(doc, file);
    file.flush();
    file.close();
}
