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

#include QMK_KEYBOARD_H

enum GR_TRACKPAD_KEYCODES {

    HIGH_SPEED = QK_KB_0,
    LOW_SPEED,
    SCROLL_MODE,

    GRT_VAL_1,
    GRT_VAL_2,
    GRT_VAL_3,
    GRT_VAL_4,
    GRT_VAL_5,
    GRT_VAL_6,
    GRT_VAL_7,
    GRT_VAL_8,
    GRT_VAL_9,

    GRT_ON,
    GRT_OFF,

    GRT_R0,
    GRT_R90,
    GRT_R180,
    GRT_R270,

    // Label use only.
    GRTL_CS,
    GRTL_SS,
    GRTL_CC,
    GRTL_CA,
    GRTL_RVS,
    GRTL_RHS,
    GRTL_IC,
    GRTL_IS,
    GRTL_SO,
    GRTL_R,
    GRTL_1T,
    GRTL_2T,
    GRTL_3T,
    GRTL_4T

} gr_trackpad_keycodes_t;

typedef enum {
    ROTATE_0 = GRT_R0,
    ROTATE_90 = GRT_R90,
    ROTATE_180 = GRT_R180,
    ROTATE_270 = GRT_R270
} rotate_modes_t;

typedef struct {
    uint16_t cursor_speed;
    uint16_t cursor_accel;
    uint16_t scroll_speed;

    bool enable_accel;
    uint16_t cursor_correct;

    bool reverse_vertical_scroll;
    bool reverse_horizontal_scroll;

    bool inertia_cursor;
    bool inertia_scroll;

    bool scroll_only;
    bool scroll_mode;

    rotate_modes_t rotate;
} gr_trackpad65_config_t;


extern gr_trackpad65_config_t gr_trackpad_config;

void load_gr_trackpad65_config(uint8_t config_layer, uint8_t config_row, bool allow_rotate) ;
