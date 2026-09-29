#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "public.h"
#include "../platform_log/platform_log.h"

void printhex(void *buffer, int size, int linecnt)
{
    int i;
    unsigned char *p = (unsigned char *)buffer;
    for (i = 0; i < size; i++)
    {
        if ((i % linecnt == 0) && (i > 0))
        {
            printf("\n");
        }
        printf("0x%02x ", p[i]);
    }
    printf("\n");
}

/**
 * @brief petalinux2025.2 systemd‑networkd 设置静态IP
 * 生成 /usr/lib/systemd/network/80-wired.network
 * Address 需要带掩码如 192.168.1.100/24，注意入参ip是不带掩码，netmask需要转换
 * @param interface end0
 * @param ip 192.168.1.100
 * @param gateway 192.168.1.1
 * @param netmask 255.255.255.0
 * @param mac mac地址，systemd‑networkd Match MACAddress绑定网卡
 */
void set_static_ip(const char *interface, const char *ip, const char *gateway, const char *netmask, const char *mac)
{
    char conf_path[128];
    char tmp_conf[128];
    // 配置文件路径，30‑ 数字代表优先级
    snprintf(conf_path, sizeof(conf_path), "/usr/lib/systemd/network/80-wired.network");
    snprintf(tmp_conf, sizeof(tmp_conf), "/tmp/80-wired.network.tmp", interface);

    FILE *fp = fopen(tmp_conf, "w");
    if (fp == NULL)
    {
        P_LOG_ERROR("open tmp conf file failed %s", tmp_conf);
        return;
    }

    /*
    systemd‑networkd static ip 标准格式
    [Match]
    Type=ether
    Name=!veth*
    KernelCommandLine=!nfsroot
    KernelCommandLine=!ip
    [Network]
    Address=192.168.1.11/24
    Gateway=192.168.1.1
    */
    fprintf(fp, "[Match]\n");
    fprintf(fp, "Type=ether\n");
    fprintf(fp, "Name=!veth*\n");
    fprintf(fp, "KernelCommandLine=!nfsroot\n");
    fprintf(fp, "KernelCommandLine=!ip\n");
    fprintf(fp, "[Link]\n");
    fprintf(fp, "MACAddress=%s\n\n", mac);
    fprintf(fp, "[Network]\n");
    // 需要把netmask转成prefix‑len，这里简单处理：代码中拼接；业务层保证netmask合法255.255.255.0 → /24
    // 简易只支持255.255.255.0场景；如果需要完整netmask转前缀可以加工具函数
    if (strcmp(netmask, "255.255.255.0") == 0)
    {
        fprintf(fp, "Address=%s/24\n", ip);
    }
    else if (strcmp(netmask, "255.255.0.0") == 0)
    {
        fprintf(fp, "Address=%s/16\n", ip);
    }
    else if (strcmp(netmask, "255.0.0.0") == 0)
    {
        fprintf(fp, "Address=%s/8\n", ip);
    }
    else
    {
        P_LOG_WARNING("unsupport netmask %s, fallback /24", netmask);
        fprintf(fp, "Address=%s/24\n", ip);
    }
    fprintf(fp, "Gateway=%s\n", gateway);
    fclose(fp);

    // 备份原配置
    // char bak_cmd[256];
    // snprintf(bak_cmd, sizeof(bak_cmd), "cp %s %s.bak 2>/dev/null", conf_path, conf_path);
    // system(bak_cmd);

    // 替换正式配置文件
    if (rename(tmp_conf, conf_path) != 0)
    {
        P_LOG_ERROR("rename conf failed %s → %s", tmp_conf, conf_path);
        remove(tmp_conf);
        return;
    }

    // 通知systemd‑networkd重载配置，热生效，不需要重启网卡
    int ret = system("networkctl reload");
    (void)ret;
    P_LOG_INFO("systemd‑networkd reload done, %s static ip: %s gw:%s", interface, ip, gateway);

    printf("Static IP config for %s completed:\n", interface);
    printf("  IP: %s\n  Gateway: %s\n  Netmask: %s\n  MAC: %s\n", ip, gateway, netmask, mac);
}

void start_dhcp_ip(const char *interface, const char *mac)
{
    char conf_path[128];
    char tmp_conf[128];
    snprintf(conf_path, sizeof(conf_path), "/usr/lib/systemd/network/80-wired.network");
    snprintf(tmp_conf, sizeof(tmp_conf), "/tmp/80-wired.network.tmp", interface);

    FILE *fp = fopen(tmp_conf, "w");
    if (fp == NULL)
    {
        P_LOG_ERROR("open tmp dhcp conf failed");
        return;
    }
    fprintf(fp, "[Match]\n");
    fprintf(fp, "Name=%s\n", interface);
    fprintf(fp, "MACAddress=%s\n\n", mac);
    fprintf(fp, "[Network]\n");
    fprintf(fp, "DHCP=yes\n");
    fprintf(fp, "DNS=8.8.8.8\n");
    fclose(fp);

    // char bak_cmd[256];
    // snprintf(bak_cmd, sizeof(bak_cmd), "cp %s %s.bak 2>/dev/null", conf_path, conf_path);
    // system(bak_cmd);

    if (rename(tmp_conf, conf_path) != 0)
    {
        P_LOG_ERROR("rename dhcp conf failed");
        remove(tmp_conf);
        return;
    }
    system("networkctl reload");
    P_LOG_INFO("systemd‑networkd set DHCP mode for %s", interface);
}

/**
 * @brief 获取网口MAC，适配petalinux2025.2 systemd‑networkd，使用ip link，不依赖ifconfig
 * @param instance 网卡名 end0
 * @param mac_out 输出，buf≥18字节 "xx:xx:xx:xx:xx:xx"
 */
void get_mac_address_ifconfig(const char *instance, char *mac_out)
{
    char cmd[256];
    // ip link show end0 | grep -oE 'link/ether [0-9a-fA-F:]+' | awk '{print $2}'
    snprintf(cmd, sizeof(cmd),
             "ip link show %s | grep -oE 'link/ether [0-9a-fA-F:]+' | awk '{print $2}'",
             instance);
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
    {
        P_LOG_ERROR("popen cmd error: %s", cmd);
        return;
    }
    mac_out[0] = '\0';
    if (fgets(mac_out, 18, pipe) == NULL)
    {
        pclose(pipe);
        P_LOG_ERROR("interface %s get mac failed.", instance);
        return;
    }
    pclose(pipe);
    mac_out[strcspn(mac_out, "\n")] = '\0';
    P_LOG_DEBUG("get mac[%s] for %s", mac_out, instance);
}

#define GPIO_PATH_MAX 64

/**
 * @brief ľźłöGPIO
 * @param gpio_num GPIOąŕşĹ
 * @return łÉšŚˇľťŘ0ŁŹĘ§°ÜˇľťŘ-1
 */
int sys_gpio_export(int gpio_num)
{
    int fd;
    char path[GPIO_PATH_MAX];
    char buf[16];

    // ´ňżŞexportÎÄźţ
    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open export");
        return -1;
    }

    // Đ´ČëGPIOąŕşĹ
    snprintf(buf, sizeof(buf), "%d", gpio_num);
    if (write(fd, buf, strlen(buf)) < 0)
    {
        // ČçšűŇŃž­ľźłöŁŹżÉÄÜťáą¨´íŁŹŐâŔďÖť´ňÓĄžŻ¸ć
        perror("Warning: Failed to export GPIO (may already be exported)");
    }

    close(fd);
    // ľČ´ýsysfsÎÄźţ´´˝¨ÍęłÉ
    usleep(10000);
    return 0;
}

/**
 * @brief ÉčÖĂGPIOˇ˝Ďň
 * @param gpio_num GPIOąŕşĹ
 * @param direction "in" ťň "out"
 * @return łÉšŚˇľťŘ0ŁŹĘ§°ÜˇľťŘ-1
 */
int sys_gpio_set_direction(int gpio_num, const char *direction)
{
    int fd;
    char path[GPIO_PATH_MAX];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio_num);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open direction");
        return -1;
    }

    if (write(fd, direction, strlen(direction)) < 0)
    {
        perror("Failed to set direction");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

/**
 * @brief ÉčÖĂGPIOÖľ
 * @param gpio_num GPIOąŕşĹ
 * @param value 0ťň1
 * @return łÉšŚˇľťŘ0ŁŹĘ§°ÜˇľťŘ-1
 */
int sys_gpio_set_value(int gpio_num, int value)
{
    int fd;
    char path[GPIO_PATH_MAX];
    char buf[2];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_num);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open value");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d", value);
    if (write(fd, buf, strlen(buf)) < 0)
    {
        perror("Failed to set value");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
