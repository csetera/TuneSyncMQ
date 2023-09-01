/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "DisplayManager.h"
#include <lvgl.h>

DisplayManager &DisplayManager::get() {
    static DisplayManager singleton;
    return singleton;
}

/**
 * Set the current screen to the provided screen.
 *
 * @param screen a pointer to the screen object to set as the current screen
 *
 * @throws None
 */
Screen *DisplayManager::completeProgress() {
    if (_currentScreen != nullptr) {
        lv_scr_load(_currentScreen->getLvglObject());
    }

    if (_progressScreen != nullptr) {
        delete _progressScreen;
        _progressScreen = nullptr;
    }

    return _currentScreen;
}

/**
 * Check if the progress screen is currently being displayed.
 *
 * @return true if the progress screen is displayed, false otherwise
 */
bool DisplayManager::isProgressDisplayed() {
    return _progressScreen != nullptr;
}

/**
 * This function refreshes the display.
 *
 * @throws ErrorType description of error
 */
void DisplayManager::refreshDisplay()
{
    if (_refreshDisplayHandler != nullptr) {
        _refreshDisplayHandler();
    }
}

/**
 * Sets the current screen for the DisplayManager.
 *
 * @param screen a pointer to the Screen object to set as the current screen
 */
void DisplayManager::setCurrentScreen(Screen *screen) {
    _currentScreen = screen;

    // If progress is being displayed, don't overwrite it... just save it for later
    if (_progressScreen == nullptr) {
        lv_scr_load(screen->getLvglObject());
    }
}

/**
 * Creates and starts a progress screen.
 *
 * @param indeterminate if true, the progress screen is indeterminate
 *
 * @return a pointer to the created ProgressScreen object
 */
ProgressScreen *DisplayManager::startProgress(bool indeterminate) {
    if (_progressScreen != nullptr) {
        delete _progressScreen;
        _progressScreen = nullptr;
    }

    _progressScreen = new ProgressScreen(indeterminate);
    _progressScreen->createWidgets();

    lv_scr_load(_progressScreen->getLvglObject());

    return _progressScreen;
}

