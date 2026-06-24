#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#include <dirent.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "common_func.h"
#include "../public/public.h"
#include "../pcie/pcie_func.h"
#include "../application/app.h"
#include "../axi_gpio/axi_gpio.h"
#include "../axi_device/axi_dev_drv.h"
#include "../platform_log/platform_log.h"

/// @brief 设备信息
static systemConfig_t *s_sysConfig;
/// @brief 从卡地址映射信息
static axiDevice_t s_axiChipAddrCtx[CHIP_NUM] = {};
static axiDevice_t s_publicPeripherlAddrCtx = {};

uint8_t s_QALEDOffset[8] = {
    LED_08_CTRL,
    LED_03_CTRL,
    LED_02_CTRL,
    LED_07_CTRL,
    LED_09_CTRL,
    LED_05_CTRL,
    LED_04_CTRL,
    LED_10_CTRL,
};

uint8_t s_AWGLEDOffset[8] = {
    LED_02_CTRL,
    LED_03_CTRL,
    LED_04_CTRL,
    LED_05_CTRL,
    LED_07_CTRL,
    LED_08_CTRL,
    LED_09_CTRL,
    LED_10_CTRL,
};

void public_dev_init(void)
{
    get_device_config(&s_sysConfig);
    axi_device_init(&s_publicPeripherlAddrCtx, "public perh", PUBLIC_PERIPHERAL_BASEAADDR, PUBLIC_PERIPHERAL_LENGTH);
}

void public_dev_deinit(void)
{
    axi_device_release(&s_publicPeripherlAddrCtx);
}

void chip2chip_dev_init(void)
{
    for (uint8_t index; index < CHIP_NUM; ++index)
    {
        uint64_t channelBaseAddr = CHID_CHIP_BASEADDR + (index * (CHIP_SIZE + 1));
        P_LOG_DEBUG("Chip %d physical base addr: %llx", index, channelBaseAddr);
        char nameBuf[AXI_DEVICE_NAME_MAX_LENTH];
        sprintf(nameBuf, "Slave chip %d", (index + 1));
        axi_device_init(&s_axiChipAddrCtx[index], nameBuf, channelBaseAddr, CHIP_SIZE);
    }
}

void chip2chip_dev_deinit(void)
{
    for (uint8_t index; index < CHIP_NUM; ++index)
    {
        if (s_axiChipAddrCtx[index].m_virt_base != NULL)
        {
            axi_device_release(&s_axiChipAddrCtx[index]);
        }
    }
}

void slave_card_init(void)
{
    P_LOG_DEBUG("Start to init slave card!");
    // AWG init func
    lnawg_trig_source_init();
    // QA init func
    QAConfigRegisterInit();
    qa_trig_source_init();
}

int chip_calculate(int64_t addr)
{
#define C_RERIO_CARD 4
    int rtn = -1;
    uint16_t highAddr = (addr >> 32) & 0xff;
    if (highAddr >= 0x10 && highAddr < 0x14)
    {
        rtn = 0;
    }
    else if (highAddr >= 0x14 && highAddr < 0x18)
    {
        rtn = 1;
    }
    else if (highAddr >= 0x18 && highAddr < 0x1C)
    {
        rtn = 2;
    }
    else if (highAddr >= 0x1C && highAddr < 0x20)
    {
        rtn = 3;
    }
    else
    {
        rtn = C_RERIO_CARD;
    }
    P_LOG_DEBUG("Address: %#llx high addr: %#x to chip: %d", addr, highAddr, rtn);
    return rtn;
}

int common_reg_data_set(int64_t baseAddr, uint32_t value)
{
    int32_t chip = chip_calculate(baseAddr);
    int32_t *packageBaseAddr;
    int64_t offset;
    if (chip < 0)
    {
        P_LOG_ERROR("Addr phrase to chip failed!...%llx", baseAddr);
        return -1;
    }
    else if (chip == 4)
    {
        offset = (baseAddr - s_publicPeripherlAddrCtx.m_phys_base) / 4;
        packageBaseAddr = s_publicPeripherlAddrCtx.m_virt_base + offset;
    }
    else
    {
        offset = (baseAddr - s_axiChipAddrCtx[chip].m_phys_base) / 4;
        packageBaseAddr = s_axiChipAddrCtx[chip].m_virt_base + offset;
    }
    P_LOG_DEBUG("Write lnawg signal data %#x(Dec:%d) to DDR addr:%#llx(offset: %llx, virt: %#llx)",
                value,
                value,
                baseAddr,
                offset,
                packageBaseAddr);
    *(_IO uint32_t *)packageBaseAddr = value;
    return 0;
}

uint32_t common_pcie_user_reg_data_get(int32_t chip, uint64_t baseAddr)
{
    uint32_t rtn = 0;
    xdma_read_user_space(chip, baseAddr, &rtn);
    return rtn;
}

int common_pcie_user_reg_data_set(int32_t chip, uint64_t baseAddr, int32_t value)
{
    return xdma_write_user_space(chip, baseAddr, (uint32_t)value);
}

uint32_t common_reg_data_get(int64_t baseAddr)
{
    uint32_t rtn = 0;
    int32_t chip = chip_calculate(baseAddr);
    uint32_t *packageBaseAddr;
    int64_t offset;
    if (chip < 0)
    {
        P_LOG_ERROR("Addr phrase to chip failed!");
        return -1;
    }
    else if (chip == 4)
    {
        offset = (baseAddr - s_publicPeripherlAddrCtx.m_phys_base) / 4;
        packageBaseAddr = s_publicPeripherlAddrCtx.m_virt_base + offset;
    }
    else
    {
        // 计算偏移量(4字节偏移)
        offset = (baseAddr - s_axiChipAddrCtx[chip].m_phys_base) / 4;
        packageBaseAddr = s_axiChipAddrCtx[chip].m_virt_base + offset;
    }
    P_LOG_DEBUG("Read lnawg signal reg data: %#x(Dec: %d) from addr:%#llx(offset: %llx, virt: %#llx)",
                rtn,
                rtn,
                baseAddr,
                offset,
                packageBaseAddr);
    rtn = *(_IO uint32_t *)packageBaseAddr;
    return rtn;
}

int common_huge_data_set(int64_t addr,
                         int totalPackageNum,
                         int sequenceNum,
                         int packageIndex,
                         int *sequenceBuffer)
{
    int32_t chip = chip_calculate(addr);
    if (chip < 0 || chip == 4)
    {
        P_LOG_ERROR("Addr parse to chip failed!");
        return -1;
    }
    int64_t byte_offset = addr - s_axiChipAddrCtx[chip].m_phys_base;
    int64_t word_offset = byte_offset / 4;
    int64_t pkg_start_word = word_offset;
    int32_t *packageBaseAddr = s_axiChipAddrCtx[chip].m_virt_base + pkg_start_word;
    uint64_t phy_addr = s_axiChipAddrCtx[chip].m_phys_base + (pkg_start_word * 4);
    P_LOG_DEBUG("Write package %d/%d to DDR: virt=%p, phy=0x%llx, words=%d",
                packageIndex, totalPackageNum,
                packageBaseAddr,
                phy_addr,
                sequenceNum);
    memcpy(packageBaseAddr, sequenceBuffer, sequenceNum * sizeof(int32_t));

    return 0;
}

int common_huge_data_get(int64_t addr,
                         int32_t length,
                         int32_t *buffer)
{
    int32_t chip = chip_calculate(addr);
    if (chip < 0)
    {
        P_LOG_ERROR("Addr parse to chip failed!");
        return -1;
    }
    // 字节偏移
    int64_t byte_offset = addr - s_axiChipAddrCtx[chip].m_phys_base;
    // 32bit 字偏移
    int64_t word_offset = byte_offset / 4;
    // 正确的源地址
    int32_t *src = s_axiChipAddrCtx[chip].m_virt_base + word_offset;
    // 正确的物理地址
    uint64_t real_phy_addr = s_axiChipAddrCtx[chip].m_phys_base + (word_offset * 4);
    P_LOG_DEBUG("Read package from DDR: virt=%p, phy=0x%llx, length=%d words",
                src,
                real_phy_addr,
                length);
    memcpy(buffer, src, length * sizeof(int32_t));
    return 0;
}

void rerioTriggerSourceSet(uint32_t source)
{
    P_LOG_DEBUG("Set rerio trigger source: %d", source);
    if (source == 0)
    {
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_0, 1);
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_1, 0);
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_1, 1);
    }
    else if (source == 1)
    {
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_0, 1);
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_1, 1);
        common_reg_data_set(C_RERIO_TRIGGER_BASEADDR + E_TRIGGER_CTRL_1, 1);
    }
}

void LED_control(uint8_t offset, uint32_t status)
{
    common_reg_data_set(PUBLIC_PERIPHERAL_BASEAADDR + offset, status);
}

void status_led_ctrl(uint8_t ok)
{
    if (ok)
    {
        DIR *dir = opendir("/dev");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            // 检测xdma设备节点
            if (strncmp(entry->d_name, "xdma", 4) == 0)
            {
                closedir(dir);
                LED_control(LED_01_CTRL, LED_GREEN);
                return;
            }
        }
        LED_control(LED_01_CTRL, LED_GREEN | LED_RED);
        closedir(dir);
        return;
    }
    else
    {
        LED_control(LED_01_CTRL, LED_RED);
        return;
    }
}

#define TEMPERATURE_WALL 95.0f
#define RERIO_KERNEL_TEMP_PATH "/sys/bus/iio/devices/iio:device0/in_temp0_ps_temp_raw"
#define RERIO_FPGA_TEMP_PATH "/sys/bus/iio/devices/iio:device0/in_temp2_pl_temp_raw"

float read_temperature(void)
{
    FILE *fp;
    int raw;

    fp = fopen(RERIO_KERNEL_TEMP_PATH, "r");
    if (fp == NULL)
    {
        perror("Failed to open temperature file");
        return -1.0f;
    }

    fscanf(fp, "%d", &raw);
    fclose(fp);

    // ZynqMP�¶�ת����ʽ
    return ((raw * 509.314f) / 65536.0f) - 280.23f;
}

void cut_off_slave_power(void)
{
    int gpios[] = {427, 428, 429, 430};
    int i;

    for (i = 0; i < sizeof(gpios) / sizeof(gpios[0]); i++)
    {
        sys_gpio_export(gpios[i]);
        sys_gpio_set_direction(gpios[i], "out");
        sys_gpio_set_value(gpios[i], 0);
    }
}

PcieBoardInfo *s_boardInfo;

void temp_monitor(void)
{
#define AWG_TEMP_OFFSET 0x00010000 + (100 << 2)
#define QA_TEMP_OFFSET 0x00010000 + (1 << 2)
    s_boardInfo = get_pcie_board_info();
    float kernel_temp = read_temperature();
    P_LOG_DEBUG("Kernel temp: %.2f", kernel_temp);
    if (kernel_temp >= TEMPERATURE_WALL)
    {
        cut_off_slave_power();
        status_led_ctrl(0);
        P_LOG_ERROR("Temperature too high! Kernel temp: %.2f", kernel_temp);
    }
    uint32_t value;
    for (uint8_t i = 0; i < s_boardInfo->awg_board_num; ++i)
    {
        value = common_pcie_user_reg_data_get(i, AWG_TEMP_OFFSET);
        float slave_temp = value * 507.5921310 / pow(2, 16) - 279.42657680;
        P_LOG_REPEAT("AWG%d temp: %.2f", (i + 1), slave_temp);
        if (slave_temp >= TEMPERATURE_WALL)
        {
            cut_off_slave_power();
            status_led_ctrl(0);
            P_LOG_ERROR("Temperature too high! Kernel temp: %.2f", slave_temp);
            while (1)
            {
                // 当温度过高时，直接进入死循环，等待人工干预重启设备
            }
        }
    }
    for (uint8_t i = 0; i < s_boardInfo->qa_board_num; ++i)
    {
        value = common_pcie_user_reg_data_get(i, QA_TEMP_OFFSET);
        float slave_temp = value * 507.5921310 / pow(2, 16) - 279.42657680;
        P_LOG_REPEAT("QA%d temp: %.2f", (i + 1), slave_temp);
        if (slave_temp >= TEMPERATURE_WALL)
        {
            cut_off_slave_power();
            status_led_ctrl(0);
            P_LOG_ERROR("Temperature too high! QA%d temp: %.2f", (i + 1), slave_temp);
            while (1)
            {
                // 当温度过高时，直接进入死循环，等待人工干预重启设备
            }
        }
    }
}

char *device_ip_addr_get(void)
{
    char ip[20];
    strncpy(ip, s_sysConfig->ip, IP_STR_LEN - 1);
    P_LOG_DEBUG("Get sys ip: %s to file", ip);
    return ip;
}

void device_ip_addr_set(const char *ip)
{
    strncpy(s_sysConfig->ip, ip, IP_STR_LEN - 1);
    s_sysConfig->ip[IP_STR_LEN - 1] = '\0';
    P_LOG_DEBUG("Ready to write ip: %s to file", s_sysConfig->ip);
    if (0 == save_device_config(DEVICE_CONFIG_PATH, s_sysConfig))
    {
        P_LOG_DEBUG("Write ip: %s to file succeed", s_sysConfig->ip);
    }
}

void device_ip_addr_dhcp_set(int dhcpEn)
{
    s_sysConfig->dhcpen = dhcpEn;
    P_LOG_DEBUG("Ready to write dhcpen: %d to file", s_sysConfig->dhcpen);
    if (0 == save_device_config(DEVICE_CONFIG_PATH, s_sysConfig))
    {
        P_LOG_DEBUG("Write dchpen: %d to file succeed", s_sysConfig->dhcpen);
    }
}

void device_mac_addr_set(const char *mac)
{
    strncpy(s_sysConfig->mac, mac, MAC_STR_LEN - 1);
    s_sysConfig->mac[MAC_STR_LEN - 1] = '\0';
    P_LOG_DEBUG("Ready to write mac: %s to file", s_sysConfig->mac);
    if (0 == save_device_config(DEVICE_CONFIG_PATH, s_sysConfig))
    {
        P_LOG_DEBUG("Write mac: %s to file succeed", s_sysConfig->mac);
    }
}

void set_qa_ch_led_status(int32_t logical_ch, int32_t status)
{
    P_LOG_DEBUG("Set channel %d led status: %d", logical_ch, status);
    LED_control(s_QALEDOffset[logical_ch - 1], status);
}

void set_awg_ch_led_status(int32_t logical_ch, int32_t status)
{
    P_LOG_DEBUG("Set channel %d led status: %d", logical_ch, status);
    LED_control(s_AWGLEDOffset[logical_ch - 1], status);
}
