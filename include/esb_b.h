#pragma once

#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/nrf_clock_control.h>
#if defined(NRF54L15_XXAA)
#include <hal/nrf_clock.h>
#endif /* defined(NRF54L15_XXAA) */
#include <zephyr/drivers/gpio.h>
#include <zephyr/irq.h>
#include <zephyr/logging/log.h>
#include <nrf.h>
#include <esb.h>
#include <stdint.h>
#include <limits.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <dk_buttons_and_leds.h>
#include <afe_thread.h>
#if defined(CONFIG_CLOCK_CONTROL_NRF2)
#include <hal/nrf_lrcconf.h>
#endif

// Nordic Radio Hardware Shortcuts 설정 - 특정 이벤트 발생 시, 자동으로 다른 태스크 실행하도록 묶는 기능
#define _RADIO_SHORTS_COMMON                                                   \
	(RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk |         \
	 RADIO_SHORTS_ADDRESS_RSSISTART_Msk |                                  \
	 RADIO_SHORTS_DISABLED_RSSISTOP_Msk)

// Two consecutive time samples per packet, raw signed int32 EMG.
// Preserve the original first 28 bytes; append right EMG in bilateral mode.
#if defined(CONFIG_EMG_BILATERAL)
#define SENSOR_PAYLOAD_LEN (12 + 16 + 16)
#else
#define SENSOR_PAYLOAD_LEN (12 + 16)
#endif

#if SENSOR_PAYLOAD_LEN > CONFIG_ESB_MAX_PAYLOAD_LENGTH
#error "ESB buffer is too small for the selected EMG mode"
#endif

#define PAYLOAD_FLAG_EMG_UNDERFLOW      BIT(0)
#define PAYLOAD_FLAG_EMG_SEQUENCE_ERROR BIT(1)

// 수신된 buffer를 저장하는 버퍼 - ESP API가 제공하는 기본 payload 구조체
extern struct esb_payload rx_payload;
extern struct esb_payload tx_payload;
extern bool ready;

void make_payload(int16_t left_angle, int16_t right_angle, int16_t left_torque, int16_t right_torque);
void make_payload_emg(int16_t left_angle,
                      int16_t right_angle,
                      int16_t left_torque,
                      int16_t right_torque,
					  uint8_t flags,
                      const emg_sample_t *emg1,
                      const emg_sample_t *emg2);

int clocks_start(void);
int esb_initialize(void);

uint32_t get_esb_tx_success_count(void);
uint32_t get_esb_tx_failed_count(void);