#pragma once

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define UART_HEADER_SIZE       2
#define UART_EMG_PAYLOAD_SIZE  8 + 8 // L/R EMG (TA and GM)
#define UART_FRAME_LEN         (UART_HEADER_SIZE + UART_EMG_PAYLOAD_SIZE)

extern const struct device *uart_dev;

/* UART TX queue */
extern struct k_msgq uart_tx_q;

int uart_bridge_init(void);
void uart_tx_kick(void);