#include <adc_sample.h>

LOG_MODULE_REGISTER(adc_sample, LOG_LEVEL_INF);

static int16_t saadc_sample_buffer[2][SAADC_BUFFER_SIZE];  // 더블 버퍼 (DMA 전용 실시간 샘플 버퍼)
static uint32_t saadc_current_buffer = 0;                   // 현재 사용 중인 버퍼 인덱스
static const nrfx_timer_t timer_instance = NRFX_TIMER_INSTANCE(TIMER_INSTANCE_NUMBER);

static volatile int16_t ADC_left_angle=0;
static volatile int16_t ADC_right_angle=0;

static volatile int16_t ADC_left_torque=0;
static volatile int16_t ADC_right_torque=0;

static volatile int16_t An_left=0;
static volatile int16_t An_right=0;
static volatile bool streaming_ready = false;

static volatile uint32_t emg_underflow_count = 0;
static volatile uint32_t emg_sequence_error_count = 0;
static volatile uint32_t esb_packet_count = 0;

static volatile uint32_t emg_rc1_fail_count = 0;
static volatile uint32_t emg_rc2_fail_count = 0;

static volatile uint32_t saadc_work_count = 0;
static volatile uint32_t esb_write_ok_count = 0;
static volatile uint32_t esb_enomem_count = 0;
static volatile uint32_t esb_retry_fail_count = 0;

uint32_t adc_get_emg_underflow_count(void)
{
    return emg_underflow_count;
}

uint32_t adc_get_emg_sequence_error_count(void)
{
    return emg_sequence_error_count;
}

uint32_t adc_get_esb_packet_count(void)
{
    return esb_packet_count;
}

uint32_t adc_get_emg_rc1_fail_count(void)
{
    return emg_rc1_fail_count;
}

uint32_t adc_get_emg_rc2_fail_count()
{
    return emg_rc2_fail_count;
}

uint32_t get_saadc_work_count(void)
{
    return saadc_work_count;
}

uint32_t get_esb_write_ok_count(void)
{
    return esb_write_ok_count;
}

uint32_t get_esb_enomem_count(void)
{
    return esb_enomem_count;
}

uint32_t get_esb_retry_fail_count(void)
{
    return esb_retry_fail_count;
}

void saadc_esb_send(struct k_work *item);
K_WORK_DEFINE(saadc_esb_work, saadc_esb_send);

void saadc_esb_send(struct k_work *item)
{
	ARG_UNUSED(item);

	saadc_work_count++;

	uint8_t flags = 0;

	int16_t deg_left = 0;
	int16_t deg_right = 0;

	int16_t l_torque = 0;
	int16_t r_torque = 0;

	emg_sample_t emg1 = {0}; // Channel 1
	emg_sample_t emg2 = {0}; // Channel 2

#if defined(CONFIG_HALL_MODE)
		
		// ADC 0~4096
		uint16_t l = ADC_left_angle;
		uint16_t r = ADC_right_angle;
		
		// Calibrated Angle Look up table
		deg_left  = hall_lut_left[l];
		deg_right = hall_lut_right[r];

		// Hall Sensor ADC
		// deg_left = l;
		// deg_right = r;		

		// Normalized Angle
		// float Ap_left = Norm_ADC(l, 'H');
		// float Ap_right = Norm_ADC(r, 'S');
		// deg_left = (int16_t) (Ap_left*100);
		// deg_right = (int16_t) (Ap_right*100);
#endif

#if defined(CONFIG_CAN_MODE)
		l_torque = ADC_left_torque;
		r_torque = ADC_right_torque;
#endif

#if defined(CONFIG_EMG_MODE)
    if (afe_get_queue_used() >= 2) {

        int rc1 = afe_thread_get_sample(&emg1, K_NO_WAIT);
        int rc2 = afe_thread_get_sample(&emg2, K_NO_WAIT);

        if (rc1 != 0 || rc2 != 0) {
            emg_underflow_count++;
            flags |= PAYLOAD_FLAG_EMG_UNDERFLOW;

            emg1 = (emg_sample_t){0};
            emg2 = (emg_sample_t){0};

        } else if (emg2.sequence != (emg1.sequence + 1)) {
            emg_sequence_error_count++;
            flags |= PAYLOAD_FLAG_EMG_SEQUENCE_ERROR;
        }

    } else {
        /* 아직 EMG sample 2개가 준비되지 않음 */
        emg_underflow_count++;
        flags |= PAYLOAD_FLAG_EMG_UNDERFLOW;

        emg1 = (emg_sample_t){0};
        emg2 = (emg_sample_t){0};
    }

#endif
		make_payload_emg(deg_left, deg_right, l_torque, r_torque, flags, &emg1, &emg2);
		
		// Payload 송신 지연 에러 모니터링 필요
		int err = esb_write_payload(&tx_payload);
		if (err == 0) {
			esb_write_ok_count++;
		} else if (err == -ENOMEM) {
			esb_enomem_count++;
			esb_flush_tx();
			err = esb_write_payload(&tx_payload);
			if (err == 0) {
				esb_write_ok_count++;
			} else {
				esb_retry_fail_count++;
			}
		} else {
			esb_retry_fail_count++;
		}
}

void adc_streaming_start(void)
{
    streaming_ready = true;
}

/* ===============================
 * SAADC 이벤트 핸들러
 * =============================== */
static void saadc_event_handler(nrfx_saadc_evt_t const *p_event)
{
	nrfx_err_t err;

	switch (p_event->type) {
	case NRFX_SAADC_EVT_READY:
		nrfx_timer_enable(&timer_instance);
		break;

	case NRFX_SAADC_EVT_BUF_REQ:
		err = nrfx_saadc_buffer_set(saadc_sample_buffer[(saadc_current_buffer++) % 2], SAADC_BUFFER_SIZE);
		if (err != NRFX_SUCCESS)
			LOG_ERR("nrfx_saadc_buffer_set error: %08x", err);
		break;

	case NRFX_SAADC_EVT_DONE: {
		int16_t *buf = p_event->data.done.p_buffer;

#if defined(CONFIG_HALL_MODE)
		ADC_left_angle  = buf[0];
        ADC_right_angle = buf[1];
#endif

#if defined(CONFIG_CAN_MODE)
		ADC_left_torque  = buf[2];
        ADC_right_torque = buf[3];
#endif

		if (streaming_ready) {
        	k_work_submit(&saadc_esb_work);
    	}		
		break;
	}
	default:
		LOG_WRN("Unhandled SAADC evt %d", p_event->type);
		break;
	}
}

/* ===============================
 * Timer2 설정 (1MHz, 주기적 샘플 트리거)
 * =============================== */
void configure_timer(void)
{
	nrfx_err_t err;
	nrfx_timer_config_t timer_config = NRFX_TIMER_DEFAULT_CONFIG(1000000); // 1MHz

	err = nrfx_timer_init(&timer_instance, &timer_config, NULL);
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_timer_init error: %08x", err);
		return;
	}else{
		LOG_INF("nrfx_timer_init clear");
	}

	uint32_t timer_ticks = nrfx_timer_us_to_ticks(&timer_instance, SAADC_SAMPLE_INTERVAL_US);
	nrfx_timer_extended_compare(&timer_instance, NRF_TIMER_CC_CHANNEL0,
								timer_ticks,
								NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
								false);
}

/* ===============================
 * SAADC 설정 (4채널, 더블버퍼)
 * =============================== */
void configure_saadc(void)
{
	nrfx_err_t err;

	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(adc)),
		    DT_IRQ(DT_NODELABEL(adc), priority),
		    nrfx_isr, nrfx_saadc_irq_handler, 0);

	err = nrfx_saadc_init(DT_IRQ(DT_NODELABEL(adc), priority));
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_saadc_init error: %08x", err);
		return;
	}else{
		LOG_INF("nrfx_saadc_init clear");
	}

	/* 공통 채널 설정 */
	channel_Hall_L.channel_config.gain = NRF_SAADC_GAIN1_6;
	channel_Hall_R.channel_config.gain = NRF_SAADC_GAIN1_6;
	channel_Can_L.channel_config.gain  = NRF_SAADC_GAIN1_6;
	channel_Can_R.channel_config.gain  = NRF_SAADC_GAIN1_6;

	nrfx_saadc_channel_t channels[4];
	int ch_count = 0;

#if defined(CONFIG_HALL_MODE)
	channels[ch_count++] = channel_Hall_L;   // SAADC ch 0
	channels[ch_count++] = channel_Hall_R;   // SAADC ch 1
#endif

#if defined(CONFIG_CAN_MODE)
	channels[ch_count++] = channel_Can_L;    // SAADC ch 2
	channels[ch_count++] = channel_Can_R;    // SAADC ch 3
#endif

	if (ch_count == 0) {
		LOG_ERR("⚠️ No SAADC channels enabled (check CONFIG_HALL_MODE / CONFIG_CAN_MODE)");
		return;
	}

	/* 1) 선택된 채널들 실제로 적용 */
	err = nrfx_saadc_channels_config(channels, ch_count);
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_saadc_channels_config error: %08x", err);
		return;
	}

	/* 2) advanced mode에서 활성화할 채널 비트마스크 직접 구성 */
	uint32_t active_mask = 0;

#if defined(CONFIG_HALL_MODE)
	active_mask |= BIT(0);   // Hall L
	active_mask |= BIT(1);   // Hall R
#endif

#if defined(CONFIG_CAN_MODE)
	active_mask |= BIT(2);   // Can L
	active_mask |= BIT(3);   // Can R
#endif

	nrfx_saadc_adv_config_t saadc_adv_config = NRFX_SAADC_DEFAULT_ADV_CONFIG;
	err = nrfx_saadc_advanced_mode_set(active_mask,
					   NRF_SAADC_RESOLUTION_12BIT,
					   &saadc_adv_config,
					   saadc_event_handler);
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_saadc_advanced_mode_set error: %08x", err);
		return;
	}

	/* 더블 버퍼 등록 */
	err  = nrfx_saadc_buffer_set(saadc_sample_buffer[0], SAADC_BUFFER_SIZE);
	err |= nrfx_saadc_buffer_set(saadc_sample_buffer[1], SAADC_BUFFER_SIZE);
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_saadc_buffer_set error: %08x", err);
		return;
	}

	err = nrfx_saadc_mode_trigger();
	if (err != NRFX_SUCCESS) {
		LOG_ERR("nrfx_saadc_mode_trigger error: %08x", err);
	}
}
/* ===============================
 * PPI 연결 (Timer → SAADC Trigger)
 * =============================== */
void configure_ppi(void)
{
	nrfx_err_t err;
	// uint8_t ppi_ch_sample, ppi_ch_start;

    const uint8_t ppi_ch_sample = 10;   // Timer → SAMPLE
    const uint8_t ppi_ch_start  = 11;   // END → START

	nrfx_gppi_channels_disable(BIT(ppi_ch_sample));
	nrfx_gppi_channels_disable(BIT(ppi_ch_start));
	
	nrfx_gppi_channel_endpoints_setup(ppi_ch_sample,
		nrfx_timer_compare_event_address_get(&timer_instance, NRF_TIMER_CC_CHANNEL0),
		nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_SAMPLE));

	nrfx_gppi_channel_endpoints_setup(ppi_ch_start,
		nrf_saadc_event_address_get(NRF_SAADC, NRF_SAADC_EVENT_END),
		nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_START));

	nrfx_gppi_channels_enable(BIT(ppi_ch_sample) | BIT(ppi_ch_start));
}
