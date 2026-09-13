/* Copyright 2017 Joshua Broekhuijsen <snipeye+qmk@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2021 Dasky (@daskygit)
 * Copyright 2024 Geek-rabb1t (@geek-rabb1t)
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
#include "i2c_master.h"
#include "azoteq_iqs5xx.h"
#include "pointing_device_internal.h"
#include "pointing_device.h"
#include "debug.h"
#include "wait.h"
#include "timer.h"
#include "gr_trackpad65_driver.h"
#include "gr_trackpad65_cursor_corrector.h"
#include "rotate_cursor.h"
#include "gesture/gr_trackpad65_state.h"
#include <math.h>

trackpad_event_t trackpad_event = {
    .type = trackpad_event_none,
    .num_of_fingers = 0
};

static i2c_status_t azoteq_iqs5xx_init_status = 1;

void pointing_device_driver_init(void) {
    i2c_init();
    azoteq_iqs5xx_wake();
    azoteq_iqs5xx_reset_suspend(true, false, true);
    wait_ms(100);
    azoteq_iqs5xx_wake();
    if (azoteq_iqs5xx_get_product() != AZOTEQ_IQS5XX_UNKNOWN) {
        azoteq_iqs5xx_setup_resolution();
        azoteq_iqs5xx_init_status = azoteq_iqs5xx_set_report_rate(AZOTEQ_IQS5XX_REPORT_RATE, AZOTEQ_IQS5XX_ACTIVE, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_event_mode(false, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_reati(true, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_xy_config(false, false, false, true, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_gesture_config(true);
        wait_ms(AZOTEQ_IQS5XX_REPORT_RATE + 1);
    }
};

static report_mouse_t report(azoteq_iqs5xx_base_data_t base_data) {
    trackpad_base_data_t trackpad_data = cursor_corrector_correct(base_data);
     trackpad_base_data_t rotated = gr_trackpad65_rotate_cursor(trackpad_data);
    return trackpad_report(rotated);
}

static uint16_t count = 0;
static uint16_t rateTimer = 0;
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    report_mouse_t temp_report           = {0};
    static uint8_t previous_button_state = 0;
    static uint8_t read_error_count      = 0;
    count++;
    if (rateTimer == 0) {
        rateTimer = timer_read();
        count = 0;
    }
    if (timer_elapsed(rateTimer) >= 10000) {
        int elapsed = timer_elapsed(rateTimer);
        uprintf("rate %d, %d, %d\n", count / 10, count, elapsed);
        rateTimer = timer_read();
        count = 0;
    }

    if (azoteq_iqs5xx_init_status == I2C_STATUS_SUCCESS) {
        azoteq_iqs5xx_base_data_t base_data = {0};
        i2c_status_t status = azoteq_iqs5xx_get_base_data(&base_data);

        if (status == I2C_STATUS_SUCCESS) {
            read_error_count = 0;
            temp_report = report(base_data);
            previous_button_state = temp_report.buttons;

        } else {
            if (read_error_count > 10) {
                read_error_count      = 0;
                previous_button_state = 0;
            } else {
                read_error_count++;
            }
            temp_report.buttons = previous_button_state;
        }
    } else {
        pd_dprintf("IQS5XX - Init failed: %d \n", azoteq_iqs5xx_init_status);
    }

    return temp_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return azoteq_iqs5xx_get_cpi();
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    azoteq_iqs5xx_set_cpi(cpi);
}

void reset_trackpad_event(void) {
    trackpad_event.type = trackpad_event_none;
    trackpad_event.num_of_fingers = 0;
}
