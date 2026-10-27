#include "ads1294.h"

LOG_MODULE_REGISTER(ads1294, LOG_LEVEL_INF);
struct spi_dt_spec ads1294_spispec = SPI_DT_SPEC_GET(DT_NODELABEL(ads1294), ADS1294_SPI_OP, 0);

static const struct gpio_dt_spec ads1294_start_gpio =
    GPIO_DT_SPEC_GET(DT_NODELABEL(ads1294_start), gpios);

static const struct gpio_dt_spec ads1294_reset_pwdn_gpio =
    GPIO_DT_SPEC_GET(DT_NODELABEL(ads1294_reset_pwdn), gpios);

static const struct gpio_dt_spec ads1294_drdy_gpio =
    GPIO_DT_SPEC_GET(DT_NODELABEL(ads1294_drdy), gpios);

static struct gpio_callback ads1294_drdy_cb;
K_SEM_DEFINE(ads1294_drdy_sem, 0, 1);

static void ads1294_drdy_handler(const struct device *port,
                                 struct gpio_callback *cb,
                                 uint32_t pins)
{
    ARG_UNUSED(port);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    k_sem_give(&ads1294_drdy_sem);
}

static int ads1294_configure_control_gpios(void)
{
    int rc;

    if (!gpio_is_ready_dt(&ads1294_start_gpio)) {
        LOG_ERR("ADS1294 START GPIO not ready");
        return -ENODEV;
    }

    if (!gpio_is_ready_dt(&ads1294_reset_pwdn_gpio)) {
        LOG_ERR("ADS1294 RESET/PWDN GPIO not ready");
        return -ENODEV;
    }

    rc = gpio_pin_configure_dt(&ads1294_start_gpio, GPIO_OUTPUT_INACTIVE);
    if (rc) {
        LOG_ERR("ADS1294 START GPIO configure failed: rc=%d", rc);
        return rc;
    }

    rc = gpio_pin_configure_dt(&ads1294_reset_pwdn_gpio, GPIO_OUTPUT_INACTIVE);
    if (rc) {
        LOG_ERR("ADS1294 RESET/PWDN GPIO configure failed: rc=%d", rc);
        return rc;
    }

    return 0;
}

static int ads1294_configure_drdy_gpio(void)
{
    int rc;

    if (!gpio_is_ready_dt(&ads1294_drdy_gpio)) {
        LOG_ERR("ADS1294 DRDY GPIO not ready");
        return -ENODEV;
    }

    rc = gpio_pin_configure_dt(&ads1294_drdy_gpio, GPIO_INPUT);
    if (rc) {
        LOG_ERR("ADS1294 DRDY GPIO configure failed: rc=%d", rc);
        return rc;
    }

    gpio_init_callback(&ads1294_drdy_cb, ads1294_drdy_handler,
                       BIT(ads1294_drdy_gpio.pin));

    rc = gpio_add_callback(ads1294_drdy_gpio.port, &ads1294_drdy_cb);
    if (rc) {
        LOG_ERR("ADS1294 DRDY callback add failed: rc=%d", rc);
        return rc;
    }

    rc = gpio_pin_interrupt_configure_dt(&ads1294_drdy_gpio,
                                         GPIO_INT_EDGE_TO_ACTIVE);
    if (rc) {
        LOG_ERR("ADS1294 DRDY interrupt configure failed: rc=%d", rc);
        return rc;
    }

    return 0;
}

static int ads1294_hold_start_low(void)
{
    int rc;

    rc = gpio_pin_set_dt(&ads1294_start_gpio, 0);
    if (rc) {
        LOG_ERR("ADS1294 START GPIO set low failed: rc=%d", rc);
        return rc;
    }

    return 0;
}

static int ads1294_pulse_reset_pwdn(void)
{
    int rc;

    rc = gpio_pin_set_dt(&ads1294_reset_pwdn_gpio, 1);
    if (rc) {
        return rc;
    }

    k_sleep(K_MSEC(1));

    rc = gpio_pin_set_dt(&ads1294_reset_pwdn_gpio, 0);
    if (rc) {
        return rc;
    }

    k_sleep(K_MSEC(100));
    return 0;
}

static int32_t ads1294_parse_s24(const uint8_t *data)
{
    int32_t value = ((int32_t)data[0] << 16) |
                    ((int32_t)data[1] << 8) |
                    (int32_t)data[2];

    if (value & 0x00800000) {
        value |= 0xFF000000;
    }

    return value;
}

static int ads1294_spi_txrx(const uint8_t *tx, uint8_t *rx, uint8_t len)
{
    const struct spi_buf tx_buf = {
        .buf = (void *)tx,
        .len = len,
    };

    const struct spi_buf_set tx_set = {
        .buffers = &tx_buf,
        .count = 1,
    };
    if (rx == NULL) {
        return spi_write_dt(&ads1294_spispec, &tx_set);
    }

    struct spi_buf rx_buf = {
        .buf = rx,
        .len = len,
    };

    const struct spi_buf_set rx_set = {
        .buffers = &rx_buf,
        .count = 1,
    };

    return spi_transceive_dt(&ads1294_spispec, &tx_set, &rx_set);
}

int ads1294_wait_for_data(k_timeout_t timeout)
{
    return k_sem_take(&ads1294_drdy_sem, timeout);
}

int ads1294_read_data_frame(ads1294_data_frame_t *frame)
{
    int rc;
    uint8_t tx[ADS1294_FRAME_SIZE] = { 0 };

    if (frame == NULL) {
        return -EINVAL;
    }

    rc = ads1294_spi_txrx(tx, frame->raw, sizeof(frame->raw));
    if (rc) {
        LOG_ERR("ADS1294 data frame read failed: rc=%d", rc);
        return rc;
    }

    frame->status = ((uint32_t)frame->raw[0] << 16) |
                    ((uint32_t)frame->raw[1] << 8) |
                    (uint32_t)frame->raw[2];

    for (size_t i = 0; i < REX_ADS_NUM_CHANNELS; i++) {
        const size_t offset = ADS1294_STATUS_BYTES + (i * ADS1294_BYTES_PER_CHANNEL);
        frame->channel[i] = ads1294_parse_s24(&frame->raw[offset]);
    }

    return 0;
}

int ads1294_read_data_frame_wait(ads1294_data_frame_t *frame, k_timeout_t timeout)
{
    int rc;

    rc = ads1294_wait_for_data(timeout);
    if (rc) {
        return rc;
    }

    return ads1294_read_data_frame(frame);
}

int ads1294_send_command(uint8_t opcode)
{
    int rc;
    uint8_t tx = opcode;

    rc = ads1294_spi_txrx(&tx, NULL, 1);
    if (rc) {
        LOG_ERR("ADS1294 command failed: opcode=0x%02X rc=%d", opcode, rc);
        return rc;
    }

    k_busy_wait(5);
    return 0;
}

int ads1294_write_register(uint8_t address, uint8_t value)
{
    int rc;
    uint8_t tx[3];

    tx[0] = ADS129X_CMD_WREG_ADDR(address);
    tx[1] = ADS129X_REG_COUNT(1);
    tx[2] = value;

    rc = ads1294_spi_txrx(tx, NULL, sizeof(tx));
    if (rc) {
        LOG_ERR("ADS1294 WREG failed: addr=0x%02X value=0x%02X rc=%d",
                address, value, rc);
        return rc;
    }

    k_busy_wait(5);
    return 0;
}

int ads1294_read_register(uint8_t address, uint8_t *value)
{
    int rc;
    uint8_t tx[3];
    uint8_t rx[3];

    if (value == NULL) {
        return -EINVAL;
    }

    tx[0] = ADS129X_CMD_RREG_ADDR(address);
    tx[1] = ADS129X_REG_COUNT(1);
    tx[2] = 0x00;

    rc = ads1294_spi_txrx(tx, rx, sizeof(tx));
    if (rc) {
        LOG_ERR("ADS1294 RREG failed: addr=0x%02X rc=%d", address, rc);
        return rc;
    }

    *value = rx[2];

    LOG_INF("ADS1294 RREG: addr=0x%02X value=0x%02X", address, *value);
    return 0;
}

int ads1294_apply_config(uint8_t mode)
{
    size_t len = 0;
    const rex_ads129x_reg_config_t *config;

    config = rex_ads129x_get_config(mode, &len);

    for (size_t i = 0; i < len; i++) {
        int rc = ads1294_write_register(config[i].address, config[i].value);
        if (rc) {
            return rc;
        }
    }
    return 0;
}


static const char *ads_mode_to_string(int mode)
{
    switch (mode) {
    case REX_ADS_MODE_NORMAL:
        return "NORMAL";

    case REX_ADS_MODE_TEST_SIGNAL:
        return "TEST_SIGNAL";

    case REX_ADS_MODE_LEADOFF_CHECK:
        return "LEADOFF_CHECK";

    case REX_ADS_MODE_RLD_RECORDING:
        return "RLD_RECORDING";

    default:
        return "UNKNOWN";
    }
}

int ads1294_init(uint8_t mode)
{
    /*
     * ADS1294 initialization
     *
     * Sequence inside ads1294_init():
     * 1. Check SPI device ready
     * 2. Send SDATAC
     * 3. Send RESET
     * 4. Send SDATAC again
     * 5. Read ID register
     * 6. Apply selected register profile
     * 7. Send START
     * 8. Send RDATAC
     */

    int rc;
    uint8_t id = 0;

    if (!ads1294_spispec.bus) {
        LOG_ERR("ADS1294: SPI bus not found in devicetree");
        return -ENODEV;
    }

    if (!spi_is_ready_dt(&ads1294_spispec)) {
        LOG_ERR("ADS1294: SPI device not ready");
        return -ENODEV;
    }

    LOG_INF("ADS1294 init start");

    rc = ads1294_configure_control_gpios();
    if (rc) return rc;

    rc = ads1294_hold_start_low();
    if (rc) return rc;

    rc = ads1294_pulse_reset_pwdn();
    if (rc) return rc;

    rc = ads1294_configure_drdy_gpio();
    if (rc) return rc;

    rc = ads1294_send_command(ADS129X_CMD_SDATAC);
    if (rc) return rc;

    rc = ads1294_send_command(ADS129X_CMD_RESET);
    if (rc) return rc;

    k_sleep(K_MSEC(10));

    rc = ads1294_send_command(ADS129X_CMD_SDATAC);
    if (rc) return rc;

    rc = ads1294_read_register(ADS129X_REG_ID, &id);
    if (rc) return rc;

    LOG_INF("ADS1294 ID = 0x%02X", id);

    if (id != REX_ADS1294_EXPECTED_ID) {
        LOG_ERR("Unexpected ADS1294 ID: 0x%02X", id);
        return -ENODEV;
    }
    
    rc = ads1294_apply_config(mode);
    if (rc) return rc;

    LOG_INF("Active ADS1294 mode = %s",
        ads_mode_to_string(REX_ADS_ACTIVE_MODE));

    k_sleep(K_MSEC(150));  // internal reference settling margin
    
    rc = ads1294_send_command(ADS129X_CMD_START);
    if (rc) return rc;

    rc = ads1294_send_command(ADS129X_CMD_RDATAC);
    if (rc) return rc;

    LOG_INF("ADS1294 init done");
    return 0;
}

