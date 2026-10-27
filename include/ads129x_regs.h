#ifndef ADS129X_REGS_H
#define ADS129X_REGS_H

#include <stdint.h>

/* ============================================================
* ADS129x SPI Command Opcodes
*
* These values are command bytes transferred on MOSI.
* They are not register addresses.
* ============================================================ */

/* System commands */
#define ADS129X_CMD_WAKEUP              0x02
#define ADS129X_CMD_STANDBY             0x04
#define ADS129X_CMD_RESET               0x06
#define ADS129X_CMD_START               0x08
#define ADS129X_CMD_STOP                0x0A

/* Data read commands */
#define ADS129X_CMD_RDATAC              0x10
#define ADS129X_CMD_SDATAC              0x11
#define ADS129X_CMD_RDATA               0x12

/* Register read/write command base opcodes */
#define ADS129X_CMD_RREG                0x20
#define ADS129X_CMD_WREG                0x40

/* Register command helpers */
#define ADS129X_CMD_RREG_ADDR(addr)     (ADS129X_CMD_RREG | ((addr) & 0x1F))
#define ADS129X_CMD_WREG_ADDR(addr)     (ADS129X_CMD_WREG | ((addr) & 0x1F))

/* ADS129x RREG/WREG second byte is number of registers minus 1 */
#define ADS129X_REG_COUNT(n)            ((uint8_t)((n) - 1u))

/* ============================================================
 * ADS129x Register Map
 * Target device: ADS1294 / ADS1296 / ADS1298 family
 * Note: ADS1294 uses CH1-CH4 only.
 * ============================================================ */

/* Device settings: Read-only */
#define ADS129X_REG_ID              0x00

/* Global settings across channels */
#define ADS129X_REG_CONFIG1         0x01
#define ADS129X_REG_CONFIG2         0x02
#define ADS129X_REG_CONFIG3         0x03
#define ADS129X_REG_LOFF            0x04

/* Channel-specific settings */
#define ADS129X_REG_CH1SET          0x05
#define ADS129X_REG_CH2SET          0x06
#define ADS129X_REG_CH3SET          0x07
#define ADS129X_REG_CH4SET          0x08
#define ADS129X_REG_CH5SET          0x09  /* Not available in ADS1294 */
#define ADS129X_REG_CH6SET          0x0A  /* Not available in ADS1294 */
#define ADS129X_REG_CH7SET          0x0B  /* Not available in ADS1294/ADS1296 */
#define ADS129X_REG_CH8SET          0x0C  /* Not available in ADS1294/ADS1296 */

/* RLD signal derivation */
#define ADS129X_REG_RLD_SENSP       0x0D
#define ADS129X_REG_RLD_SENSN       0x0E

/* Lead-off detection */
#define ADS129X_REG_LOFF_SENSP      0x0F
#define ADS129X_REG_LOFF_SENSN      0x10
#define ADS129X_REG_LOFF_FLIP       0x11

/* Lead-off status: Read-only */
#define ADS129X_REG_LOFF_STATP      0x12
#define ADS129X_REG_LOFF_STATN      0x13

/* GPIO and other registers */
#define ADS129X_REG_GPIO            0x14
#define ADS129X_REG_PACE            0x15
#define ADS129X_REG_RESP            0x16
#define ADS129X_REG_CONFIG4         0x17
#define ADS129X_REG_WCT1            0x18
#define ADS129X_REG_WCT2            0x19

/* ============================================================
 * ID Register: address 0x00, read-only
 * Used to identify ADS1294 / ADS1296 / ADS1298 device variant.
 * ============================================================ */

#define ADS129X_ID_DEV_ID_MASK      0xE0  /* Bit[7:5] */
#define ADS129X_ID_FIXED_BITS_MASK  0x18  /* Bit[4:3], expected fixed bits */
#define ADS129X_ID_CHANNEL_MASK     0x07  /* Bit[2:0], device/channel code */

/*
 * DEV_ID[2:0] lower code is used to distinguish device family.
 * Exact expected value should be checked during board bring-up.
 */

/* ============================================================
 * CONFIG1 Register: address 0x01, reset 0x06
 * Controls resolution mode, daisy-chain mode, CLK output,
 * and output data rate.
 * ============================================================ */

/* Bit 7: HR */
#define ADS129X_CONFIG1_HR_LP_MODE          0x00
#define ADS129X_CONFIG1_HR_HIGH_RES         0x80

/* Bit 6: DAISY_EN */
#define ADS129X_CONFIG1_DAISY_CHAIN         0x00
#define ADS129X_CONFIG1_MULTIPLE_READBACK   0x40

/* Bit 5: CLK_EN */
#define ADS129X_CONFIG1_CLK_OUT_DISABLE     0x00
#define ADS129X_CONFIG1_CLK_OUT_ENABLE      0x20

/* Bit[2:0]: DR[2:0], output data rate */
#define ADS129X_CONFIG1_DR_32KSPS           0x00
#define ADS129X_CONFIG1_DR_16KSPS           0x01
#define ADS129X_CONFIG1_DR_8KSPS            0x02
#define ADS129X_CONFIG1_DR_4KSPS            0x03
#define ADS129X_CONFIG1_DR_2KSPS            0x04
#define ADS129X_CONFIG1_DR_1KSPS            0x05
#define ADS129X_CONFIG1_DR_500SPS           0x06

/* ============================================================
 * CONFIG2 Register: address 0x02, reset 0x40
 * Controls internal test signal.
 * Mostly used for board bring-up and ADC/SPI verification.
 * ============================================================ */

/* Bit[7:6]: Reserved, always write 0 */
#define ADS129X_CONFIG2_RESERVED_7_6        0x00

/* Bit 5: WCT_CHOP */
#define ADS129X_CONFIG2_WCT_CHOP_OFF        0x00
#define ADS129X_CONFIG2_WCT_CHOP_ON         0x20

/* Bit 4: INT_TEST */
/* To use the internal test signal, set CHnSET.MUX[2:0] to test signal. */
#define ADS129X_CONFIG2_INT_TEST_DISABLE    0x00
#define ADS129X_CONFIG2_INT_TEST_ENABLE     0x10

/* Bit 3: Reserved, always write 0 */
#define ADS129X_CONFIG2_RESERVED_3          0x00

/* Bit 2: TEST_AMP */
#define ADS129X_CONFIG2_TEST_AMP_1MV        0x00
#define ADS129X_CONFIG2_TEST_AMP_2MV        0x04

/* Bit[1:0]: TEST_FREQ */
#define ADS129X_CONFIG2_TEST_FREQ_1HZ       0x00
#define ADS129X_CONFIG2_TEST_FREQ_2HZ       0x01
#define ADS129X_CONFIG2_TEST_FREQ_DC        0x03


/* ============================================================
 * CONFIG3 Register: address 0x03, reset 0x40
 * Controls internal reference buffer and RLD operation.
 * ============================================================ */

/* Bit 7: PD_REFBUF */
#define ADS129X_CONFIG3_REFBUF_POWER_DOWN   0x00
#define ADS129X_CONFIG3_REFBUF_ENABLE       0x80

/* Bit 6: Reserved, must be written as 1 */
#define ADS129X_CONFIG3_RESERVED_6          0x40

/* Bit 5: VREF_4V */
#define ADS129X_CONFIG3_VREF_2V4            0x00
#define ADS129X_CONFIG3_VREF_4V             0x20  /* Use only with 5 V analog supply */

/* Bit 4: RLD_MEAS */
#define ADS129X_CONFIG3_RLD_MEAS_OPEN       0x00
#define ADS129X_CONFIG3_RLD_MEAS_ENABLE     0x10

/* Bit 3: RLDREF_INT */
#define ADS129X_CONFIG3_RLDREF_EXTERNAL     0x00
#define ADS129X_CONFIG3_RLDREF_INTERNAL     0x08

/* Bit 2: PD_RLD */
#define ADS129X_CONFIG3_RLD_POWER_DOWN      0x00
#define ADS129X_CONFIG3_RLD_ENABLE          0x04

/* Bit 1: RLD_LOFF_SENS */
#define ADS129X_CONFIG3_RLD_LOFF_DISABLE    0x00
#define ADS129X_CONFIG3_RLD_LOFF_ENABLE     0x02

/* Bit 0: RLD_STAT, read-only */
#define ADS129X_CONFIG3_RLD_STAT_MASK       0x01


/* ============================================================
 * LOFF Register: 0x04
 * Lead-off detection configuration
 * ============================================================ */
#define ADS129X_LOFF_COMP_TH_95_5       0x00
#define ADS129X_LOFF_COMP_TH_92P5_7P5   0x20
#define ADS129X_LOFF_COMP_TH_90_10      0x40
#define ADS129X_LOFF_COMP_TH_87P5_12P5  0x60
#define ADS129X_LOFF_COMP_TH_85_15      0x80
#define ADS129X_LOFF_COMP_TH_80_20      0xA0
#define ADS129X_LOFF_COMP_TH_75_25      0xC0
#define ADS129X_LOFF_COMP_TH_70_30      0xE0

#define ADS129X_LOFF_VLEAD_OFF_CURRENT_SOURCE  0x00
#define ADS129X_LOFF_VLEAD_OFF_PULLUPDOWN      0x10

#define ADS129X_LOFF_CURRENT_6NA        0x00
#define ADS129X_LOFF_CURRENT_12NA       0x04
#define ADS129X_LOFF_CURRENT_18NA       0x08
#define ADS129X_LOFF_CURRENT_24NA       0x0C

#define ADS129X_LOFF_FREQ_INVALID_WHEN_LOFF_ON  0x00
#define ADS129X_LOFF_FREQ_AC_FDR_DIV4   0x01
#define ADS129X_LOFF_FREQ_DO_NOT_USE    0x02
#define ADS129X_LOFF_FREQ_DC            0x03

/* ============================================================
 * CHnSET Registers: CH1 0x05 ~ CH8 0x0C
 * ADS1294 uses CH1~CH4 only.
 *
 * Bit7   : PDn
 * Bit6:4 : GAINn[2:0]
 * Bit3   : Reserved, always write 0
 * Bit2:0 : MUXn[2:0]
 * ============================================================ */
#define ADS129X_CHSET_POWER_ON          0x00
#define ADS129X_CHSET_POWER_DOWN        0x80

#define ADS129X_CHSET_GAIN_6            0x00
#define ADS129X_CHSET_GAIN_1            0x10
#define ADS129X_CHSET_GAIN_2            0x20
#define ADS129X_CHSET_GAIN_3            0x30
#define ADS129X_CHSET_GAIN_4            0x40
#define ADS129X_CHSET_GAIN_8            0x50
#define ADS129X_CHSET_GAIN_12           0x60

#define ADS129X_CHSET_RESERVED_0        0x00  /* Bit3: always write 0 */
#define ADS129X_CHSET_MUX_NORMAL        0x00
#define ADS129X_CHSET_MUX_SHORTED       0x01
#define ADS129X_CHSET_MUX_RLD_MEAS      0x02
#define ADS129X_CHSET_MUX_MVDD          0x03
#define ADS129X_CHSET_MUX_TEMP          0x04
#define ADS129X_CHSET_MUX_TEST_SIGNAL   0x05
#define ADS129X_CHSET_MUX_RLD_DRP       0x06
#define ADS129X_CHSET_MUX_RLD_DRN       0x07

/* ============================================================
 * RLD_SENSP Register: 0x0D
 * RLD Positive Signal Derivation Register
 *
 * Bit7:0 : RLD8P ~ RLD1P
 * 0 = Disabled
 * 1 = Enabled
 *
 * Note:
 * ADS1294: bits[7:4] are not available.
 * ADS1296: bits[7:6] are not available.
 * ============================================================ */
#define ADS129X_RLD_SENSP_RLD1P         0x01
#define ADS129X_RLD_SENSP_RLD2P         0x02
#define ADS129X_RLD_SENSP_RLD3P         0x04
#define ADS129X_RLD_SENSP_RLD4P         0x08
#define ADS129X_RLD_SENSP_RLD5P         0x10
#define ADS129X_RLD_SENSP_RLD6P         0x20
#define ADS129X_RLD_SENSP_RLD7P         0x40
#define ADS129X_RLD_SENSP_RLD8P         0x80

/* ============================================================
 * RLD_SENSN Register: 0x0E
 * RLD Negative Signal Derivation Register
 *
 * Bit7:0 : RLD8N ~ RLD1N
 * 0 = Disabled
 * 1 = Enabled
 *
 * Note:
 * ADS1294: bits[7:4] are not available.
 * ADS1296: bits[7:6] are not available.
 * ============================================================ */
#define ADS129X_RLD_SENSN_RLD1N         0x01
#define ADS129X_RLD_SENSN_RLD2N         0x02
#define ADS129X_RLD_SENSN_RLD3N         0x04
#define ADS129X_RLD_SENSN_RLD4N         0x08
#define ADS129X_RLD_SENSN_RLD5N         0x10
#define ADS129X_RLD_SENSN_RLD6N         0x20
#define ADS129X_RLD_SENSN_RLD7N         0x40
#define ADS129X_RLD_SENSN_RLD8N         0x80

/* ============================================================
 * LOFF_SENSP Register: 0x0F
 * Positive Signal Lead-Off Detection Register
 *
 * Bit7:0 : LOFF8P ~ LOFF1P
 * 0 = Disabled
 * 1 = Enabled
 *
 * Note:
 * LOFF_STATP bits are valid only when corresponding LOFF_SENSP
 * bits are set to 1.
 * ADS1294: bits[7:4] are not available.
 * ADS1296: bits[7:6] are not available.
 * ============================================================ */
#define ADS129X_LOFF_SENSP_LOFF1P       0x01
#define ADS129X_LOFF_SENSP_LOFF2P       0x02
#define ADS129X_LOFF_SENSP_LOFF3P       0x04
#define ADS129X_LOFF_SENSP_LOFF4P       0x08
#define ADS129X_LOFF_SENSP_LOFF5P       0x10
#define ADS129X_LOFF_SENSP_LOFF6P       0x20
#define ADS129X_LOFF_SENSP_LOFF7P       0x40
#define ADS129X_LOFF_SENSP_LOFF8P       0x80

/* ============================================================
 * LOFF_SENSN Register: 0x10
 * Negative Signal Lead-Off Detection Register
 *
 * Bit7:0 : LOFF8N ~ LOFF1N
 * 0 = Disabled
 * 1 = Enabled
 *
 * Note:
 * LOFF_STATN bits are valid only when corresponding LOFF_SENSN
 * bits are set to 1.
 * ADS1294: bits[7:4] are not available.
 * ADS1296: bits[7:6] are not available.
 * ============================================================ */
#define ADS129X_LOFF_SENSN_LOFF1N       0x01
#define ADS129X_LOFF_SENSN_LOFF2N       0x02
#define ADS129X_LOFF_SENSN_LOFF3N       0x04
#define ADS129X_LOFF_SENSN_LOFF4N       0x08
#define ADS129X_LOFF_SENSN_LOFF5N       0x10
#define ADS129X_LOFF_SENSN_LOFF6N       0x20
#define ADS129X_LOFF_SENSN_LOFF7N       0x40
#define ADS129X_LOFF_SENSN_LOFF8N       0x80

/* ============================================================
 * LOFF_FLIP Register: 0x11
 * Lead-Off Flip Register
 *
 * Bit7:0 : LOFF_FLIP8 ~ LOFF_FLIP1
 * 0 = No flip
 * 1 = Flip pull-up / pull-down polarity
 *
 * Note:
 * ADS1294: bits[7:4] are not available.
 * ADS1296: bits[7:6] are not available.
 * ============================================================ */
#define ADS129X_LOFF_FLIP1              0x01
#define ADS129X_LOFF_FLIP2              0x02
#define ADS129X_LOFF_FLIP3              0x04
#define ADS129X_LOFF_FLIP4              0x08
#define ADS129X_LOFF_FLIP5              0x10
#define ADS129X_LOFF_FLIP6              0x20
#define ADS129X_LOFF_FLIP7              0x40
#define ADS129X_LOFF_FLIP8              0x80

/* ============================================================
 * LOFF_STATP Register: 0x12, Read Only
 * Positive Lead-Off Status Register
 *
 * Bit7:0 : IN8P_OFF ~ IN1P_OFF
 * 0 = Electrode connected
 * 1 = Electrode disconnected
 * ============================================================ */
#define ADS129X_LOFF_STATP_IN1P_OFF     0x01
#define ADS129X_LOFF_STATP_IN2P_OFF     0x02
#define ADS129X_LOFF_STATP_IN3P_OFF     0x04
#define ADS129X_LOFF_STATP_IN4P_OFF     0x08
#define ADS129X_LOFF_STATP_IN5P_OFF     0x10
#define ADS129X_LOFF_STATP_IN6P_OFF     0x20
#define ADS129X_LOFF_STATP_IN7P_OFF     0x40
#define ADS129X_LOFF_STATP_IN8P_OFF     0x80

/* ============================================================
 * LOFF_STATN Register: 0x13, Read Only
 * Negative Lead-Off Status Register
 *
 * Bit7:0 : IN8N_OFF ~ IN1N_OFF
 * 0 = Electrode connected
 * 1 = Electrode disconnected
 * ============================================================ */
#define ADS129X_LOFF_STATN_IN1N_OFF     0x01
#define ADS129X_LOFF_STATN_IN2N_OFF     0x02
#define ADS129X_LOFF_STATN_IN3N_OFF     0x04
#define ADS129X_LOFF_STATN_IN4N_OFF     0x08
#define ADS129X_LOFF_STATN_IN5N_OFF     0x10
#define ADS129X_LOFF_STATN_IN6N_OFF     0x20
#define ADS129X_LOFF_STATN_IN7N_OFF     0x40
#define ADS129X_LOFF_STATN_IN8N_OFF     0x80

/* ============================================================
 * GPIO Register: 0x14
 *
 * Bit7:4 : GPIOD[4:1], GPIO data
 * Bit3:0 : GPIOC[4:1], GPIO control
 *
 * GPIOC bit:
 * 0 = Output
 * 1 = Input
 * ============================================================ */
#define ADS129X_GPIO_GPIOD1             0x10
#define ADS129X_GPIO_GPIOD2             0x20
#define ADS129X_GPIO_GPIOD3             0x40
#define ADS129X_GPIO_GPIOD4             0x80

#define ADS129X_GPIO_GPIOC1_INPUT       0x01
#define ADS129X_GPIO_GPIOC2_INPUT       0x02
#define ADS129X_GPIO_GPIOC3_INPUT       0x04
#define ADS129X_GPIO_GPIOC4_INPUT       0x08

#define ADS129X_GPIO_GPIOC1_OUTPUT      0x00
#define ADS129X_GPIO_GPIOC2_OUTPUT      0x00
#define ADS129X_GPIO_GPIOC3_OUTPUT      0x00
#define ADS129X_GPIO_GPIOC4_OUTPUT      0x00

/* ============================================================
 * PACE Register: 0x15
 *
 * Bit7:5 : Reserved, always write 0
 * Bit4:3 : PACEE[1:0], pace even channel selection
 * Bit2:1 : PACEO[1:0], pace odd channel selection
 * Bit0   : PD_PACE
 * ============================================================ */
#define ADS129X_PACE_RESERVED_0         0x00

#define ADS129X_PACE_PACEE_CH2          0x00
#define ADS129X_PACE_PACEE_CH4          0x08
#define ADS129X_PACE_PACEE_CH6          0x10
#define ADS129X_PACE_PACEE_CH8          0x18

#define ADS129X_PACE_PACEO_CH1          0x00
#define ADS129X_PACE_PACEO_CH3          0x02
#define ADS129X_PACE_PACEO_CH5          0x04
#define ADS129X_PACE_PACEO_CH7          0x06

#define ADS129X_PACE_BUFFER_OFF         0x00
#define ADS129X_PACE_BUFFER_ON          0x01

/* ============================================================
 * RESP Register: 0x16
 *
 * Bit7   : RESP_DEMOD_EN1
 * Bit6   : RESP_MOD_EN1
 * Bit5   : Reserved, always write 1
 * Bit4:2 : RESP_PH[2:0]
 * Bit1:0 : RESP_CTRL[1:0]
 *
 * Note:
 * RESP_DEMOD_EN1 and RESP_MOD_EN1 are ADS129xR-only.
 * For ADS129x, always write these bits as 0.
 * Register reset is 0x00, but bit5 must be written as 1 during register writes.
 * ============================================================ */
#define ADS129X_RESP_DEMOD_OFF          0x00
#define ADS129X_RESP_DEMOD_ON           0x80

#define ADS129X_RESP_MOD_OFF            0x00
#define ADS129X_RESP_MOD_ON             0x40

#define ADS129X_RESP_RESERVED_1         0x20

#define ADS129X_RESP_PHASE_22P5         0x00
#define ADS129X_RESP_PHASE_45           0x04
#define ADS129X_RESP_PHASE_67P5         0x08
#define ADS129X_RESP_PHASE_90           0x0C
#define ADS129X_RESP_PHASE_112P5        0x10
#define ADS129X_RESP_PHASE_135          0x14
#define ADS129X_RESP_PHASE_157P5        0x18

#define ADS129X_RESP_CTRL_NONE          0x00
#define ADS129X_RESP_CTRL_EXTERNAL      0x01
#define ADS129X_RESP_CTRL_INTERNAL      0x02
#define ADS129X_RESP_CTRL_USER          0x03

/* ============================================================
 * CONFIG4 Register: 0x17
 *
 * Bit7:5 : RESP_FREQ[2:0]
 * Bit4   : Reserved, always write 0
 * Bit3   : SINGLE_SHOT
 * Bit2   : WCT_TO_RLD
 * Bit1   : PD_LOFF_COMP
 * Bit0   : Reserved, always write 0
 * ============================================================ */
#define ADS129X_CONFIG4_RESP_FREQ_64KHZ     0x00
#define ADS129X_CONFIG4_RESP_FREQ_32KHZ     0x20
#define ADS129X_CONFIG4_RESP_FREQ_16KHZ     0x40
#define ADS129X_CONFIG4_RESP_FREQ_8KHZ      0x60
#define ADS129X_CONFIG4_RESP_FREQ_4KHZ      0x80
#define ADS129X_CONFIG4_RESP_FREQ_2KHZ      0xA0
#define ADS129X_CONFIG4_RESP_FREQ_1KHZ      0xC0
#define ADS129X_CONFIG4_RESP_FREQ_500HZ     0xE0

#define ADS129X_CONFIG4_RESERVED_4_0        0x00
#define ADS129X_CONFIG4_CONTINUOUS          0x00
#define ADS129X_CONFIG4_SINGLE_SHOT         0x08

#define ADS129X_CONFIG4_WCT_TO_RLD_OFF      0x00
#define ADS129X_CONFIG4_WCT_TO_RLD_ON       0x04

#define ADS129X_CONFIG4_LOFF_COMP_OFF       0x00
#define ADS129X_CONFIG4_LOFF_COMP_ON        0x02

#define ADS129X_CONFIG4_RESERVED_0_0        0x00

/* ============================================================
 * WCT1 Register: 0x18
 * Wilson Central Terminal and Augmented Lead Control Register
 *
 * Bit7   : aVF_CH6
 * Bit6   : aVL_CH5
 * Bit5   : aVR_CH7
 * Bit4   : aVR_CH4
 * Bit3   : PD_WCTA
 * Bit2:0 : WCTA[2:0]
 * ============================================================ */
#define ADS129X_WCT1_AVF_CH6_DISABLE    0x00
#define ADS129X_WCT1_AVF_CH6_ENABLE     0x80

#define ADS129X_WCT1_AVL_CH5_DISABLE    0x00
#define ADS129X_WCT1_AVL_CH5_ENABLE     0x40

#define ADS129X_WCT1_AVR_CH7_DISABLE    0x00
#define ADS129X_WCT1_AVR_CH7_ENABLE     0x20

#define ADS129X_WCT1_AVR_CH4_DISABLE    0x00
#define ADS129X_WCT1_AVR_CH4_ENABLE     0x10

#define ADS129X_WCT1_WCTA_POWER_DOWN    0x00
#define ADS129X_WCT1_WCTA_POWER_ON      0x08

#define ADS129X_WCT1_WCTA_CH1P          0x00
#define ADS129X_WCT1_WCTA_CH1N          0x01
#define ADS129X_WCT1_WCTA_CH2P          0x02
#define ADS129X_WCT1_WCTA_CH2N          0x03
#define ADS129X_WCT1_WCTA_CH3P          0x04
#define ADS129X_WCT1_WCTA_CH3N          0x05
#define ADS129X_WCT1_WCTA_CH4P          0x06
#define ADS129X_WCT1_WCTA_CH4N          0x07

/* ============================================================
 * WCT2 Register: 0x19
 * Wilson Central Terminal Control Register
 *
 * Bit7   : PD_WCTC
 * Bit6   : PD_WCTB
 * Bit5:3 : WCTB[2:0]
 * Bit2:0 : WCTC[2:0]
 * ============================================================ */
#define ADS129X_WCT2_WCTC_POWER_DOWN    0x00
#define ADS129X_WCT2_WCTC_POWER_ON      0x80

#define ADS129X_WCT2_WCTB_POWER_DOWN    0x00
#define ADS129X_WCT2_WCTB_POWER_ON      0x40

#define ADS129X_WCT2_WCTB_CH1P          0x00
#define ADS129X_WCT2_WCTB_CH1N          0x08
#define ADS129X_WCT2_WCTB_CH2P          0x10
#define ADS129X_WCT2_WCTB_CH2N          0x18
#define ADS129X_WCT2_WCTB_CH3P          0x20
#define ADS129X_WCT2_WCTB_CH3N          0x28
#define ADS129X_WCT2_WCTB_CH4P          0x30
#define ADS129X_WCT2_WCTB_CH4N          0x38

#define ADS129X_WCT2_WCTC_CH1P          0x00
#define ADS129X_WCT2_WCTC_CH1N          0x01
#define ADS129X_WCT2_WCTC_CH2P          0x02
#define ADS129X_WCT2_WCTC_CH2N          0x03
#define ADS129X_WCT2_WCTC_CH3P          0x04
#define ADS129X_WCT2_WCTC_CH3N          0x05
#define ADS129X_WCT2_WCTC_CH4P          0x06
#define ADS129X_WCT2_WCTC_CH4N          0x07


#endif /* ADS129X_REGS_H */