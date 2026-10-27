/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>

#include "adc_sample.h"
#include "esb_b.h"
#include "ads1294.h"
#include "rex_ads1294_config.h"
#include "afe_thread.h"
#include "gpio_led.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

const struct device *test_uart =
    DEVICE_DT_GET(DT_NODELABEL(uart0));

int main(void)
{
    int err;

	// HCLK 시작
	err = clocks_start();
	if (err) {
		return 0;
	}else{
        LOG_INF("HCLK Start");
    }

	// // UART 초기화
	// int rc = uart_bridge_init();
    // if (rc) {
    //     LOG_ERR("uart_bridge_init rc=%d\n", rc);
    //     return -1;  /* 실패 시 이후 경로 스킵 */
    // }else{
    //     LOG_INF("UART Bridge Init Clear");
    // }

	// ESB 초기화
	err = esb_initialize();
	if (err) {
		LOG_ERR("ESB initialization failed, err %d", err);
		return 0;
	}else{
        LOG_INF("ESB Init Clear");
    }

	/* Initialize SAADC sampling system */
	configure_timer();
	configure_saadc();
	configure_ppi();

    // Hall Sensor Look up table
    hall_lut_init();
    k_sleep(K_MSEC(500));   // CPU 낭비 방지   

	// AFE 초기화 (ADS 1294)
    LOG_INF("Rex AFE firmware start");
    err = ads1294_init(REX_ADS_ACTIVE_MODE);
    if (err < 0) {
        LOG_WRN("AFE unavailable: err=%d; continuing without AFE", err);
    }

    // AFE DRDY 감지 Thread 시작
    afe_thread_start();
    
    // 1 kHz Work Que Start Flag
    adc_streaming_start();

	tx_payload.noack = true;

    while (1) {
        k_sleep(K_SECONDS(1));

        // AFE + Que Debugging
        // LOG_INF("AFE=%u q_drop=%u q_used=%u "
        //         "under=%u seq=%u",
        //         afe_get_sample_count(),
        //         afe_get_queue_drop_count(),
        //         afe_get_queue_used(),
        //         adc_get_emg_underflow_count(),
        //         adc_get_emg_sequence_error_count());

        // ESB Debugging
        // LOG_INF("SAADC=%u ESB_WR=%u ENOMEM=%u RETRY_FAIL=%u TX_OK=%u TX_FAIL=%u",
        //         get_saadc_work_count(),
        //         get_esb_write_ok_count(),
        //         get_esb_enomem_count(),
        //         get_esb_retry_fail_count(),
        //         get_esb_tx_success_count(),
        //         get_esb_tx_failed_count());
    }
}
