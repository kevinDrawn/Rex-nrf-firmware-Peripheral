#ifndef REX_ADS1294_CONFIG_H
#define REX_ADS1294_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include "ads129x_regs.h"

/* ============================================================
 * Device Selection
 * ============================================================ */
#define REX_ADS_DEVICE_ADS1294          1
#define REX_ADS_NUM_CHANNELS            4
#define REX_ADS1294_EXPECTED_ID 0x90

/* ============================================================
 * REX ADS129x Mode Selection
 * ============================================================ */
#define REX_ADS_MODE_NORMAL             0
#define REX_ADS_MODE_TEST_SIGNAL        1
#define REX_ADS_MODE_LEADOFF_CHECK      2
#define REX_ADS_MODE_RLD_RECORDING      3

#define REX_ADS_ACTIVE_MODE             REX_ADS_MODE_RLD_RECORDING

typedef struct {
    uint8_t address;
    uint8_t value;
} rex_ads129x_reg_config_t;

/* ============================================================
 * Normal Recording Mode
 * User-editable register values
 * ============================================================ */
static const rex_ads129x_reg_config_t rex_ads_normal_config[] = {
    { ADS129X_REG_CONFIG1,
      ADS129X_CONFIG1_HR_HIGH_RES |
      ADS129X_CONFIG1_MULTIPLE_READBACK |
      ADS129X_CONFIG1_CLK_OUT_DISABLE |
      ADS129X_CONFIG1_DR_2KSPS },

    { ADS129X_REG_CONFIG2,
      ADS129X_CONFIG2_RESERVED_7_6 |
      ADS129X_CONFIG2_INT_TEST_DISABLE },

    { ADS129X_REG_CONFIG3,
      ADS129X_CONFIG3_REFBUF_ENABLE |
      ADS129X_CONFIG3_RESERVED_6 |
      ADS129X_CONFIG3_VREF_2V4 |
      ADS129X_CONFIG3_RLD_POWER_DOWN },

    { ADS129X_REG_LOFF, 0x00 },

    { ADS129X_REG_CH1SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_4 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH2SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_4 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH3SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_4 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_CH4SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_4 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_RLD_SENSP,  0x00 },
    { ADS129X_REG_RLD_SENSN,  0x00 },
    { ADS129X_REG_LOFF_SENSP, 0x00 },
    { ADS129X_REG_LOFF_SENSN, 0x00 },
    { ADS129X_REG_LOFF_FLIP,  0x00 },
    { ADS129X_REG_GPIO,       0x0F },
    { ADS129X_REG_PACE,       0x00 },
    { ADS129X_REG_RESP,       ADS129X_RESP_RESERVED_1 },
    { ADS129X_REG_CONFIG4,    0x00 },
    { ADS129X_REG_WCT1,       0x00 },
    { ADS129X_REG_WCT2,       0x00 },
};

/* ============================================================
 * Internal Test Signal Mode
 * User-editable register values
 * ============================================================ */
static const rex_ads129x_reg_config_t rex_ads_test_signal_config[] = {
    { ADS129X_REG_CONFIG1,
      ADS129X_CONFIG1_HR_HIGH_RES |
      ADS129X_CONFIG1_MULTIPLE_READBACK |
      ADS129X_CONFIG1_CLK_OUT_DISABLE |
      ADS129X_CONFIG1_DR_2KSPS },

    { ADS129X_REG_CONFIG2,
      ADS129X_CONFIG2_RESERVED_7_6 |
      ADS129X_CONFIG2_INT_TEST_ENABLE |
      ADS129X_CONFIG2_TEST_AMP_2MV |
      ADS129X_CONFIG2_TEST_FREQ_2HZ },

    { ADS129X_REG_CONFIG3,
      ADS129X_CONFIG3_REFBUF_ENABLE |
      ADS129X_CONFIG3_RESERVED_6 |
      ADS129X_CONFIG3_VREF_2V4 },

    { ADS129X_REG_LOFF, 0x00 },

    { ADS129X_REG_CH1SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_12 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_TEST_SIGNAL },

    { ADS129X_REG_CH2SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_12 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_TEST_SIGNAL },

    { ADS129X_REG_CH3SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_12 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_TEST_SIGNAL },

    { ADS129X_REG_CH4SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_12 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_TEST_SIGNAL },

    { ADS129X_REG_RLD_SENSP,  0x00 },
    { ADS129X_REG_RLD_SENSN,  0x00 },
    { ADS129X_REG_LOFF_SENSP, 0x00 },
    { ADS129X_REG_LOFF_SENSN, 0x00 },
    { ADS129X_REG_LOFF_FLIP,  0x00 },
    { ADS129X_REG_GPIO,       0x0F },
    { ADS129X_REG_PACE,       0x00 },
    { ADS129X_REG_RESP,       ADS129X_RESP_RESERVED_1 },
    { ADS129X_REG_CONFIG4,    0x00 },
    { ADS129X_REG_WCT1,       0x00 },
    { ADS129X_REG_WCT2,       0x00 },
};

/* ============================================================
 * Lead-Off Check Mode
 * User-editable register values
 * ============================================================ */
static const rex_ads129x_reg_config_t rex_ads_leadoff_check_config[] = {
    { ADS129X_REG_CONFIG1,
      ADS129X_CONFIG1_HR_HIGH_RES |
      ADS129X_CONFIG1_MULTIPLE_READBACK |
      ADS129X_CONFIG1_CLK_OUT_DISABLE |
      ADS129X_CONFIG1_DR_2KSPS },

    { ADS129X_REG_CONFIG2,
      ADS129X_CONFIG2_RESERVED_7_6 |
      ADS129X_CONFIG2_INT_TEST_DISABLE },

    { ADS129X_REG_CONFIG3,
      ADS129X_CONFIG3_REFBUF_ENABLE |
      ADS129X_CONFIG3_RESERVED_6 |
      ADS129X_CONFIG3_VREF_2V4 },

    { ADS129X_REG_LOFF,
      ADS129X_LOFF_COMP_TH_95_5 |
      ADS129X_LOFF_VLEAD_OFF_CURRENT_SOURCE |
      ADS129X_LOFF_CURRENT_6NA |
      ADS129X_LOFF_FREQ_DC },

    { ADS129X_REG_CH1SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_6 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH2SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_6 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH3SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_6 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_CH4SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_6 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_LOFF_SENSP,
      ADS129X_LOFF_SENSP_LOFF1P |
      ADS129X_LOFF_SENSP_LOFF2P },

    { ADS129X_REG_LOFF_SENSN,
      ADS129X_LOFF_SENSN_LOFF1N |
      ADS129X_LOFF_SENSN_LOFF2N },

    { ADS129X_REG_LOFF_FLIP, 0x00 },
    { ADS129X_REG_GPIO,      0x0F },
    { ADS129X_REG_PACE,      0x00 },
    { ADS129X_REG_RESP,      ADS129X_RESP_RESERVED_1 },
    { ADS129X_REG_CONFIG4,   ADS129X_CONFIG4_LOFF_COMP_ON },
    { ADS129X_REG_WCT1,      0x00 },
    { ADS129X_REG_WCT2,      0x00 },
};

/* ============================================================
 * RLD Recording Mode
 * User-editable register values
 * ============================================================ */
static const rex_ads129x_reg_config_t rex_ads_rld_recording_config[] = {
    { ADS129X_REG_CONFIG1,
      ADS129X_CONFIG1_HR_HIGH_RES |
      ADS129X_CONFIG1_MULTIPLE_READBACK |
      ADS129X_CONFIG1_CLK_OUT_DISABLE |
      ADS129X_CONFIG1_DR_2KSPS },

    { ADS129X_REG_CONFIG2,
      ADS129X_CONFIG2_RESERVED_7_6 |
      ADS129X_CONFIG2_INT_TEST_DISABLE },

    { ADS129X_REG_CONFIG3,
      ADS129X_CONFIG3_REFBUF_ENABLE |
      ADS129X_CONFIG3_RESERVED_6 |
      ADS129X_CONFIG3_VREF_2V4 |
      ADS129X_CONFIG3_RLDREF_INTERNAL |
      ADS129X_CONFIG3_RLD_ENABLE },

    { ADS129X_REG_LOFF, 0x00 },

    { ADS129X_REG_CH1SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_8 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH2SET,
      ADS129X_CHSET_POWER_ON |
      ADS129X_CHSET_GAIN_8 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_NORMAL },

    { ADS129X_REG_CH3SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_8 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_CH4SET,
      ADS129X_CHSET_POWER_DOWN |
      ADS129X_CHSET_GAIN_8 |
      ADS129X_CHSET_RESERVED_0 |
      ADS129X_CHSET_MUX_SHORTED },

    { ADS129X_REG_RLD_SENSP,
      ADS129X_RLD_SENSP_RLD1P |
      ADS129X_RLD_SENSP_RLD2P },

    { ADS129X_REG_RLD_SENSN,
      ADS129X_RLD_SENSN_RLD1N |
      ADS129X_RLD_SENSN_RLD2N },

    { ADS129X_REG_LOFF_SENSP, 0x00 },
    { ADS129X_REG_LOFF_SENSN, 0x00 },
    { ADS129X_REG_LOFF_FLIP,  0x00 },
    { ADS129X_REG_GPIO,       0x0F },
    { ADS129X_REG_PACE,       0x00 },
    { ADS129X_REG_RESP,       ADS129X_RESP_RESERVED_1 },
    { ADS129X_REG_CONFIG4,    0x00 },
    { ADS129X_REG_WCT1,       0x00 },
    { ADS129X_REG_WCT2,       0x00 },
};

#define REX_ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))

static inline const rex_ads129x_reg_config_t *
rex_ads129x_get_config(uint8_t mode, size_t *length)
{
    switch (mode) {
    case REX_ADS_MODE_NORMAL:
        *length = REX_ARRAY_SIZE(rex_ads_normal_config);
        return rex_ads_normal_config;

    case REX_ADS_MODE_TEST_SIGNAL:
        *length = REX_ARRAY_SIZE(rex_ads_test_signal_config);
        return rex_ads_test_signal_config;

    case REX_ADS_MODE_LEADOFF_CHECK:
        *length = REX_ARRAY_SIZE(rex_ads_leadoff_check_config);
        return rex_ads_leadoff_check_config;

    case REX_ADS_MODE_RLD_RECORDING:
        *length = REX_ARRAY_SIZE(rex_ads_rld_recording_config);
        return rex_ads_rld_recording_config;

    default:
        *length = REX_ARRAY_SIZE(rex_ads_normal_config);
        return rex_ads_normal_config;
    }
}

#endif /* REX_ADS1294_CONFIG_H */
