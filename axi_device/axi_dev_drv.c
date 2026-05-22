#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "axi_dev_drv.h"
#include "../platform_log/platform_log.h"

int axi_device_init(axiDevice_t *dev, const char *name, uint64_t phys_base, uint64_t map_size)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0)
    {
        P_LOG_ERROR("Failed to open /dev/mem");
        return -1;
    }

    if (strlen(name) > AXI_DEVICE_NAME_MAX_LENTH)
    {
        P_LOG_ERROR("Device name %s is too long!", name);
        return -1;
    }
    #define AXI_DEVICE_NAME_MAX_LENGTH 32
    // 复制设备名称
    strncpy(dev->m_name, name, AXI_DEVICE_NAME_MAX_LENGTH - 1);
    dev->m_name[AXI_DEVICE_NAME_MAX_LENGTH - 1] = '\0';

    dev->m_phys_base = phys_base;
    dev->m_map_size = map_size;
    // 4字节对齐映射
    dev->m_virt_base = (volatile uint32_t *)mmap(NULL,
                                              dev->m_map_size,
                                              PROT_READ | PROT_WRITE,
                                              MAP_SHARED,
                                              fd,
                                              dev->m_phys_base);
    close(fd);

    if (dev->m_virt_base == MAP_FAILED)
    {
        P_LOG_ERROR("AXI device %s initialize at base address %#llx failed (size: %#llx).",
                    name,
                    dev->m_phys_base,
                    dev->m_map_size);
        return -1;
    }

    P_LOG_INFO("AXI device %s initialized vitural addr: %#llx, at base address: %#llx (size: %#llx).",
               dev->m_name,
               dev->m_virt_base,
               dev->m_phys_base,
               dev->m_map_size);
    return 0;
}

void axi_device_release(axiDevice_t *dev)
{
    if (dev->m_virt_base)
    {
        munmap((void *)dev->m_virt_base, dev->m_map_size);
        P_LOG_INFO("AXI device %s released (size: %#x).",
                   dev->m_name,
                   dev->m_map_size);
    }
}

void axi_device_reg_write_u32(axiDevice_t *dev, uint64_t offset, uint64_t value)
{
    dev->m_virt_base[offset] = value;
    P_LOG_DEBUG("AXI device %s write data: %d(Hex: %#x) to address: %#x(offset: %#x).",
                dev->m_name,
                value,
                value,
                (dev->m_phys_base + (offset * 4)),
                (offset * 4));
    // P_LOG_DEBUG("The address just wrote: %#x value = %d(Hex: %#x)", (dev->m_phys_base + (offset * 4)), dev->m_virt_base[offset], dev->m_virt_base[offset]);
}

uint32_t axi_device_reg_read_u32(axiDevice_t *dev, uint64_t offset)
{
    uint32_t rtn = dev->m_virt_base[offset];
    P_LOG_DEBUG("AXI device %s read data: %d(Hex: %#x) from address: %#x(offset: %#x).",
                dev->m_name,
                rtn,
                rtn,
                (dev->m_phys_base + (offset * 4)),
                (offset * 4));
    return rtn;
}
