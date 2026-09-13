// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <string.h>

// Vial customKeycodes follow the shared trackpad enum in exactly this order.
enum futaba_keycodes { ALT_TAB = GRTL_4T + 1 };
_Static_assert((uint16_t)ALT_TAB <= (uint16_t)QK_KB_MAX, "Smart Alt+Tab exceeds Vial keyboard keycodes");
_Static_assert(ALTTAB_LAYER < DYNAMIC_KEYMAP_LAYER_COUNT, "Smart Alt+Tab needs a dynamic layer");

static bool smart_alt_tab_active;
static bool smart_alt_tab_physical_alt;
static uint16_t smart_alt_tab_timer;
static uint8_t smart_alt_tab_keys[MATRIX_ROWS][(MATRIX_COLS + 7) / 8];
static uint16_t smart_alt_tab_held;

static void smart_alt_tab_finish(void) {
    smart_alt_tab_active = false;
    smart_alt_tab_held = 0;
    memset(smart_alt_tab_keys, 0, sizeof(smart_alt_tab_keys));
    layer_off(ALTTAB_LAYER);
    if (!smart_alt_tab_physical_alt) {
        unregister_code(KC_LALT);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_LALT) {
        smart_alt_tab_physical_alt = record->event.pressed;
    }
    if (keycode == ALT_TAB && record->event.pressed) {
        if (!smart_alt_tab_active) {
            smart_alt_tab_active = true;
            layer_on(ALTTAB_LAYER);
        }
        register_code(KC_LALT);
        tap_code(KC_TAB);
    }

    if (smart_alt_tab_active) {
        const uint8_t row = record->event.key.row;
        const uint8_t col = record->event.key.col;
        // Encoder pseudo positions are outside the matrix and emit paired events.
        if (row < MATRIX_ROWS && col < MATRIX_COLS) {
            uint8_t *keys = &smart_alt_tab_keys[row][col / 8];
            const uint8_t mask = 1U << (col % 8);
            if (record->event.pressed && !(*keys & mask)) {
                *keys |= mask;
                ++smart_alt_tab_held;
            } else if (!record->event.pressed && (*keys & mask)) {
                *keys &= ~mask;
                --smart_alt_tab_held;
            }
        }
        smart_alt_tab_timer = timer_read();
    }
    // The physical Alt release must not clear the Alt held by this session.
    return keycode != ALT_TAB && !(smart_alt_tab_active && keycode == KC_LALT);
}

void matrix_scan_user(void) {
    if (smart_alt_tab_active && (!layer_state_is(ALTTAB_LAYER) || (!smart_alt_tab_held && timer_elapsed(smart_alt_tab_timer) > 800))) {
        smart_alt_tab_finish();
    }
}
