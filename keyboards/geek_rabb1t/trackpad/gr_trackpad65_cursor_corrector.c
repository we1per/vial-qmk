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

#include "quantum.h"
#include "azoteq_iqs5xx.h"
#include "gr_trackpad65_driver.h"
#include "timer.h"
#include <math.h>
#include "gr_trackpad65_config.h"
#include "gr_trackpad65_cursor_corrector.h"
#include "debug.h"
#include "pointing_device.h"

#define CUROSOR_MIN -1000
#define CUROSOR_MAX 1000
#define CONSTRAIN_CURSOR(amt) ((amt) < CUROSOR_MIN ? CUROSOR_MIN : ((amt) > CUROSOR_MAX ? CUROSOR_MAX : (amt)))

typedef struct {
    int x;
    int y;
    int strength;
} position_with_strength_t;

int get_touch_strength(azoteq_iqs5xx_base_data_t base_data) {
    int fingers[5] = {
        AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_1.touch_strength.h, base_data.finger_1.touch_strength.l),
        AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_2.touch_strength.h, base_data.finger_2.touch_strength.l),
        AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_3.touch_strength.h, base_data.finger_3.touch_strength.l),
        AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_4.touch_strength.h, base_data.finger_4.touch_strength.l),
        AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_5.touch_strength.h, base_data.finger_5.touch_strength.l)
    };
    int max = 0;
    for (int i = 0; i < 5; i++) {
        if (fingers[i] > max) {
            max = fingers[i];
        }
    }
    return max;
}


static position_t prev_positions[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
static int cursor_finger_num = 0;
static uint16_t timer;
// 指が 0本から 1本以上に変わった場合、一定サイクルは座標の変更を無視したほうがよさそう。
void get_finger_delta(azoteq_iqs5xx_base_data_t base_data, position_t *delta) {
    position_with_strength_t fingers[5] = {
        {
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_1.absolute_x.h, base_data.finger_1.absolute_x.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_1.absolute_y.h, base_data.finger_1.absolute_y.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_1.touch_strength.h, base_data.finger_1.touch_strength.l),
        },
        {
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_2.absolute_x.h, base_data.finger_2.absolute_x.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_2.absolute_y.h, base_data.finger_2.absolute_y.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_2.touch_strength.h, base_data.finger_2.touch_strength.l),
        },
        {
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_3.absolute_x.h, base_data.finger_3.absolute_x.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_3.absolute_y.h, base_data.finger_3.absolute_y.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_3.touch_strength.h, base_data.finger_3.touch_strength.l),
        },
        {
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_4.absolute_x.h, base_data.finger_4.absolute_x.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_4.absolute_y.h, base_data.finger_4.absolute_y.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_4.touch_strength.h, base_data.finger_4.touch_strength.l),
        },
        {
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_5.absolute_x.h, base_data.finger_5.absolute_x.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_5.absolute_y.h, base_data.finger_5.absolute_y.l),
            AZOTEQ_IQS5XX_COMBINE_H_L_BYTES(base_data.finger_5.touch_strength.h, base_data.finger_5.touch_strength.l),
        },
    };

    position_t deltas[5] = {0};
    bool first_touch = true;
    for (int i = 0; i < 5; i++) {
        if (fingers[i].strength == 0 ) {
            fingers[i].x = -1;
            fingers[i].y = -1;
            prev_positions[i].x = -1;
            prev_positions[i].y = -1;
            continue;
        }

        if (prev_positions[i].x != -1) {
            first_touch = false;
            // Limit the maximum movement.
            int x = fingers[i].x - prev_positions[i].x;
            x = x > 300 ? 300 : x < -300 ? -300 : x;
            deltas[i].x = x;

            int y = fingers[i].y - prev_positions[i].y;
            y = y > 300 ? 300 : y < -300 ? -300 : y;
            deltas[i].y = y;
        }

        prev_positions[i].x = fingers[i].x;
        prev_positions[i].y = fingers[i].y;
    }

    if (base_data.number_of_fingers > 0 && first_touch) {
        timer = timer_read();
    }

    if (base_data.number_of_fingers == 1 && timer_elapsed(timer) < GR_TRACKPAD65_WAIT_FOR_CURSOR_MOVEMENT) {
        delta->x = 0;
        delta->y = 0;
        return;
    }

    if (base_data.number_of_fingers >= 2 && timer_elapsed(timer) < GR_TRACKPAD65_WAIT_FOR_MULTI_TAP_CURSOR_MOVEMENT) {
        delta->x = 0;
        delta->y = 0;
        return;
    }

    // 前回のサイクルでカーソル移動に使った指を優先する。
    if (cursor_finger_num != -1 && (abs(deltas[cursor_finger_num].x) + abs(deltas[cursor_finger_num].y) > 2)) {
        delta->x = deltas[cursor_finger_num].x;
        delta->y = deltas[cursor_finger_num].y;
        return;
    }

    // カーソル飛び対策として、指が変わった直後は移動量を無視する。
    for (int i = 0; i < 5; i++) {
        if (abs(deltas[i].x) + abs(deltas[i].y) > 2) {
            delta->x = 0;
            delta->y = 0;
            cursor_finger_num = i;
            return;
        }
    }

    delta->x = 0;
    delta->y = 0;
    cursor_finger_num = -1;

    return;
}

typedef struct {
    mouse_xy_report_t mov;
    int carryover;
} correct_cursor_result_t;

correct_cursor_result_t correct_cursor(int delta, int history[], int carryover) {

    int mov = 0;
    int next_carry = 0;

    // 本当は移動量の平均を求めたいんだけど、演算誤差を減らすため、割り算は後で行う。
    int sum = delta;
    for (int i = 0 ; i < gr_trackpad_config.cursor_correct - 1; i++) {
        sum += history[i];
    }

    if (gr_trackpad_config.enable_accel) {
        int accel = (abs(sum));
        mov = (carryover + sum * (gr_trackpad_config.cursor_speed) * accel) / (100 * gr_trackpad_config.cursor_correct);
        next_carry = (carryover + sum * (gr_trackpad_config.cursor_speed * accel)) % (100 * gr_trackpad_config.cursor_correct);

        // 最大・最小スピードを制限する
        int max_cursor_speed = gr_trackpad_config.cursor_speed * 100;
        int min_cursor_speed = max_cursor_speed * -1;
        mov = ((mov < min_cursor_speed) ? min_cursor_speed : ((mov > max_cursor_speed) ? max_cursor_speed : mov));

    } else {
        int total = (carryover + sum * (gr_trackpad_config.cursor_speed));
        mov = total / (10 * gr_trackpad_config.cursor_correct);
        next_carry = total - (mov * 10 * gr_trackpad_config.cursor_correct);
    }

    correct_cursor_result_t result = {
        .mov = CONSTRAIN_HID_XY(CONSTRAIN_CURSOR((int)mov)),
        .carryover = next_carry
    };

    return result;
}

#define HISTORY_LENGTH 10
static int history_x[HISTORY_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int history_y[HISTORY_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static position_t prev_report = {
    .x = 0,
    .y = 0
};
static position_t carryover = {
    .x = 0,
    .y = 0
};

void update_history(position_t position) {
    for (int i = HISTORY_LENGTH - 1; i >= 1; i--) {
        history_x[i] = history_x[i - 1];
        history_y[i] = history_y[i - 1];
    }
    history_x[0] = position.x;
    history_y[0] = position.y;
}

trackpad_base_data_t cursor_corrector_correct(azoteq_iqs5xx_base_data_t base_data) {

    position_t position = {0};
    get_finger_delta(base_data, &position);

    correct_cursor_result_t cursor_x = correct_cursor(position.x, history_x, carryover.x);
    correct_cursor_result_t cursor_y = correct_cursor(position.y, history_y, carryover.y);

    trackpad_base_data_t trackpad_data = {
        .pos.x = position.x,
        .pos.y = position.y,
        .prev_pos.x = history_x[0],
        .prev_pos.y = history_y[0],

        .mouse_report_x = cursor_x.mov,
        .mouse_report_y = cursor_y.mov,
        .prev_report_x = prev_report.x,
        .prev_report_y = prev_report.y,
        .touch_strength = get_touch_strength(base_data),
        .num_of_fingers = base_data.number_of_fingers,
    };

    update_history(position);

    prev_report.x = cursor_x.mov;
    prev_report.y = cursor_y.mov;
    carryover.x = cursor_x.carryover;
    carryover.y = cursor_y.carryover;

    return trackpad_data;
}
