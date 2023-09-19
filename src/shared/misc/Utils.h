/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#define FORMATTED_BUILD_TIMESTAMP_LENGTH 22

namespace Utils {
    /**
     * @brief Format the build timestamp into the provided buffer.
     *
     * @param buffer
     */
    void formatBuildTimestamp(char* buffer);

		uint32_t reverseByteOrder(uint32_t value);
}
