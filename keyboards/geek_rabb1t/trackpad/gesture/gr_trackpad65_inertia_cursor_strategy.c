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

#define MIN_INERTIA_CYCLE 35
#define MAX_DELTA 10
#define INERTIA_THREDSHOLD 30
static int cycle = 0;
static int max_cycle = MIN_INERTIA_CYCLE;

static position_t inertia = {0};

trackpad_state_t update_inertia_cursor_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);

    if (touch_state == touch_state_none) {
        cycle = cycle % max_cycle;
        if (cycle == 0) {
            return trackpad_state_idle;
        }
    } else {
        cycle = 0;
        reset_gesture_status();
        return update_idle_state(trackpad_data);
    }
    return trackpad_state_inertia_cursor;
}


report_mouse_t inertia_cursor_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};

    if (cycle == 0) {
        inertia.x = trackpad_data->prev_report_x;
        inertia.y = trackpad_data->prev_report_y;

        if (abs(inertia.x) <= INERTIA_THREDSHOLD  && abs(inertia.y) <= INERTIA_THREDSHOLD) {
            return temp_report;
        }

        max_cycle = MAX((MAX(abs(inertia.x), abs(inertia.y)) / MAX_DELTA), MIN_INERTIA_CYCLE);
    }

    temp_report.x = inertia.x - (inertia.x * cycle / max_cycle);
    temp_report.y = inertia.y - (inertia.y * cycle / max_cycle);
    cycle++;

    return temp_report;
}
