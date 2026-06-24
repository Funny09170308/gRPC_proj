#ifndef __AXI_GPIO_H__
#define __AXI_GPIO_H__

#include <stdint.h>
#define EMIO_BASE_ID (78)

#define PCIE_PS_RST 43 // gpio512

#define IO_CONFIG_1 EMIO_BASE_ID + 0
#define IO_CONFIG_2 EMIO_BASE_ID + 1
#define IO_CONFIG_3 EMIO_BASE_ID + 2
#define AURORA_ENABLE_CTRL EMIO_BASE_ID + 11
#define PCIE_SWITCH_RST_CTRL EMIO_BASE_ID + 12
#define LED_ENABLE_CTRL EMIO_BASE_ID + 13
#define S01_PWR_CTRL EMIO_BASE_ID + 15
#define S02_PWR_CTRL EMIO_BASE_ID + 16
#define S03_PWR_CTRL EMIO_BASE_ID + 17
#define S04_PWR_CTRL EMIO_BASE_ID + 18
#define INIT_FINISH_CTRL EMIO_BASE_ID + 21
#define SLOT_SYNC_SIGNAL EMIO_BASE_ID + 23
#define SLOT_SYNC_FINISH EMIO_BASE_ID + 25
#define SLOT_SYNC_PULSE EMIO_BASE_ID + 26
#define CLOCK_SOURCE_IDENTITY EMIO_BASE_ID + 27
#ifdef __cplusplus
extern "C"
{
#endif

    int gpio_set_value(int logic_gpio_num, int value);
    void pcie_switch_reset(void);
    void slot_mio_pulse_init(void);
    void clock_sync(void);
    void clock_source_sel(uint32_t sel);
    void slave_card_pwr_suplly(void);
    void clock_select(int8_t sel);
    void clock_init_identify(uint32_t sel);
#ifdef __cplusplus
}
#endif
#endif // __AXI_GPIO_H__
