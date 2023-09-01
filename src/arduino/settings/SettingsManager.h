/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <cstddef>
#include <ArduinoJson.h>

class SettingsManager {
public:
    /**
     * @brief Return the singleton instance of the logger.
     *
     * @return Logger&
     */
    static SettingsManager& get();

    // Disable copy semantics
    SettingsManager(const SettingsManager&) = delete;

    /**
     * @brief Get the Wifi SSID
     *
     * @return const char*
     */
    const char *getWifiSSID() {
        return wifiSSID;
    }

    /**
     * @brief Get the Wifi Password
     *
     * @return const char*
     */
    const char *getWifiPassword() {
        return wifiPassword;
    }

    /**
     * @brief Read all settings in from file system.
     *
     * @return true
     * @return false
     */
    bool readSettings();

    /**
     * @brief Clear all settings.
     */
    void reset();

    /**
     * @brief Send the settings JSON reponse.
     *
     * @param root
     */
    void sendSettingsResponse(JsonVariant &root);

    /**
     * @brief Set the Wifi SSID
     *
     * @param value
     */
    void setWifiSSID(const char *value);

    /**
     * @brief Set the Wifi password
     *
     * @param value
     */
    void setWifiPassword(const char *value);

    /**
     * @brief Update the settings based on the specified JSON object.
     *
     * @param obj
     */
    void updateSettings(JsonObject &obj);

    /**
     * @brief Write out the current settings to the file system
     */
    void writeSettings();

private:
    static const size_t MAX_WIFI_SSID_LENGTH = 33;
    static const size_t MAX_WIFI_PASS_LENGTH = 65;

    static constexpr const char *WIFI_SETTINGS      = "WiFi";
    static constexpr const char *WIFI_SSID_KEY      = "ssid";
    static constexpr const char *WIFI_PASS_KEY      = "pass";

    static constexpr const char *SETTINGS_FILE = "/settings.json";

    SettingsManager();

    char *wifiSSID;
    char *wifiPassword;

    /**
     * @brief Update the wifi settings based on the JsonArray
     * information.
     *
     * @param settings
     */
    bool updateWifiSettings(JsonArray &settings);
};
