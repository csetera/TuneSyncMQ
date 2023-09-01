/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#include "Screen.h"

Screen::Screen(): lv_screen(nullptr) {
}

Screen::~Screen() {
    if (lv_screen != nullptr) {
        lv_obj_del(lv_screen);
    }
}

/**
 * Creates and initializes a screen object.
 *
 * @return The created screen object.
 */
lv_obj_t *Screen::createWidgets() {
    if (lv_screen != nullptr) {
        return lv_screen;
    }

    lv_screen = createLayoutContainer(NULL);
    lv_theme_apply(lv_screen);
    createScreenWidgets(lv_screen);

    return lv_screen;
}

/**
 * Creates a layout container and returns a pointer to it.
 *
 * @param parent a pointer to the parent object
 *
 * @return a pointer to the created layout container
 */
lv_obj_t *Screen::createLayoutContainer(lv_obj_t *parent) {
    lv_obj_t *layout = lv_obj_create(parent);

    lv_obj_clear_flag(layout, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(layout, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_remove_style_all(layout); /*Make it transparent*/

    return layout;
}

/**
 * Dispatches an event to the proper screen handler.
 *
 * @param event The event to be dispatched.
 *
 * @throws ErrorType If the event is invalid or if an error occurs during event handling.
 */
void Screen::dispatchEvent(lv_event_t *event) {
    if (event->user_data != nullptr) {
        EventHandlerRegistration *registration = (EventHandlerRegistration *) event->user_data;
        registration->getHandler()->handleEvent(event, registration->getAction());
    }
}

/**
 * Retrieves the lvgl object associated with the Screen.
 *
 * @return The lv_obj_t object of the Screen.
 */
lv_obj_t *Screen::getLvglObject() {
    return lv_screen;
}

/**
 * Handles the event and performs the specified action.
 *
 * @param event a pointer to the event to be handled
 * @param action the action to be performed
 */
void Screen::handleEvent(lv_event_t *event, int action) {
    // Do nothing by default
}

/**
 * Registers an event handler for the given object and event filter.
 *
 * @param obj The object to register the event handler for.
 * @param filter The event filter to apply to the events.
 * @param handler The handler to be called when the event is triggered.
 * @param action The action to be performed by the handler.
 */
void Screen::registerEventHandler(lv_obj_t *obj, lv_event_code_t filter, Screen *handler, int action)
{
    EventHandlerRegistration *handlerRegistration = new EventHandlerRegistration(handler, action);
    lv_obj_add_event_cb(obj, Screen::dispatchEvent, filter, (void *) handlerRegistration);
}
