#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ads1294.h"
#include "afe_thread.h"

LOG_MODULE_REGISTER(afe_thread, LOG_LEVEL_INF);

#define AFE_THREAD_STACK_SIZE 1024
#define AFE_THREAD_PRIORITY   5
#define EMG_QUEUE_LEN         64

K_MSGQ_DEFINE(emg_queue, sizeof(emg_sample_t), EMG_QUEUE_LEN, 4);
K_SEM_DEFINE(afe_start_sem, 0, 1);

static volatile uint32_t afe_sample_count = 0;
static volatile uint32_t afe_read_error_count = 0;
static volatile uint32_t emg_queue_drop_count = 0;

uint32_t afe_get_queue_used(void)
{
    // 현재 message queue 안에 몇 개의 message가 들어있는지 반환하는 함수
    return k_msgq_num_used_get(&emg_queue);
}

uint32_t afe_get_sample_count(void)
{
    return afe_sample_count;
}

uint32_t afe_get_read_error_count(void)
{
    return afe_read_error_count;
}

uint32_t afe_get_queue_drop_count(void)
{
    return emg_queue_drop_count;
}

static void afe_thread(void *a, void *b, void *c)
{
    ads1294_data_frame_t frame;
    emg_sample_t emg;

    uint32_t uart_drop_count = 0;
    uint32_t sequence = 0;
    int rc;

    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);
    k_sem_take(&afe_start_sem, K_FOREVER);

    while (1) {
        rc = ads1294_read_data_frame_wait(&frame, K_FOREVER);
        if (rc) {
            afe_read_error_count++;
            continue;
        }

        /* 정상적으로 획득한 ADS1294 frame 수 */
        afe_sample_count++;

        // EMG ESB 전송 큐 저장 
        emg.sequence = sequence++;
        emg.ch1_l_tibialis = frame.channel[0];
        emg.ch2_l_gastrocnemius = frame.channel[1];
        emg.ch3_r_tibialis = frame.channel[2];
        emg.ch4_r_gastrocnemius = frame.channel[3]; 

        rc = k_msgq_put(&emg_queue, &emg, K_NO_WAIT);
        if (rc) {
            emg_queue_drop_count++;
        }

        // UART 로깅 데이터 TX 송신
        uint8_t uart_frame[UART_FRAME_LEN];
        size_t off = 0;

        // /* Header */
        // uart_frame[off++] = 0xAA;
        // uart_frame[off++] = 0x55;

        // /* CH1 : int32_t */
        // memcpy(&uart_frame[off],
        //        &frame.channel[0],
        //        sizeof(int32_t));

        // off += sizeof(int32_t);

        // /* CH2 : int32_t */
        // memcpy(&uart_frame[off],
        //        &frame.channel[1],
        //        sizeof(int32_t));

        // off += sizeof(int32_t);

        // /* CH3 : int32_t */
        // memcpy(&uart_frame[off],
        //        &frame.channel[2],
        //        sizeof(int32_t));

        // off += sizeof(int32_t);

        // /* CH4 : int32_t */
        // memcpy(&uart_frame[off],
        //        &frame.channel[3],
        //        sizeof(int32_t));

        // off += sizeof(int32_t);

        // /* UART TX queue */
        // rc = k_msgq_put(&uart_tx_q,
        //                 uart_frame,
        //                 K_NO_WAIT);

        // if (rc == 0) {
        //     uart_tx_kick();
        // } else {
        //     uart_drop_count++;
        //     if ((uart_drop_count % 100) == 1) {
        //         LOG_WRN("UART TX queue full, dropped=%u",
        //                 uart_drop_count);
        //     }
        // }


    }
}

void afe_thread_start(void)
{
    k_sem_give(&afe_start_sem);
}

int afe_thread_get_sample(emg_sample_t *sample, k_timeout_t timeout)
{
    if (sample == NULL) {
        return -EINVAL;
    }

    return k_msgq_get(&emg_queue, sample, timeout);
}

K_THREAD_DEFINE(afe_thread_id,
                AFE_THREAD_STACK_SIZE,
                afe_thread,
                NULL, NULL, NULL,
                AFE_THREAD_PRIORITY,
                0,
                0);