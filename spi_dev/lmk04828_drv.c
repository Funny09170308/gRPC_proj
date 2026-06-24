#include "spi_func.h"
#include "lmk04828_drv.h"
#include "../axi_gpio/axi_gpio.h"
#include "../platform_log/platform_log.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

struct tireg lmk04828_reset_bank[] = {
    {0x000090},
    {0x000010},
    {0x014A33}};

struct tireg awg_lmk04828_out_reg[] = {
    {0x000200},
    {0x000306},
    {0x0004D0},
    {0x00055B},
    {0x000600},
    {0x000C51},
    {0x000D04},
    {0x010005},
    {0x010155},
    {0x010255},
    {0x010300},
    {0x010422},
    {0x010510},
    {0x0106F0},
    {0x010715},
    {0x010805},
    {0x010955},
    {0x010A55},
    {0x010B00},
    {0x010C22},
    {0x010D10},
    {0x010EF0},
    {0x010F15},
    {0x011005},
    {0x011155},
    {0x011255},
    {0x011300},
    {0x011422},
    {0x011510},
    {0x0116F0},
    {0x011715},
    {0x011805},
    {0x011955},
    {0x011A55},
    {0x011B00},
    {0x011C22},
    {0x011D10},
    {0x011EF0},
    {0x011F15},
    {0x012001},
    {0x012155},
    {0x012255},
    {0x012302},
    {0x012402},
    {0x012500},
    {0x0126F0},
    {0x012755},
    {0x012801},
    {0x012955},
    {0x012A55},
    {0x012B02},
    {0x012C02},
    {0x012D00},
    {0x012EF0},
    {0x012F55},
    {0x01300A},
    {0x013155},
    {0x013255},
    {0x013300},
    {0x013402},
    {0x013500},
    {0x0136F0},
    {0x013755},
    {0x013840},
    {0x013900},
    {0x013A00},
    {0x013B19},
    {0x013C00},
    {0x013D01},
    {0x013E00},
    {0x013F00},
    {0x0140F0},
    {0x014100},
    {0x014200},
    {0x014352},
    {0x014400},
    {0x01457F},
    {0x014610},
    {0x014712},
    {0x014802},
    {0x014902},
    {0x014A33},
    {0x014B36},
    {0x014C00},
    {0x014D00},
    {0x014EC0},
    {0x014F7F},
    {0x015003},
    {0x015102},
    {0x015200},
    {0x015300},
    {0x015478},
    {0x015500},
    {0x015632},
    {0x015700},
    {0x015814},
    {0x015900},
    {0x015A14},
    {0x015BD4},
    {0x015C20},
    {0x015D00},
    {0x015E00},
    {0x015F0B},
    {0x016000},
    {0x016101},
    {0x016244},
    {0x016300},
    {0x016400},
    {0x01650C},
    {0x0171AA},
    {0x017202},
    {0x017C15},
    {0x017D33},
    {0x016600},
    {0x016700},
    {0x01680C},
    {0x016959},
    {0x016A20},
    {0x016B00},
    {0x016C00},
    {0x016D00},
    {0x016E13},
    {0x017360},
    {0x018200},
    {0x018300},
    {0x018400},
    {0x018500},
    {0x018800},
    {0x018900},
    {0x018A00},
    {0x018B00},
    {0x1FFD00},
    {0x1FFE00},
    {0x1FFF53},
};

struct tireg qa_lmk04828_out_reg[] = {
    {0x000200},
    {0x000306},
    {0x0004D0},
    {0x00055B},
    {0x000600},
    {0x000C51},
    {0x000D04},
    {0x010001},
    {0x010155},
    {0x010255},
    {0x010302},
    {0x010422},
    {0x010510},
    {0x0106F0},
    {0x010715},
    {0x010801},
    {0x010955},
    {0x010A55},
    {0x010B02},
    {0x010C22},
    {0x010D10},
    {0x010EF0},
    {0x010F15},
    {0x011001},
    {0x011155},
    {0x011255},
    {0x011302},
    {0x011422},
    {0x011510},
    {0x0116F0},
    {0x011715},
    {0x011801},
    {0x011955},
    {0x011A55},
    {0x011B02},
    {0x011C22},
    {0x011D10},
    {0x011EF0},
    {0x011F15},
    {0x012001},
    {0x012155},
    {0x012255},
    {0x012302},
    {0x012402},
    {0x012500},
    {0x0126F0},
    {0x012755},
    {0x012801},
    {0x012955},
    {0x012A55},
    {0x012B02},
    {0x012C02},
    {0x012D00},
    {0x012EF0},
    {0x012F55},
    {0x01300A},
    {0x013155},
    {0x013255},
    {0x013300},
    {0x013402},
    {0x013500},
    {0x0136F0},
    {0x013755},
    {0x013840},
    {0x013900},
    {0x013A00},
    {0x013B19},
    {0x013C00},
    {0x013D01},
    {0x013E00},
    {0x013F00},
    {0x0140F0},
    {0x014100},
    {0x014200},
    {0x014352},
    {0x014400},
    {0x01457F},
    {0x014610},
    {0x014712},
    {0x014802},
    {0x014902},
    {0x014A33},
    {0x014B36},
    {0x014C00},
    {0x014D00},
    {0x014EC0},
    {0x014F7F},
    {0x015003},
    {0x015102},
    {0x015200},
    {0x015300},
    {0x015478},
    {0x015500},
    {0x015632},
    {0x015700},
    {0x015814},
    {0x015900},
    {0x015A14},
    {0x015BD4},
    {0x015C20},
    {0x015D00},
    {0x015E00},
    {0x015F0B},
    {0x016000},
    {0x016101},
    {0x016244},
    {0x016300},
    {0x016400},
    {0x01650C},
    {0x0171AA},
    {0x017202},
    {0x017C15},
    {0x017D33},
    {0x016600},
    {0x016700},
    {0x01680C},
    {0x016959},
    {0x016A20},
    {0x016B00},
    {0x016C00},
    {0x016D00},
    {0x016E13},
    {0x017360},
    {0x018200},
    {0x018300},
    {0x018400},
    {0x018500},
    {0x018800},
    {0x018900},
    {0x018A00},
    {0x018B00},
    {0x1FFD00},
    {0x1FFE00},
    {0x1FFF53},
};

struct tireg qa_lmk04828InternelReg[] = {
    {0x000200},
    {0x000306},
    {0x0004D0},
    {0x00055B},
    {0x000600},
    {0x000C51},
    {0x000D04},
    {0x01000A},
    {0x010155},
    {0x010255},
    {0x010300},
    {0x010422},
    {0x010510},
    {0x0106F0},
    {0x010715},
    {0x01080A},
    {0x010955},
    {0x010A55},
    {0x010B00},
    {0x010C22},
    {0x010D10},
    {0x010EF0},
    {0x010F15},
    {0x01100A},
    {0x011155},
    {0x011255},
    {0x011300},
    {0x011420},
    {0x011510},
    {0x0116F0},
    {0x011715},
    {0x01180A},
    {0x011955},
    {0x011A55},
    {0x011B00},
    {0x011C22},
    {0x011D10},
    {0x011EF0},
    {0x011F15},
    {0x01200A},
    {0x012155},
    {0x012255},
    {0x012300},
    {0x012402},
    {0x012500},
    {0x0126F0},
    {0x012755},
    {0x01280A},
    {0x012955},
    {0x012A55},
    {0x012B00},
    {0x012C02},
    {0x012D00},
    {0x012EF0},
    {0x012F11},
    {0x013019},
    {0x013155},
    {0x013255},
    {0x013300},
    {0x013422},
    {0x013500},
    {0x0136F0},
    {0x013701},
    {0x013800},
    {0x013902},
    {0x013A00},
    {0x013BFA},
    {0x013C00},
    {0x013D01},
    {0x013E00},
    {0x013F00},
    {0x014000},
    {0x014100},
    {0x014200},
    {0x01435D},
    {0x014400},
    {0x01457F},
    {0x014610},
    {0x01471A},
    {0x014802},
    {0x014902},
    {0x014A33},
    {0x014B36},
    {0x014C00},
    {0x014D00},
    {0x014EC0},
    {0x014F7F},
    {0x015003},
    {0x015102},
    {0x015200},
    {0x015300},
    {0x015432},
    {0x015500},
    {0x015614},
    {0x015700},
    {0x015814},
    {0x015900},
    {0x015A14},
    {0x015BD4},
    {0x015C20},
    {0x015D00},
    {0x015E00},
    {0x015F0B},
    {0x016000},
    {0x016102},
    {0x016224},
    {0x016300},
    {0x016400},
    {0x016519},
    {0x0171AA},
    {0x017202},
    {0x017C15},
    {0x017D33},
    {0x016600},
    {0x016700},
    {0x016819},
    {0x016959},
    {0x016A20},
    {0x016B00},
    {0x016C00},
    {0x016D00},
    {0x016E13},
    {0x017300},
    {0x018200},
    {0x018300},
    {0x018400},
    {0x018500},
    {0x018800},
    {0x018900},
    {0x018A00},
    {0x018B00},
    {0x1FFD00},
    {0x1FFE00},
    {0x1FFF53},

};

struct tireg awg_lmk04828InternelReg[] = {
    {0x000200},
    {0x000306},
    {0x0004D0},
    {0x00055B},
    {0x000600},
    {0x000C51},
    {0x000D04},
    {0x010019},
    {0x010155},
    {0x010255},
    {0x010300},
    {0x010422},
    {0x010510},
    {0x0106F0},
    {0x010715},
    {0x010819},
    {0x010955},
    {0x010A55},
    {0x010B00},
    {0x010C22},
    {0x010D10},
    {0x010EF0},
    {0x010F15},
    {0x011019},
    {0x011155},
    {0x011255},
    {0x011300},
    {0x011420},
    {0x011510},
    {0x0116F0},
    {0x011715},
    {0x011819},
    {0x011955},
    {0x011A55},
    {0x011B00},
    {0x011C22},
    {0x011D10},
    {0x011EF0},
    {0x011F15},
    {0x01200A},
    {0x012155},
    {0x012255},
    {0x012300},
    {0x012402},
    {0x012500},
    {0x0126F0},
    {0x012755},
    {0x01280A},
    {0x012955},
    {0x012A55},
    {0x012B00},
    {0x012C02},
    {0x012D00},
    {0x012EF0},
    {0x012F11},
    {0x013019},
    {0x013155},
    {0x013255},
    {0x013300},
    {0x013422},
    {0x013500},
    {0x0136F0},
    {0x013701},
    {0x013800},
    {0x013902},
    {0x013A00},
    {0x013BFA},
    {0x013C00},
    {0x013D01},
    {0x013E00},
    {0x013F00},
    {0x014000},
    {0x014100},
    {0x014200},
    {0x01435D},
    {0x014400},
    {0x01457F},
    {0x014610},
    {0x01471A},
    {0x014802},
    {0x014902},
    {0x014A33},
    {0x014B36},
    {0x014C00},
    {0x014D00},
    {0x014EC0},
    {0x014F7F},
    {0x015003},
    {0x015102},
    {0x015200},
    {0x015300},
    {0x015432},
    {0x015500},
    {0x015614},
    {0x015700},
    {0x015814},
    {0x015900},
    {0x015A14},
    {0x015BD4},
    {0x015C20},
    {0x015D00},
    {0x015E00},
    {0x015F0B},
    {0x016000},
    {0x016102},
    {0x016224},
    {0x016300},
    {0x016400},
    {0x016519},
    {0x0171AA},
    {0x017202},
    {0x017C15},
    {0x017D33},
    {0x016600},
    {0x016700},
    {0x016819},
    {0x016959},
    {0x016A20},
    {0x016B00},
    {0x016C00},
    {0x016D00},
    {0x016E13},
    {0x017300},
    {0x018200},
    {0x018300},
    {0x018400},
    {0x018500},
    {0x018800},
    {0x018900},
    {0x018A00},
    {0x018B00},
    {0x1FFD00},
    {0x1FFE00},
    {0x1FFF53},

};

struct tireg lmk04828_sync[] = {
    {0x0143F1},
    {0x0143D1},
    {0x013902},
    {0x0144FF},
    {0x014352}};

struct tireg awg_lmk04828_spisync[] = {
    {0x010510}, // analog== 6时  不稳定 选0
    {0x010d10}, //
    {0x011510}, //
    {0x011d10}, //
    {0x010005},
};
struct tireg lmk04828_spisync[] = {
    {0x010510}, // analog== 6时  不稳定 选0
    {0x010d10}, //
    {0x011510}, //
    {0x011d10}, //
};

int lmk_write_reg(int fd, uint16_t addr, uint8_t val)
{
    uint8_t tx[3];

    tx[0] = (addr >> 8) & 0xFF;
    tx[1] = addr & 0xFF;
    tx[2] = val;

    return spi_write_bytes(fd, tx, 3);
}

int lmk_read_reg(int fd, uint16_t addr, uint8_t *val)
{
    uint8_t tx[3];
    uint8_t rx[3];

    tx[0] = 0x80 | ((addr >> 8) & 0x7F);
    tx[1] = addr & 0xFF;
    tx[2] = 0x00;

    if (spi_transfer_raw(fd, tx, rx, 3) < 0)
        return -1;

    *val = rx[2];
    return 0;
}

// LMK04828初始化函数
int lmk04828_reg_init(struct tireg *reset_group, uint32_t resetNum,
                      struct tireg *reg_group, uint32_t regNum,
                      struct tireg *spi_sync_group, uint32_t spiSyncNum,
                      struct tireg *sync_group, uint32_t syncNum)
{
    uint8_t val = 0;
    int i, regnums;
    int spi_fd = 0;

    spi_fd = spi_open_device(SPI1_DEV_PATH);
    if (spi_fd < 0)
    {
        fprintf(stderr, "SPI device open failed");
        return -1;
    }

    P_LOG_DEBUG("spi %s open succeed, fd = %d", SPI1_DEV_PATH, spi_fd);

    // ================= RESET GROUP =================
    if (reset_group != NULL)
    {
        regnums = resetNum;
        P_LOG_DEBUG("lmk04828 reset reg num: %d", regnums);

        for (i = 0; i < regnums; i++)
        {
            uint16_t addr = (reset_group[i].addr_data >> 8) & 0xFFFF;
            uint8_t data = reset_group[i].addr_data & 0xFF;

            lmk_write_reg(spi_fd, addr, data);

            usleep(100);

            uint8_t rdata = 0;
            if (lmk_read_reg(spi_fd, addr, &rdata) == 0)
            {
                P_LOG_REPEAT("RESET reg 0x%04X write=0x%02X read=0x%02X %s",
                             addr, data, rdata,
                             (rdata == data) ? "OK" : "MISMATCH");
            }
            else
            {
                P_LOG_REPEAT("RESET reg 0x%04X read failed", addr);
            }
        }
    }

    // ================= CONFIG GROUP =================
    if (reg_group != NULL)
    {
        regnums = regNum;
        P_LOG_DEBUG("lmk04828 reg num: %d", regnums);

        for (i = 0; i < regnums; i++)
        {
            uint16_t addr = (reg_group[i].addr_data >> 8) & 0xFFFF;
            uint8_t data = reg_group[i].addr_data & 0xFF;

            lmk_write_reg(spi_fd, addr, data);

            usleep(100);

            uint8_t rdata = 0;
            if (lmk_read_reg(spi_fd, addr, &rdata) == 0)
            {
                P_LOG_REPEAT("CFG reg 0x%04X write=0x%02X read=0x%02X %s",
                             addr, data, rdata,
                             (rdata == data) ? "OK" : "MISMATCH");
            }
            else
            {
                P_LOG_REPEAT("CFG reg 0x%04X read failed", addr);
            }
        }
    }

    // ================= SYNC GROUP =================
    if (sync_group != NULL)
    {
        regnums = syncNum;
        P_LOG_DEBUG("lmk04828 sync reg num: %d", regnums);

        for (i = 0; i < regnums; i++)
        {
            uint16_t addr = (sync_group[i].addr_data >> 8) & 0xFFFF;
            uint8_t data = sync_group[i].addr_data & 0xFF;

            lmk_write_reg(spi_fd, addr, data);

            usleep(100);
        }
    }

    // ================= SPI SYNC GROUP =================
    if (spi_sync_group != NULL)
    {
        regnums = spiSyncNum;
        P_LOG_DEBUG("lmk04828 spi sync reg num: %d", regnums);

        for (i = 0; i < regnums; i++)
        {
            uint16_t addr = (spi_sync_group[i].addr_data >> 8) & 0xFFFF;
            uint8_t data = spi_sync_group[i].addr_data & 0xFF;

            lmk_write_reg(spi_fd, addr, data);

            usleep(100);
        }
    }

    close(spi_fd);

    P_LOG_INFO("lmk04828 reg init end");

    return 0;
}

int lmk04828_default_init(int type)
{
    if (type == 0)
    {
        P_LOG_DEBUG("Device type AWG! Use EXTERNEL clk.");
        lmk04828_reg_init(lmk04828_reset_bank, sizeof(lmk04828_reset_bank) / sizeof(struct tireg),
                          awg_lmk04828_out_reg, sizeof(awg_lmk04828_out_reg) / sizeof(struct tireg),
                          awg_lmk04828_spisync, sizeof(awg_lmk04828_spisync) / sizeof(struct tireg),
                          lmk04828_sync, sizeof(lmk04828_sync) / sizeof(struct tireg));
    }
    else if (type == 1)
    {
        P_LOG_DEBUG("Device type QA! Use EXTERNEL clk.");
        lmk04828_reg_init(lmk04828_reset_bank, sizeof(lmk04828_reset_bank) / sizeof(struct tireg),
                          qa_lmk04828_out_reg, sizeof(qa_lmk04828_out_reg) / sizeof(struct tireg),
                          lmk04828_spisync, sizeof(lmk04828_spisync) / sizeof(struct tireg),
                          lmk04828_sync, sizeof(lmk04828_sync) / sizeof(struct tireg));
    }
    clock_init_identify(0);
}

int lmk04828_internel_init(int type)
{
    if (type == 0)
    {
        P_LOG_DEBUG("Device type AWG! Use INTERNEL clk.");
        lmk04828_reg_init(lmk04828_reset_bank, sizeof(lmk04828_reset_bank) / sizeof(struct tireg),
                          awg_lmk04828InternelReg, sizeof(awg_lmk04828InternelReg) / sizeof(struct tireg),
                          lmk04828_spisync, sizeof(lmk04828_spisync) / sizeof(struct tireg),
                          lmk04828_sync, sizeof(lmk04828_sync) / sizeof(struct tireg));
    }
    else if (type == 1)
    {
        P_LOG_DEBUG("Device type QA! Use INTERNEL clk.");
        lmk04828_reg_init(lmk04828_reset_bank, sizeof(lmk04828_reset_bank) / sizeof(struct tireg),
                          qa_lmk04828InternelReg, sizeof(qa_lmk04828InternelReg) / sizeof(struct tireg),
                          lmk04828_spisync, sizeof(lmk04828_spisync) / sizeof(struct tireg),
                          lmk04828_sync, sizeof(lmk04828_sync) / sizeof(struct tireg));
    }
    clock_init_identify(1);
}
