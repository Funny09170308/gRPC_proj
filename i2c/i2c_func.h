#ifndef __I2C_FUNC_H__
#define __I2C_FUNC_H__
#include <stdio.h>
#include <stdint.h>

#define C_MAX_SLAVE_NUM 12
#define C_DONT_CARE 0
#define C_PS_I2C0_PATH "/dev/i2c-0"
#define C_PS_I2C1_PATH "/dev/i2c-1"
#define C_AXI_I2C_0_PATH "/dev/i2c-2"
typedef enum {
    E_MAX7300_LED_CTRL = 0x40,
} eAXII2CSlaveEnum_t;

int write_reg_var(int fd, uint8_t slave,
                  uint32_t addr, size_t addrLen,
                  uint32_t data, size_t dataLen);

int read_reg_var(int fd, uint8_t slave,
                 uint32_t addr, size_t addrLen,
                 uint8_t *data, size_t dataLen);

#endif // __I2C_FUNC_H__
