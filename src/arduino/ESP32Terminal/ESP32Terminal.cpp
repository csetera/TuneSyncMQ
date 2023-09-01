/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "ESP32Terminal.h"

#include <FT6236.h>
#include <Wire.h>

/**
 * @brief Delegate the flushing of the display buffer to the singleton instance
 *
 * @param disp
 * @param area
 * @param color_p
 */
void ESP32Terminal::flushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  ESP32Terminal::get().instFlushDisplay(disp, area, color_p);
}

/**
 * @brief Delegate the reading of the touchpad to the singleton instance
 *
 * @param indev_driver
 * @param data
 */
void ESP32Terminal::readTouchpad(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  ESP32Terminal::get().instReadTouchpad(indev_driver, data);
}

/**
 * @brief Return the singleton instance.
 *
 * @return ESP32Terminal&
 */
ESP32Terminal &ESP32Terminal::get()
{
  static ESP32Terminal singleton;
  return singleton;
}

ESP32Terminal::ESP32Terminal() {
  {
    auto cfg = bus.config();

    cfg.port = 0;
    cfg.freq_write = 80000000;
    cfg.pin_wr = 18;
    cfg.pin_rd = 48;
    cfg.pin_rs = 45;

    cfg.pin_d0 = 47;
    cfg.pin_d1 = 21;
    cfg.pin_d2 = 14;
    cfg.pin_d3 = 13;
    cfg.pin_d4 = 12;
    cfg.pin_d5 = 11;
    cfg.pin_d6 = 10;
    cfg.pin_d7 = 9;
    cfg.pin_d8 = 3;
    cfg.pin_d9 = 8;
    cfg.pin_d10 = 16;
    cfg.pin_d11 = 15;
    cfg.pin_d12 = 7;
    cfg.pin_d13 = 6;
    cfg.pin_d14 = 5;
    cfg.pin_d15 = 4;

    bus.config(cfg);
    panel.setBus(&bus);
  }

  {
    auto cfg = panel.config();

    cfg.pin_cs = -1;
    cfg.pin_rst = -1;
    cfg.pin_busy = -1;
    cfg.memory_width = SCREEN_HEIGHT;
    cfg.memory_height = SCREEN_WIDTH;
    cfg.panel_width = SCREEN_HEIGHT;
    cfg.panel_height = SCREEN_WIDTH;
    cfg.offset_x = 0;
    cfg.offset_y = 0;
    cfg.offset_rotation = 2;
    cfg.dummy_read_pixel = 8;
    cfg.dummy_read_bits = 1;
    cfg.readable = true;
    cfg.invert = false;
    cfg.rgb_order = false;
    cfg.dlen_16bit = true;
    cfg.bus_shared = true;

    panel.config(cfg);
  }

  setPanel(&panel);
}

/**
 * @brief Initialize the touch panel
 *
 */
void ESP32Terminal::initTouch() {
  byte error, address;

  // I2C init
  Wire.begin(SDA_FT6236, SCL_FT6236);
  Wire.beginTransmission(TOUCH_I2C_ADD);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("I2C device found at address 0x");
    Serial.print(TOUCH_I2C_ADD, HEX);
    Serial.println("  !");
  } else if (error == 4) {
    Serial.print("Unknown error at address 0x");
    Serial.println(TOUCH_I2C_ADD, HEX);
  }
}

/**
 * @brief Flush the buffer to the display
 *
 * @param disp
 * @param area
 * @param color_p
 */
void ESP32Terminal::instFlushDisplay(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  startWrite();
  setAddrWindow(area->x1, area->y1, w, h);
  writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  endWrite();

  lv_disp_flush_ready(disp);
}

/**
 * @brief Read the current state of the touchpad
 *
 * @param indev_driver
 * @param data
 */
void ESP32Terminal::instReadTouchpad(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  int pos[2] = {0, 0};

  ft6236_pos(pos);
  if (pos[0] > 0 && pos[1] > 0)
  {
    data->state = LV_INDEV_STATE_PR;
//    data->point.x = tft.width()-pos[1];
//    data->point.y = pos[0];
    data->point.x = width()-pos[1];
    data->point.y = pos[0];
    // Serial.printf("x-%d,y-%d\n", data->point.x, data->point.y);
  }
  else {
    data->state = LV_INDEV_STATE_REL;
  }
}

/**
 * @brief Handle a loop call
 */
void ESP32Terminal::loop() {
  lv_timer_handler(); // let the GUI do its work
  delay(5);
}

/**
 * @brief Set up the ESP32Terminal LVGL implementation
 */
void ESP32Terminal::setup() {
  begin();
  setRotation(1); /* Landscape orientation, flipped */
  fillScreen(TFT_BLACK);

  delay(500);

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  initTouch();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, BUF_SIZE);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = &ESP32Terminal::flushDisplay;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = &ESP32Terminal::readTouchpad;
  lv_indev_drv_register(&indev_drv);
}
