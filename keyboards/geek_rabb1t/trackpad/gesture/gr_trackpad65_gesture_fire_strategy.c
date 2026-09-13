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

#include "gr_trackpad65_gesture_fire_strategy.h"
#include "gr_trackpad65_strategy_helper.h"
#include "../gr_trackpad65_driver.h"
#include "timer.h"
#include <stdlib.h>

extern trackpad_gesture_handle_state_t gesture_handle_state;

trackpad_state_t update_gesture_fire_state(trackpad_base_data_t *trackpad_data) {
    touch_state_t touch_state = get_touch_state(trackpad_data);
    reset_trackpad_event();

    if (touch_state == touch_state_touch) {
        return trackpad_state_touch;
    }
    if (touch_state == touch_state_press) {
        gesture_handle_state.max_fingers = calc_max_fingers(
            trackpad_data->num_of_fingers, gesture_handle_state.max_fingers);
        return trackpad_state_press;
    }

    return trackpad_state_idle;
}

report_mouse_t gesture_fire_strategy(trackpad_base_data_t *trackpad_data) {
    report_mouse_t temp_report = {0};

    // dispatch swipe gesture
    // pd_dprintf("gesture: %d fingers.(%d, %d)\n",max_fingers, swipe_distance_x, swipe_distance_y);
    gesture_handle_state.swipe_distance_x =
        abs(gesture_handle_state.swipe_distance_x) >= GR_TRACKPAD65_SWIPE_THRESHOLD_PIXEL ?
            gesture_handle_state.swipe_distance_x : 0;
    gesture_handle_state.swipe_distance_y =
        abs(gesture_handle_state.swipe_distance_y) >= GR_TRACKPAD65_SWIPE_THRESHOLD_PIXEL ?
            gesture_handle_state.swipe_distance_y : 0;
    if (gesture_handle_state.swipe_distance_x == 0 && gesture_handle_state.swipe_distance_y == 0) {
        reset_trackpad_event();
        return temp_report;
    }

    trackpad_gesture_event_t event = abs(gesture_handle_state.swipe_distance_x) > abs(gesture_handle_state.swipe_distance_y) ?
            (gesture_handle_state.swipe_distance_x > 0 ? trackpad_event_swipe_right : trackpad_event_swipe_left):
            (gesture_handle_state.swipe_distance_y > 0 ? trackpad_event_swipe_down : trackpad_event_swipe_up);

    // pd_dprintf("swipe - %d fingers, direction: %d, x:%d, y:%d.\n",max_fingers, event, swipe_distance_x, swipe_distance_y);

    trackpad_event.type = event;
    trackpad_event.num_of_fingers = gesture_handle_state.max_fingers;

    return temp_report;
}
