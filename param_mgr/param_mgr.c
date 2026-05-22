#include "cJSON.h"
#include "assert.h"
#include "param_mgr.h"
#include "../public/public.h"
#include "../platform_log/platform_log.h"

static systemConfig_t g_deviceCfg;

bool get_device_config(systemConfig_t **pdev)
{
	assert(pdev != NULL);
	*pdev = &g_deviceCfg;
	return true;
}

// 从JSON文件解析deviceInfo_t结构体
int parse_device_info(const char *json_file, systemConfig_t *sysConfig)
{
	// 1. 读取JSON文件内容
	FILE *file = fopen(json_file, "r");
	if (!file)
	{
		P_LOG_ERROR("File open failed: %s\n", json_file);
		return -1;
	}

	// 获取文件大小
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// 分配缓冲区并读取内容
	char *json_data = (char *)malloc(file_size + 1);
	if (!json_data)
	{
		fclose(file);
		P_LOG_ERROR("Buffer mallic failed!\n");
		return -1;
	}
	fread(json_data, 1, file_size, file);
	json_data[file_size] = '\0'; // 添加字符串结束符
	fclose(file);

	// 2. 解析顶层sysConfig对象
	cJSON *root = cJSON_Parse(json_data);
	if (!root)
	{
		P_LOG_ERROR("JSON prase faield: %s\n", cJSON_GetErrorPtr());
		free(json_data);
		return -1;
	}

	cJSON *item = cJSON_GetObjectItem(root, "dhcpen");
	if (cJSON_IsNumber(item))
	{
		sysConfig->dhcpen = item->valueint;
	}

	// 字符串字段
	item = cJSON_GetObjectItem(root, "interface");
	if (cJSON_IsString(item))
	{
		strncpy(sysConfig->interface, item->valuestring, IP_STR_LEN - 1);
		sysConfig->interface[IP_STR_LEN - 1] = '\0'; // 确保结束符
	}

	item = cJSON_GetObjectItem(root, "ip");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->ip, item->valuestring, IP_STR_LEN - 1);
		sysConfig->ip[IP_STR_LEN - 1] = '\0'; // 确保结束符
	}

	item = cJSON_GetObjectItem(root, "mask");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->mask, item->valuestring, IP_STR_LEN - 1);
		sysConfig->mask[IP_STR_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "gw");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->gw, item->valuestring, IP_STR_LEN - 1);
		sysConfig->gw[IP_STR_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "mac");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->mac, item->valuestring, MAC_STR_LEN - 1);
		sysConfig->mac[MAC_STR_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "vendors");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->vendors, item->valuestring, STR_SMALL_LEN - 1);
		sysConfig->vendors[STR_SMALL_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "model");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->model, item->valuestring, STR_MEDIUM_LEN - 1);
		sysConfig->model[STR_MEDIUM_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "serial");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->serial, item->valuestring, STR_SMALL_LEN - 1);
		sysConfig->serial[STR_SMALL_LEN - 1] = '\0';
	}

	item = cJSON_GetObjectItem(root, "version");
	if (cJSON_IsString(item) && item->valuestring)
	{
		strncpy(sysConfig->version, item->valuestring, STR_SMALL_LEN - 1);
		sysConfig->version[STR_SMALL_LEN - 1] = '\0';
	}

	// 释放资源
	cJSON_Delete(root);
	free(json_data);
	return 0;
}

int save_device_config(const char *filename, const systemConfig_t *sysConfig)
{
	if (!filename || !sysConfig)
		return -1;

	// 创建JSON根对象
	cJSON *root = cJSON_CreateObject();
	if (!root)
		return -2;

	// 添加系统配置
	cJSON_AddNumberToObject(root, "dhcpen", sysConfig->dhcpen);
	cJSON_AddStringToObject(root, "interface", sysConfig->interface);
	cJSON_AddStringToObject(root, "ip", sysConfig->ip);
	cJSON_AddStringToObject(root, "mask", sysConfig->mask);
	cJSON_AddStringToObject(root, "gw", sysConfig->gw);
	cJSON_AddStringToObject(root, "mac", sysConfig->mac);
	cJSON_AddStringToObject(root, "vendors", sysConfig->vendors);
	cJSON_AddStringToObject(root, "model", sysConfig->model);
	cJSON_AddStringToObject(root, "serial", sysConfig->serial);
	cJSON_AddStringToObject(root, "version", sysConfig->version);

	// 转换为JSON字符串并写入文件
	char *json_data = cJSON_Print(root);
	cJSON_Delete(root);

	if (!json_data)
		return -3;

	FILE *file = fopen(filename, "w");
	if (!file)
	{
		free(json_data);
		return -4;
	}

	fputs(json_data, file);
	fclose(file);
	free(json_data);

	return 0;
}

void device_info_init(void)
{
	int rtn = parse_device_info(DEVICE_CONFIG_PATH, &g_deviceCfg);
	if (rtn == 0)
	{
		P_LOG_INFO("Read device info file %s succeed!", DEVICE_CONFIG_PATH);
		P_LOG_DEBUG(" file version: %s, software version: %s", g_deviceCfg.version, CURRENT_SOFTWARE_VERSION);
		if (0 != strncmp(g_deviceCfg.version, CURRENT_SOFTWARE_VERSION, STR_SMALL_LEN))
		{
			P_LOG_INFO("Current file is not newest setting, rewrite the file.");
			// 当前设备信息写入文件中
			get_mac_address_ifconfig(g_deviceCfg.interface, g_deviceCfg.mac);
			snprintf(g_deviceCfg.version, sizeof(g_deviceCfg.version), "%s", CURRENT_SOFTWARE_VERSION);
			save_device_config(DEVICE_CONFIG_PATH, &g_deviceCfg);
		}
	}
	else
	{
		P_LOG_ERROR("Prase JSON failed, using local parameter!");
		g_deviceCfg.dhcpen = 0;
		snprintf(g_deviceCfg.interface, sizeof(g_deviceCfg.interface), "%s", "eth0");
		snprintf(g_deviceCfg.ip, sizeof(g_deviceCfg.ip), "%s", "192.168.1.10");
		snprintf(g_deviceCfg.mask, sizeof(g_deviceCfg.mask), "%s", "255.255.255.0");
		snprintf(g_deviceCfg.gw, sizeof(g_deviceCfg.gw), "%s", "192.168.1.1");
		snprintf(g_deviceCfg.mac, sizeof(g_deviceCfg.mac), "%s", "00:0A:35:00:58:46");
		snprintf(g_deviceCfg.vendors, sizeof(g_deviceCfg.vendors), "%s", VENDER_NAME);
		snprintf(g_deviceCfg.model, sizeof(g_deviceCfg.model), "%s", DEVICE_NAME);
		snprintf(g_deviceCfg.serial, sizeof(g_deviceCfg.serial), "%s", "202604100001");
		snprintf(g_deviceCfg.version, sizeof(g_deviceCfg.version), "%s", CURRENT_SOFTWARE_VERSION);
	}
	// 上电设置IP
	P_LOG_INFO("g_deviceCfg.dhcpen = %d", g_deviceCfg.dhcpen);
	if (g_deviceCfg.dhcpen == 0)
	{
		P_LOG_INFO("DHCP enable = %d, set static IP.", g_deviceCfg.dhcpen);
		set_static_ip(g_deviceCfg.interface, g_deviceCfg.ip, g_deviceCfg.gw, g_deviceCfg.mask, g_deviceCfg.mac);
	}
	else if (g_deviceCfg.dhcpen == 1)
	{
		P_LOG_INFO("DHCP enable = %d, set DHCP.", g_deviceCfg.dhcpen);
		start_dhcp_ip();
	}
	sleep(1);
	P_LOG_INFO("************************************************");
	P_LOG_INFO("*    Device Interface: %s", g_deviceCfg.interface);
	P_LOG_INFO("*                DHCP: %s", ((g_deviceCfg.dhcpen == 0) ? "DISABLE" : "ENABLE"));
	P_LOG_INFO("*    static Device IP: %s", g_deviceCfg.ip);
	P_LOG_INFO("*         Device Mask: %s", g_deviceCfg.mask);
	P_LOG_INFO("*           Device GW: %s", g_deviceCfg.gw);
	P_LOG_INFO("*          Device MAC: %s", g_deviceCfg.mac);
	P_LOG_INFO("*              Vender: %s", g_deviceCfg.vendors);
	P_LOG_INFO("*               Model: %s", g_deviceCfg.model);
	P_LOG_INFO("*                  SN: %s", g_deviceCfg.serial);
	P_LOG_INFO("*        Software Ver: %s", g_deviceCfg.version);
	P_LOG_INFO("************************************************");
	fflush(stdout);
}
