#ifndef __IP_EXPAND_H__
#define __IP_EXPAND_H__

#include <stdint.h>
#define PIN_SW_RESET (1 << 0)  // P0: SW_PESRT
#define PIN_SSD_RESET (1 << 1) // P1: SSD_PESRT
#define PIN_S01_RESET (1 << 2) // P2: S01_PESRT
#define PIN_S02_RESET (1 << 3) // P3: S02_PESRT
#define PIN_S03_RESET (1 << 4) // P4: S03_PESRT
#define PIN_S04_RESET (1 << 5) // P5: S04_PESRT

#define TCA6408_CFG_REG 0x03
#define TCA6408_OUT_REG 0x01
#define DEV3_ADDR_A 0x20U // 6408
#define DEV3_ADDR_B 0x21U // 6408

#ifdef __cplusplus
extern "C"
{
#endif
    int pcie_slave_reset(void);
#ifdef __cplusplus
}
#endif
#endif // __IP_EXPAND_H__
