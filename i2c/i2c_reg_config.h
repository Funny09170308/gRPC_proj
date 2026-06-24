#ifndef __I2C_REG_CONFIG_H__
#define __I2C_REG_CONFIG_H__

#include <stdint.h>

#define CDCE6124_I2C_DEV "/dev/i2c-1"

#define CDCE6214_USE_LAST_CONFIG 0
#define CDCE6214_USE_LAST_CONFIG 1
#if CDCE6214_DEFAULT_CONFIG
#define CDCE6124_I2C_SLAVE_ADDR 0x67
#elif CDCE6214_USE_LAST_CONFIG
#define CDCE6124_I2C_SLAVE_ADDR 0x68
#endif

#define TCA_6408_6214_EXPAND 0x20
#define TCA_6408_PCI_RESET_EXPAND 0x21

#define LMKDB1108_I2C_DEV "/dev/i2c-0"
#define LMKDB1108_I2C_SLAVE_ADDR 0xC4

#ifdef __cplusplus
extern "C"
{
#endif

void cdce6214_reg_config(void);
void cdce6214_internel_reg_config(void);

void lmkdb1108_reg_config(void);

void power_suplly_init(void);

void pcie_reset(void);

#ifdef __cplusplus
}
#endif
#endif // __I2C_REG_CONFIG_H__
