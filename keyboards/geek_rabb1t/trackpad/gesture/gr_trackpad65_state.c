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


#include "gr_trackpad65_state.h"
#include "gr_trackpad65_idle_strategy.h"
#include "gr_trackpad65_touch_strategy.h"
#include "gr_trackpad65_move_strategy.h"
#include "gr_trackpad65_inertia_cursor_strategy.h"
#include "gr_trackpad65_scroll_strategy.h"
#include "gr_trackpad65_inertia_scroll_strategy.h"
#include "gr_trackpad65_gesture_strategy.h"
#include "gr_trackpad65_gesture_fire_strategy.h"
#include "gr_trackpad65_press_strategy.h"
#include "gr_trackpad65_wait_strategy.h"

static trackpad_state_t trackpad_state = trackpad_state_idle;

report_mouse_t (*find_strategy(trackpad_state_t state))(trackpad_base_data_t *) {
    switch (state) {
        case trackpad_state_idle: return idle_strategy;
        case trackpad_state_touch: return touch_strategy;
        case trackpad_state_move: return move_strategy;
        case trackpad_state_inertia_cursor: return inertia_cursor_strategy;
        case trackpad_state_scroll: return scroll_strategy;
        case trackpad_state_inertia_scroll: return inertia_scroll_strategy;
        case trackpad_state_gesture: return gesture_strategy;
        case trackpad_state_gesture_fire: return gesture_fire_strategy;
        case trackpad_state_press: return press_strategy;
        case trackpad_state_wait: return wait_strategy;
    }
    return idle_strategy;
}

trackpad_state_t (*update_state_strategy(trackpad_state_t prev_state))(trackpad_base_data_t *) {
    switch (prev_state) {
        case trackpad_state_idle: return update_idle_state;
        case trackpad_state_touch: return update_touch_state;
        case trackpad_state_move: return update_move_state;
        case trackpad_state_inertia_cursor: return update_inertia_cursor_state;
        case trackpad_state_scroll: return update_scroll_state;
        case trackpad_state_inertia_scroll: return update_inertia_scroll_state;
        case trackpad_state_gesture: return update_gesture_state;
        case trackpad_state_gesture_fire: return update_gesture_fire_state;
        case trackpad_state_press: return update_press_state;
        case trackpad_state_wait: return update_wait_state;
    }

    return update_idle_state;
}


report_mouse_t trackpad_report(trackpad_base_data_t trackpad_data) {

    trackpad_state_t (*update_state)(trackpad_base_data_t *) = update_state_strategy(trackpad_state);
    trackpad_state = update_state(&trackpad_data);

    report_mouse_t (*reporter)(trackpad_base_data_t *) = find_strategy(trackpad_state);
    return reporter(&trackpad_data);
}
