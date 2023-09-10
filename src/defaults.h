/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

// Communication related
#ifndef MDNS_NAME
  #define MDNS_NAME "tunesyncmq"
#endif

#ifndef MQTT_BUFFER_SIZE
	#define MQTT_BUFFER_SIZE 32768
#endif

// Hardware related
#ifndef LCD_BL
    #define LCD_BL 46
#endif

#ifndef SDA_FT6236
    #define SDA_FT6236 38
#endif

#ifndef SCL_FT6236
    #define SCL_FT6236 39
#endif

#ifndef SCREEN_HEIGHT
    #define SCREEN_HEIGHT 320
#endif

#ifndef SCREEN_WIDTH
    #define SCREEN_WIDTH 480
#endif

// Application related
#ifndef COVERIMAGE_HEIGHT
	#define COVERIMAGE_HEIGHT 160
#endif

#ifndef COVERIMAGE_WIDTH
	#define COVERIMAGE_WIDTH 160
#endif
