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

#include "gr_trackpad65_gesture_strategy.h"
#include "gr_trackpad65_strategy_helper.h"
#include "../gr_trackpad65_driver.h"
#include "timer.h"

extern trackpad_gesture_handle_state_t gesture_handle_state;

trackpad_state_t update_gesture_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);

    if (touch_state == touch_state_none) {
        if (timer_elapsed(gesture_handle_state.gesture_timer) <= GR_TRACKPAD65_MAX_GESTURE_ACTIVE_TIME) {
            return trackpad_state_gesture_fire;
        }
        return trackpad_state_idle;
    }

    return trackpad_state_gesture;
}



report_mouse_t gesture_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};

    gesture_handle_state.swipe_distance_x += trackpad_data->pos.x;
    gesture_handle_state.swipe_distance_y += trackpad_data->pos.y;

    return temp_report;
}
