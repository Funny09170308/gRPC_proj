#include "i2c_func.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

int i2c_write_buffer(int fd, uint8_t slave,
                     const uint8_t *data, size_t dataLen)
{
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data ioctl_data;

    if (fd < 0 || data == NULL || dataLen == 0 || dataLen > USHRT_MAX)
    {
        errno = EINVAL;
        return -1;
    }

    msg.addr = slave;
    msg.flags = 0;
    msg.len = (uint16_t)dataLen;
    msg.buf = (uint8_t *)data;
    ioctl_data.msgs = &msg;
    ioctl_data.nmsgs = 1;

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        perror("i2c_write_buffer ioctl failed");
        return -1;
    }
    return 0;
}

int i2c_write_read_buffer(int fd, uint8_t slave,
                          const uint8_t *writeData, size_t writeLen,
                          uint8_t *readData, size_t readLen)
{
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data ioctl_data;

    if (fd < 0 || writeData == NULL || writeLen == 0 ||
        readData == NULL || readLen == 0 ||
        writeLen > USHRT_MAX || readLen > USHRT_MAX)
    {
        errno = EINVAL;
        return -1;
    }

    msgs[0].addr = slave;
    msgs[0].flags = 0;
    msgs[0].len = (uint16_t)writeLen;
    msgs[0].buf = (uint8_t *)writeData;
    msgs[1].addr = slave;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = (uint16_t)readLen;
    msgs[1].buf = readData;
    ioctl_data.msgs = msgs;
    ioctl_data.nmsgs = 2;

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        perror("i2c_write_read_buffer ioctl failed");
        return -1;
    }
    return 0;
}

/**
 * @brief 写可变长度寄存器
 * @param fd         i2c 文件描述符
 * @param slave      7-bit 从机地址
 * @param reg        指向寄存器地址的缓冲区
 * @param reg_len    寄存器地址长度（字节数，1 或 2 或更多）
 * @param data       指向数据缓冲区
 * @param data_len   写入数据长度（字节数）
 */
int write_reg_var(int fd, uint8_t slave,
                  uint32_t addr, size_t addrLen,
                  uint32_t data, size_t dataLen)
{
    if (addrLen > 4 || dataLen > 4)
    {
        fprintf(stderr, "Error: addrLen or dataLen too large\n");
        return -1;
    }

    uint8_t buf[8]; // addrLen + dataLen ≤ 8
    // 地址高字节在前
    for (size_t i = 0; i < addrLen; i++)
    {
        buf[i] = (addr >> (8 * (addrLen - 1 - i))) & 0xFF;
    }
    // 数据高字节在前
    for (size_t i = 0; i < dataLen; i++)
    {
        buf[addrLen + i] = (data >> (8 * (dataLen - 1 - i))) & 0xFF;
    }

    struct i2c_msg msg;
    msg.addr = slave;
    msg.flags = 0; // 写
    msg.len = addrLen + dataLen;
    msg.buf = buf;

    struct i2c_rdwr_ioctl_data ioctl_data = {
        .msgs = &msg,
        .nmsgs = 1};

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        perror("write_reg_var ioctl failed");
        return -1;
    }
    return 0;
}

/**
 * @brief 读可变长度寄存器
 * @brief fd         i2c 文件描述符
 * @brief slave      7-bit 从机地址
 * @brief reg        指向寄存器地址缓冲区
 * @brief reg_len    寄存器地址长度（字节数）
 * @brief data       输出缓冲区
 * @brief data_len   读取数据长度
 */
int read_reg_var(int fd, uint8_t slave,
                 uint32_t addr, size_t addrLen,
                 uint8_t *data, size_t dataLen)
{
    if (addrLen > 4 || dataLen > 4)
    {
        fprintf(stderr, "Error: addrLen or dataLen too large\n");
        return -1;
    }

    uint8_t addr_buf[4];
    for (size_t i = 0; i < addrLen; i++)
    {
        addr_buf[i] = (addr >> (8 * (addrLen - 1 - i))) & 0xFF;
    }

    struct i2c_msg msgs[2];
    // 写寄存器地址
    msgs[0].addr = slave;
    msgs[0].flags = 0;
    msgs[0].len = addrLen;
    msgs[0].buf = addr_buf;

    // 读寄存器数据
    msgs[1].addr = slave;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = dataLen;
    msgs[1].buf = data;

    struct i2c_rdwr_ioctl_data ioctl_data = {
        .msgs = msgs,
        .nmsgs = 2};

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        perror("read_reg_var ioctl failed");
        return -1;
    }
    return 0;
}
