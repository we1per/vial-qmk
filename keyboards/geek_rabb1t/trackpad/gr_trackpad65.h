/* Copyright 2025 Geek-rabb1t (@geek-rabb1t)
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

#define MAX_FINGERS 4

typedef struct {
    uint8_t finger_gesture_rows[MAX_FINGERS];
    uint8_t configuration_row;
    uint8_t configuration_layer;
    bool allow_rotate;
} trackpad_matrix_config_t;

void matrix_scan_trackpad(void);
report_mouse_t pointing_device_task_trackpad(report_mouse_t mouse_report);
bool process_record_trackpad(uint16_t keycode, keyrecord_t *record);
void keyboard_post_init_trackpad(trackpad_matrix_config_t config);
