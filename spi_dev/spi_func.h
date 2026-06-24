#ifndef SPI_FUNC_H
#define SPI_FUNC_H


#include <stdint.h>

// SPI设备路径
#define SPI1_DEV_PATH "/dev/spidev1.0"
#define SPI2_DEV_PATH "/dev/spidev2.0"

// SPI通信参数
#define SPI_SPEED_HZ 1000000  // 1MHz
#define SPI_BITS_PER_WORD 8

int spi_open_device(const char *dev_path);
int spi_set_mode(int fd, uint8_t mode);
int spi_set_bits_per_word(int fd, uint8_t bits);
int spi_set_frequence(int fd, uint32_t speed);
int spi_transfer_data(int fd, const uint8_t *tx_buf, uint8_t *rx_buf, int len);
int spi_read_only(int fd, uint8_t *rx_buf, int len);
int spi_read_with_cmd(int fd, uint8_t cmd, uint8_t reg, uint8_t *rx_buf, int len);

#endif // SPI_FUNC_H
