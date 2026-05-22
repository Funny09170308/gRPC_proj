#include <stdio.h>
#include <sys/time.h>

#include "main.h"
#include "public/public.h"
#include "./application/app.h"
#include "./pcie/pcie_func.h"
#include "./axi_gpio/axi_gpio.h"
#include "./spi_dev/spi2_test.h"
#include "./param_mgr/param_mgr.h"
#include "./spi_dev/lmk04828_drv.h"
#include "./platform_log/platform_log.h"

int main(void)
{
    setbuf(stdout, NULL);

    plog_init(
        // P_LOG_LEVEL_DEBUG |
        P_LOG_LEVEL_INFO |
        P_LOG_LEVEL_WARNING |
        P_LOG_LEVEL_ERROR |
        P_LOG_LEVEL_FATAL);
    // 设备参数初始化
    device_info_init();
    // 子卡空间初始化

#define USE_FSBL_PCIE
#ifndef USE_FSBL_PCIE
    // 04828时钟选择
    gpio_set_value(IO_CONFIG_1, 1);
    gpio_set_value(IO_CONFIG_2, 0);
    gpio_set_value(IO_CONFIG_3, 1);
    lmk04828_reario_init();
    cdce6214_reg_config();
    lmkdb1108_reg_config();
    sleep(2);
    pcie_reset(); // i2c IO扩展芯片初始化
#endif
// 子卡空间初始化
#if SLAVE_USE_PCIE
    pcie_dev_init();
    QAConfigRegisterInit();
#else
    chip2chip_dev_init();
#endif
    // 后IO板内存空间初始化
    public_dev_init();
    slave_card_init();
    task_creat();
    while (1)
    {
        temp_monitor();
        sleep(10);
    }
#if SLAVE_USE_PCIE
    pcie_dev_deinit();
#else
    chip2chip_dev_deinit();
#endif
    return 0;
}
