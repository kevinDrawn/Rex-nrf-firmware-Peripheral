#include <esb_b.h>

LOG_MODULE_REGISTER(esb_ptx, LOG_LEVEL_INF);

bool ready=true;

static volatile uint32_t esb_tx_success_count = 0;
static volatile uint32_t esb_tx_failed_count = 0;

static uint16_t packet_seq = 0;

uint32_t get_esb_tx_success_count(void)
{
    return esb_tx_success_count;
}

uint32_t get_esb_tx_failed_count(void)
{
    return esb_tx_failed_count;
}

struct esb_payload tx_payload = {
    .pipe = 0,
    .length = SENSOR_PAYLOAD_LEN,
};

static void put_i16_be(uint8_t *data, uint8_t offset, int16_t value)
{
    data[offset] = (uint8_t)((uint16_t)value >> 8);
    data[offset + 1] = (uint8_t)((uint16_t)value & 0xFF);
}

static inline void put_i32_be(uint8_t *buf, int offset, int32_t value)
{
    uint32_t v = (uint32_t)value;

    buf[offset + 0] = (uint8_t)((v >> 24) & 0xFF);
    buf[offset + 1] = (uint8_t)((v >> 16) & 0xFF);
    buf[offset + 2] = (uint8_t)((v >> 8)  & 0xFF);
    buf[offset + 3] = (uint8_t)(v & 0xFF);
}

static void put_u16_be(uint8_t *data, uint8_t offset, uint16_t value)
{
    data[offset] = (uint8_t)(value >> 8);
    data[offset + 1] = (uint8_t)(value & 0xFF);
}

void make_payload(int16_t left_angle, int16_t right_angle, int16_t left_torque, int16_t right_torque)
{
	tx_payload.data[0] = (uint8_t)(left_angle >> 8);
    tx_payload.data[1] = (uint8_t)(left_angle & 0xFF);
    tx_payload.data[2] = (uint8_t)(right_angle >> 8);
    tx_payload.data[3] = (uint8_t)(right_angle & 0xFF);

#if defined(CONFIG_HALL_MODE) && defined(CONFIG_CAN_MODE)
	tx_payload.data[4] = (uint8_t)(left_torque >> 8);
    tx_payload.data[5] = (uint8_t)(left_torque & 0xFF);
    tx_payload.data[6] = (uint8_t)(right_torque >> 8);
    tx_payload.data[7] = (uint8_t)(right_torque & 0xFF);
#endif
}

void make_payload_emg(int16_t left_angle,
                      int16_t right_angle,
                      int16_t left_torque,
                      int16_t right_torque,
                      uint8_t flags,
                      const emg_sample_t *emg1,
                      const emg_sample_t *emg2)
{
    // Payload reset
    memset(tx_payload.data, 0, sizeof(tx_payload.data));

    // EMG는 int32_t 원본 그대로 사용
    int32_t l_tb_emg_1 = emg1->ch1_l_tibialis;
    int32_t l_gt_emg_1 = emg1->ch2_l_gastrocnemius;
    int32_t l_tb_emg_2 = emg2->ch1_l_tibialis;
    int32_t l_gt_emg_2 = emg2->ch2_l_gastrocnemius;

#if defined(CONFIG_EMG_BILATERAL)
    int32_t r_tb_emg_1 = emg1->ch3_r_tibialis;
    int32_t r_gt_emg_1 = emg1->ch4_r_gastrocnemius;
    int32_t r_tb_emg_2 = emg2->ch3_r_tibialis;
    int32_t r_gt_emg_2 = emg2->ch4_r_gastrocnemius;
#endif

    put_u16_be(tx_payload.data, 0, packet_seq++);

    tx_payload.data[2] = flags;
    tx_payload.data[3] = 0;  // reserved

    put_i16_be(tx_payload.data, 4, left_angle);
    put_i16_be(tx_payload.data, 6, right_angle);
    put_i16_be(tx_payload.data, 8, left_torque);
    put_i16_be(tx_payload.data, 10, right_torque);

    // EMG: int32_t → 4 bytes
    put_i32_be(tx_payload.data, 12, l_tb_emg_1);
    put_i32_be(tx_payload.data, 16, l_gt_emg_1);
    put_i32_be(tx_payload.data, 20, l_tb_emg_2);
    put_i32_be(tx_payload.data, 24, l_gt_emg_2);

	// EMG: int32_t → 4 bytes
#if defined(CONFIG_EMG_BILATERAL)
    put_i32_be(tx_payload.data, 28, r_tb_emg_1);
    put_i32_be(tx_payload.data, 32, r_gt_emg_1);
    put_i32_be(tx_payload.data, 36, r_tb_emg_2);
    put_i32_be(tx_payload.data, 40, r_gt_emg_2);
#endif

    tx_payload.length = SENSOR_PAYLOAD_LEN;
}
// ESB 콜백 이벤트 처리 함수
static void event_handler(struct esb_evt const *event)
{
	// 패킷 전송 후, 성공/실패 관계없이 다음 패킷을 전송할 준비 완료 상태 만듬
	ready = true;

	switch (event->evt_id) {
	// Auto-ack를 받았다는 뜻 - 재전송 없이 잘 갔음
	case ESB_EVENT_TX_SUCCESS:
        esb_tx_success_count++;
		break;
	// 정해진 재전송 횟수만큼 보내고도 ack를 못 받았을 때
	case ESB_EVENT_TX_FAILED:
		esb_tx_failed_count++;
		// LOG_DBG("TX FAILED EVENT");
		break;
	// PTX 모드에서도 ACK Payload 기능 때문에 RX를 수신함 
	case ESB_EVENT_RX_RECEIVED:
		// LOG_DBG("Packet received");
		break;
	}
}

// ESB를 위해 HFCLK(32 MHz) 강제 On - ESB는 무조건 정확한 HFCLK(고주파 클럭)를 필요로 한다.
#if defined(CONFIG_CLOCK_CONTROL_NRF)
int clocks_start(void)
{
	int err;
	int res;
	struct onoff_manager *clk_mgr;
	struct onoff_client clk_cli;

	clk_mgr = z_nrf_clock_control_get_onoff(CLOCK_CONTROL_NRF_SUBSYS_HF);
	if (!clk_mgr) {
		LOG_ERR("Unable to get the Clock manager");
		return -ENXIO;
	}

	sys_notify_init_spinwait(&clk_cli.notify);

	// HFCLK 켜달라고 요청
	err = onoff_request(clk_mgr, &clk_cli);
	if (err < 0) {
		LOG_ERR("Clock request failed: %d", err);
		return err;
	}

	do {
		// 클럭이 켜질 때까지 block -> HFCLK 활성 보장
		err = sys_notify_fetch_result(&clk_cli.notify, &res);
		if (!err && res) {
			LOG_ERR("Clock could not be started: %d", res);
			return res;
		}
	} while (err);

	LOG_DBG("HF clock started");
	return 0;
}

#else
BUILD_ASSERT(false, "No Clock Control driver");
#endif /* defined(CONFIG_CLOCK_CONTROL_NRF2) */

// ESB 설정 전체 정리
int esb_initialize(void)
{
	int err;
	/* These are arbitrary default addresses. In end user products
	 * different addresses should be used for each set of devices.
	 */

	// 기본 주소 설정 
	// ESB 주소 - Base Address + Prefix (파이프 선택 비트)
	uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
	uint8_t prefix[1] = {0xE7};

	// ESB 설정 구조체
	struct esb_config config = ESB_DEFAULT_CONFIG;

	// 기본값에서 아래만 수정
	config.protocol = ESB_PROTOCOL_ESB_DPL; // Dynamic Payload Length
	config.bitrate = ESB_BITRATE_2MBPS;		// 속도
	config.event_handler = event_handler;	// 이벤트 콜백 연결
	config.mode = ESB_MODE_PTX;				// 송신기
	config.retransmit_count = 1;
	config.selective_auto_ack = true;		// Selective Auto ack, 재전송 모드, ACK를 받아서 수신 확인용
	config.retransmit_delay = 500;			// 재전송 간격 us

	// NOACK
	// config.selective_auto_ack    = false;
	// config.retransmit_delay      = 0;

	// TX - RX 전환 -> RF Ready 시간 3배 빨라진다.
	if (IS_ENABLED(CONFIG_ESB_FAST_SWITCHING)) {
		config.use_fast_ramp_up = true;
	}

	// 위 설정으로 ESB 초기화
	err = esb_init(&config);
	if (err) {
		return err;
	}

	/* 주소 설정 */
	// pipe0 : base_addr_0 + prefix[0]
	err = esb_set_base_address_0(base_addr_0);
	if (err) {
		return err;
	}
	err = esb_set_prefixes(prefix, 1);
	if (err) {
		return err;
	}

	return 0;
}
