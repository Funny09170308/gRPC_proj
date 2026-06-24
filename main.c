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
#include "./platform_log/platform_log.h"

int main(void)
{
    setbuf(stdout, NULL);

    plog_init(
        P_LOG_LEVEL_REPEAT |
        P_LOG_LEVEL_DEBUG |
        P_LOG_LEVEL_INFO |
        P_LOG_LEVEL_WARNING |
        P_LOG_LEVEL_ERROR |
        P_LOG_LEVEL_FATAL);
    // 后续初始化依赖此项，必须先初始化本地AXI从机访问空间
    public_dev_init();
    clock_detect();
    device_initalization();
    while (1)
    {
        temp_monitor();
        sleep(10);
    }
    pcie_dev_deinit();
    return 0;
}
