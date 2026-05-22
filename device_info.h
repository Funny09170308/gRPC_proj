/********************************************
 * File : device_info.h
 * Version : V0.1.0
 * Author : ZWDX
 * Date : 2026.03.10
 ********************************************/
#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

#define _IO volatile

#include "stdint.h"
#define FACTORY_VERSION "Ver0.1.0"

#define VENDER_NAME "ZWDX"
#define CURRENT_SOFTWARE_VERSION "Ver0.2.0"

#define SLAVE_USE_PCIE 1

#if SLAVE_USE_PCIE == 0
#define DEVICE_NAME "SILICON_BASED_c2c"
#else
#define DEVICE_NAME "SILICON_BASED_pcie"
#endif

// ===================== 通用地址地址分区定义 ===================== //
#define PUBLIC_PERIPHERAL_BASEAADDR 0x80040000
#define PUBLIC_PERIPHERAL_LENGTH 0x20000

// ===================== Chip2Chip 子卡地址分区定义 ===================== //

// 子卡数量
#define CHIP_NUM 4
// 子卡起始地址
#define CHID_CHIP_BASEADDR 0x1000000000
// 子卡可访问空间
#define CHIP_SIZE 0x3ffffffff

// ===================== Chip2Chip 子卡地址分区定义 ===================== //

// ===================== PCIe 子卡地址分区定义 ===================== //
#define CHIP_USER_SPACE_BASEADDR 0x0
#define CHIP_USER_SPACE_LENGTH 0x400

#define CHIP_USER_SPACE_BASEADDR 0x0
#define CHIP_USER_SPACE_LENGTH 0x400

#endif // __DEVICE_INFO_H__
