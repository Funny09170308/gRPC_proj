#include "max7300.h"
#include "../i2c_func.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    uint8_t red_port;
    uint8_t green_port;
    uint8_t blue_port;
} max7300LEDPort_t;

static const max7300LEDPort_t s_ledPortMap[] = {
    {7, 29, 28},  /* LED1: CH01 */
    {24, 6, 25},  /* LED2: CH02 */
    {22, 15, 23}, /* LED3: CH03 */
    {14, 10, 11}, /* LED4: CH04 */
    {5, 31, 30},  /* LED5: CH05 */
    {21, 4, 12},  /* LED6: CH06 */
    {19, 18, 20}, /* LED7: CH07 */
    {17, 16, 13}, /* LED8: CH08 */
    {26, 27, 0},  /* LEDA: CH0A */
    {9, 8, 0},    /* LEDB: CH0B */
};

static int max7300_write_reg(const char *i2cDevPath, uint8_t slaveAddr, uint8_t cmd, const uint8_t *tx_buf, uint8_t length)
{
    int fd;
    int ret;
    uint8_t write_buf[32];

    if (i2cDevPath == NULL || tx_buf == NULL || length > 31)
    {
        return -1;
    }

    fd = open(i2cDevPath, O_RDWR);
    if (fd < 0)
    {
        perror("open i2c device failed");
        return -1;
    }

    write_buf[0] = cmd;
    memcpy(&write_buf[1], tx_buf, length);

    ret = write_reg_var(fd, slaveAddr, write_buf, length + 1, C_DONT_CARE, 0);
    close(fd);
    return ret;
}

static int max7300_read_reg(const char *i2cDevPath, uint8_t slaveAddr, uint8_t cmd, uint8_t *rx_buf, uint8_t length)
{
    int fd;
    int ret;

    if (i2cDevPath == NULL || rx_buf == NULL || length == 0)
    {
        return -1;
    }

    fd = open(i2cDevPath, O_RDWR);
    if (fd < 0)
    {
        perror("open i2c device failed");
        return -1;
    }

    ret = read_reg_var(fd, slaveAddr, &cmd, 1, rx_buf, length);
    close(fd);
    return ret;
}

int max7300_init_all_output_high(const char *i2cDevPath, uint8_t slaveAddr)
{
    int ret;
    uint8_t config;

    ret = max7300_set_all_io_level(i2cDevPath, slaveAddr, 1);
    if (ret != 0)
    {
        return ret;
    }

    ret = max7300_set_all_io_direction(i2cDevPath, slaveAddr, MAX7300_IO_OUTPUT);
    if (ret != 0)
    {
        return ret;
    }

    config = C_MAX7300_NORMAL_OPERATION;
    ret = max7300_write_reg(i2cDevPath, slaveAddr, C_CONFIGURATION_ADDR, &config, 1);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

int max7300_set_all_io_level(const char *i2cDevPath, uint8_t slaveAddr, uint8_t level)
{
    uint8_t out_buf[28];

    if (level > 1)
    {
        return -1;
    }

    memset(out_buf, level, sizeof(out_buf));
    return max7300_write_reg(i2cDevPath, slaveAddr, C_SIGNAL_PORT_CTRL_START_ADDR, out_buf, sizeof(out_buf));
}

int max7300_set_all_io_direction(const char *i2cDevPath, uint8_t slaveAddr, uint8_t direction)
{
    uint8_t cfg_buf[7];
    uint8_t cfg;

    if (direction == MAX7300_IO_INPUT)
    {
        cfg = C_MAX7300_PORT_INPUT_CFG;
    }
    else if (direction == MAX7300_IO_OUTPUT)
    {
        cfg = C_MAX7300_PORT_OUTPUT_CFG;
    }
    else
    {
        return -1;
    }

    memset(cfg_buf, cfg, sizeof(cfg_buf));
    return max7300_write_reg(i2cDevPath, slaveAddr, C_PORT_CONFG_START_ADDR, cfg_buf, sizeof(cfg_buf));
}

int max7300_set_single_io(const char *i2cDevPath, uint8_t slaveAddr, uint8_t port, uint8_t level)
{
    int ret;
    uint8_t reg_addr;
    uint8_t cur_val;

    if (port < 4 || port > 31)
    {
        return -1;
    }
    if (level > 1)
    {
        return -2;
    }

    reg_addr = C_SIGNAL_PORT_CTRL_START_ADDR + port;

    ret = max7300_read_reg(i2cDevPath, slaveAddr, reg_addr, &cur_val, 1);
    if (ret != 0)
    {
        return ret;
    }

    cur_val &= 0xFE;
    cur_val |= (level & 0x01);

    return max7300_write_reg(i2cDevPath, slaveAddr, reg_addr, &cur_val, 1);
}

int max7300_set_led_color(const char *i2cDevPath, uint8_t slaveAddr, uint8_t led, uint8_t color)
{
    int ret;
    const max7300LEDPort_t *ports;

    if (led < 1 || led > (sizeof(s_ledPortMap) / sizeof(s_ledPortMap[0])))
    {
        return -1;
    }
    if ((color & ~0x07) != 0)
    {
        return -2;
    }

    ports = &s_ledPortMap[led - 1];

    ret = max7300_set_single_io(i2cDevPath, slaveAddr, ports->red_port, (color & 0x02) ? 1 : 0);
    if (ret != 0)
    {
        return ret;
    }

    ret = max7300_set_single_io(i2cDevPath, slaveAddr, ports->green_port, (color & 0x01) ? 1 : 0);
    if (ret != 0)
    {
        return ret;
    }

    return max7300_set_single_io(i2cDevPath, slaveAddr, ports->blue_port, (color & 0x04) ? 1 : 0);
}
