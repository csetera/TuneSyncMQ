/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <shared/AppCommon.h>
#include <shared/ui/PlaybackScreen.h>

class EmulatorApp : public AppCommon {
public:
	EmulatorApp(): AppCommon() {}
	virtual void loop();

protected:
	virtual void afterLvglInit();
	virtual void beforeLvglInit();

	PlaybackScreen playbackScreen;
};
