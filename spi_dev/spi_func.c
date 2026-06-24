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
int spi_transfer_raw(int fd, const uint8_t *tx_buf, uint8_t *rx_buf, int len)
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

int spi_write_bytes(int fd, const uint8_t *tx, int len)
{
    // RX 不重要 → 必须置NULL，避免AXI SPI RX wait
    return spi_transfer_raw(fd, tx, NULL, len);
}

int spi_read_bytes(int fd, uint8_t *rx, int len)
{
    uint8_t *tx = (uint8_t *)malloc(len);
    if (!tx)
        return -1;

    memset(tx, 0xFF, len);

    int ret = spi_transfer_raw(fd, tx, rx, len);

    free(tx);
    return ret;
}
