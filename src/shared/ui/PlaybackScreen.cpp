/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "PlaybackScreen.h"

/**
 * @brief Add the album coverimage image to the parent.
 *
 * @param parent
 */
void PlaybackScreen::addCoverImage(lv_obj_t *parent) {
	// Place the image into a container to avoid the mosaic
	// feature for smaller images
	lv_obj_t *container = createLayoutContainer(parent);
	lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	LV_IMG_DECLARE(logo);
	coverImage = lv_img_create(container);
	lv_img_set_src(coverImage, &logo);
	lv_obj_align(coverImage, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_size(coverImage, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_radius(coverImage, 10, 0);
	lv_obj_set_style_clip_corner(coverImage, true, LV_PART_MAIN);
}

/**
 * @brief Add the progress information and playback controls.
 *
 * @param parent
 * @return lv_obj_t*
 */
lv_obj_t *PlaybackScreen::addInfoAndControls(lv_obj_t *parent) {
	lv_obj_t *layout = createLayoutContainer(parent);
	lv_obj_set_layout(layout, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_height(layout, lv_pct(100));
	lv_obj_set_flex_align(layout, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	titleLabel = lv_label_create(layout);
	lv_label_set_text(titleLabel, "Title");
	lv_obj_set_style_pad_all(titleLabel, 5, LV_PART_MAIN);
	lv_obj_set_style_pad_top(titleLabel, 10, LV_PART_MAIN);
	lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_18, LV_PART_MAIN);

	artistLabel = lv_label_create(layout);
	lv_label_set_text(artistLabel, "Artist");
	lv_obj_set_style_pad_all(artistLabel, 5, LV_PART_MAIN);

	lv_obj_t *playbackControlsLayout = addPlaybackControls(layout);
	lv_obj_set_flex_grow(playbackControlsLayout, 1);

	lv_obj_t *progressControlsLayout = addProgressControls(layout);
	lv_obj_set_flex_grow(playbackControlsLayout, 1);

	return layout;
}

/**
 * @brief Add a single playback control button.
 *
 * @param parent
 * @param label
 * @return lv_obj_t*
 */
lv_obj_t *PlaybackScreen::addPlaybackControlButton(lv_obj_t *parent, const char *label) {
	lv_obj_t *btn = lv_btn_create(parent);
	lv_obj_set_size(btn, 70, 70);
	lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_img_src(btn, label, 0);
	lv_obj_set_style_text_font(btn, &lv_font_montserrat_18, LV_PART_MAIN);

	return btn;
}

/**
 * @brief Add all of the playback control buttons.
 *
 * @param parent
 * @return lv_obj_t*
 */
lv_obj_t *PlaybackScreen::addPlaybackControls(lv_obj_t *parent) {
	lv_obj_t *layout = createLayoutContainer(parent);
	lv_obj_set_layout(layout, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(layout, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_size(layout, lv_pct(100), LV_SIZE_CONTENT);

	previousButton = addPlaybackControlButton(layout, LV_SYMBOL_PREV);
	playButton = addPlaybackControlButton(layout, LV_SYMBOL_PLAY);
	nextButton = addPlaybackControlButton(layout, LV_SYMBOL_NEXT);

	return layout;
}

/**
 * @brief Add the progress controls.
 *
 * @param parent
 * @return lv_obj_t*
 */
lv_obj_t *PlaybackScreen::addProgressControls(lv_obj_t *parent) {
	lv_obj_t *layout = createLayoutContainer(parent);
	lv_obj_set_layout(layout, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(layout, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_width(layout, lv_pct(100));

	progressStartLabel = lv_label_create(layout);
	lv_label_set_text(progressStartLabel, "0:00");
	lv_obj_set_style_pad_all(progressStartLabel, 5, LV_PART_MAIN);

	progressSlider = lv_slider_create(layout);
	lv_obj_set_width(progressSlider, lv_pct(60));
	lv_slider_set_value(progressSlider, 35, LV_ANIM_ON);
	lv_obj_clear_flag(progressSlider, LV_OBJ_FLAG_CLICKABLE);

	progressEndLabel = lv_label_create(layout);
	lv_label_set_text(progressEndLabel, "5:00");
	lv_obj_set_style_pad_all(progressEndLabel, 5, LV_PART_MAIN);

	return layout;
}

void PlaybackScreen::createScreenWidgets(lv_obj_t *parent) {
	lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
	lv_obj_set_size(parent, lv_pct(100), lv_pct(100));

	lv_obj_t *infoAndControls = addInfoAndControls(parent);
	lv_obj_set_size(infoAndControls, lv_pct(55), lv_pct(100));

	lv_obj_t *rightLayout = createLayoutContainer(parent);
	lv_obj_set_size(rightLayout, lv_pct(45), lv_pct(100));
	addCoverImage(rightLayout);
}

void PlaybackScreen::handleEvent(lv_event_t *event, int action) {
	switch (action)	{
		case PLAY:
			playButtonHandler(event);
			break;

		case NEXT:
			nextButtonHandler(event);
			break;

		case PREVIOUS:
			previousButtonHandler(event);
			break;
	}
}

void PlaybackScreen::onPlayClick(EventHandler eventHandler) {
	playButtonHandler = eventHandler;
	registerEventHandler(playButton, LV_EVENT_CLICKED, this, Action::PLAY);
}

void PlaybackScreen::onNextClick(EventHandler eventHandler) {
	nextButtonHandler = eventHandler;
	registerEventHandler(nextButton, LV_EVENT_CLICKED, this, Action::NEXT);
}

void PlaybackScreen::onPreviousClick(EventHandler eventHandler) {
	previousButtonHandler = eventHandler;
	registerEventHandler(previousButton, LV_EVENT_CLICKED, this, Action::PREVIOUS);
}

void PlaybackScreen::setArtist(const char *artist) {
	lv_label_set_text(artistLabel, artist);
}

void PlaybackScreen::setCoverImage(const void *src) {
	lv_img_set_src(coverImage, src);
}

void PlaybackScreen::setTitle(const char *title) {
	lv_label_set_text(titleLabel, title);
}

void PlaybackScreen::setProgress(int progress) {
	lv_slider_set_value(progressSlider, progress, LV_ANIM_ON);
}

void PlaybackScreen::setProgressEnd(const char *text) {
	lv_label_set_text(progressEndLabel, text);
}

void PlaybackScreen::setProgressStart(const char *text) {
	lv_label_set_text(progressStartLabel, text);
}
