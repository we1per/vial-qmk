/*
 * Copyright 2025 Geek-rabb1t (@geek-rabb1t)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "gr_trackpad65_state.h"

typedef enum {
    touch_state_none,
    touch_state_touch,
    touch_state_press,
} touch_state_t;


typedef struct {
    bool is_pressed;
    pointing_device_buttons_t button_num;
} dispatch_button_t;

typedef enum {
    scroll_direction_vertical,
    scroll_direction_horizontal,
    scroll_direction_both,
    scroll_direction_tbd,
} scroll_direction_t;

typedef struct {
    uint8_t max_fingers;
    int16_t swipe_distance_x;
    int16_t swipe_distance_y;
    uint16_t tap_interval;
    uint16_t tap_timer;
    uint16_t gesture_timer;
    bool doubleTap;

    scroll_direction_t scroll_direction;
    position_t scroll_rest;
}trackpad_gesture_handle_state_t;

trackpad_gesture_handle_state_t gesture_handle_state;

int8_t calc_max_fingers(int8_t current, int8_t previous);
touch_state_t get_touch_state(trackpad_base_data_t *trackpad_data);
void reset_gesture_status(void);
