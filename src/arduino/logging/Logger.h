/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Print.h>

/**
 * @brief A Print object for handling logging to
 * Serial and is also capable to send printed
 * information to a WebSocket as well.
 */
class Logger : public Print {
public:
    /**
     * @brief Return the singleton instance of the logger.
     *
     * @return Logger&
     */
    static Logger& get();

    // Disable copy semantics
    Logger(const Logger&) = delete;

    /**
     * @brief Set the target WebSocket
     *
     * @param webSocket
     */
    void publishTo(AsyncWebSocket* webSocket);

    /**
     * @brief Handle the character being written.
     *
     * @param c
     * @return size_t
     */
    virtual size_t write(uint8_t c) override;

private:
    String buffer;
    AsyncWebSocket* webSocket;

    /**
     * @brief Construct a new Logger object
     */
    Logger();
};
