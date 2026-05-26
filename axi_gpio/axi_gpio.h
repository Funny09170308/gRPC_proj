#ifndef __AXI_GPIO_H__
#define __AXI_GPIO_H__

#define PCIE_PS_RST 43          // gpio377
#define IO_CONFIG_1 78          // gpio412
#define IO_CONFIG_2 79          // gpio413
#define IO_CONFIG_3 80          // gpio414
#define IO_POWER_CTRL 89        // gpio423
#define PCIE_SWITCH_RST_CTRL 90 // gpio424
#define SLOT_SYNC_SIGNAL 101    // gpio435
#define SLOT_SYNC_FINISH 103    // gpio437

void gpio_power_suplly(void);
int gpio_set_value(int logic_gpio_num, int value);
void pcie_switch_reset(void);
void clock_sync(void);
#endif // __AXI_GPIO_H__
