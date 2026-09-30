#include "max7300.h"
#include "../i2c_func.h"
#include "../../lib/include/platform_log/platform_log.h"
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
    {23 + 31, 21 + 31, 18 + 31}, // 1
    {19 + 31, 17 + 31, 20 + 31}, // 2
    {27 + 31, 21, 26 + 31},      // 3
    {23, 24, 20},                // 4
    {26, 27, 25},                // 5
    {28, 6, 22},                 // 6
    {18, 17, 19},                // 7
    {16, 14, 15},                // 8
    {14 + 31, 11 + 31, 16 + 31}, // 9
    {13 + 31, 9 + 31, 10 + 31},  // 10
    {8 + 27, 13, 12 + 27},       // 11
    {7, 29, 11},                 // 12
    {5, 31, 30},                 // 13
    {10, 6 + 31, 4},             // 14
    {12, 29 + 31, 9},            // 15
    {4 + 31, 5 + 31, 8},         // 16
    {22 + 31, 25 + 31, 24 + 31}, // status
    {7 + 31, 15 + 31, 28 + 31},  // err
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

    ret = i2c_write_buffer(fd, slaveAddr, write_buf, (size_t)length + 1U);
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

    ret = i2c_write_read_buffer(fd, slaveAddr, &cmd, 1, rx_buf, length);
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

    /* Register 0x24 controls P4, 0x25 controls P5, ... 0x3f controls P31. */
    reg_addr = C_SIGNAL_PORT_CTRL_START_ADDR + (port - 4U);

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

    if (led >= (sizeof(s_ledPortMap) / sizeof(s_ledPortMap[0])))
    {
        return -1;
    }
    if ((color & ~0x07) != 0)
    {
        return -2;
    }

    ports = &s_ledPortMap[led];
    uint8_t port = 0;
    uint8_t slave = MAX7300_SLAVE_ADDR;
    if (ports->red_port > C_CHIP_1_PORT_NUM)
    {
        port = ports->red_port - C_CHIP_1_PORT_NUM;
        slave = MAX7300_SLAVE_ADDR_1;
    }
    else
    {
        port = ports->red_port;
        slave = MAX7300_SLAVE_ADDR;
    }
    P_LOG_DEBUG("ports: R = %d, G = %d, B = %d, slave addr: %x\r\n",
                ports->red_port, ports->green_port, ports->blue_port, slave);
    P_LOG_DEBUG("calc red port: %d, slave addr: %x\r\n", port, slave);
    ret = max7300_set_single_io(i2cDevPath, slave, port, (color & 0x02) ? 1 : 0);
    if (ret != 0)
    {
        return ret;
    }

    if (ports->green_port > C_CHIP_1_PORT_NUM)
    {
        port = ports->green_port - C_CHIP_1_PORT_NUM;
        slave = MAX7300_SLAVE_ADDR_1;
    }
    else
    {
        port = ports->green_port;
        slave = MAX7300_SLAVE_ADDR;
    }
    P_LOG_DEBUG("calc green port: %d, slave addr: %x\r\n", port, slave);
    ret = max7300_set_single_io(i2cDevPath, slave, port, (color & 0x01) ? 1 : 0);
    if (ret != 0)
    {
        return ret;
    }

    if (ports->blue_port > C_CHIP_1_PORT_NUM)
    {
        port = ports->blue_port - C_CHIP_1_PORT_NUM;
        slave = MAX7300_SLAVE_ADDR_1;
    }
    else
    {
        port = ports->blue_port;
        slave = MAX7300_SLAVE_ADDR;
    }
    P_LOG_DEBUG("calc blue port: %d, slave addr: %x\r\n", port, slave);
    return max7300_set_single_io(i2cDevPath, slave, port, (color & 0x04) ? 1 : 0);
}
