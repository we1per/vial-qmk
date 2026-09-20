#!/usr/bin/env python3
"""Host tests of the actual Smart Alt+Tab header and quadrature driver.

Run from the repository root with Python and a host C compiler on PATH:
    python keyboards/geek_rabb1t/futaba/tests/integration_test.py
Hardware, USB delivery and the shared trackpad algorithms are not emulated.
"""
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[4]
FUTABA = ROOT / "keyboards/geek_rabb1t/futaba"
CC = os.environ.get("CC", "gcc")

SMART = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define MATRIX_ROWS 16
#define MATRIX_COLS 9
#define ALTTAB_LAYER 7
#define DYNAMIC_KEYMAP_LAYER_COUNT 8
enum mock_keycode_ranges { QK_KB_MAX = 0x7e3f };
enum mock_trackpad_keycodes { GRTL_4T = 0x7e1f };
#define KC_LALT 0xe2
#define KC_TAB 0x2b
typedef struct { struct { struct { uint8_t row, col; } key; bool pressed; } event; } keyrecord_t;
static uint16_t now;
static bool alt;
static uint8_t layers;
static unsigned tabs;
static uint16_t timer_read(void) { return now; }
static uint16_t timer_elapsed(uint16_t t) { return (uint16_t)(now - t); }
static void register_code(uint8_t k) { assert(k == KC_LALT); alt = true; }
static void unregister_code(uint8_t k) { assert(k == KC_LALT); alt = false; }
static void tap_code(uint8_t k) { assert(k == KC_TAB && alt); ++tabs; }
static void layer_on(uint8_t l) { layers |= 1U << l; }
static void layer_off(uint8_t l) { layers &= ~(1U << l); }
static bool layer_state_is(uint8_t l) { return layers & (1U << l); }
#include "smart_alt_tab.h"
static void key(uint16_t code, uint8_t row, uint8_t col, bool down) {
    keyrecord_t r = {.event = {.key = {row, col}, .pressed = down}};
    bool forward = process_record_user(code, &r);
    assert(forward == (code != ALT_TAB && !(smart_alt_tab_active && code == KC_LALT)));
    if (forward && code == KC_LALT) {
        if (down) register_code(code); else unregister_code(code);
    }
}
static void tick(uint16_t dt) { now += dt; matrix_scan_user(); }
static void start(void) { key(ALT_TAB, 0, 0, true); assert(alt && layer_state_is(7)); }
static void finish(void) { assert(!alt && !layer_state_is(7) && !smart_alt_tab_active); }
int main(void) {
    assert(ALT_TAB == 0x7e20);
    start(); tick(2000); assert(alt); // Long hold never times out.
    key(ALT_TAB, 0, 0, false); tick(800); assert(alt); tick(1); finish();
    start(); key(ALT_TAB, 0, 0, false); tick(700);
    start(); key(ALT_TAB, 0, 0, false); tick(700); assert(alt); tick(101); finish();
    start(); key(1, 0, 1, true); key(ALT_TAB, 0, 0, false);
    tick(1000); assert(alt); key(1, 0, 1, false); tick(801); finish();
    start(); key(1, 0, 1, false); tick(1000); assert(alt); // Release of a pre-session key.
    key(ALT_TAB, 0, 0, false); tick(801); finish();
    start(); key(ALT_TAB, 0, 0, true); key(ALT_TAB, 0, 0, false);
    tick(801); finish(); // Duplicate press cannot inflate held count.
    start(); layers = 0; tick(0); finish(); // External layer exit while held.
    key(ALT_TAB, 0, 0, false); finish();
    now = 65500; start(); key(ALT_TAB, 0, 0, false); tick(801); finish();
    start(); key(ALT_TAB, 0, 0, false); tick(700);
    key(1, 254, 0, true); key(1, 254, 0, false); tick(700); assert(alt); tick(101); finish();
    start(); key(1, 12, 4, true); key(ALT_TAB, 0, 0, false);
    tick(1000); assert(alt); key(1, 12, 4, false); tick(801); finish();
    start(); key(KC_LALT, 0, 2, true); key(KC_LALT, 0, 2, false);
    assert(alt); key(ALT_TAB, 0, 0, false); tick(801); finish();
    key(KC_LALT, 0, 2, true); start(); key(ALT_TAB, 0, 0, false);
    tick(801); assert(alt && !smart_alt_tab_active && !layer_state_is(7));
    key(KC_LALT, 0, 2, false); finish();
    assert(tabs == 13);
    puts("Smart Alt+Tab: hold, repeat, overlap, duplicate/unmatched events, layer exit, timer wrap, encoder and gesture PASS");
}
'''

ENCODER = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define NUM_ENCODERS 2
#define ENCODER_RESOLUTION 4
static unsigned events[2][2];
void encoder_queue_event(uint8_t index, bool clockwise) { assert(index < 2); ++events[index][clockwise]; }
#include "driver_source.h"
void encoder_quadrature_init_pin(uint8_t index, bool pad_b) { (void)index; (void)pad_b; }
uint8_t encoder_quadrature_read_pin(uint8_t index, bool pad_b) { (void)index; (void)pad_b; return 0; }
static void reset(void) {
    memset(events, 0, sizeof(events));
    memset(encoder_pulses, 0, sizeof(encoder_pulses));
    encoder_driver_init();
}
static void pulses(uint8_t i, int n) {
    while (n > 0) { encoder_handle_state_change(i, 2); --n; }
    while (n < 0) { encoder_handle_state_change(i, 1); ++n; }
}
int main(void) {
    reset();
#ifdef ENCODER_EVENT_OFFSET
    const int threshold = 4 - ENCODER_EVENT_OFFSET;
    pulses(0, threshold - 1); assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 0);
    pulses(0, 1); assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 1);
    pulses(0, 4); assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 2);
    reset(); pulses(0, threshold); pulses(0, -threshold);
    assert(events[0][ENCODER_CLOCKWISE] == 1); // Reverse directly after early event.
    reset(); pulses(0, 4); pulses(0, -threshold);
    assert(events[0][ENCODER_CLOCKWISE] == 1); // Reverse after final detent pulse.
    reset(); pulses(0, threshold); encoder_handle_state_change(0, 0); pulses(0, -threshold);
    assert(events[0][ENCODER_CLOCKWISE] == 1); // Invalid transition preserves pending event.
    reset(); pulses(0, threshold); pulses(1, 1); pulses(0, -threshold);
    assert(events[0][ENCODER_CLOCKWISE] == 1 && events[1][ENCODER_COUNTER_CLOCKWISE] == 0);
    reset(); pulses(0, threshold - 1); pulses(0, -(threshold - 1));
    assert(events[0][0] == 0 && events[0][1] == 0); // Partial turn cancels before any event.
#else
    pulses(0, 3); assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 0);
    pulses(0, 1); assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 1);
    pulses(0, -4); assert(events[0][ENCODER_CLOCKWISE] == 1);
    reset(); pulses(0, 3); pulses(0, -3); assert(events[0][0] == 0 && events[0][1] == 0);
#endif
    reset(); // Exercise public pin-state decoding, not only the pulse accumulator.
    const uint8_t states[] = {2, 3, 1, 0, 2, 3, 1, 0};
    for (unsigned j = 0; j < sizeof(states); ++j) {
        encoder_quadrature_handle_read(0, states[j] & 1, states[j] >> 1);
    }
    assert(events[0][ENCODER_COUNTER_CLOCKWISE] == 2 && events[0][ENCODER_CLOCKWISE] == 0);
    puts("Encoder: event timing, continuous turns, reversals, cancellation, independent encoders and pin decoding PASS");
}
'''

CONFIG_HEADER = r'''
#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define QK_KB_0 0x7e00
#include "gr_trackpad65_config.h"
static unsigned reads;
static uint16_t config_key(uint8_t layer, uint8_t row, uint8_t col) {
    assert(layer == 0 && row == 14 && col < 10);
    ++reads;
    const uint16_t values[] = {GRT_VAL_5, GRT_VAL_4, GRT_VAL_2, GRT_ON, GRT_OFF, GRT_ON, GRT_ON, GRT_OFF, GRT_OFF, GRT_R90};
    return values[col];
}
#ifdef DYNAMIC_KEYMAP_ENABLE
static uint16_t dynamic_keymap_get_keycode(uint8_t l, uint8_t r, uint8_t c) { return config_key(l, r, c); }
#else
static uint16_t keycode_at_keymap_location(uint8_t l, uint8_t r, uint8_t c) { return config_key(l, r, c); }
#endif
'''

CONFIG = r'''
#include "config_source.h"
int main(void) {
    load_gr_trackpad65_config(0, 14, false);
    assert(reads == 9);
    assert(gr_trackpad_config.cursor_speed == 8 && gr_trackpad_config.scroll_speed == 5);
    assert(gr_trackpad_config.cursor_correct == 2 && gr_trackpad_config.enable_accel);
    assert(!gr_trackpad_config.reverse_vertical_scroll && gr_trackpad_config.reverse_horizontal_scroll);
    assert(gr_trackpad_config.inertia_cursor && !gr_trackpad_config.inertia_scroll);
    assert(!gr_trackpad_config.scroll_only && !gr_trackpad_config.scroll_mode);
    assert(gr_trackpad_config.rotate == ROTATE_0);
    load_gr_trackpad65_config(0, 14, true);
    assert(reads == 19 && gr_trackpad_config.rotate == ROTATE_90);
    puts("Shared trackpad: dynamic configuration and all settings PASS");
}
'''


def run(*args):
    subprocess.run(args, cwd=ROOT, check=True)


def main():
    # Validate JSON mapping against the actual shared C enum, including numeric range.
    definition = json.loads((FUTABA / "keymaps/vial/vial.json").read_text())
    keyboard = json.loads((FUTABA / "keyboard.json").read_text())
    enum = (ROOT / "keyboards/geek_rabb1t/trackpad/gr_trackpad65_config.h").read_text()
    enum = enum.split("enum GR_TRACKPAD_KEYCODES {")[1].split("}")[0]
    enum = re.sub(r"//[^\n]*", "", enum)
    names = [part.strip().split("=")[0].strip() for part in enum.split(",") if part.strip()]
    assert [k["name"] for k in definition["customKeycodes"]] == names + ["ALT_TAB"]
    assert len(names) == 32 and len(names) < 64
    assert definition["matrix"] == {"rows": 16, "cols": 9}
    assert len(keyboard["matrix_pins"]["rows"]) == 16
    assert len(keyboard["matrix_pins"]["cols"]) == 9
    assert keyboard["encoder"]["rotary"][0]["resolution"] == 4
    print("Vial JSON / shared C enum / matrix / encoder resolution PASS", flush=True)
    output = ROOT / ".build/integration-tests"
    output.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=output) as temp:
        tmp = Path(temp)
        (tmp / "smart.c").write_text(SMART)
        run(CC, "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(FUTABA / "keymaps/vial"), str(tmp / "smart.c"), "-o", str(tmp / "smart.exe"))
        run(str(tmp / "smart.exe"))
        for header in ("encoder.h", "gpio.h", "keyboard.h", "action.h", "keycodes.h", "wait.h"):
            (tmp / header).write_text("#pragma once\n#include <stdbool.h>\n")
        (tmp / "encoder.c").write_text(ENCODER)
        driver = ROOT / "drivers/encoder/encoder_quadrature.c"
        (tmp / "driver_source.h").write_text(driver.read_text())
        for label, defines in (("offset1", ["-DENCODER_EVENT_OFFSET=1"]), ("offset0", ["-DENCODER_EVENT_OFFSET=0"]), ("offset1_flip", ["-DENCODER_EVENT_OFFSET=1", "-DENCODER_DIRECTION_FLIP"]), ("unconfigured", []), ("default_position_unconfigured", ["-DENCODER_DEFAULT_POS=0"]), ("per_encoder_resolutions", ["-DENCODER_RESOLUTIONS={4,4}", "-DENCODER_EVENT_OFFSET=1"])):
            print(label, flush=True)
            executable = tmp / (label + ".exe")
            run(CC, "-std=c11", "-Wall", "-Wextra", "-Werror", *defines, "-I", str(tmp), str(tmp / "encoder.c"), "-o", str(executable))
            run(str(executable))
        # Same host assertions must pass against the unchanged upstream driver.
        upstream = subprocess.check_output(["git", "show", "upstream/gr_master:drivers/encoder/encoder_quadrature.c"], cwd=ROOT, text=True)
        (tmp / "driver_source.h").write_text(upstream)
        run(CC, "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(tmp), str(tmp / "encoder.c"), "-o", str(tmp / "upstream.exe"))
        run(str(tmp / "upstream.exe"))
        (tmp / "qmk_test.h").write_text(CONFIG_HEADER)
        (tmp / "config.c").write_text(CONFIG)
        for name in ("dynamic_keymap.h", "keymap_introspection.h"):
            (tmp / name).write_text('#pragma once\n#include "qmk_test.h"\n')
        shared = ROOT / "keyboards/geek_rabb1t/trackpad"
        (tmp / "config_source.h").write_text((shared / "gr_trackpad65_config.c").read_text())
        # Match QMK's warning set for existing shared code (no -Wextra).
        config_flags = ["-std=c11", "-Wall", "-Werror", '-DQMK_KEYBOARD_H="qmk_test.h"', "-I", str(tmp), "-I", str(shared)]
        for name, defines in (("dynamic", ["-DDYNAMIC_KEYMAP_ENABLE"]),):
            executable = tmp / (name + ".exe")
            run(CC, *config_flags, *defines, str(tmp / "config.c"), "-o", str(executable))
            run(str(executable))
        # Reproduce the upstream default-keymap failure with the original source.
        original = subprocess.check_output(["git", "show", "upstream/gr_master:keyboards/geek_rabb1t/trackpad/gr_trackpad65_config.c"], cwd=ROOT, text=True)
        (tmp / "config_source.h").write_text(original)
        failure = subprocess.run([CC, *config_flags, str(tmp / "config.c"), "-o", str(tmp / "original_config.exe")], capture_output=True, text=True)
        assert failure.returncode != 0 and "dynamic_keymap_get_keycode" in failure.stderr
        print("Upstream non-dynamic configuration build failure reproduced (expected)", flush=True)
    print("All integration host tests PASS", flush=True)


if __name__ == "__main__":
    main()
