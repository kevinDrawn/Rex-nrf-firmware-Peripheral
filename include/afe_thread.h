#ifndef AFE_THREAD_H
#define AFE_THREAD_H

#include <stdint.h>
#include <zephyr/kernel.h>
#include <uart_bridge.h>

typedef struct {
    uint32_t sequence;
    int32_t ch1_l_tibialis;
    int32_t ch2_l_gastrocnemius;
#if defined(CONFIG_EMG_BILATERAL)
    int32_t ch3_r_tibialis;
    int32_t ch4_r_gastrocnemius;
#endif
} emg_sample_t;

void afe_thread_start(void);
int afe_thread_get_sample(emg_sample_t *sample, k_timeout_t timeout);

uint32_t afe_get_sample_count(void);
uint32_t afe_get_read_error_count(void);
uint32_t afe_get_queue_drop_count(void);
uint32_t afe_get_queue_used(void);

#endif /* AFE_THREAD_H */