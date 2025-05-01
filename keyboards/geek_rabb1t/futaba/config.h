// Copyright 2024 geek-rabb1t (@geek-rabb1t)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

#define AZOTEQ_IQS5XX_TPS65

#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3

#define MOUSE_EXTENDED_REPORT

// #define POINTING_DEVICE_DEBUG

// #define ENCODER_RESOLUTION 4
// #define ENCODER_DEFAULT_POS 0x0
#define ALTTAB_LAYER 7  // Smart Alt+Tab専用のLayer
#define ENCODER_EVENT_OFFSET 1  // 何パルス前にイベント送信するか
