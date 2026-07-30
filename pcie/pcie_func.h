#ifndef __PCIE_FUNC_H__
#define __PCIE_FUNC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DEV_TYPE_REG_ADDR 0x10000
#define XDMA_MAP_SIZE 0x40000

#define BOARD_NUM_MAX 4
#define AWG_CH_MAX 8
#define QA_IN_CH_MAX 16
#define QA_OUT_CH_MAX 16

    typedef enum
    {
        DEV_TYPE_NONE = 0,
        DEV_TYPE_AWG = 0x000000F0,
        DEV_TYPE_QA = 0x000000F1,
        DEV_TYPE_QA_4G = 0x00000042,
        DEV_TYPE_QA_8G = 0x00000081,
    } DeviceType;

    typedef struct
    {
        uint8_t chip_id;     /* xdma号: 0/1/2/3 */
        DeviceType dev_type; /* AWG / QA */
    } BoardItem;

    typedef struct
    {
        uint8_t chip_id;  /* 路由到哪个 xdma */
        uint8_t local_ch; /* 板内通道号 */
    } ChannelRoute;

    typedef struct
    {
        int board_num; /* 当前总共识别到几张卡 */

        int awg_board_num; /* AWG卡数量 */
        int qa_board_num;  /* QA卡数量 */

        int awg_ch_num;                 /* AWG总逻辑通道数 */
        int qa_in_ch_num;               /* QA总IN通道数 */
        int qa_out_ch_num;              /* QA总OUT通道数 */
        int qa_sub_type[BOARD_NUM_MAX]; /* QA子类型 */
        BoardItem items[BOARD_NUM_MAX];

        ChannelRoute awg_map[AWG_CH_MAX + 1];       /* AWG逻辑通道从1开始 */
        ChannelRoute qa_in_map[QA_IN_CH_MAX + 1];   /* QA IN逻辑通道从1开始 */
        ChannelRoute qa_out_map[QA_OUT_CH_MAX + 1]; /* QA OUT逻辑通道从1开始 */
    } PcieBoardInfo;
#define PCIE_DEVICE_NAME_MAX_LENGTH 32
#define DMA_ONCE_SIZE_MAX (1 * 1024 * 1024)
    typedef struct
    {

        uint8_t m_devUserSpaceName[PCIE_DEVICE_NAME_MAX_LENGTH];
        uint8_t m_devH2CSpaceName[PCIE_DEVICE_NAME_MAX_LENGTH];
        uint8_t m_devC2HSpaceName[PCIE_DEVICE_NAME_MAX_LENGTH];
        uint8_t *m_pbuffer;        // DMA buffer
        uint64_t m_buffer_size;    // DMA buffer大小
        uint8_t *m_user_mmap_addr; // 用户空间映射
        uint64_t m_map_size;       // 映射大小
        int m_h2c_fd;              // 主->从设备描述符
        int m_c2h_fd;              // 从->主设备描述符
    } xdmaDevContext_t;

    /// @brief Bar0空间映射
    int xdma_mmap(uint8_t chip, uint64_t map_size);
    void xdma_unmap(uint8_t chip);

    int pcie_dev_init(void);
    void pcie_dev_deinit(void);
    PcieBoardInfo *get_pcie_board_info();
    int get_subcard_count(void);
    int xdma_read_user_space(int chip, uint64_t offset, uint32_t *readVal);
    int xdma_write_user_space(int chip, uint64_t offset, uint32_t writeVal);
    int dma_write_data(int chip, uint64_t address, uint64_t bytes, uint8_t *buffer);
    int dma_read_data(int chip, uint64_t address, uint64_t bytes, uint8_t *buffer);
    int get_awg_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch);
    int get_qa_in_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch);
    int get_qa_out_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch);
    int get_qa_in_channel_count(void);
    int get_qa_out_channel_count(void);
    void dac_sync_init(void);
    void sync_init(void);
    void dac_sync_init(void);
#ifdef __cplusplus
}
#endif

#endif
