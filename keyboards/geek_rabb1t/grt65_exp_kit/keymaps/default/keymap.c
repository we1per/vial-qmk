// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <print.h>
#include "gr_trackpad65_driver.h"
#include "pointing_device.h"

enum my_keycodes {
  HIGH_SPEED = QK_KB_0,
  LOW_SPEED,
  TGL_V_SCL,
  TGL_H_SCL,
  EN_3_TAP,
  DIS_3_TAP,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        MS_BTN1 , MS_BTN3,    MS_BTN2,
        G(KC_TAB), KC_BTN5, G(KC_D), KC_BTN4,
        G(KC_TAB), G(C(KC_RGHT)), G(KC_D), G(C(KC_LEFT))
    ),
};



void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    // debug_enable = true;
    //debug_matrix = true;
    //debug_keyboard = true;
    //debug_mouse = true;
}

typedef enum  {
    SPEED_MODE_LOW,
    SPEED_MODE_NORMAL,
    SPEED_MODE_HIGH
} speed_mode_t;

static speed_mode_t speed_mode = SPEED_MODE_NORMAL;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case HIGH_SPEED:
      if (record->event.pressed) {
        speed_mode = SPEED_MODE_HIGH;
      } else {
        speed_mode = SPEED_MODE_NORMAL;
      }
      return true;

    case LOW_SPEED:
      if (record->event.pressed) {
        speed_mode = SPEED_MODE_LOW;
      } else {
        speed_mode = SPEED_MODE_NORMAL;
      }
      return true;

    case TGL_V_SCL:
      if (record->event.pressed) {
        trackpad_config.reverse_vertical_scroll = !trackpad_config.reverse_vertical_scroll;
        update_trackpad_config(trackpad_config);
      }
      return false;

    case TGL_H_SCL:
      if (record->event.pressed) {
        trackpad_config.reverse_horizontal_scroll = !trackpad_config.reverse_horizontal_scroll;
        update_trackpad_config(trackpad_config);
      }
      return false;

    case EN_3_TAP:
      if (record->event.pressed) {
        trackpad_config.disable_3fingers_tap = false;
        update_trackpad_config(trackpad_config);
      }
      return false;

    case DIS_3_TAP:
      if (record->event.pressed) {
        trackpad_config.disable_3fingers_tap = true;
        update_trackpad_config(trackpad_config);
      }
      return false;

    default:
      return true;
  }
}

int LOW_SPEED_RATIO = 2;
int HIGH_SPEED_RATIO = 2;

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {

    if (speed_mode == SPEED_MODE_LOW) {
        mouse_report.x = CONSTRAIN_HID_XY(mouse_report.x / LOW_SPEED_RATIO);
        mouse_report.y = CONSTRAIN_HID_XY(mouse_report.y / LOW_SPEED_RATIO);
    }

    if (speed_mode == SPEED_MODE_HIGH) {
        mouse_report.x = CONSTRAIN_HID_XY(mouse_report.x * HIGH_SPEED_RATIO);
        mouse_report.y = CONSTRAIN_HID_XY(mouse_report.y * HIGH_SPEED_RATIO);
    }

    return mouse_report;
}
