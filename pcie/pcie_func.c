#include "pcie_func.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include "../device_info.h"
#include "../platform_log/platform_log.h"

static xdmaDevContext_t s_xdmaDevContx[CHIP_NUM] = {
    {.m_devUserSpaceName = "/dev/xdma0_user",
     .m_devH2CSpaceName = "/dev/xdma0_h2c_0",
     .m_devC2HSpaceName = "/dev/xdma0_c2h_0"},
    {.m_devUserSpaceName = "/dev/xdma1_user",
     .m_devH2CSpaceName = "/dev/xdma1_h2c_0",
     .m_devC2HSpaceName = "/dev/xdma1_c2h_0"},
    {.m_devUserSpaceName = "/dev/xdma2_user",
     .m_devH2CSpaceName = "/dev/xdma2_h2c_0",
     .m_devC2HSpaceName = "/dev/xdma2_c2h_0"},
    {.m_devUserSpaceName = "/dev/xdma3_user",
     .m_devH2CSpaceName = "/dev/xdma3_h2c_0",
     .m_devC2HSpaceName = "/dev/xdma3_c2h_0"},
};

PcieBoardInfo g_pcie_board_info;

PcieBoardInfo *get_pcie_board_info()
{
    return &g_pcie_board_info;
}

int xdma_mmap(uint8_t chip, uint64_t map_size)
{
#define DMA_BUFFER_SIZE 1024 * 1024 * 10
    if (chip >= CHIP_NUM)
    {
        P_LOG_ERROR("Chip num set error!...%d", chip);
        return -1;
    }
    int pgsz = sysconf(_SC_PAGESIZE);
    uint64_t aligned_map_size = ((map_size + pgsz - 1) / pgsz) * pgsz;
    s_xdmaDevContx[chip].m_map_size = aligned_map_size;
    posix_memalign((void **)&s_xdmaDevContx[chip].m_pbuffer, 4096, DMA_BUFFER_SIZE + 4096);
    s_xdmaDevContx[chip].m_buffer_size = DMA_BUFFER_SIZE;
    int fd = open(s_xdmaDevContx[chip].m_devUserSpaceName, O_RDWR | O_SYNC);
    if (fd < 0)
    {
        P_LOG_ERROR("Failed to mount XDMA %s user space!", s_xdmaDevContx[chip].m_devUserSpaceName);
        return -2;
    }
    s_xdmaDevContx[chip].m_user_mmap_addr = (uint8_t *)mmap(NULL,
                                                            s_xdmaDevContx[chip].m_map_size,
                                                            PROT_READ | PROT_WRITE,
                                                            MAP_SHARED,
                                                            fd,
                                                            0);
    if (s_xdmaDevContx[chip].m_user_mmap_addr == MAP_FAILED)
    {
        P_LOG_ERROR("XDMA user space %s initialize failed.", s_xdmaDevContx[chip].m_devUserSpaceName);
        return -3;
    }
    P_LOG_INFO("XDMA user space %s initialize at vertual addr %#llx suucceed(size: %#llx).",
               s_xdmaDevContx[chip].m_devUserSpaceName,
               s_xdmaDevContx[chip].m_user_mmap_addr,
               s_xdmaDevContx[chip].m_map_size);

    s_xdmaDevContx[chip].m_c2h_fd = open(s_xdmaDevContx[chip].m_devC2HSpaceName, O_RDWR);
    s_xdmaDevContx[chip].m_h2c_fd = open(s_xdmaDevContx[chip].m_devH2CSpaceName, O_RDWR);
    if (s_xdmaDevContx[chip].m_h2c_fd < 0 || s_xdmaDevContx[chip].m_c2h_fd < 0)
    {
        P_LOG_ERROR("Failed to mount XDMA %s DMA space!", s_xdmaDevContx[chip].m_devUserSpaceName);
        return -2;
    }
    close(fd);
    return 0;
}

void xdma_unmap(uint8_t chip)
{
    if (s_xdmaDevContx[chip].m_user_mmap_addr != NULL)
    {
        munmap(s_xdmaDevContx[chip].m_user_mmap_addr, s_xdmaDevContx[chip].m_map_size);
    }
    if (s_xdmaDevContx[chip].m_c2h_fd >= 0)
    {
        close(s_xdmaDevContx[chip].m_c2h_fd);
    }
    if (s_xdmaDevContx[chip].m_c2h_fd >= 0)
    {
        close(s_xdmaDevContx[chip].m_c2h_fd);
    }
    if (s_xdmaDevContx[chip].m_pbuffer != NULL)
    {
        free(s_xdmaDevContx[chip].m_pbuffer);
    }
}

int get_subcard_count(void)
{
    int count = 0;
    int i;
    char dev_path[64];

    for (i = 0; i < BOARD_NUM_MAX; i++)
    {
        snprintf(dev_path, sizeof(dev_path), "/dev/xdma%d_user", i);
        if (access(dev_path, F_OK) == 0)
        {
            count++;
        }
    }

    return count;
}

/* --------------------------------------------------------------------------
 * AWG æ˜ å°„è§„åˆ™è¯´æ˜Ž
 *
 * ä½ çŽ°åœ¨ç»™å‡ºçš„å›ºå®šé€»è¾‘å¯ä»¥ç†è§£ä¸ºï¼š
 * - xdma æ˜¯æŒ‰å½“å‰å­˜åœ¨è®¾å¤‡ä»Žå°åˆ°å¤§åˆ†é…
 * - AWG åœ¨ç”¨æˆ·å±‚æœ€ç»ˆçœ‹åˆ°çš„æ˜¯é€»è¾‘é€šé“
 *
 * è¿™é‡Œç”¨ä¸€ä¸ªâ€œæŒ‰å½“å‰ AWG å¡æ•°é‡â€æ¥ç”Ÿæˆé€»è¾‘é€šé“åŸºå€çš„è§„åˆ™è¡¨ã€‚
 *
 * å½“å‰é‡‡ç”¨çš„è§„åˆ™ï¼š
 * 1å¼ AWG: (CH1,CH2)
 * 2å¼ AWG: (CH1,CH2)(CH3,CH4)
 * 3å¼ AWG: (CH5,CH6)(CH1,CH2)(CH3,CH4)
 * 4å¼ AWG: (CH5,CH6)(CH1,CH2)(CH3,CH4)(CH7,CH8)
 *
 * å¦‚æžœä½ åŽé¢ç¡®è®¤ 3 å¼ å¡åœºæ™¯ä¸æ˜¯è¿™ä¸ªé¡ºåºï¼Œåªéœ€è¦æ”¹ä¸‹é¢è¿™ä¸ªè¡¨ã€‚
 * -------------------------------------------------------------------------- */
static int get_awg_base_ch_by_index(int awg_count, int index)
{
    static const int awg_base_1[1] = {1};
    static const int awg_base_2[2] = {1, 3};
    static const int awg_base_3[3] = {5, 1, 3};
    static const int awg_base_4[4] = {5, 1, 3, 7};

    if (index < 0)
    {
        return -1;
    }

    switch (awg_count)
    {
    case 1:
        return (index < 1) ? awg_base_1[index] : -1;
    case 2:
        return (index < 2) ? awg_base_2[index] : -1;
    case 3:
        return (index < 3) ? awg_base_3[index] : -1;
    case 4:
        return (index < 4) ? awg_base_4[index] : -1;
    default:
        return -1;
    }
}

/* --------------------------------------------------------------------------
 * QA æ˜ å°„è§„åˆ™è¯´æ˜Ž
 *
 * æ¯å¼  QA å¡æ˜¯ 4 é€šé“ã€‚
 * è¿™é‡Œæ²¿ç”¨å’Œ AWG åŒæ ·çš„æ§½ä½é¡ºåºæ€æƒ³ï¼Œåªæ˜¯æ¯å¼ å¡å  4 ä¸ªé€»è¾‘é€šé“ã€‚
 *
 * å½“å‰é‡‡ç”¨çš„è§„åˆ™ï¼š
 * 1å¼ QA: CH1~CH4
 * 2å¼ QA: CH1~CH4, CH5~CH8
 * 3å¼ QA: CH9~CH12, CH1~CH4, CH5~CH8
 * 4å¼ QA: CH9~CH12, CH1~CH4, CH5~CH8, CH13~CH16
 *
 * å¦‚æžœä½ åŽé¢ç¡®è®¤ QA çš„ 3/4 å¡é€»è¾‘é¡ºåºä¸åŒï¼Œä¹Ÿåªæ”¹è¿™ä¸ªè¡¨ã€‚
 * -------------------------------------------------------------------------- */
static int get_qa_base_ch_by_index(int qa_count, int index)
{
    static const int qa_base_1[1] = {1};
    static const int qa_base_2[2] = {1, 5};
    static const int qa_base_3[3] = {9, 1, 5};
    static const int qa_base_4[4] = {9, 1, 5, 13};

    if (index < 0)
    {
        return -1;
    }

    switch (qa_count)
    {
    case 1:
        return (index < 1) ? qa_base_1[index] : -1;
    case 2:
        return (index < 2) ? qa_base_2[index] : -1;
    case 3:
        return (index < 3) ? qa_base_3[index] : -1;
    case 4:
        return (index < 4) ? qa_base_4[index] : -1;
    default:
        return -1;
    }
}

static void build_awg_map(PcieBoardInfo *info)
{
    int i;
    int awg_index = 0;
    int awg_chip_list[BOARD_NUM_MAX];

    memset(info->awg_map, 0, sizeof(info->awg_map));
    info->awg_board_num = 0;
    info->awg_ch_num = 0;

    /* å…ˆæŒ‰ xdma ä»Žå°åˆ°å¤§æ”¶é›†æ‰€æœ‰ AWG è®¾å¤‡ */
    for (i = 0; i < info->board_num; i++)
    {
        if (info->items[i].dev_type == DEV_TYPE_AWG)
        {
            awg_chip_list[awg_index++] = info->items[i].chip_id;
        }
    }

    info->awg_board_num = awg_index;
    info->awg_ch_num = awg_index * 2;

    /* å†æŒ‰æ•°é‡è§„åˆ™ç”Ÿæˆé€»è¾‘é€šé“æ˜ å°„ */
    for (i = 0; i < awg_index; i++)
    {
        int base_ch = get_awg_base_ch_by_index(awg_index, i);
        int chip = awg_chip_list[i];

        if (base_ch < 0)
        {
            continue;
        }

        info->awg_map[base_ch].chip_id = chip;
        info->awg_map[base_ch].local_ch = 1;

        info->awg_map[base_ch + 1].chip_id = chip;
        info->awg_map[base_ch + 1].local_ch = 2;
    }
}

static void build_qa_map(PcieBoardInfo *info)
{
    int i;
    int qa_index = 0;
    int qa_chip_list[BOARD_NUM_MAX];

    memset(info->qa_in_map, 0, sizeof(info->qa_in_map));
    memset(info->qa_out_map, 0, sizeof(info->qa_out_map));

    info->qa_board_num = 0;
    info->qa_in_ch_num = 0;
    info->qa_out_ch_num = 0;

    /* ÏÈ°´ xdma ´ÓÐ¡µ½´óÊÕ¼¯ËùÓÐ QA Éè±¸ */
    for (i = 0; i < info->board_num; i++)
    {
        if (info->items[i].dev_type == DEV_TYPE_QA)
        {
            qa_chip_list[qa_index++] = info->items[i].chip_id;
        }
    }

    info->qa_board_num = qa_index;
    info->qa_in_ch_num = qa_index * 4;
    info->qa_out_ch_num = qa_index * 4;

    /* Ã¿ÕÅ QA ¿¨Ó³Éä 4 ¸ö IN + 4 ¸ö OUT */
    for (i = 0; i < qa_index; i++)
    {
        int chip = qa_chip_list[i];
        int base_in = i * 4 + 1;
        int base_out = i * 4 + 1;

        /* IN1~IN4 */
        info->qa_in_map[base_in + 0].chip_id = chip;
        info->qa_in_map[base_in + 0].local_ch = 1;

        info->qa_in_map[base_in + 1].chip_id = chip;
        info->qa_in_map[base_in + 1].local_ch = 2;

        info->qa_in_map[base_in + 2].chip_id = chip;
        info->qa_in_map[base_in + 2].local_ch = 3;

        info->qa_in_map[base_in + 3].chip_id = chip;
        info->qa_in_map[base_in + 3].local_ch = 4;

        /* OUT1~OUT4 */
        info->qa_out_map[base_out + 0].chip_id = chip;
        info->qa_out_map[base_out + 0].local_ch = 1;

        info->qa_out_map[base_out + 1].chip_id = chip;
        info->qa_out_map[base_out + 1].local_ch = 2;

        info->qa_out_map[base_out + 2].chip_id = chip;
        info->qa_out_map[base_out + 2].local_ch = 3;

        info->qa_out_map[base_out + 3].chip_id = chip;
        info->qa_out_map[base_out + 3].local_ch = 4;
    }
}

int pcie_dev_init(void)
{
    int ret;
    uint32_t dev_type;
    uint32_t i;

    memset(&g_pcie_board_info, 0, sizeof(g_pcie_board_info));

    ret = get_subcard_count();

    if (ret < 0)
    {
        return -1;
    }

    if (ret > BOARD_NUM_MAX)
    {
        ret = BOARD_NUM_MAX;
    }

    g_pcie_board_info.board_num = ret;

    for (i = 0; i < (uint32_t)g_pcie_board_info.board_num; i++)
    {
        ret = xdma_mmap(i, XDMA_MAP_SIZE);
        if (ret != 0)
        {
            return -1;
        }
    }

    for (i = 0; i < (uint32_t)g_pcie_board_info.board_num; i++)
    {
        ret = xdma_read_user_space(i, DEV_TYPE_REG_ADDR, &dev_type);

        if (ret != 0)
        {
            return -1;
        }

        g_pcie_board_info.items[i].chip_id = i;
#define MASK 0xFFFFFFF0
        if (dev_type == DEV_TYPE_AWG)
        {
            g_pcie_board_info.items[i].dev_type = DEV_TYPE_AWG;
        }
        else if ((dev_type & MASK) == (DEV_TYPE_QA_4G & MASK))
        {
            g_pcie_board_info.qa_sub_type[i] = 4;
            g_pcie_board_info.items[i].dev_type = DEV_TYPE_QA;
        }
        else if ((dev_type & MASK) == (DEV_TYPE_QA_8G & MASK))
        {
            g_pcie_board_info.qa_sub_type[i] = 8;
            g_pcie_board_info.items[i].dev_type = DEV_TYPE_QA;
        }
        else
        {
            g_pcie_board_info.items[i].dev_type = DEV_TYPE_NONE;
        }
    }

    build_awg_map(&g_pcie_board_info);
    build_qa_map(&g_pcie_board_info);

    P_LOG_DEBUG("board_num=%d, awg_board_num=%d, qa_board_num=%d, awg_ch_num=%d, qa_in_ch_num=%d, qa_out_ch_num=%d",
                g_pcie_board_info.board_num,
                g_pcie_board_info.awg_board_num,
                g_pcie_board_info.qa_board_num,
                g_pcie_board_info.awg_ch_num,
                g_pcie_board_info.qa_in_ch_num,
                g_pcie_board_info.qa_out_ch_num);

    for (i = 0; i < g_pcie_board_info.board_num; i++)
    {
        P_LOG_DEBUG("board[%d]: chip_id=%d, dev_type=0x%08X",
                    i,
                    g_pcie_board_info.items[i].chip_id,
                    g_pcie_board_info.items[i].dev_type);
    }

    for (i = 1; i <= (uint32_t)g_pcie_board_info.awg_ch_num; i++)
    {
        P_LOG_DEBUG("AWG CH%d -> chip_id=%d, local_ch=%d",
                    i,
                    g_pcie_board_info.awg_map[i].chip_id,
                    g_pcie_board_info.awg_map[i].local_ch);
    }

    for (i = 1; i <= (uint32_t)g_pcie_board_info.qa_in_ch_num; i++)
    {
        P_LOG_DEBUG("QA IN%d -> chip_id=%d, local_ch=%d",
                    i,
                    g_pcie_board_info.qa_in_map[i].chip_id,
                    g_pcie_board_info.qa_in_map[i].local_ch);
    }

    for (i = 1; i <= (uint32_t)g_pcie_board_info.qa_out_ch_num; i++)
    {
        P_LOG_DEBUG("QA OUT%d -> chip_id=%d, local_ch=%d",
                    i,
                    g_pcie_board_info.qa_out_map[i].chip_id,
                    g_pcie_board_info.qa_out_map[i].local_ch);
    }

    return 0;
}

int get_awg_channel_count(void)
{
    return g_pcie_board_info.awg_ch_num;
}

int get_qa_in_channel_count(void)
{
    return g_pcie_board_info.qa_in_ch_num;
}

int get_qa_out_channel_count(void)
{
    return g_pcie_board_info.qa_out_ch_num;
}

int get_awg_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch)
{
    if (logical_ch <= 0 || logical_ch > g_pcie_board_info.awg_ch_num)
    {
        return -1;
    }

    if (chip_id == NULL || local_ch == NULL)
    {
        return -1;
    }

    *chip_id = g_pcie_board_info.awg_map[logical_ch].chip_id;
    *local_ch = g_pcie_board_info.awg_map[logical_ch].local_ch;

    return 0;
}

int get_qa_in_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch)
{
    if (logical_ch <= 0 || logical_ch > g_pcie_board_info.qa_in_ch_num)
    {
        return -1;
    }

    if (chip_id == NULL || local_ch == NULL)
    {
        return -1;
    }

    *chip_id = g_pcie_board_info.qa_in_map[logical_ch].chip_id;
    *local_ch = g_pcie_board_info.qa_in_map[logical_ch].local_ch;

    return 0;
}

int get_qa_out_route(int logical_ch, uint8_t *chip_id, uint8_t *local_ch)
{
    if (logical_ch <= 0 || logical_ch > g_pcie_board_info.qa_out_ch_num)
    {
        return -1;
    }

    if (chip_id == NULL || local_ch == NULL)
    {
        return -1;
    }

    *chip_id = g_pcie_board_info.qa_out_map[logical_ch].chip_id;
    *local_ch = g_pcie_board_info.qa_out_map[logical_ch].local_ch;

    return 0;
}

void pcie_dev_deinit(void)
{
    int num = get_subcard_count();
    for (uint8_t index = 0; index < num; ++index)
    {
        xdma_unmap(index);
    }
}

int xdma_read_user_space(int chip, uint64_t offset, uint32_t *readVal)
{
    if (readVal == NULL)
    {
        return -1;
    }
    *readVal = *(uint32_t *)(s_xdmaDevContx[chip].m_user_mmap_addr + offset);
    P_LOG_REPEAT("PCIE read data: %d(Hex:%#x) from user space addr: %#llx.", *readVal, *readVal, offset);
    return 0;
}

int xdma_write_user_space(int chip, uint64_t offset, uint32_t writeVal)
{
    if (chip < 0 || chip >= CHIP_NUM)
        return -1;

    if (s_xdmaDevContx[chip].m_user_mmap_addr == NULL)
        return -1;

    if (offset + sizeof(uint32_t) > s_xdmaDevContx[chip].m_map_size)
        return -1;

    if (offset % 4 != 0)
        return -1;

    *(volatile uint32_t *)(s_xdmaDevContx[chip].m_user_mmap_addr + offset) = writeVal;

    P_LOG_REPEAT("PCIE write data: %u(Hex:%#x) to user space addr: %#llx.",
                 writeVal, writeVal, offset);

    return 0;
}

int dma_write_data(int chip, uint64_t address, uint64_t bytes, uint8_t *buffer)
{
    memcpy(s_xdmaDevContx[chip].m_pbuffer, buffer, bytes); // 拷贝用户数据到 DMA 缓冲区
    P_LOG_DEBUG("s_xdmaDevContx[chip].m_h2c_fd = %d.", s_xdmaDevContx[chip].m_h2c_fd);
    size_t rc = write_from_buffer(s_xdmaDevContx[chip].m_devH2CSpaceName,
                                  s_xdmaDevContx[chip].m_h2c_fd,
                                  s_xdmaDevContx[chip].m_pbuffer,
                                  bytes,
                                  address);
    if (rc < 0)
    {
        P_LOG_ERROR("PCIe DMA write data to device %s failed!", s_xdmaDevContx[chip].m_devH2CSpaceName);
    }
    P_LOG_DEBUG("PCIe DMA write data to device %s succeed!...%d", s_xdmaDevContx[chip].m_devH2CSpaceName, rc);
    return 0;
}

int dma_read_data(int chip, uint64_t address, uint64_t bytes, uint8_t *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    P_LOG_DEBUG("PCIe chip %d DMA access addr %#llx, read bytes: %d.", chip, address, bytes);
    uint64_t size = bytes;
    int index = 0, int_num, rem_num, count = 0;
    int_num = size / DMA_ONCE_SIZE_MAX;
    rem_num = size % DMA_ONCE_SIZE_MAX;
    for (int index = 0; index < int_num; index++)
    {
        memset(s_xdmaDevContx[chip].m_pbuffer, 0x00, DMA_ONCE_SIZE_MAX);
        count += read_to_buffer(s_xdmaDevContx[chip].m_devC2HSpaceName,
                                s_xdmaDevContx[chip].m_c2h_fd,
                                s_xdmaDevContx[chip].m_pbuffer,
                                DMA_ONCE_SIZE_MAX,
                                address + index * DMA_ONCE_SIZE_MAX);
        memcpy(buffer + index * DMA_ONCE_SIZE_MAX, s_xdmaDevContx[chip].m_pbuffer, DMA_ONCE_SIZE_MAX);
        P_LOG_DEBUG("DMA read index: %d, read count: %d", index, count);
    }
    memset(s_xdmaDevContx[chip].m_pbuffer, 0x00, rem_num);
    count += read_to_buffer(s_xdmaDevContx[chip].m_devC2HSpaceName,
                            s_xdmaDevContx[chip].m_c2h_fd,
                            s_xdmaDevContx[chip].m_pbuffer,
                            rem_num,
                            address + index * DMA_ONCE_SIZE_MAX);
    memcpy(buffer + index * DMA_ONCE_SIZE_MAX, s_xdmaDevContx[chip].m_pbuffer, rem_num);
    P_LOG_DEBUG("DMA total read count: %d", count);
    if (count < 0)
        P_LOG_ERROR("PCIe DMA read data from device %s failed!", s_xdmaDevContx[chip].m_devC2HSpaceName);
    P_LOG_DEBUG("PCIe DMA Read count: %d, expect: %d.", count, bytes);
    return count;
}

void chip_dac_sync_init(uint32_t chip)
{
#define DAC_SYNC_OFFSET 0x101 << 2
    xdma_write_user_space(chip, DAC_SYNC_OFFSET, 0);
    usleep(10);
    xdma_write_user_space(chip, DAC_SYNC_OFFSET, 1);
    usleep(10);
    xdma_write_user_space(chip, DAC_SYNC_OFFSET, 0);
}

void sync_init(void)
{
    for (uint8_t i = 0; i < g_pcie_board_info.awg_board_num; ++i)
    {
        xdma_write_user_space(i, 0x10000 + (103 << 2), 1);
        P_LOG_DEBUG("Init chip %d dac sync.", i);
    }
}

void dac_sync_init(void)
{
    for (uint8_t i = 0; i < g_pcie_board_info.awg_board_num; ++i)
    {
        chip_dac_sync_init(i);
        P_LOG_DEBUG("Init chip %d dac sync.", i);
    }
}
