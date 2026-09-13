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
#include "wait.h"
#include "gr_trackpad65_driver.h"
#include "trackpad/gr_trackpad65_config.h"
#include "trackpad/gr_trackpad65.h"
#include "pointing_device.h"

static trackpad_matrix_config_t trackpad_matrix_config = {0};

static bool need_release = false;
static bool pressed = false;
static keypos_t prev_gesture = {0};

static uint16_t gesture_release_timer = 0;



void matrix_scan_trackpad(void) {

    // リリースする前に他のジェスチャが発火するとバグるのでは？
    // キューに入れて処理すればいいけど、たぶん発生しないので後回し。
    if (trackpad_event.type == trackpad_event_none) {
        if (need_release) {
            action_exec(MAKE_KEYEVENT(prev_gesture.row, prev_gesture.col, false));
            need_release = false;
            pressed = false;
        }
    } else if (trackpad_event.type == trackpad_event_press) {
        if (1 <= trackpad_event.num_of_fingers && trackpad_event.num_of_fingers <= MAX_FINGERS) {
            if (!pressed) {
                prev_gesture.row =  trackpad_matrix_config.finger_gesture_rows[trackpad_event.num_of_fingers - 1];
                prev_gesture.col = trackpad_event.type;
                gesture_release_timer = timer_read();
                need_release = true;
                pressed = true;
                action_exec(MAKE_KEYEVENT(prev_gesture.row, prev_gesture.col, true));
            }
        }
    } else {
        if (1 <= trackpad_event.num_of_fingers && trackpad_event.num_of_fingers <= MAX_FINGERS) {
            prev_gesture.row =  trackpad_matrix_config.finger_gesture_rows[trackpad_event.num_of_fingers - 1];
            prev_gesture.col = trackpad_event.type;
            gesture_release_timer = timer_read();
            need_release = true;
            action_exec(MAKE_KEYEVENT(prev_gesture.row, prev_gesture.col, true));
        }
    }

    matrix_scan_user();
}


typedef enum  {
    SPEED_MODE_LOW,
    SPEED_MODE_NORMAL,
    SPEED_MODE_HIGH
} speed_mode_t;

static speed_mode_t speed_mode = SPEED_MODE_NORMAL;

#define LOW_SPEED_RATIO 2
#define HIGH_SPEED_RATIO 2

report_mouse_t pointing_device_task_trackpad(report_mouse_t mouse_report) {
    if (speed_mode == SPEED_MODE_LOW) {
        mouse_report.x = CONSTRAIN_HID_XY(mouse_report.x / LOW_SPEED_RATIO);
        mouse_report.y = CONSTRAIN_HID_XY(mouse_report.y / LOW_SPEED_RATIO);
    }

    if (speed_mode == SPEED_MODE_HIGH) {
        mouse_report.x = CONSTRAIN_HID_XY(mouse_report.x * HIGH_SPEED_RATIO);
        mouse_report.y = CONSTRAIN_HID_XY(mouse_report.y * HIGH_SPEED_RATIO);
    }

    return pointing_device_task_user(mouse_report);
}


bool process_record_trackpad(uint16_t keycode, keyrecord_t *record) {
    // uprintf("cursor_speed: %u\n", gr_trackpad_config.cursor_speed);
    // uprintf("enable_accel: %u\n", gr_trackpad_config.enable_accel);
    // uprintf("scroll_speed: %u\n", gr_trackpad_config.scroll_speed);
    // uprintf("cursor_correct: %u\n", gr_trackpad_config.cursor_correct);

    // uprintf("reverse_vertical_scroll: %u\n", gr_trackpad_config.reverse_vertical_scroll);
    // uprintf("reverse_horizontal_scroll: %u\n", gr_trackpad_config.reverse_horizontal_scroll);
    // uprintf("inertia_cursor: %u\n", gr_trackpad_config.inertia_cursor);
    // uprintf("inertia_scroll: %u\n", gr_trackpad_config.inertia_scroll);
    // uprintf("scroll_only: %u\n", gr_trackpad_config.scroll_only);
    // uprintf("rotate: %u\n\n", gr_trackpad_config.rotate);

    // uprintf("event-type: %d, fingers: %d\n", trackpad_event.type, trackpad_event.num_of_fingers);
    // uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);


    switch (keycode) {
        case HIGH_SPEED:
        if (record->event.pressed) {
            speed_mode = SPEED_MODE_HIGH;
        } else {
            speed_mode = SPEED_MODE_NORMAL;
        }
        break;

        case LOW_SPEED:
        if (record->event.pressed) {
            speed_mode = SPEED_MODE_LOW;
        } else {
            speed_mode = SPEED_MODE_NORMAL;
        }
        break;

        case SCROLL_MODE:
        if (record->event.pressed) {
            gr_trackpad_config.scroll_mode = !gr_trackpad_config.scroll_only;
        } else {
            gr_trackpad_config.scroll_mode = gr_trackpad_config.scroll_only;
        }
        break;

        default:
        break;
    }

    return process_record_user(keycode, record);
}

void keyboard_post_init_trackpad(trackpad_matrix_config_t config) {

    for (int i = 0; i < MAX_FINGERS; i++) {
        trackpad_matrix_config.finger_gesture_rows[i] = config.finger_gesture_rows[i];
    }

    trackpad_matrix_config.configuration_row = config.configuration_row;
    trackpad_matrix_config.configuration_layer = config.configuration_layer;
    trackpad_matrix_config.allow_rotate = config.allow_rotate;

    load_gr_trackpad65_config(
        trackpad_matrix_config.configuration_layer,
        trackpad_matrix_config.configuration_row,
        trackpad_matrix_config.allow_rotate);


    // Customise these values to desired behaviour
    debug_enable = true;
    // debug_matrix = true;
    debug_keyboard = true;
    debug_mouse = true;
}
