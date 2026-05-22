#ifndef __I2C_FUNC_H__
#define __I2C_FUNC_H__
#include <stdio.h>
#include <stdint.h>

int write_reg_var(int fd, uint8_t slave,
                  uint32_t addr, size_t addrLen,
                  uint32_t data, size_t dataLen);

int read_reg_var(int fd, uint8_t slave,
                 uint32_t addr, size_t addrLen,
                 uint8_t *data, size_t dataLen);

#endif // __I2C_FUNC_H__
