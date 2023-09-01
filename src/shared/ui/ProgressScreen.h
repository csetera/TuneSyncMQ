/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <functional>
#include "Screen.h"

class ProgressScreen : public Screen {
public:
    ProgressScreen(bool indeterminate) : Screen(), indeterminate(indeterminate) {}

    /**
     * Determines whether the value is indeterminate.
     *
     * @return true if the value is indeterminate, false otherwise
     */
    bool isIndeterminate() {
        return indeterminate;
    }

    void setMessage(const char *message);
    void setProgress(int progress);

protected:
    virtual void createScreenWidgets(lv_obj_t *parent);

private:
    lv_obj_t    *messageLabel;
    bool        indeterminate;
    int         progress;
    lv_obj_t    *progressSlider;
    lv_obj_t    *progressSpinner;
};
