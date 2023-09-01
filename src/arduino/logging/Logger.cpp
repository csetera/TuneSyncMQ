/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "Logger.h"

#define BUFFER_LENGTH 256

Logger& Logger::get() {
    static Logger singleton;
    return singleton;
}

Logger::Logger() {
    buffer.reserve(BUFFER_LENGTH);
}

/**
 * @brief Set the target WebSocket
 *
 * @param webSocket
 */
void Logger::publishTo(AsyncWebSocket* publishWebSocket) {
    Serial.printf("publishing to: %p\n", publishWebSocket);
    webSocket = publishWebSocket;
}

/**
 * @brief Handle the character being written.
 *
 * @param c
 * @return size_t
 */
size_t Logger::write(uint8_t c) {
    if (c == '\n') {
        Serial.println(buffer);
        if (webSocket && webSocket->enabled() && (webSocket->count() > 0)) {
            webSocket->textAll(buffer);
        }

        buffer.remove(0);
    } else {
        buffer.concat((char) c);
    }

    return 1;
}
