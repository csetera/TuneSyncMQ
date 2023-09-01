/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "NetworkUtils.h"

#include <HTTPClient.h>
#include <arduino/logging/Logger.h>

#ifdef ESP32
// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

/**
 * @brief Return a WIFiClient taking into account whether the
 * URL is http or https.
 *
 * @param url
 * @return WiFiClient*
 */
WiFiClient* getWifiClient(String& url) {
    if (url.startsWith("https:")) {
        WiFiClientSecure* client = new WiFiClientSecure();

        // Disable certificate validation
        client->setInsecure();

        return client;
    } else {
        return new WiFiClient();
    }
}

/**
 * @brief Retrieve a payload from the specified URL.
 *
 * @param url
 * @param receiver
 */
void NetworkUtils::httpGet(String& url, StreamReceiver receiver) {
    WiFiClient* client = getWifiClient(url);

    // Set up the REST client to make it easy to handle JSON
    HTTPClient http;
    http.useHTTP10(true);

    // Set up the REST call
    http.begin(*client, url);

    // Make the REST call
    Logger::get().println("Executing GET request");
    int httpResponseCode = http.GET();
    Logger::get().printf("GET request result code: %d\n", httpResponseCode);

    receiver(httpResponseCode, http.getStream());

    http.end();
    client->stop();
    delete(client);
}
