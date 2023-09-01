/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#include <defaults.h>
#include <arduino/logging/Logger.h>
#include <arduino/settings/SettingsManager.h>
#include <shared/misc/Utils.h>
#include <shared/ui/DisplayManager.h>

#ifdef ESP32
// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html
#include <WiFi.h>
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "NetworkManager.h"

/**
 * @brief A AsyncWebHandler implementation that handles CORS preflight
 * requests.
 */
class CORSPreflightHandler : public AsyncWebHandler {
    virtual bool canHandle(AsyncWebServerRequest *request) {
        return (request->method() == HTTP_OPTIONS) && request->url().startsWith("/api");
    }

    virtual void handleRequest(AsyncWebServerRequest *request) {
        request->send(200);
    }
} corsPreflightHandler;

/**
 * @brief Incoming websocket event handler
 *
 * @param server
 * @param client
 * @param type
 * @param arg
 * @param data
 * @param len
 */
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.println("WS_EVT_CONNECT");
            break;

        case WS_EVT_DISCONNECT:
            Serial.println("WS_EVT_DISCONNECT");
            break;

        case WS_EVT_PONG:
            Serial.println("WS_EVT_PONG");
            break;

        case WS_EVT_ERROR:
            Serial.println("WS_EVT_ERROR");
            break;

        case WS_EVT_DATA:
            Serial.println("WS_EVT_DATA");
            break;
    }
}

NetworkManager::NetworkManager() : webServer(80), wsSerial("/ws_serial"), state(BEFORE_START) {
    // CORS headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Authorization, Content-Type");
    DefaultHeaders::Instance().addHeader("Access-Control-Max-Age", "86400");
    DefaultHeaders::Instance().addHeader("Vary", "Accept-Encoding, Origin");

    // Handler implementation for receiving settings updates and
    // passing them on to the SettingsManager.
    updateSettingsHandler = new AsyncCallbackJsonWebHandler("/api/settings", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("Received Settings update request");
        JsonObject jsonObj = json.as<JsonObject>();
        SettingsManager::get().updateSettings(jsonObj);
        this->getSettings(request);
    });
}

/**
 * Check if the network is connected.
 *
 * @return true if the network is connected, false otherwise.
 */
bool NetworkManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Start up configure the networking stack
 */
void NetworkManager::start() {
    // Calculate a unique hostname
    auto hostname = String("conductor-");
    hostname.concat(String((unsigned long)ESP.getEfuseMac(), HEX));
    WiFi.setHostname(hostname.c_str());

    WiFi.mode(WIFI_STA);
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        this->onWifiEvent(event, info);
    });

    configuredViaSmartConfig = false;
    registerHandlers();
    configureOTAUpdates();

    // Communicate with the user
    DisplayManager::get().startProgress(true);

    if (!startWifi()) {
        startSmartConfig();
    }

    webServer.begin();
}

/**
 * @brief Handle Espressif Smartconfig to configure SSID and password.
 * See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_smartconfig.html
 * for more information.
 */
void NetworkManager::startSmartConfig() {
    Serial.println("Starting SmartConfig");
    updateProgress(0, "Waiting for SmartConfig...");

    state = SMART_CONFIG_WAIT;
    WiFi.beginSmartConfig();
}

/**
 * Starts the WiFi connection.
 *
 * @return true if the WiFi connection is successfully started, false otherwise.
 */
bool NetworkManager::startWifi() {
    SettingsManager &settingsManager = SettingsManager::get();
    settingsManager.readSettings();

    const char *ssid = settingsManager.getWifiSSID();
    const char *psk = settingsManager.getWifiPassword();

    if ((ssid != nullptr) && (psk != nullptr)) {
        Serial.printf("Settings were intialized SSID: %s; Pass: %s.  Starting WIFI\n", ssid, psk);
        updateProgress(0, "Waiting for WiFi...");
        WiFi.begin(ssid, psk);

        state = CONNECTION_WAIT;
        beginWaitMillis = millis();

        return true;
    } else {
        return false;
    }
}

/**
 * @brief Handle the recurring loop.
 */
void NetworkManager::loop() {
    switch (state) {
        case SMART_CONFIG_WAIT:
            if (WiFi.smartConfigDone()) {
                Serial.println("SmartConfig received.");
                updateProgress(0, "Waiting for WiFi...");

                configuredViaSmartConfig = true;
                state = CONNECTION_WAIT;
                beginWaitMillis = millis();
            }

            break;

        case CONNECTION_WAIT:
            if (isConnected()) {
                Serial.println("\nWiFi Connected.");
                Serial.print("IP Address: ");
                Serial.println(WiFi.localIP());

                if (configuredViaSmartConfig) {
                    configuredViaSmartConfig = false;

                    Serial.printf("Smartconfig value: %s/%s\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
                    SettingsManager &settingsManager = SettingsManager::get();
                    settingsManager.setWifiPassword(WiFi.psk().c_str());
                    settingsManager.setWifiSSID(WiFi.SSID().c_str());
                    Serial.println("Writing out settings");
                    settingsManager.writeSettings();
                }

                DisplayManager::get().completeProgress();
                state = CONNECTED;
            } else {
                unsigned long connectionAttemptMillis = millis() - beginWaitMillis;
                if (connectionAttemptMillis > maxConnectionWaitMillis) {
                    Serial.println();
                    Serial.println("WiFi did not connect, starting SmartConfig");
                    startSmartConfig();
                }
            }
            break;

        case CONNECTED:
            ArduinoOTA.handle();
            break;

        default:
            break;
    }
}

/**
 * @brief Handle an info requerst
 *
 * @param request
 */
void NetworkManager::getInfo(AsyncWebServerRequest *request, bool featuresOnly) {
    Logger::get().println("Received info request");

    char build_timestamp[FORMATTED_BUILD_TIMESTAMP_LENGTH];
    Utils::formatBuildTimestamp(build_timestamp);

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
    JsonObject obj = root.to<JsonObject>();

    JsonArray features = obj.createNestedArray("Features");
    #ifdef ENABLE_REMOTE_VIEWER
        features.add("RemoteView");
    #endif

    if (!featuresOnly) {
        JsonObject general = obj.createNestedObject("General");
        general["Build"] = build_timestamp;
        general["IpAddr"] = WiFi.localIP();
        general["SdkVersion"] = ESP.getSdkVersion();

        JsonObject sketch = obj.createNestedObject("Sketch");
        sketch["Size"] = ESP.getSketchSize();
        sketch["FreeSpace"] = ESP.getFreeSketchSpace();

        JsonObject heap = obj.createNestedObject("Heap");
        heap["Size"] = ESP.getHeapSize(); //total heap size
        heap["Free"] = ESP.getFreeHeap(); //available heap
        heap["MinFree"] = ESP.getMinFreeHeap(); //lowest level of free heap since boot
        heap["MaxAlloc"] = ESP.getMaxAllocHeap(); //largest block of heap that can be allocated at once

        JsonObject psram = obj.createNestedObject("Psram");
        psram["Size"] = ESP.getPsramSize();
        psram["Free"] = ESP.getFreePsram();
        psram["MinFree"] = ESP.getMinFreePsram();
        psram["MaxAlloc"] = ESP.getMaxAllocPsram();

        JsonObject chip = obj.createNestedObject("Chip");
        chip["Model"] = ESP.getChipModel();
        chip["Revision"] = ESP.getChipRevision();
        chip["Cores"] = ESP.getChipCores();

        JsonObject flash = obj.createNestedObject("Flash");
        flash["ChipSpeed"] = ESP.getFlashChipSpeed();
        flash["ChipMode"] = ESP.getFlashChipMode();
    }

    response->setLength();
    request->send(response);
}

/**
 * @brief Handle a request for a network scan for available
 * wireless networks.
 *
 * @param request The incoming reuquest information.
 */
void getNetworks(AsyncWebServerRequest *request) {
    Logger::get().println("Received networks request");
    int n = WiFi.scanNetworks();

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
    JsonArray array = root.to<JsonArray>();

    for (int i = 0; i < n; i++) {
        JsonObject obj = array.createNestedObject();
        obj["CHANNEL"] = WiFi.channel(i);
        obj["ENC_TYPE"] = WiFi.encryptionType(i);
        obj["RSSI"] = WiFi.RSSI(i);
        obj["SSID"] = WiFi.SSID(i);
    }

    response->setLength();
    request->send(response);

    WiFi.scanDelete();
}

/**
 * @brief Handle a request for current settings.
 *
 * @param request The incoming reuquest information.
 */
void NetworkManager::getSettings(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
    SettingsManager::get().sendSettingsResponse(root);

    response->setLength();
    request->send(response);
}

/**
 * @brief Handler for Wifi connected events
 */
void NetworkManager::onWifiConnected() {
    Serial.println("onWifiConnected");

    Logger &logger = Logger::get();
    logger.publishTo(&wsSerial);

    if (!MDNS.begin(MDNS_NAME)) {
        Serial.println("Error setting up MDNS responder!");
        delay(1000);
    }

    MDNS.addService("http", "tcp", 80);
    logger.println("mDNS responder started");

    ArduinoOTA.begin();
}

/**
 * @brief Handler for Wifi disconnected events
 */
void NetworkManager::onWifiDisconnected() {
    Serial.println("onWifiDisconnected");

    Logger &logger = Logger::get();
    logger.publishTo(nullptr);

    ArduinoOTA.end();
    MDNS.end();
}

/**
 * @brief Handler for Wifi events
 *
 * @param event
 * @param info
 */
void NetworkManager::onWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            onWifiConnected();
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            onWifiDisconnected();
            break;

        default:
            Logger::get().printf("Received Wifi Event: %d\n", event);
            break;
    }
}

/**
 * @brief Configure the web server
 */
void NetworkManager::registerHandlers() {
    Serial.println("Registering web handlers");

    // CORS handling
    webServer.addHandler(&corsPreflightHandler);

    // API endpoints
    webServer.on("/api/features", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Logger::get().println("Received get features request");
        this->getInfo(request, true);
    });
    webServer.on("/api/info", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Logger::get().println("Received get info request");
        this->getInfo(request);
    });
    webServer.on("/api/networks", getNetworks);

    // Web application serving
    webServer.rewrite("/", "/index.html");
    webServer.rewrite("/index.htm", "/index.html");
    webServer.serveStatic("/", LittleFS, "/webapp/");

    // Serial over Websocket handling
    wsSerial.onEvent(onWsEvent);
    webServer.addHandler(&wsSerial);

    #ifdef ENABLE_REMOTE_VIEWER
        webServer.addHandler(&wsRemote);
        DisplayManager::get().setRemoteViewerSocket(&wsRemote);
    #endif

    // Catch all
    webServer.onNotFound([](AsyncWebServerRequest *request){
        request->send(404);
    });
}

/**
 * @brief Setup for OTA updates
 */
void NetworkManager::configureOTAUpdates() {
    Logger::get().println("Configuring OTA Updates");

    ArduinoOTA.setHostname(MDNS_NAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([this]() {
        lastLoggedOtaPercentage = -1.0;
        const char *message = (ArduinoOTA.getCommand() == 0) ?
            "OTA firmware update starting" :
            "OTA filesystem update starting";

        Logger::get().println(message);
        DisplayManager::get().startProgress(false);
        updateProgress(0, message);
        DisplayManager::get().refreshDisplay();
    });

    ArduinoOTA.onEnd([]() {
        Logger::get().println("OTA Complete");
        DisplayManager::get().completeProgress();
        DisplayManager::get().refreshDisplay();
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        char buffer[30];

        uint percent_complete = (uint) ((double) progress / ((double) total / 100));
        if ((percent_complete % 10) == 0) {
            if (percent_complete > lastLoggedOtaPercentage) {
                lastLoggedOtaPercentage = percent_complete;
                sprintf(buffer, "OTA Progress: %u%%", percent_complete);
                Logger::get().println(buffer);
                updateProgress(percent_complete, buffer);
                DisplayManager::get().refreshDisplay();
            }
        }
    });

    ArduinoOTA.onError([](ota_error_t error) {
        switch (error) {
            case OTA_AUTH_ERROR:
                Logger::get().println("OTA - Auth failed\n");
                break;

            case OTA_BEGIN_ERROR:
                Logger::get().println("OTA - Begin failed");
                break;

            case OTA_CONNECT_ERROR:
                Logger::get().println("OTA - Connect failed");
                break;

            case OTA_RECEIVE_ERROR:
                Logger::get().println("OTA - Receive failed");
                break;

            case OTA_END_ERROR:
                Logger::get().println("OTA - End failed");
                break;

            default:
                Logger::get().printf("OTA - Unknown OTA error: %u\n", error);
        }
    });

    Logger::get().println("OTA configured");
}


/**
 * Updates the progress displayed on the screen.
 *
 * @param message the message to be displayed
 */
void NetworkManager::updateProgress(int percent, const char *message) {
    DisplayManager &displayManager = DisplayManager::get();
    if (displayManager.isProgressDisplayed()) {
        displayManager.getProgressScreen()->setMessage(message);
        displayManager.getProgressScreen()->setProgress(percent);
    }
};
