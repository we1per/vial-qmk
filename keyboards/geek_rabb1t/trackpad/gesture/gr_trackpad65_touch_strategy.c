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
#include "gr_trackpad65_touch_strategy.h"
#include "gr_trackpad65_strategy_helper.h"
#include "../gr_trackpad65_driver.h"
#include "../gr_trackpad65_config.h"
#include "timer.h"

extern trackpad_gesture_handle_state_t gesture_handle_state;

trackpad_state_t update_touch_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);

    gesture_handle_state.max_fingers = calc_max_fingers(trackpad_data->num_of_fingers, gesture_handle_state.max_fingers);
    if (touch_state == touch_state_none) {
        if (timer_elapsed(gesture_handle_state.tap_timer) <= GR_TRACKPAD65_MAX_TAP_TIME) {
            // pd_dprintf("touch : %d fingers.time: (%d)\n",max_fingers, timer_elapsed(tap_timer));
            return trackpad_state_press;
        }
        return trackpad_state_idle;
    }

    if (trackpad_data->mouse_report_x != 0 || trackpad_data->mouse_report_y != 0) {
        if (gesture_handle_state.max_fingers > 2) {
            // pd_dprintf("start gesture: %d fingers.(x,y): (%d, %d)\n",max_fingers,trackpad_data->mouse_report_x, trackpad_data->mouse_report_y);
            gesture_handle_state.gesture_timer = timer_read();
            return trackpad_state_gesture;
        }
        if (gr_trackpad_config.scroll_mode || gesture_handle_state.max_fingers == 2) {
            return trackpad_state_scroll;
        }
        return trackpad_state_move;
    }

    if (touch_state == touch_state_press) {
        if (timer_elapsed(gesture_handle_state.tap_timer) <= GR_TRACKPAD65_MAX_TAP_TIME) {
            // pd_dprintf("strong touch : %d fingers.time: (%d)\n",max_fingers, timer_elapsed(tap_timer));
            return trackpad_state_press;
        }
    }

    return trackpad_state_touch;
}


report_mouse_t touch_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};
    return temp_report;
}
