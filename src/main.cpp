/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "defaults.h"
#include <lvgl.h>
#include <shared/ui/DisplayManager.h>
#include <shared/ui/PlaybackScreen.h>
#include <shared/ui/ProgressScreen.h>
#include <cstdio>

#ifdef USE_SDL
  #include <emulator/EmulatorApp.h>
  EmulatorApp app;
#else
  #include <arduino/ArduinoApp.h>
  ArduinoApp app;
#endif

/**
 * @brief Standard Arduino infinite loop
 */
void loop() {
  app.loop();
}

/**
 * @brief Standard Arduino setup function
 */
void setup() {
  app.setup();
}

#ifdef USE_SDL
/**
 * @brief Main method to match behavior of Arduino
 *
 * @return int
 */
int main(void)
{
	setup();
  while (1) {
  	loop();
  }
}
#endif
