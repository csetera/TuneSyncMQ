/**********************************************************************************
 * Copyright (C) 2023 Craig Setera
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 **********************************************************************************/
#pragma once

#include <functional>
#include <lvgl.h>

typedef std::function<void(lv_event_t *event)> EventHandler;

/**
 * @brief Base class for all Screen implementations.  Helps to
 * organize the LVGL functionality that is tied to a particular
 * screen.
 */
class Screen
{
public:
    virtual lv_obj_t *createWidgets();
    lv_obj_t *getLvglObject();

protected:
    class EventHandlerRegistration {
    public:

        EventHandlerRegistration(Screen *handler, int action): _handler(handler), _action(action) {}

        /**
         * Retrieves the action value.
         *
         * @return The action value.
         */
        int getAction() {
            return _action;
        }

        /**
         * Returns the handler for the screen.
         *
         * @return the handler for the screen
         */
        Screen *getHandler() {
            return _handler;
        }

    protected:
        Screen *_handler;
        int _action;
    };

    static void dispatchEvent(lv_event_t *event);

    lv_obj_t *lv_screen;

    Screen();
    ~Screen();

    virtual void createScreenWidgets(lv_obj_t *parent) = 0;
    virtual lv_obj_t *createLayoutContainer(lv_obj_t *parent);

    virtual void handleEvent(lv_event_t *event, int action);
    void registerEventHandler(lv_obj_t * obj, lv_event_code_t filter, Screen *handler, int action);
};

