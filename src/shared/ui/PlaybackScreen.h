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

class PlaybackScreen : public Screen {
public:
	PlaybackScreen() : Screen() {}

	virtual void createScreenWidgets(lv_obj_t *parent);

	void onPlayClick(EventHandler eventHandler);
	void onNextClick(EventHandler eventHandler);
	void onPreviousClick(EventHandler eventHandler);

	void setArtist(const char *artist);
	void setCoverImage(const void *src);
	void setTitle(const char *title);
	void setProgress(int progress);
	void setProgressEnd(const char *text);
	void setProgressStart(const char *text);

protected:
	virtual void handleEvent(lv_event_t *event, int action);

private:
	enum Action {
		PLAY,
		NEXT,
		PREVIOUS
	};

	EventHandler playButtonHandler;
	EventHandler nextButtonHandler;
	EventHandler previousButtonHandler;

	lv_obj_t *titleLabel;
	lv_obj_t *artistLabel;
	lv_obj_t *coverImage;
	lv_obj_t *progressStartLabel;
	lv_obj_t *progressSlider;
	lv_obj_t *progressEndLabel;
	lv_obj_t *previousButton;
	lv_obj_t *playButton;
	lv_obj_t *nextButton;

	void addCoverImage(lv_obj_t *parent);
	lv_obj_t *addInfoAndControls(lv_obj_t *parent);
	lv_obj_t *addPlaybackControlButton(lv_obj_t *parent, const char *label);
	lv_obj_t *addPlaybackControls(lv_obj_t *parent);
	lv_obj_t *addProgressControls(lv_obj_t *parent);
};
