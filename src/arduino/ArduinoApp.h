/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <arduino/network/NetworkManager.h>
#include <shared/AppCommon.h>
#include <shared/ui/PlaybackScreen.h>

class ArduinoApp : public AppCommon {
public:
    ArduinoApp(): AppCommon() {}
    virtual void loop();

private:
    NetworkManager networkManager;
		PlaybackScreen playbackScreen;

		virtual void beforeLvglInit();
    virtual void afterLvglInit();
};
