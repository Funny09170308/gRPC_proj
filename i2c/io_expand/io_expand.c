#include <fcntl.h>
#include "io_expand.h"
#include "../i2c_func.h"
#include "../i2c_reg_config.h"
#include "../../platform_log/platform_log.h"

int pcie_slave_reset(void)
{
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        P_LOG_ERROR("CDCE6124_I2C_DEV device open failed!\n");
    }
    int ret;
    uint8_t current_out = 0x00;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_CFG_REG, 1, 0x00, 1);
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(50000);
    current_out |= PIN_SW_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(10000);
    current_out |= PIN_SSD_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(10000);
    current_out |= PIN_S01_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(10000);
    current_out |= PIN_S02_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(10000);
    current_out |= PIN_S03_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    usleep(10000);
    current_out |= PIN_S04_RESET;
    write_reg_var(fd, DEV3_ADDR_B, TCA6408_OUT_REG, 1, current_out, 1);
    return 0;
}
