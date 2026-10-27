#pragma once
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

/* ============================================
 * GPIO LED Blink Module
 * ============================================ */

/* LED 노드 정의 (DeviceTree의 led2 alias 사용) */
#define LED0_NODE DT_ALIAS(led0)

#define HAS_LED        1
#define BLINK_INTERVAL K_MSEC(1000)   // 500 ms 주기로 깜빡임

#define STATE_CONNECTED    1U
#define STATE_DISCONNECTED 2U

// ATOMIC_DEFINE(state, 2U);

extern const struct gpio_dt_spec led;

int blink_setup(void);
void blink_start(void);
void blink_stop(void);