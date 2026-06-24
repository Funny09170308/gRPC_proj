#ifndef __COMMON_FUNCTION__
#define __COMMON_FUNCTION__

#include "../device_info.h"
#include "../param_mgr/param_mgr.h"
#include "../axi_device/axi_dev_drv.h"

#define C_USER_SPACE_CONFIG_OFFSET 0x10000
#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        PCIE_CHIP_ID = 0x0,
    } publicUserSpaceReg_t;

    typedef enum
    {
        DEVICE_TYPE_LNAWG = 0x000000F0,
        DEVICE_TYPE_QA = 0x000000F1,
    } slaveCardType_t;

#define DEV_LNAWG DEVICE_TYPE_LNAWG & 0x01
#define DEV_QA DEVICE_TYPE_QA & 0x01

#define LED_OFF 0x07
#define LED_GREEN 0x06
#define LED_RED 0x05
#define LED_BLUE 0x03

#define C_RERIO_TRIGGER_OFFSET 0x8000
#define C_RERIO_TRIGGER_BASEADDR (PUBLIC_PERIPHERAL_BASEAADDR + C_RERIO_TRIGGER_OFFSET)
    typedef enum
    {
        E_FAN_SPEED_CTRL = 0 * 4, // max:1600
        E_LED_01_CTRL = 6 * 4,    // staus(OK:green)
        E_LED_02_CTRL = 10 * 4,   // ch status(on/off)
        E_LED_03_CTRL = 5 * 4,    // ch status(on/off)
        E_LED_04_CTRL = 4 * 4,    // ch status(on/off)
        E_LED_05_CTRL = 3 * 4,    // ch status(on/off)
        E_LED_06_CTRL = 7 * 4,    // error
        E_LED_07_CTRL = 8 * 4,    // lnawg ch staus(on/off)
        E_LED_08_CTRL = 9 * 4,    // lnawg ch staus(on/off)
        E_LED_09_CTRL = 2 * 4,    // lnawg ch staus(on/off)
        E_LED_10_CTRL = 1 * 4,    // lnawg ch staus(on/off)
    } publicPerReg_t;

    typedef enum
    {
        E_TRIGGER_CTRL_0 = 0 * 4, // 后IOtrigger控制_0
        E_TRIGGER_CTRL_1 = 5 * 4, // 后IOtrigger控制_1
    } rerioTriggerReg_t;

    /// @brief 从卡访问初始化
    /// @param  NONE
    void chip2chip_dev_init(void);
    void chip2chip_dev_deinit(void);

    /// @brief chip2chip寄存器读写
    int common_reg_data_set(int64_t baseAddr, uint32_t value);
    uint32_t common_reg_data_get(int64_t baseAddr);

    void public_dev_init(void);
    void public_dev_deinit(void);

    /// @brief 子卡初始化
    void slave_card_init(void);

    /// @brief PCIe user space寄存器读写
    uint32_t common_pcie_user_reg_data_get(int32_t chip, uint64_t baseAddr);
    int common_pcie_user_reg_data_set(int32_t chip, uint64_t baseAddr, int32_t value);

    // []: 频繁寄存器读写会导致总线浪费, 暂未实现
    /// @brief DMA寄存器读写
    int common_pcie_reg_data_get(int32_t chip, int64_t baseAddr);
    int common_pcie_reg_data_set(int32_t chip, int64_t baseAddr, int32_t value);

    /// @brief chip2chip内存访问
    int common_huge_data_get(int64_t addr,
                             int32_t length,
                             int32_t *buffer);
    int common_huge_data_set(int64_t addr,
                             int totalPackageNum,
                             int sequenceNum,
                             int packageIndex,
                             int *sequenceBuffer);

    void LED_control(uint8_t offset, uint32_t status);
    void status_led_ctrl(uint8_t ok);
    void temp_monitor(void);

    void device_ip_addr_set(const char *ip);
    void device_ip_addr_dhcp_set(int dhcpEn);
    void device_mac_addr_set(const char *mac);
    void set_qa_ch_led_status(int32_t logical_ch, int32_t status);
    void set_awg_ch_led_status(int32_t logical_ch, int32_t status);
    int common_reg_data_set(int64_t baseAddr, uint32_t value);
#ifdef __cplusplus
}
#endif

#endif // __COMMON_FUNCTION__
