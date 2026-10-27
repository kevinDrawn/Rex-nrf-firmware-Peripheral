#pragma once

// SAADC + PPI
#include <nrfx_saadc.h>
#include <nrfx_timer.h>
#include <helpers/nrfx_gppi.h>
#include <nrfx_ppi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h> 
#include <esb_b.h>
#include <hall_sensor.h>
#include "afe_thread.h"
#include "hall_sensor.h"

/* ===========================
 * TIMER2 인스턴스 선언
 * =========================== */
#define TIMER_INSTANCE_NUMBER 1

#if defined(CONFIG_HALL_MODE) && defined(CONFIG_CAN_MODE)
#define SAADC_BUFFER_SIZE 4
#elif defined(CONFIG_HALL_MODE)
#define SAADC_BUFFER_SIZE 2
#endif

#define SAADC_SAMPLE_INTERVAL_US 1000                     // 샘플링 주기: 1000us = 1ms

#define SAADC_INPUT_PIN_1 NRF_SAADC_INPUT_AIN0            // ADC 채널 0 입력 핀
#define SAADC_INPUT_PIN_2 NRF_SAADC_INPUT_AIN3            // ADC 채널 1 입력 핀
#define SAADC_INPUT_PIN_3 NRF_SAADC_INPUT_AIN1            // ADC 채널 2 입력 핀
#define SAADC_INPUT_PIN_4 NRF_SAADC_INPUT_AIN2            // ADC 채널 3 입력 핀

#define M_PI 3.141592

/* SAADC 채널 설정 */
static nrfx_saadc_channel_t channel_Hall_L   = NRFX_SAADC_DEFAULT_CHANNEL_SE(SAADC_INPUT_PIN_1, 0);  // 채널 0 구성
static nrfx_saadc_channel_t channel_Hall_R = NRFX_SAADC_DEFAULT_CHANNEL_SE(SAADC_INPUT_PIN_2, 1);  // 채널 1 구성
static nrfx_saadc_channel_t channel_Can_L   = NRFX_SAADC_DEFAULT_CHANNEL_SE(SAADC_INPUT_PIN_3, 2);  // 채널 0 구성
static nrfx_saadc_channel_t channel_Can_R = NRFX_SAADC_DEFAULT_CHANNEL_SE(SAADC_INPUT_PIN_4, 3);  // 채널 1 구성

/* ===========================
 * SAADC 버퍼 및 데이터 저장소
 * =========================== */
void configure_ppi(void);
void configure_saadc(void);
void configure_timer(void);
void adc_streaming_start(void);

// AFE Debugging
uint32_t adc_get_emg_underflow_count(void);
uint32_t adc_get_emg_sequence_error_count(void);
uint32_t adc_get_esb_packet_count(void);
uint32_t adc_get_emg_rc1_fail_count(void);
uint32_t adc_get_emg_rc2_fail_count(void);

// ESB Debugging
uint32_t get_saadc_work_count(void);
uint32_t get_esb_write_ok_count(void);
uint32_t get_esb_enomem_count(void);
uint32_t get_esb_retry_fail_count(void);