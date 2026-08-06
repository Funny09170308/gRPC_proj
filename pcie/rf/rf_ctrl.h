#ifndef __RF_CTRL_H__
#define __RF_CTRL_H__

#include <stdio.h>
#include <stdint.h>

#define C_RF_DA_CH_NUM 4
#define C_RF_AD_CH_NUM 4
#define C_RF_TOTAL_CH_NUM 8

#define C_ATTEN_START_BIT 0
#define C_ATTEN_BIT_WIDTH 8

#define C_CH_SEL_START_BIT 8
#define C_CH_SEL_BIT_WIDTH 8

#define C_LE_SEL_START_BIT 14
#define C_LE_SEL_BIT_WIDTH 1

#define C_CH_EN_START_BIT 15
#define C_CH_EN_BIT_WIDTH 1

// 生成指定位宽的掩码
#define C_FIELD_MASK_RAW(bitWidth) ((1 << bitWidth) - 1)
// 生成在寄存器中对齐后的掩码
#define C_FIELD_MASK(start, width) (C_FIELD_MASK_RAW(width) << (start))
// 将val打包到对应的字段
#define C_FIELD_PACK(val, start, width)  (((val) & C_FIELD_MASK_RAW(width)) << (start))
// 从对应字段获取val
#define C_FIELD_UNPACK(reg_val, start, width) (((reg_val) >> (start)) & FIELD_MASK_RAW(width))
// 更新寄存器变量中某一个字段
#define C_FIELD_MODIFY(dst, val, start, width)  \
do {                                          \
    (dst) &= ~C_FIELD_MASK((start), (width));    \
    (dst) |= C_FIELD_PACK((val), (start), (width)); \
} while(0)

#ifdef __cplusplus
extern "C"
{
#endif

void set_ch_atten(uint8_t ch, uint8_t sel, uint8_t attenVal);

#ifdef __cplusplus
}
#endif

#endif // __RF_CTRL_H__
