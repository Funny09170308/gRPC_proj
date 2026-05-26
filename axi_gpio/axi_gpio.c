#include <stdio.h>
#include "axi_gpio.h"
#include "../platform_log/platform_log.h"

#define GPIO334 334
void gpio_power_suplly(void)
{
    gpio_set_value(IO_POWER_CTRL, 1);
}

int gpio_set_value(int logic_gpio_num, int value)
{
    int global_gpio = logic_gpio_num + GPIO334;

    char path[64];
    FILE *f;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", global_gpio);
    f = fopen(path, "w");
    if (!f)
    {
        perror("fopen");
        return -1;
    }

    fprintf(f, "%d", value ? 1 : 0);
    fclose(f);
    P_LOG_DEBUG("Set GPIO%d value: %d", global_gpio, value);
    return 0;
}

void pcie_switch_reset(void)
{
    gpio_set_value(PCIE_PS_RST, 0);
    sleep(1);
    gpio_set_value(PCIE_PS_RST, 1);
    P_LOG_INFO("PCIe slave reset release succeed!");
}
void clock_sync(void)
{
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    gpio_set_value(SLOT_SYNC_SIGNAL, 1);
    usleep(1);
    gpio_set_value(SLOT_SYNC_SIGNAL, 0);
    usleep(100);
    gpio_set_value(SLOT_SYNC_FINISH, 1);
}
