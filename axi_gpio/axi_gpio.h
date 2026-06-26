#ifndef __AXI_GPIO_H__
#define __AXI_GPIO_H__

#define EMIO_CHIP_OFFSET 78

#define PCIE_PS_RST 43
#define IO_CONFIG_1 (EMIO_CHIP_OFFSET + 0)
#define IO_CONFIG_2 (EMIO_CHIP_OFFSET + 1)
#define IO_CONFIG_3 (EMIO_CHIP_OFFSET + 2)
#define IO_POWER_CTRL (EMIO_CHIP_OFFSET + 11)
#define PCIE_SWITCH_RST_CTRL (EMIO_CHIP_OFFSET + 12)
#define SLOT_SYNC_SIGNAL (EMIO_CHIP_OFFSET + 23)
#define SLOT_SYNC_FINISH (EMIO_CHIP_OFFSET + 25)
#define SLOT_SYNC_PULSE (EMIO_CHIP_OFFSET + 26)

void gpio_power_suplly(void);
int gpio_set_value(int logic_gpio_num, int value);
void pcie_switch_reset(void);
void slot_mio_pulse_init(void);
void clock_sync(void);
#endif // __AXI_GPIO_H__
