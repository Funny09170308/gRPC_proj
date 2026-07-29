#ifndef __PARAM_MGR_H_
#define __PARAM_MGR_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../device_info.h"

#define DEVICE_NAME_LEN 32
#define DEVICE_CONFIG_PATH "/root/app/DeviceInfo.json"
#define DEVICE_KB_PARAM_PATH "/root/app/KBParam.csv"
#define IP_STR_LEN 16
#define MAC_STR_LEN 18
#define STR_SMALL_LEN 32
#define STR_MEDIUM_LEN 64
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int dhcpen;
        char interface[IP_STR_LEN];
        char ip[IP_STR_LEN];
        char mask[IP_STR_LEN];
        char gw[IP_STR_LEN];
        char mac[MAC_STR_LEN];

        char vendors[STR_SMALL_LEN]; // 设备厂商
        char model[STR_MEDIUM_LEN];  // 设备型号
        char serial[STR_SMALL_LEN];  // 设备序列号
        char version[STR_SMALL_LEN]; // 设备固件版本
    } systemConfig_t;

    /// @brief get all the infomation of device
    /// @param pdev device info context
    /// @return
    bool get_device_config(systemConfig_t **pdev);

    // 从JSON文件加载设备配置
    int parse_device_info(const char *json_file, systemConfig_t *dev_info);

    // 保存设备配置到JSON文件
    int save_device_config(const char *filename, const systemConfig_t *config);

    /// @brief param
    /// @param
    void device_info_init(void);
#ifdef __cplusplus
}
#endif

#endif
