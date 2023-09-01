/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <defaults.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>

class ESP32Terminal : public lgfx::LGFX_Device
{
public:
  static void flushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
  static void readTouchpad(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

  /**
   * @brief Return the singleton instance.
   *
   * @return Logger&
   */
  static ESP32Terminal &get();

  // Disable copy semantics
  ESP32Terminal(const ESP32Terminal &) = delete;

  virtual void setup();
  virtual void loop();

private:
  static const int BUF_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 5;
  lv_disp_draw_buf_t draw_buf;
  lv_color_t buf[BUF_SIZE];

  lv_disp_drv_t disp_drv;
  lv_indev_drv_t indev_drv;

  lgfx::Panel_ILI9488 panel;
  lgfx::Bus_Parallel16 bus;

  ESP32Terminal();
  void initTouch();
  void instFlushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
  void instReadTouchpad(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
};
