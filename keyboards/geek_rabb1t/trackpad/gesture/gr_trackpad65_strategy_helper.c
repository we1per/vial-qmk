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

#include "gr_trackpad65_strategy_helper.h"
#include "../gr_trackpad65_driver.h"
#include "../gr_trackpad65_config.h"

trackpad_gesture_handle_state_t gesture_handle_state = {
    .max_fingers = 0,
    .swipe_distance_x = 0,
    .swipe_distance_y = 0,
    .tap_interval = 0,
    .tap_timer = 0,
    .gesture_timer = 0,
    .doubleTap = false,

    .scroll_direction = scroll_direction_tbd,
    .scroll_rest = {0}
};


int8_t calc_max_fingers(int8_t current, int8_t previous) {
    return (previous > current) ? previous :current;
}

touch_state_t get_touch_state(trackpad_base_data_t *trackpad_data) {
    if (trackpad_data->touch_strength == 0) {
        return touch_state_none;
    } else if (trackpad_data->touch_strength >= GR_TRACKPAD65_TAP_STRENGTH_THRESHOLD) {
        return touch_state_press;
    }
    return touch_state_touch;
}

void reset_gesture_status(void) {
    gesture_handle_state.max_fingers = 0;
    gesture_handle_state.swipe_distance_x = 0;
    gesture_handle_state.swipe_distance_y = 0;
    gesture_handle_state.doubleTap = false;
    gesture_handle_state.scroll_direction = scroll_direction_tbd;
    gesture_handle_state.scroll_rest.x = 0;
    gesture_handle_state.scroll_rest.y = 0;
}
