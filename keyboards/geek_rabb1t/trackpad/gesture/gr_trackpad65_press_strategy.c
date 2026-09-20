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

#include "gr_trackpad65_idle_strategy.h"
#include "gr_trackpad65_strategy_helper.h"
#include "../gr_trackpad65_driver.h"
#include "../gr_trackpad65_config.h"
#include "timer.h"
#include "quantum.h"

extern trackpad_gesture_handle_state_t gesture_handle_state;

trackpad_state_t update_press_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);

    if (touch_state == touch_state_none) {
        gesture_handle_state.tap_interval = timer_read();
        return trackpad_state_wait;
    }

    return trackpad_state_press;
}


report_mouse_t press_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};

    trackpad_event.type = trackpad_event_press;
    trackpad_event.num_of_fingers = gesture_handle_state.max_fingers;

    if (trackpad_data->mouse_report_x != 0 || trackpad_data->mouse_report_y != 0) {
        gesture_handle_state.doubleTap = false;
    }

    if (!gr_trackpad_config.scroll_mode) {
        temp_report.x = trackpad_data->mouse_report_x;
        temp_report.y = trackpad_data->mouse_report_y;
    }

    return temp_report;
}
