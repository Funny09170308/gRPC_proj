#include <stdio.h>
#include <sys/time.h>

#include "main.h"
#include "public/public.h"
#include "./application/app.h"
#include "./pcie/pcie_func.h"
#include "./pcie/qa/qa_func.h"
#include "./axi_gpio/axi_gpio.h"
#include "./spi_dev/spi2_test.h"
#include "./param_mgr/param_mgr.h"
#include "./spi_dev/lmk04828_drv.h"
#include "./lib/include/platform_log/platform_log.h"

int main(void)
{
    setbuf(stdout, NULL);

    plog_init(
        // P_LOG_LEVEL_DEBUG |
        P_LOG_LEVEL_INITIAL |
        // P_LOG_LEVEL_MONITOR |
        P_LOG_LEVEL_INFO |
        P_LOG_LEVEL_WARNING |
        P_LOG_LEVEL_ERROR |
        P_LOG_LEVEL_FATAL);
    // 子卡同步初始化
    clock_sync();
    // 设备参数初始化
    device_info_init();

    // 子卡空间初始化
    pcie_dev_init();
    sleep(2);
    sync_init();
    sleep(1);
    slot_mio_pulse_init();
    sleep(2);
    dac_sync_init();
    AWGConfigRegisterInit();
    QAConfigRegisterInit();

    // 后IO板内存空间初始化
    public_dev_init();
    slave_card_init();
    sleep(5);
    rf_pwr_supply();
    slave_card_detect();
    task_creat();
    while (1)
    {
        temp_monitor();
        sleep(10);
    }

    pcie_dev_deinit();
    return 0;
}
