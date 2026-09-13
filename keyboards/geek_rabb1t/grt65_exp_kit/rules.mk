# This file intentionally left blank

POINTING_DEVICE_ENABLE = yes
# MOUSEKEY_ENABLE = yes
POINTING_DEVICE_DRIVER = custom

# ENCODER_MAP_ENABLE = yes
I2C_DRIVER_REQUIRED = yes

SRC += ../trackpad/azoteq_iqs5xx.c
SRC += ../trackpad/gr_trackpad65.c
SRC += ../trackpad/gr_trackpad65_driver.c
SRC += ../trackpad/gr_trackpad65_config.c
SRC += ../trackpad/gr_trackpad65_cursor_corrector.c
SRC += ../trackpad/gesture/gr_trackpad65_gesture_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_gesture_fire_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_idle_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_inertia_cursor_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_inertia_scroll_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_move_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_press_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_scroll_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_state.c
SRC += ../trackpad/gesture/gr_trackpad65_strategy_helper.c
SRC += ../trackpad/gesture/gr_trackpad65_touch_strategy.c
SRC += ../trackpad/gesture/gr_trackpad65_wait_strategy.c
SRC += ../trackpad/rotate_cursor.c
