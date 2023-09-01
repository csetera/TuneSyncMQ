/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "AppCommon.h"
#include <InMemoryFS.h>

void AppCommon::lvglInit() {
  // Setup the graphics and hardware abstraction
  lv_init();
  lv_theme_default_init(
    lv_disp_get_default(),
    lv_palette_main(LV_PALETTE_DEEP_PURPLE),
    lv_palette_main(LV_PALETTE_BLUE),
    true, LV_FONT_DEFAULT);

	InMemoryFS::registerInMemoryDriver();
}

void AppCommon::setup() {
  beforeLvglInit();
  lvglInit();
  afterLvglInit();
}
