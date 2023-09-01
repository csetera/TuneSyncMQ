/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <functional>
#include <ArduinoJson.h>

/**
 * @brief A function to receive the result of an HTTP operation.
 *
 */
typedef std::function<void(int httpResponse, Stream &responseStream)> StreamReceiver;

/**
 * @brief Static networking utility functions
 */
namespace NetworkUtils {
    /**
     * @brief Retrieve a payload from the specified URL.
     *
     * @param url
     * @param receiver
     */
    void httpGet(String& url, StreamReceiver receiver);
};
