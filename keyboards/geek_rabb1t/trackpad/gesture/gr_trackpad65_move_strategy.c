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
#include <stdlib.h>

extern trackpad_gesture_handle_state_t gesture_handle_state;

trackpad_state_t update_move_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);

    if (touch_state == touch_state_none) {
        if (gr_trackpad_config.inertia_cursor) {
            return trackpad_state_inertia_cursor;
        } else {
            return trackpad_state_idle;
        }
    }

    return trackpad_state_move;
}


report_mouse_t move_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};

    gesture_handle_state.scroll_direction = scroll_direction_tbd;
    temp_report.x = trackpad_data->mouse_report_x;
    temp_report.y = trackpad_data->mouse_report_y;

    return temp_report;
}
