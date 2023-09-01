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
#include "ProgressScreen.h"

typedef std::function<void()> RefreshDisplayHandler;

/**
 * @brief Manages the display
 */
class DisplayManager {
public:
    static DisplayManager& get();

    // Disable copy semantics
    DisplayManager(const DisplayManager&) = delete;

    Screen *completeProgress();

    /**
     * Returns the currently displayed screen.
     *
     * @return the currently displayed screen
     */
    Screen *getCurrentScreen() {
        return _currentScreen;
    }

    /**
     * Returns the ProgressScreen object.
     *
     * @return a pointer to the ProgressScreen object.
     */
    ProgressScreen *getProgressScreen() {
        return _progressScreen;
    }

    bool isProgressDisplayed();

    void refreshDisplay();

    /**
     * Set the current screen to the provided screen.
     *
     * @param screen a pointer to the screen object to set as the current screen
     *
     * @throws None
     */
    void setCurrentScreen(Screen *screen);

    /**
     * Sets the refresh display handler for the object.
     *
     * @param handler the refresh display handler to set
     */
    void setRefreshDisplayHandler(RefreshDisplayHandler handler) {
        _refreshDisplayHandler = handler;
    }

    ProgressScreen *startProgress(bool indeterminate);

private:
    DisplayManager(): _currentScreen(nullptr), _progressScreen(nullptr) {}

    Screen          *_currentScreen;
    ProgressScreen  *_progressScreen;

    RefreshDisplayHandler _refreshDisplayHandler;
};
