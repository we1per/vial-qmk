// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "../trackpad/gr_trackpad65_config.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        MS_BTN1 , MS_BTN3,    MS_BTN2,
        MS_BTN1, KC_NO, KC_NO, KC_NO, KC_NO,
        MS_BTN2, KC_NO, KC_NO, KC_NO, KC_NO,
        MS_BTN3, G(KC_TAB), KC_BTN5, G(KC_D), KC_BTN4,
        G(KC_S), G(KC_TAB), G(C(KC_RGHT)), G(KC_D), G(C(KC_LEFT)),
        GRTL_CS, GRTL_SS, GRTL_CC, GRTL_CA, GRTL_RVS, GRTL_RHS, GRTL_IC, GRTL_IS, GRTL_SO, GRTL_R,
        GRT_VAL_5, GRT_VAL_4, GRT_VAL_2, GRT_OFF, GRT_ON, GRT_OFF, GRT_ON, GRT_ON, GRT_OFF, GRT_R0,
        GRTL_1T, GRTL_2T, GRTL_3T, GRTL_4T
    ),
};
