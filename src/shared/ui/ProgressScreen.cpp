/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "ProgressScreen.h"

/**
 * Builds the waiting screen.
 *
 * @param parent pointer to the parent object
 */
void ProgressScreen::createScreenWidgets(lv_obj_t *parent) {
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(parent, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    messageLabel = lv_label_create(parent);
    lv_label_set_text(messageLabel, "Loading...");
    lv_obj_set_style_text_font(messageLabel, &lv_font_montserrat_18, LV_PART_MAIN);

    if (indeterminate) {
        progressSpinner = lv_spinner_create(parent, 1000, 60);
        lv_obj_set_size(progressSpinner, 150, 150);
        lv_obj_center(progressSpinner);
    } else {
        progressSlider = lv_slider_create(parent);
        lv_obj_set_width(progressSlider, lv_pct(80));
        lv_slider_set_value(progressSlider, 0, LV_ANIM_OFF);
        lv_obj_clear_flag(progressSlider, LV_OBJ_FLAG_CLICKABLE);
    }
}

/**
 * Sets the message on the waiting screen.
 *
 * @param message The message to be set.
 */
void ProgressScreen::setMessage(const char *message) {
    lv_label_set_text(messageLabel, message);
}

/**
 * Sets the progress value of the ProgressScreen.
 *
 * @param progress the progress value to set
 */
void ProgressScreen::setProgress(int progress) {
    if (!indeterminate) {
        if (progress < 0) {
            this->progress = 0;
        } else if (progress > 100) {
            this->progress = 100;
        } else {
            this->progress = progress;
        }

        lv_slider_set_value(progressSlider, progress, LV_ANIM_ON);
    }
}
