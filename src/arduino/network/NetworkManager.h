/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <Esp.h>
#define __ESP32__
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#define __ESP__
#endif

#include <AsyncJson.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <ustd_map.h>

typedef std::function<void(char* topic, byte* payload, unsigned int length)> MQTTMessageHandler;

/**
 * @brief Separate the network management functionality
 */
class NetworkManager {
public:
    NetworkManager();

    /**
     * @brief Start up the network.
     */
    void start();

    bool isConnected();

    /**
     * @brief Handle a loop call as necessary
     */
    void loop();

    /**
     * @brief Get the Web Server object
     *
     * @return AsyncWebServer&
     */
    AsyncWebServer& getWebServer() {
        return webServer;
    }

		void publishToMQTT(const char *topic, const char *message);

		void onTopicMessageReceived(String topic, MQTTMessageHandler handler);

private:
    static const long maxConnectionWaitMillis = 30 * 1000;

    enum NetworkState {
        BEFORE_START,
        SMART_CONFIG_WAIT,
        CONNECTION_WAIT,
        CONNECTED,
        DISCONNECTED
    };

		// Base functioality support
		String                          hostname;
    NetworkState                    state;
    unsigned long                   beginWaitMillis;
    bool                            configuredViaSmartConfig;
    AsyncCallbackJsonWebHandler*    updateSettingsHandler;
    AsyncWebServer                  webServer;
    AsyncWebSocket                  wsSerial;
		WiFiClient 											wifiClient;

    void getInfo(AsyncWebServerRequest *request, bool featuresOnly = false);
    void getSettings(AsyncWebServerRequest *request);
    void onWifiConnected();
    void onWifiDisconnected();
    void onWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void registerHandlers();
    void startSmartConfig();
    bool startWifi();
    void updateProgress(int percent, const char *message);

		// MQTT support
		long 														lastMqttReconnectAttempt;
    PubSubClient 										mqttClient;
		ustd::map<String, MQTTMessageHandler> 	mqttMessageHandlers;

    void mqttMessageCallback(char* topic, byte* payload, unsigned int length);
    boolean mqttReconnect();

		// OTA support
    double                          lastLoggedOtaPercentage;

    void configureOTAUpdates();
};
