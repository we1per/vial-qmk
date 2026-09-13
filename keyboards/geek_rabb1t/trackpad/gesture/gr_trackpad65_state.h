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

#include "../gr_trackpad65_cursor_corrector.h"

typedef enum {
    trackpad_state_idle,
    trackpad_state_touch,
    trackpad_state_move,
    trackpad_state_inertia_cursor,
    trackpad_state_scroll,
    trackpad_state_inertia_scroll,
    trackpad_state_gesture,
    trackpad_state_gesture_fire,
    trackpad_state_press,
    trackpad_state_wait,
} trackpad_state_t;

report_mouse_t trackpad_report(trackpad_base_data_t trackpad_data);
