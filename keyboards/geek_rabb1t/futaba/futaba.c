/* Copyright 2024 Geek-rabb1t (@geek-rabb1t)
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

#include "quantum.h"
#include "../trackpad/gr_trackpad65.h"

void matrix_scan_kb(void) {
    matrix_scan_trackpad();
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    return pointing_device_task_trackpad(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    return process_record_trackpad(keycode, record);
}

void keyboard_post_init_kb(void) {
    trackpad_matrix_config_t matrix_config = {
        .finger_gesture_rows = {9,10,11,12},
        .configuration_row = 14,
        .configuration_layer = 0,
        .allow_rotate = false
    };

    keyboard_post_init_trackpad(matrix_config);
    // Customise these values to desired behaviour
    debug_enable = true;
    // debug_matrix = true;
    debug_keyboard = true;
    debug_mouse = true;
}
