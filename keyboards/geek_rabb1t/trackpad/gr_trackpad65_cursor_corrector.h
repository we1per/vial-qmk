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

#include "azoteq_iqs5xx.h"

typedef struct {
    int x;
    int y;
} position_t;

typedef struct {
    position_t pos;
    position_t prev_pos;
    mouse_xy_report_t mouse_report_x;
    mouse_xy_report_t mouse_report_y;
    mouse_xy_report_t prev_report_x;
    mouse_xy_report_t prev_report_y;
    int touch_strength;
    int num_of_fingers;
} trackpad_base_data_t;


trackpad_base_data_t cursor_corrector_correct(azoteq_iqs5xx_base_data_t base_data);
