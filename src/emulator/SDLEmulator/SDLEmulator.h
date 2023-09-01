/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <defaults.h>
#include <lvgl.h>
#include <sdl.h>

class SDLEmulator {
public:
  /**
   * @brief Return the singleton instance.
   *
   * @return SDLEmulator&
   */
  static SDLEmulator &get();

  // Disable copy semantics
  SDLEmulator(const SDLEmulator &) = delete;

  virtual void setup();
  virtual void loop();

private:
    lv_disp_draw_buf_t disp_buf;
    lv_color_t buf[SDL_HOR_RES * 10];                          /*Declare a buffer for 10 lines*/

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;

    SDLEmulator() {}
};
