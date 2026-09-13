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

#include "rotate_cursor.h"
#include "gr_trackpad65_config.h"

extern gr_trackpad65_config_t gr_trackpad_config;

static trackpad_base_data_t rotate_90(trackpad_base_data_t* base) {
    trackpad_base_data_t new_data = {
        .pos.x = base->pos.y,
        .pos.y = base->pos.x * -1,
        .prev_pos.x = base->prev_pos.y,
        .prev_pos.y = base->prev_pos.x * -1,
        .mouse_report_x = base->mouse_report_y,
        .mouse_report_y = base->mouse_report_x * -1,

        .touch_strength = base->touch_strength,
        .num_of_fingers = base->num_of_fingers,
    };

    return new_data;
}

static trackpad_base_data_t rotate_180(trackpad_base_data_t* base) {
    trackpad_base_data_t new_data = {
        .pos.x = base->pos.x * -1,
        .pos.y = base->pos.y * -1,
        .prev_pos.x = base->prev_pos.x * -1,
        .prev_pos.y = base->prev_pos.y * -1,
        .mouse_report_x = base->mouse_report_x * -1,
        .mouse_report_y = base->mouse_report_y * -1,

        .touch_strength = base->touch_strength,
        .num_of_fingers = base->num_of_fingers,
    };

    return new_data;
}

static trackpad_base_data_t rotate_270(trackpad_base_data_t* base) {
    trackpad_base_data_t new_data = {
        .pos.x = base->pos.y * -1,
        .pos.y = base->pos.x,
        .prev_pos.x = base->prev_pos.y * -1,
        .prev_pos.y = base->prev_pos.x,
        .mouse_report_x = base->mouse_report_y * -1,
        .mouse_report_y = base->mouse_report_x,

        .touch_strength = base->touch_strength,
        .num_of_fingers = base->num_of_fingers,
    };

    return new_data;
}

trackpad_base_data_t gr_trackpad65_rotate_cursor(trackpad_base_data_t trackpad_data) {
    switch (gr_trackpad_config.rotate) {
        case ROTATE_0:
            return trackpad_data;
        case ROTATE_90:
            return rotate_90(&trackpad_data);
        case ROTATE_180:
            return rotate_180(&trackpad_data);
        case ROTATE_270:
            return rotate_270(&trackpad_data);
        default:
            return trackpad_data;
    }
}
