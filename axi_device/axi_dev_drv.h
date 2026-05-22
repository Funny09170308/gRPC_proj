#ifndef __AXI_DEV_DRV_H_
#define __AXI_DEV_DRV_H_

#include <stdint.h>

#define AXI_DEVICE_NAME_MAX_LENTH 32

typedef struct
{
    char m_name[AXI_DEVICE_NAME_MAX_LENTH]; // 设备名称
    uint64_t m_phys_base;                   // 设备基地址
    uint64_t m_map_size;                    // 映射大小
    volatile uint32_t *m_virt_base;         // 映射后的寄存器地址(4字节对齐)
} axiDevice_t;

/// @brief axi设备资源申请, 将axi设备映射到虚拟内存
/// @param dev axi设备上下文
/// @param name axi设备名称
/// @param base_addr axi设备基地址
/// @param map_size axi设备地址长度
/// @return
int axi_device_init(axiDevice_t *dev, const char *name, uint64_t base_addr, uint64_t map_size);

/// @brief 释放 mmap 资源
/// @param dev
void axi_device_release(axiDevice_t *dev);

/// @brief axi设备写32位数据
/// @param dev axi设备上下文
/// @param offset 地址偏移(4Byte)
/// @param value 写入值
void axi_device_reg_write_u32(axiDevice_t *dev, uint64_t offset, uint64_t value);

/// @brief axi设备读32位数据
/// @param dev axi设备上下文
/// @param offset 地址偏移(4Byte)
/// @return 寄存器值
uint32_t axi_device_reg_read_u32(axiDevice_t *dev, uint64_t offset);

#endif // __AXI_DEVICE_DRV_H_
