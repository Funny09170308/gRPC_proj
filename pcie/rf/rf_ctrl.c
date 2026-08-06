#include "rf_ctrl.h"
#include "../common_func.h"
#include "../../platform_log/platform_log.h"

uint32_t build_reg_value(uint8_t chSel, uint8_t chEn, uint8_t LESel, uint8_t ATTEN)
{
    uint32_t reg_val = C_FIELD_PACK(ATTEN, C_ATTEN_START_BIT, C_ATTEN_BIT_WIDTH) |
                       C_FIELD_PACK(LESel, C_CH_SEL_START_BIT, C_CH_SEL_BIT_WIDTH) |
                       C_FIELD_PACK(chSel, C_LE_SEL_START_BIT, C_LE_SEL_BIT_WIDTH) |
                       C_FIELD_PACK(chEn, C_CH_EN_START_BIT, C_CH_EN_BIT_WIDTH);
    return reg_val;
}

void set_latch(void)
{
    rf_reg_data_set(0x80010000, 0);
    rf_reg_data_set(0x80010000, 1);
    rf_reg_data_set(0x80010000, 0);
}

#define C_SET_ATTEN_0 0
#define C_SET_ATTEN_1 1
void set_ch_atten(uint8_t ch, uint8_t sel, uint8_t attenVal)
{
    if (ch > C_RF_TOTAL_CH_NUM)
    {
        return;
    }
    uint32_t regVal = build_reg_value(ch, (uint8_t)true, sel, attenVal);
    P_LOG_DEBUG("Set rf ch%d atten%d value%#x, regVal = %#x", ch, sel, attenVal, regVal);
    rf_reg_data_set(0x80010000 + 1 * 4,regVal);
    set_latch();
}
