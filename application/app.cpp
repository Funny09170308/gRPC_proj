#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "app.h"
#include "../public/public.h"
#include "../pcie/pcie_func.h"
#include "../pcie/common_func.h"
#include "../axi_gpio/axi_gpio.h"
#include "../i2c/i2c_reg_config.h"
#include "../param_mgr/param_mgr.h"
#include "../spi_dev/lmk04828_drv.h"
#include "../i2c/io_expand/io_expand.h"
#include "../platform_log/platform_log.h"

uint8_t periphrailAddr[12] = {
    FAN_SPEED_CTRL,
    LED_01_CTRL,
    LED_02_CTRL,
    LED_03_CTRL,
    LED_04_CTRL,
    LED_05_CTRL,
    LED_06_CTRL,
    LED_07_CTRL,
    LED_08_CTRL,
    LED_09_CTRL,
    LED_10_CTRL,
};

/*
 * 这里故意屏蔽底层 dma_xxx 的返回值差异。
 * 如果你底层已经有明确返回值，可在这里自行补充错误判定。
 */
static int app_dma_write_data(uint32_t chip_id, uint64_t addr, uint32_t len, const uint8_t *buf)
{
    dma_write_data((int)chip_id, addr, len, (uint8_t *)buf);
    return 0;
}

static int app_dma_read_data(uint32_t chip_id, uint64_t addr, uint32_t len, uint8_t *buf)
{
    dma_read_data((int)chip_id, addr, len, buf);
    return 0;
}

void task_creat(void)
{
    pthread_t grpcServer_tid;
    status_led_ctrl(1);
    P_LOG_INFO("Task init finish!");
}

void slave_card_detect(void)
{
    PcieBoardInfo *g_pcie_board_info = get_pcie_board_info();
    int cardCount = get_subcard_count();
    // TODO:此检测方式不支持混插, 否则会误报
    if (g_pcie_board_info->awg_board_num != 0)
    {
        if (cardCount < 4)
        {
            status_led_ctrl(0);
        }
    }
    else if (g_pcie_board_info->qa_board_num != 0)
    {
        if (cardCount < 2)
        {
            status_led_ctrl(0);
        }
    }
}

void device_initalization(void)
{
    // 设备参数初始化
    device_info_init();
    // 子卡空间初始化
    pcie_dev_init();
    // 子卡同步初始化
    clock_sync();
    sleep(2);
    sync_init();
    sleep(1);
    slot_mio_pulse_init();
    sleep(2);
    dac_sync_init();

    // 后IO板内存空间初始化
    slave_card_init();
    slave_card_detect();
    // 创建任务
    task_creat();
}

void clock_detect(void)
{
    // 默认以外时钟启动
    clock_source_sel(0);
    lmk04828_internel_init(DEV_LNAWG);
    sleep(1);
    fan_speed_set(600);
    led_init();
    // 检测时钟状态0:AWG机箱;1:QA机箱;
    clk_status_decet(0);
    slave_card_pwr_suplly();
    power_suplly_init();
    if (1 == common_reg_data_get(PUBLIC_PERIPHERAL_BASEAADDR + CLOCK_STATUS))
    {
        cdce6214_reg_config();
        P_LOG_DEBUG("6214 init finished ");
    }
    else if (0 == common_reg_data_get(PUBLIC_PERIPHERAL_BASEAADDR + CLOCK_STATUS))
    {
        cdce6214_internel_reg_config();
    }
    lmkdb1108_reg_config();
    sleep(3);
    pcie_switch_reset();
    pcie_slave_reset();
}

void fan_speed_set(uint32_t fanSpeed)
{
    P_LOG_DEBUG("Set fan speed %d", fanSpeed);
    common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + FAN_SPEED_CTRL, fanSpeed);
}

void led_init(void)
{
    gpio_set_value(LED_ENABLE_CTRL, 1);
    for (uint8_t i = 1; i <= 10; ++i)
    {
        common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + (periphrailAddr[i]), LED_OFF);
        usleep(10000);
    }
    gpio_set_value(INIT_FINISH_CTRL, 1);
}

void clk_status_decet(uint32_t devType)
{
    uint32_t count = 1000;
    uint32_t state = 0;
    do
    {
        state = common_reg_data_get(PUBLIC_PERIPHERAL_BASEAADDR + CLOCK_STATUS);
        P_LOG_REPEAT("FPGA PLL status:%d", state);
        if (1 == state)
        {
            common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + LED_06_CTRL, LED_OFF);
            break;
        }
        if (0 == state)
        {
            common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + LED_06_CTRL, LED_RED);
        }
        usleep(10000);
    } while (count--);
    if (0 == state)
    {
        // 选为内参考
        P_LOG_INFO("switch to internel clock source");
        clock_select(0);
        common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + LED_06_CTRL, LED_GREEN & LED_RED);
        lmk04828_internel_init(devType);
    }
}
