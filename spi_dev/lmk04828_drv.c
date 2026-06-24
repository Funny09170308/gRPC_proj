#include "spi_func.h"
#include "lmk04828_drv.h"
#include "../platform_log/platform_log.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

// LMK04828寄存器配置数组
struct tireg lmk04828_reset_bank[] = {
    {0x000090},
    {0x000010},
    {0x014A33}};

struct tireg lmk04828reg[] = {
    {0x000200},
    {0x000306},
    {0x0004D0},
    {0x00055B},
    {0x000600},
    {0x000C51},
    {0x000D04},
    {0x010001},
    {0x010155},
    {0x010255},
    {0x010302},
    {0x010400},
    {0x010500},
    {0x0106F0},
    {0x010755},
    {0x010801},
    {0x010955},
    {0x010A55},
    {0x010B02},
    {0x010C00},
    {0x010D00},
    {0x010E70},
    {0x010F55},
    {0x011001},
    {0x011155},
    {0x011255},
    {0x011302},
    {0x011400},
    {0x011500},
    {0x0116F0},
    {0x011755},
    {0x011801},
    {0x011955},
    {0x011A55},
    {0x011B02},
    {0x011C00},
    {0x011D00},
    {0x011EF0},
    {0x011F55},
    {0x012001},
    {0x012155},
    {0x012255},
    {0x012302},
    {0x012400},
    {0x012500},
    {0x0126F0},
    {0x012755},
    {0x012801},
    {0x012955},
    {0x012A55},
    {0x012B02},
    {0x012C00},
    {0x012D00},
    {0x012EF0},
    {0x012F55},
    {0x01300A},
    {0x013155},
    {0x013255},
    {0x013300},
    {0x013400},
    {0x013500},
    {0x0136F0},
    {0x013755},
    {0x013840},
    {0x013903},
    {0x013A00},
    {0x013B01},
    {0x013C00},
    {0x013D01},
    {0x013E03},
    {0x013F00},
    {0x0140F7},
    {0x014100},
    {0x014200},
    {0x01431F},
    {0x014480},
    {0x01457F},
    {0x014610},
    {0x014713},
    {0x014802},
    {0x014902},
    {0x014A33},
    {0x014B36},
    {0x014C00},
    {0x014D00},
    {0x014EC0},
    {0x014F7F},
    {0x015003},
    {0x015102},
    {0x015200},
    {0x015300},
    {0x015478},
    {0x015500},
    {0x015632},
    {0x015700},
    {0x015814},
    {0x015900},
    {0x015A14},
    {0x015BD4},
    {0x015C20},
    {0x015D00},
    {0x015E00},
    {0x015F0B},
    {0x016000},
    {0x016101},
    {0x016244},
    {0x016300},
    {0x016400},
    {0x01650C},
    {0x0171AA},
    {0x017202},
    {0x017360},
    {0x017400},
    {0x017C15},
    {0x017D33},
    {0x016600},
    {0x016700},
    {0x01680C},
    {0x016959},
    {0x016A20},
    {0x016B00},
    {0x016C00},
    {0x016D00},
    {0x016E13},
    {0x1FFD00},
    {0x1FFE00},
    {0x1FFF53},
};

struct tireg lmk04828_sync[] = {
    {0x01439f},
    {0x01431f},
    {0x01433f},
    {0x01431f},
    {0x0144ff}};

// LMK04828初始化函数
int lmk04828_reario_init()
{
    uint8_t txbuf[3] = {0};
    uint8_t rxbuf[3] = {0}; // 接收缓冲区
    int i, regnums;
    int spi_fd;

    // 打开SPI设备
    spi_fd = spi_open_device(SPI1_DEV_PATH);
    if (spi_fd < 0)
    {
        fprintf(stderr, "SPI device open failed\n");
        return -1;
    }
    spi_set_mode(spi_fd, 0);
    spi_set_bits_per_word(spi_fd, SPI_BITS_PER_WORD);
    spi_set_frequence(spi_fd, SPI_SPEED_HZ);

    // 写入复位寄存器组
    regnums = sizeof(lmk04828_reset_bank) / sizeof(struct tireg);
    P_LOG_DEBUG("lmk04828 reset reg num: %d\n", regnums);
    for (i = 0; i < regnums; i++)
    {
        // 拆分24位addr_data为3个8位字节
        txbuf[0] = (lmk04828_reset_bank[i].addr_data >> 16) & 0xFF;
        txbuf[1] = (lmk04828_reset_bank[i].addr_data >> 8) & 0xFF;
        txbuf[2] = lmk04828_reset_bank[i].addr_data & 0xFF;

        // 传输数据
        if (spi_transfer_data(spi_fd, txbuf, rxbuf, sizeof(txbuf)) < 0)
        {
            close(spi_fd);
            return -1;
        }
        usleep(100);
    }

    // 写入主配置寄存器组
    regnums = sizeof(lmk04828reg) / sizeof(struct tireg);
    P_LOG_DEBUG("lmk04828 reg num: %d\n", regnums);
    for (i = 0; i < regnums; i++)
    {
        txbuf[0] = (lmk04828reg[i].addr_data >> 16) & 0xFF;
        txbuf[1] = (lmk04828reg[i].addr_data >> 8) & 0xFF;
        txbuf[2] = lmk04828reg[i].addr_data & 0xFF;

        if (spi_transfer_data(spi_fd, txbuf, rxbuf, sizeof(txbuf)) < 0)
        {
            close(spi_fd);
            return -1;
        }
        usleep(100);
    }

    // 写入同步寄存器组
    regnums = sizeof(lmk04828_sync) / sizeof(struct tireg);
    P_LOG_DEBUG("lmk04828 sync reg num: %d\n", regnums);
    for (i = 0; i < regnums; i++)
    {
        txbuf[0] = (lmk04828_sync[i].addr_data >> 16) & 0xFF;
        txbuf[1] = (lmk04828_sync[i].addr_data >> 8) & 0xFF;
        txbuf[2] = lmk04828_sync[i].addr_data & 0xFF;

        if (spi_transfer_data(spi_fd, txbuf, rxbuf, sizeof(txbuf)) < 0)
        {
            close(spi_fd);
            return -1;
        }
        usleep(100);
    }

    // 关闭SPI设备，输出完成信息
    close(spi_fd);
    P_LOG_INFO("lmk04828reg init end");

    return 0;
}
