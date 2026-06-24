#include "i2c_func.h"
#include "i2c_reg_config.h"
#include "../axi_gpio/axi_gpio.h"
#include "../platform_log/platform_log.h"

#include <fcntl.h>

uint32_t cdce6214RegValue[] = {
    {0x00550000},
    {0x00540000},
    {0x00530000},
    {0x00520000},
    {0x00510004},
    {0x00500000},
    {0x004F0008},
    {0x004E1000},
    {0x004D0000},
    {0x004C0008},
    {0x004B0008},
    {0x004AA181},
    {0x00492000},
    {0x00480006},
    {0x00470000},
    {0x00460008},
    {0x0045A181},
    {0x00442000},
    {0x00430006},
    {0x00420000},
    {0x00410008},
    {0x0040A181},
    {0x003F2000},
    {0x003E0006},
    {0x003D0000},
    {0x003C0008},
    {0x003B0008},
    {0x003A502C},
    {0x00394000},
    {0x00380006},
    {0x0037001E},
    {0x00363400},
    {0x00350069},
    {0x00345000},
    {0x003340C0},
    {0x003207C0},
    {0x00310013},
    {0x003023C7},
    {0x002F0380},
    {0x002E0000},
    {0x002D4F80},
    {0x002C0318},
    {0x002B0051},
    {0x002A0002},
    {0x00290000},
    {0x00280000},
    {0x00270000},
    {0x00260000},
    {0x00250000},
    {0x00240000},
    {0x00230000},
    {0x00220000},
    {0x00210000},
    {0x00200000},
    {0x001F0000},
    {0x001E0060},
    {0x001D0000},
    {0x001C0000},
    {0x001B0004},
    {0x001A0000},
    {0x00190401},
    {0x00188024},
    {0x00170000},
    {0x00160000},
    {0x00150000},
    {0x00140000},
    {0x00130000},
    {0x00120000},
    {0x001126C4},
    {0x0010921F},
    {0x000FA037},
    {0x000E0000},
    {0x000D0000},
    {0x000C0000},
    {0x000B0000},
    {0x000A0000},
    {0x00090000},
    {0x00080000},
    {0x00070C0D},
    {0x00060000},
    {0x00050008},
    {0x00040000},
    {0x00030000},
    {0x00020003},
    {0x00018310},
    {0x00001010},
    {0x00001000},
};

uint32_t cdce6214InternelRegValue[] = {
    {0x00550000},
    {0x00540000},
    {0x00530000},
    {0x00520000},
    {0x00510004},
    {0x00500000},
    {0x004F0008},
    {0x004E1000},
    {0x004D0000},
    {0x004C0008},
    {0x004B0008},
    {0x004AA181},
    {0x00492000},
    {0x00480006},
    {0x00470000},
    {0x00460008},
    {0x0045A181},
    {0x00442000},
    {0x00430006},
    {0x00420000},
    {0x00410008},
    {0x0040A181},
    {0x003F2000},
    {0x003E0006},
    {0x003D0000},
    {0x003C0008},
    {0x003B0008},
    {0x003A502C},
    {0x00394000},
    {0x00380006},
    {0x0037001E},
    {0x00363400},
    {0x00350069},
    {0x00345000},
    {0x003340C0},
    {0x003207C0},
    {0x00310013},
    {0x003023C7},
    {0x002F0380},
    {0x002E0000},
    {0x002D4F80},
    {0x002C0318},
    {0x002B0051},
    {0x002A0002},
    {0x00290000},
    {0x00280000},
    {0x00270000},
    {0x00260000},
    {0x00250000},
    {0x00240000},
    {0x00230000},
    {0x00220000},
    {0x00210000},
    {0x00200000},
    {0x001F0000},
    {0x001E0060},
    {0x001D0000},
    {0x001C0000},
    {0x001B0004},
    {0x001A0000},
    {0x00190404},
    {0x00188024},
    {0x00170000},
    {0x00160000},
    {0x00150000},
    {0x00140000},
    {0x00130000},
    {0x00120000},
    {0x001126C4},
    {0x0010921F},
    {0x000FA037},
    {0x000E0000},
    {0x000D0000},
    {0x000C0000},
    {0x000B0000},
    {0x000A0000},
    {0x00090000},
    {0x00080000},
    {0x00070C0D},
    {0x00060000},
    {0x00050008},
    {0x00040000},
    {0x00030000},
    {0x00020003},
    {0x00018310},
    {0x00001000},

};

uint16_t lmkdb1108RegValue[] = {
    {0x00EA},
    {0x0124},
    {0x0200},
    {0x0300},
    {0x0410},
    {0x050A},
    {0x0608},
    {0x0707},
    {0x0800},
    {0x0900},
    {0x0BEE},
    {0x0C24},
    {0x1166},
    {0x1248},
    {0x14EA},
    {0x1524},
    {0x2600},
    {0x2702},

};

void cdce6214_reg_config(void)
{
    int rtn = 0;
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "CDCE6124_I2C_DEV device open failed!\n");
    }
    uint8_t elementNum = sizeof(cdce6214RegValue) / sizeof(cdce6214RegValue[0]);
    uint16_t slaveAddr;
    uint16_t data, rescvData;
    for (uint8_t index = 0; index < elementNum; ++index)
    {
        slaveAddr = (uint16_t)((cdce6214RegValue[index] >> 16) & 0xffff);
        data = (uint16_t)(cdce6214RegValue[index] & 0xffff);
        rtn = write_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, slaveAddr, 2, data, 2);
        rtn = read_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, slaveAddr, 2, &rescvData, 2);
        rescvData = (rescvData >> 8 & 0xff) | (rescvData << 8 & 0xff00);
        P_LOG_DEBUG("Write addr %#x value:%#x, read back value: %#x; equiality %s", slaveAddr, data, rescvData, (data == rescvData) ? "true" : "false");
    }
    uint16_t resvBuf;
    sleep(2);
    rtn = read_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, 0x0007, 2, &resvBuf, 2);
    resvBuf = (resvBuf >> 8 & 0xff) | (resvBuf << 8 & 0xff00);
    P_LOG_DEBUG("Read addr 0x0007 value: %#x", resvBuf);
    close(fd);
    P_LOG_INFO("CDCE6124_I2C_DEV device config finished!");
}

void cdce6214_internel_reg_config(void)
{
    int rtn = 0;
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "CDCE6124_I2C_DEV device open failed!\n");
    }
    uint8_t elementNum = sizeof(cdce6214InternelRegValue) / sizeof(cdce6214InternelRegValue[0]);
    uint16_t slaveAddr;
    uint16_t data, rescvData;
    for (uint8_t index = 0; index < elementNum; ++index)
    {
        slaveAddr = (uint16_t)((cdce6214InternelRegValue[index] >> 16) & 0xffff);
        data = (uint16_t)(cdce6214InternelRegValue[index] & 0xffff);
        rtn = write_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, slaveAddr, 2, data, 2);
        rtn = read_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, slaveAddr, 2, &rescvData, 2);
        rescvData = (rescvData >> 8 & 0xff) | (rescvData << 8 & 0xff00);
        P_LOG_REPEAT("Write addr %#x value:%#x, read back value: %#x; equiality %s", slaveAddr, data, rescvData, (data == rescvData) ? "true" : "false");
    }
    uint16_t resvBuf;
    sleep(2);
    rtn = read_reg_var(fd, CDCE6124_I2C_SLAVE_ADDR, 0x0007, 2, &resvBuf, 2);
    resvBuf = (resvBuf >> 8 & 0xff) | (resvBuf << 8 & 0xff00);
    P_LOG_DEBUG("Read addr 0x0007 value: %#x", resvBuf);
    close(fd);
    P_LOG_INFO("CDCE6124_I2C_DEV device config finished!");
}

void lmkdb1108_reg_config(void)
{
    int rtn = 0;
    int fd = open(LMKDB1108_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "LMKDB1108_I2C_DEV device open failed!\n");
    }
    uint8_t elementNum = sizeof(lmkdb1108RegValue) / sizeof(lmkdb1108RegValue[0]);
    uint8_t slaveAddr;
    uint8_t data;
    uint8_t addr = (uint8_t)LMKDB1108_I2C_SLAVE_ADDR >> 1;
    uint8_t resvData;
    for (uint8_t index = 0; index < elementNum; ++index)
    {
        slaveAddr = (uint8_t)((lmkdb1108RegValue[index] >> 8) & 0xff);
        data = (uint8_t)(lmkdb1108RegValue[index] & 0xff);
        rtn = write_reg_var(fd, addr, (slaveAddr | 0x80), 1, data, 1);
    }
    uint8_t resvData_1, resvData_2;
    rtn = read_reg_var(fd, addr, (0x27) | 0x80, 1, &resvData_1, 1);
    rtn = read_reg_var(fd, addr, (0x12) | 0x80, 1, &resvData_2, 1);
    P_LOG_REPEAT("Read addr 0x27 value: %#x, 0x12 value: %#x", resvData_1, resvData_2);
    close(fd);
    P_LOG_INFO("LMKDB1108_I2C_DEV device config finished!");
}

void power_suplly_init(void)
{
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "CDCE6124_I2C_DEV device open failed!\n");
    }
    write_reg_var(fd, TCA_6408_6214_EXPAND, 0x3, 1, 0x3, 1);
    write_reg_var(fd, TCA_6408_6214_EXPAND, 0x1, 1, 0x20, 1);
    gpio_set_value(AURORA_ENABLE_CTRL, 1);
    write_reg_var(fd, TCA_6408_6214_EXPAND, 0x1, 1, 0x24, 1);
    close(fd);
    P_LOG_INFO("Power supply init finished!");
}

void pcie_reset(void)
{
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "CDCE6124_I2C_DEV device open failed!\n");
    }
    write_reg_var(fd, TCA_6408_PCI_RESET_EXPAND, 0x3, 1, 0x0, 1);
    write_reg_var(fd, TCA_6408_PCI_RESET_EXPAND, 0x1, 1, 0x0, 1);
    sleep(1);
    write_reg_var(fd, TCA_6408_PCI_RESET_EXPAND, 0x1, 1, 0x1, 1);
    close(fd);
    P_LOG_INFO("PCIE reset finished!");
}

void cdce6214_eeprom_en(int en)
{
    en = (en == 0) ? 0 : 1;
    int fd = open(CDCE6124_I2C_DEV, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "CDCE6124_I2C_DEV device open failed!\n");
    }
    uint8_t accessVal = 0;
    read_reg_var(fd, TCA_6408_6214_EXPAND, 0x1, 1, accessVal, 1);
    if (en)
    {
        accessVal|= 0x04;
    }
    else
    {
        accessVal &= 0xFB;
    }
    read_reg_var(fd, TCA_6408_6214_EXPAND, 0x1, 1, accessVal, 1);
    close(fd);
    P_LOG_INFO("CDCE6214!");
}
