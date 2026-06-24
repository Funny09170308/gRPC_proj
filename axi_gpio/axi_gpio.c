#include <stdio.h>
#include "axi_gpio.h"
#include "../platform_log/platform_log.h"

#define GPIOBASE 512

int gpio_set_value(int logic_gpio_num, int value)
{
    int global_gpio = logic_gpio_num + GPIOBASE;

    char path[64];
    FILE *f;
    P_LOG_REPEAT("Set GPIO%d value: %d", global_gpio, value);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", global_gpio);
    f = fopen(path, "w");
    if (!f)
    {
        perror("fopen");
        return -1;
    }

    fprintf(f, "%d", value ? 1 : 0);
    fclose(f);
    return 0;
}

void pcie_switch_reset(void)
{
    gpio_set_value(PCIE_PS_RST, 0);
    sleep(1);
    gpio_set_value(PCIE_PS_RST, 1);
    P_LOG_INFO("PCIe slave reset release succeed!");
}

void slot_mio_pulse_init(void)
{
    gpio_set_value(SLOT_SYNC_PULSE, 0);
    gpio_set_value(SLOT_SYNC_PULSE, 1);
    gpio_set_value(SLOT_SYNC_PULSE, 0);
}

void clock_sync(void)
{
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    gpio_set_value(SLOT_SYNC_SIGNAL, 1);
    usleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    sleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    gpio_set_value(SLOT_SYNC_SIGNAL, 1);
    usleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    sleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    gpio_set_value(SLOT_SYNC_SIGNAL, 1);
    usleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    usleep(100);
    gpio_set_value(SLOT_SYNC_FINISH, 1);
}

void clock_source_sel(uint32_t sel)
{
    if (0 == sel)
    {
        gpio_set_value(IO_CONFIG_1, 1);
        gpio_set_value(IO_CONFIG_2, 0);
        gpio_set_value(IO_CONFIG_3, 1);
    }
    else if (1 == sel)
    {
        gpio_set_value(IO_CONFIG_1, 1);
        gpio_set_value(IO_CONFIG_2, 0);
        gpio_set_value(IO_CONFIG_3, 0);
    }
}

void slave_card_pwr_suplly(void)
{
    gpio_set_value(S01_PWR_CTRL, 1);
    usleep(150000);
    gpio_set_value(S02_PWR_CTRL, 1);
    usleep(150000);
    gpio_set_value(S03_PWR_CTRL, 1);
    usleep(150000);
    gpio_set_value(S04_PWR_CTRL, 1);
}

void clock_select(int8_t sel)
{
    gpio_set_value(IO_CONFIG_3, sel);
}
void clock_init_identify(uint32_t sel)
{
    gpio_set_value(CLOCK_SOURCE_IDENTITY, sel);
    P_LOG_INFO("clock identify emio set to %d \n\r", sel);
}
