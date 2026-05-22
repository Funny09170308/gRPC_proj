#include "spi_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

// 打开SPI设备
int spi_open_device(const char *dev_path)
{
    int fd = open(dev_path, O_RDWR);
    if (fd < 0)
    {
        perror("SPI open error!");
        return -1;
    }
    // 设置时钟频率
    unsigned int speed = SPI_SPEED_HZ;
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0 ||
        ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("Failed to set SPI speed");
        close(fd);
        return -1;
    }

    return fd;
}

int spi_set_mode(int fd, uint8_t mode)
{
    if (fd < 0)
    {
        perror("SPI fd error!");
        return -1;
    }

    // 设置SPI模式
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0)
    {
        perror("Failed to set SPI mode");
        close(fd);
        return -2;
    }
    return 0;
}

int spi_set_bits_per_word(int fd, uint8_t bits)
{
    if (fd < 0)
    {
        perror("SPI fd error!");
        return -1;
    }

    // 设置字长
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        perror("Failed to set SPI bits per word");
        close(fd);
        return -2;
    }
    return 0;
}

int spi_set_frequence(int fd, uint32_t speed)
{
    if (fd < 0)
    {
        perror("SPI fd error!");
        return -1;
    }

    // 设置时钟频率
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0 ||
        ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("Failed to set SPI speed");
        close(fd);
        return -2;
    }
    return 0;
}

// SPI数据传输
int spi_transfer_data(int fd, const uint8_t *tx_buf, uint8_t *rx_buf, int len)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = len,
        .cs_change = 0, // 传输后不释放片选（保持选中）
        .delay_usecs = 0,
        .speed_hz = SPI_SPEED_HZ,
        .bits_per_word = SPI_BITS_PER_WORD,
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0)
    {
        perror("SPI transfer failed");
        return -1;
    }
    return 0;
}

int spi_read_only(int fd, uint8_t *rx_buf, int len)
{
    // 分配发送缓冲区
    uint8_t *tx_buf = (uint8_t *)malloc(len);
    if (tx_buf == NULL) {
        perror("Malloc tx_buf failed");
        return -1;
    }
    memset(tx_buf, 0xFF, len);

    // 调用核心传输函数
    int ret = spi_transfer_data(fd, tx_buf, rx_buf, len);

    // 释放临时缓冲区
    free(tx_buf);

    return ret;
}

int spi_read_with_cmd(int fd, uint8_t cmd, uint8_t reg, uint8_t *rx_buf, int len)
{
    // 构造发送缓冲区
    int tx_len = 2 + len;
    uint8_t *tx_buf = (uint8_t *)malloc(tx_len);
    if (tx_buf == NULL) {
        perror("Malloc tx_buf failed");
        return -1;
    }

    tx_buf[0] = cmd;          // 读指令
    tx_buf[1] = reg;          // 寄存器地址
    memset(&tx_buf[2], 0xFF, len);  // 后续填充空数据

    // 构造接收缓冲区
    uint8_t *temp_rx_buf = (uint8_t *)malloc(tx_len);
    if (temp_rx_buf == NULL) {
        free(tx_buf);
        perror("Malloc temp_rx_buf failed");
        return -1;
    }

    // 执行SPI传输
    int ret = spi_transfer_data(fd, tx_buf, temp_rx_buf, tx_len);
    if (ret == 0) {
        // 提取有效数据
        memcpy(rx_buf, &temp_rx_buf[2], len);
    }

    // 释放缓冲区
    free(tx_buf);
    free(temp_rx_buf);

    return ret;
}
