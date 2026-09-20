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

#include QMK_KEYBOARD_H
#include "gr_trackpad65_config.h"

typedef enum  {
    cursor_speed = 0,
    scroll_speed = 1,
    cursor_correct = 2,

    enable_accel = 3,
    reverse_vertical_scroll = 4,
    reverse_horizontal_scroll = 5,

    inertia_cursor = 6,
    inertia_scroll = 7,

    scroll_only = 8,

    rotate = 9

} config_col_t;


gr_trackpad65_config_t gr_trackpad_config = {0};

const static int MIN_VALUE = 1;
const static int MAX_VALUE = 9;

int resolve_value(uint16_t keycode, int defaultValue) {
    int value = keycode - GRT_VAL_1 + 1;
    return
        value < MIN_VALUE ? defaultValue :
        value > MAX_VALUE ? defaultValue : value;
}

bool resolve_bool(uint16_t keycode, bool defaultValue) {
    return
        keycode == GRT_ON ? true :
        keycode == GRT_OFF ? false : defaultValue;
}

rotate_modes_t resolve_rotate(uint16_t keycode, rotate_modes_t defaultValue) {
    switch (keycode) {
        case ROTATE_0:
        case ROTATE_90:
        case ROTATE_180:
        case ROTATE_270:
            return keycode;
        default:
            return defaultValue;
    }
}

static uint8_t configuration_layer = 0;
static uint8_t configuration_row = 0;

int fibonacci_array[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};

int read_value(uint8_t col, int defaultValue) {
    uint16_t keycode = dynamic_keymap_get_keycode(configuration_layer, configuration_row, col);
    return resolve_value(keycode, defaultValue);
}

int read_fibonacci_value(uint8_t col, int defaultValue) {
    uint16_t keycode = dynamic_keymap_get_keycode(configuration_layer, configuration_row, col);
    return fibonacci_array[resolve_value(keycode, defaultValue)];
}

bool read_bool(uint8_t col, bool defaultValue) {
    uint16_t keycode = dynamic_keymap_get_keycode(configuration_layer, configuration_row, col);
    return resolve_bool(keycode, defaultValue);
}

rotate_modes_t read_rotate(uint8_t col, rotate_modes_t defaultValue) {
    uint16_t keycode = dynamic_keymap_get_keycode(configuration_layer, configuration_row, col);
    return resolve_rotate(keycode, defaultValue);
}

void load_gr_trackpad65_config(uint8_t config_layer, uint8_t config_row, bool allow_rotate)  {
    configuration_layer = config_layer;
    configuration_row = config_row;

    gr_trackpad_config.cursor_speed    = read_fibonacci_value(cursor_speed, 5);
    gr_trackpad_config.scroll_speed    = read_fibonacci_value(scroll_speed, 4);

    gr_trackpad_config.cursor_correct  = read_value(cursor_correct, 2);

    gr_trackpad_config.enable_accel    = read_bool(enable_accel, false);

    gr_trackpad_config.reverse_vertical_scroll = read_bool(reverse_vertical_scroll, false);
    gr_trackpad_config.reverse_horizontal_scroll = read_bool(reverse_horizontal_scroll, false);

    gr_trackpad_config.inertia_cursor  = read_bool(inertia_cursor, true);
    gr_trackpad_config.inertia_scroll  = read_bool(inertia_scroll, true);

    gr_trackpad_config.scroll_only     = read_bool(scroll_only, false);
    gr_trackpad_config.scroll_mode     = gr_trackpad_config.scroll_only;

    if (allow_rotate) {
        gr_trackpad_config.rotate = read_rotate(rotate, ROTATE_0);
    } else {
        gr_trackpad_config.rotate = ROTATE_0;
    }
}
