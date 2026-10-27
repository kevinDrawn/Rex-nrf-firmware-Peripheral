#ifndef ADS1294_H
#define ADS1294_H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ads129x_regs.h"
#include "rex_ads1294_config.h"

#define ADS1294_STATUS_BYTES            3
#define ADS1294_BYTES_PER_CHANNEL       3
#define ADS1294_FRAME_SIZE              \
    (ADS1294_STATUS_BYTES + (REX_ADS_NUM_CHANNELS * ADS1294_BYTES_PER_CHANNEL))

#define ADS1294_SPI_OP \
    (SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA)

typedef struct {
    uint32_t status;
    int32_t channel[REX_ADS_NUM_CHANNELS];
    uint8_t raw[ADS1294_FRAME_SIZE];
} ads1294_data_frame_t;

int ads1294_init(uint8_t mode);
int ads1294_send_command(uint8_t opcode);
int ads1294_write_register(uint8_t address, uint8_t value);
int ads1294_read_register(uint8_t address, uint8_t *value);
int ads1294_apply_config(uint8_t mode);

int ads1294_wait_for_data(k_timeout_t timeout);
int ads1294_read_data_frame(ads1294_data_frame_t *frame);
int ads1294_read_data_frame_wait(ads1294_data_frame_t *frame, k_timeout_t timeout);

#endif /* ADS1294_H */
