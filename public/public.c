#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "public.h"
#include "../lib/include/platform_log/platform_log.h"

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

int sync_filter_params(uint32_t fin, uint32_t *M1, uint32_t *M2, uint32_t *Nm)
{

    // ĺĺ§ĺź
    uint32_t m1 = 1;
    double threshold = 1024.0;

    if (fin == 0)
    {
        printf("input freq is zero\r\n");
        return -1;
    }

    // ćŻä¸éä¸ćĄŁé˘çďźM1 *= 2ďźç´ĺ° fin >= threshold
    while (fin < threshold)
    {
        m1 <<= 1;         // M1 *= 2
        threshold /= 2.0; // ä¸ä¸ćĄŁéĺź
    }

    // M2 ĺ Nm ćç§ĺŹĺźčŽĄçŽ
    uint32_t m2 = 8 / m1;
    if (m2 < 1)
        m2 = 1;

    uint32_t nm = (uint32_t)(4000000.0 / ((double)fin * m1));

    // čžĺşĺć°
    *M1 = m1;
    *M2 = m2;
    *Nm = nm;

    return 0;
}

// čˇłčżĺ­çŹŚä¸˛ĺźĺ¤´çéćĺ°ĺ­çŹŚćçŠşç˝ĺ­çŹŚ
char *skip_prefix(char *str)
{
    if (str == NULL)
        return NULL;
    while (*str != '\0' && (!isprint((unsigned char)*str)))
    {
        str++; // čˇłčżéćĺ°ĺ­çŹŚ
    }
    return str;
}

// ĺťé¤ĺ­çŹŚä¸˛ćŤĺ°žççŠşç˝ĺ­çŹŚďźĺćŹ \ră\năçŠşć źç­ďź
void trim_trailing_whitespace(char *str)
{
    if (str == NULL || *str == '\0')
        return;

    int len = strlen(str);
    // äťćŤĺ°žĺĺćĽćžçŹŹä¸ä¸ŞéçŠşç˝ĺ­çŹŚ
    while ((len > 0) && (isspace((unsigned char)str[len - 1])))
    {
        len--;
    }
    str[len] = '\0'; // ĺ¨éçŠşç˝ĺ­çŹŚĺćˇťĺ çťć­˘çŹŚ
}

#define CONFIG_IP_CORRECT 0x01
#define CONFIG_MAC_CORRECT 0x02
#define CONFIG_IS_STATIC 0x04 // ćĽĺŁĺˇ˛ćŻéćéç˝Žć¨Ąĺź
#define CONFIG_IS_DHCP 0x08   // ćĽĺŁćŻDHCPéç˝Žć¨Ąĺź
// ä¸´ćśéç˝ŽćäťśčˇŻĺžďźéżĺç´ćĽčŚçĺćäťśĺŻźč´éç˝Žćĺďź
#define TMP_CONFIG_FILE "/etc/network/interfaces.tmp"
void set_static_ip(const char *interface, const char *ip, const char *gateway, const char *netmask, const char *mac)
{
    const char *config_file = "/etc/network/interfaces";
    FILE *fp = fopen(config_file, "r");
    FILE *tmp_fp = fopen(TMP_CONFIG_FILE, "w");
    char line[256];
    int config_status = 0;
    int interface_found = 0;      // ćŻĺŚćžĺ°çŽć ç˝ĺĄéç˝Ž
    int interface_config_end = 0; // ćŻĺŚĺˇ˛čˇłčżçŽć ç˝ĺĄçéç˝ŽćŽľ
#define CONFIG_IP_CORRECT 0x01
#define CONFIG_MAC_CORRECT 0x02
    if (fp)
    {
        // čŻťĺĺéç˝Žćäťśďźĺćĺ˝ĺçśćĺšśĺ¤ĺśéçŽć ç˝ĺĄéç˝Ž
        while (fgets(line, sizeof(line), fp))
        {
            char trim_line[256];
            strcpy(trim_line, line);
            // ĺťé¤čĄéŚçŠşć źďźäžżäşĺ¤ć­éç˝Žçąťĺďź
            char *ptr = trim_line;
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;

            // ćžĺ°çŽć ç˝ĺĄçéç˝ŽćŽľďźĺšé "iface ç˝ĺĄĺ" ć "auto ç˝ĺĄĺ"ďź
            if (!interface_found && !interface_config_end)
            {
                // ĺšééćéç˝ŽćŽľďźiface xxx inet static
                if (strstr(ptr, "iface") && strstr(ptr, interface) && strstr(ptr, "inet"))
                {
                    interface_found = 1;
                    // ĺ¤ć­ćŻĺŚä¸şDHCPć¨Ąĺź
                    if (strstr(ptr, "dhcp"))
                    {
                        config_status |= CONFIG_IS_DHCP;
                    }
                    // ĺ¤ć­ćŻĺŚä¸şéćć¨Ąĺź
                    else if (strstr(ptr, "static"))
                    {
                        config_status |= CONFIG_IS_STATIC;
                    }
                }
                // ĺšéDHCPčŞĺ¨ĺŻĺ¨ďźauto xxx
                else if (strstr(ptr, "auto") && strstr(ptr, interface))
                {
                    interface_found = 1;
                    config_status |= CONFIG_IS_DHCP; // auto éĺ¸¸éĺdhcpä˝żç¨
                }
            }

            // 2. ĺˇ˛ćžĺ°çŽć ç˝ĺĄďźĺćĺśç°ćéç˝Žďźäťĺ¨éćć¨Ąĺźä¸ććďź
            if (interface_found && !interface_config_end && (config_status & CONFIG_IS_STATIC))
            {
                // ćŁćĽIPéç˝Ž
                if (strncmp(ptr, "address", 7) == 0)
                {
                    char cur_ip[64];
                    if (sscanf(ptr, "address %63s", cur_ip) == 1)
                    {
                        if (strcmp(cur_ip, ip) == 0)
                        {
                            config_status |= CONFIG_IP_CORRECT;
                        }
                    }
                    continue; // ćä¸ĺĺĽďźĺçť­çťä¸ćżć˘ä¸şć°éç˝Ž
                }

                // ćŁćĽMACĺ°ĺéç˝Ž
                if (strncmp(ptr, "hwaddress ether", 15) == 0)
                {
                    char cur_mac[64];
                    if (sscanf(ptr, "hwaddress ether %63s", cur_mac) == 1)
                    {
                        if (strcmp(cur_mac, mac) == 0)
                        {
                            config_status |= CONFIG_MAC_CORRECT;
                        }
                    }
                    continue; // ćä¸ĺĺĽďźĺçť­çťä¸ćżć˘ä¸şć°éç˝Ž
                }

                // ĺż˝çĽç˝ĺłăĺ­ç˝ćŠç éç˝ŽčĄďźĺçť­çťä¸ćżć˘ďź
                if (strncmp(ptr, "gateway", 7) == 0 || strncmp(ptr, "netmask", 8) == 0)
                {
                    continue;
                }

                // ćŁćľéç˝ŽćŽľçťćďźéĺ°ä¸ä¸ä¸Şiface/autoďźćçŠşčĄďź
                if ((strstr(ptr, "iface") || strstr(ptr, "auto")) && !strstr(ptr, interface))
                {
                    interface_config_end = 1;
                }
            }

            // ĺ¤ĺśéçŽć ç˝ĺĄçéç˝Žĺ°ä¸´ćśćäťś
            if (!interface_found || interface_config_end)
            {
                fputs(line, tmp_fp);
            }
        }

        // ĺĺĽçŽć ç˝ĺĄçéćéç˝ŽďźčŚçDHCPćć§éćéç˝Žďź
        fprintf(tmp_fp, "\n# Config for %s (static IP)\n", interface);
        fprintf(tmp_fp, "auto %s\n", interface); // ĺźćşčŞĺ¨ĺŻç¨
        fprintf(tmp_fp, "iface %s inet static\n", interface);
        fprintf(tmp_fp, "    address %s\n", ip);
        fprintf(tmp_fp, "    gateway %s\n", gateway);
        fprintf(tmp_fp, "    netmask %s\n", netmask);
        fprintf(tmp_fp, "    hwaddress ether %s\n", mac); // MACĺ°ĺçťĺŽ

        // ĺłé­ćäťś
        fclose(fp);
        fclose(tmp_fp);

        // ćżć˘ĺéç˝Žćäťś
        if (access(config_file, W_OK) != 0)
        {
            fprintf(stderr, "Warning: No write permission for %s, please run with sudo\n", config_file);
            remove(TMP_CONFIG_FILE);
            return;
        }

        // ĺ¤äť˝ĺéç˝Žćäťś
        char backup_cmd[256];
        snprintf(backup_cmd, sizeof(backup_cmd), "cp %s %s.bak", config_file, config_file);
        system(backup_cmd);

        // ćżć˘ĺéç˝Žćäťś
        if (rename(TMP_CONFIG_FILE, config_file) != 0)
        {
            fprintf(stderr, "Error: Failed to replace config file %s\n", config_file);
            remove(TMP_CONFIG_FILE);
            return;
        }

        // čžĺşéç˝ŽçśćäżĄćŻ
        printf("Static IP config for %s completed:\n", interface);
        printf("  IP: %s\n  Gateway: %s\n  Netmask: %s\n  MAC: %s\n", ip, gateway, netmask, mac);

        if (config_status & CONFIG_IS_DHCP)
        {
            printf("  Note: Interface was in DHCP mode, has been switched to static\n");
        }
        else if (config_status & CONFIG_IS_STATIC)
        {
            if ((config_status & CONFIG_IP_CORRECT) && (config_status & CONFIG_MAC_CORRECT))
            {
                printf("  Note: IP and MAC are already correct (no change needed)\n");
            }
            else
            {
                printf("  Note: Updated old static config to new parameters\n");
            }
        }
        else
        {
            printf("  Note: No existing config for %s, created new static config\n", interface);
        }

        P_LOG_INFO("eth interface %s set static ip success, ip address: %s, gateway:%s", interface, ip, gateway);
    }

// çŤĺłĺşç¨ć° IP éç˝Ž
#if 0
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "ifdown %s && ifup %s", interface, interface);
    system(cmd);
    P_LOG_INFO("eth interface %s set static ip success, ip address: %s, gateway:%s", interface, ip, gateway);
#endif
}

int start_dhcp_ip(void)
{
    const char *ifname = "end0";
    const char *config_file = "/etc/network/interfaces";
    FILE *fp;
    char line[256];
    int dhcp_in_file = 0;

    fp = fopen(config_file, "r");
    if (fp)
    {
        while (fgets(line, sizeof(line), fp))
        {
            line[strcspn(line, "\r\n")] = '\0';
            if (strstr(line, "iface end0 inet dhcp"))
            {
                dhcp_in_file = 1;
                break;
            }
        }
        fclose(fp);
    }

    // 2) ä¸ćŻ DHCP ĺ°ąéĺéç˝Ž
    if (!dhcp_in_file)
    {
        fp = fopen(config_file, "w");
        if (!fp)
        {
            perror("open interfaces");
            return -1;
        }
        fprintf(fp, "auto %s\n", ifname);
        fprintf(fp, "iface %s inet dhcp\n", ifname);
        fclose(fp);
    }
    else
    {
        printf("Already is dhcp, no need reconfig\r\n");
        return -1;
    }

    // 3) ĺć§ç DHCP ĺŽ˘ćˇçŤŻďźĺšśéĺŻç˝ĺĄ
    system("killall udhcpc 2>/dev/null");
    system("ifdown end0 2>/dev/null"); // ĺż˝çĽéčŻŻ
    system("ifup end0 2>/dev/null");

    // 4) äťĽâćééčŻăééťć¨ĄĺźâĺŻĺ¨ udhcpcďźéżĺéżćśé´éťĺĄďź
    //	  -n: ĺ¤ąč´ĽçŤĺłéĺş	 -q: ééť	-T 2: čśćś 2 ç§   -t 3: éčŻ 3 ćŹĄ
    int ret = system("udhcpc -i end0 -n -q -T 2 -t 3");
    if (ret != 0)
    {
        fprintf(stderr, "udhcpc failed (ret=%d)\n", ret);
    }
    printf("DHCP set Success\r\n");
}

void get_mac_address_ifconfig(const char *instance, char *mac_out)
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "ifconfig %s | grep -oE '([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}' | head -n 1",
             instance);

    FILE *pipe = popen(cmd, "r");
    if (!pipe)
    {
        P_LOG_ERROR("cmd error: %s", cmd);
        return;
    }

    if (fgets(mac_out, 18, pipe) == NULL)
    {
        pclose(pipe);
        P_LOG_ERROR("eth instance %s mac get error.", instance);
        return;
    }
    pclose(pipe);

    mac_out[strcspn(mac_out, "\n")] = '\0';
}

void print_binary_u8(uint8_t data)
{
    int i = 0, bit_value;
    printf("0b");
    for (i = 0; i < sizeof(uint8_t); ++i)
    {
        bit_value = data & (1 << i);
        printf("%d", bit_value);
    }
    printf("\r\n");
}

#define GPIO_PATH_MAX 64

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
        perror("Warning: Failed to export GPIO (may already be exported)");
    }

    close(fd);
    usleep(10000);
    return 0;
}

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

void set_static_ip_networkctl(const char *interface, const char *ip,  const char *gateway, const char *netmask, const char *mac)
{
    char conf_path[128];
    char tmp_conf[128];
    // 系统/user/lib/systemd/network下优先级为80, 大于此值才能够覆盖
    snprintf(conf_path, sizeof(conf_path), "%s/90-%s.network", C_OVERLAY_NET_CONFIG_PATH, interface);
    snprintf(tmp_conf, sizeof(tmp_conf), "/tmp/90-%s.network.tmp", interface);
    FILE *fp = fopen(tmp_conf, "w");
    if (fp == NULL)
    {
        P_LOG_ERROR("open tmp conf file failed %s", tmp_conf);
        return;
    }
    /*
    systemd-networkd static ip 标准格式
    [Match]
    Name=end0
    [Link]
    MACAddress=xx:xx:xx:xx:xx:xx

    [Network]
    Address=192.168.1.100/24
    Gateway=192.168.1.1
    */
    fprintf(fp, "[Match]\n");
    fprintf(fp, "Name=%s\n", interface);
    fprintf(fp, "[Link]\n");
    fprintf(fp, "MACAddress=%s\n\n", mac);
    fprintf(fp, "[Network]\n");
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
    char bak_cmd[256];
    snprintf(bak_cmd, sizeof(bak_cmd), "cp %s %s.bak 2>/dev/null", conf_path, conf_path);
    system(bak_cmd);

    // 替换正式配置文件
    if (rename(tmp_conf, conf_path) != 0)
    {
        P_LOG_ERROR("rename conf failed %s → %s", tmp_conf, conf_path);
        remove(tmp_conf);
        return;
    }

    // 通知systemd-networkd重载配置，热生效，不需要重启网卡
    int ret = system("networkctl reload");
    (void)ret;
    P_LOG_INFO("systemd-networkd reload done, %s static ip: %s gw:%s", interface, ip, gateway);

    printf("Static IP config for %s completed:\n", interface);
    printf("  IP: %s\n  Gateway: %s\n  Netmask: %s\n  MAC: %s\n", ip, gateway, netmask, mac);
}

int set_dhcp_ip_networkctl(const char *interface, const char *mac)
{
    char conf_path[128];
    char tmp_conf[128];
    // 系统/user/lib/systemd/network下优先级为80, 大于此值才能够覆盖
    snprintf(conf_path, sizeof(conf_path), "%s/90-%s.network", C_OVERLAY_NET_CONFIG_PATH, interface);
    snprintf(tmp_conf, sizeof(tmp_conf), "/tmp/90-%s.network.tmp", interface);
    FILE *fp = fopen(tmp_conf, "w");
    if (fp == NULL)
    {
        P_LOG_ERROR("open tmp conf file failed %s", tmp_conf);
        return;
    }
    /*
    systemd-networkd static ip 标准格式
    [Match]
    Name=end0
    [Link]
    MACAddress=xx:xx:xx:xx:xx:xx

    [Network]
    DHCP=ipv4 // 只跑 DHCPv4
    DHCP=yes  // ipv4 + ipv6 都开启 DHCP

    [DHCPv4]
    UseDNS=yes
    UseRoutes=yes
    UseNTP=yes
    #ClientIdentifier=mac  # 用mac作为dhcp client id，部分交换机/服务器需要
    */
    fprintf(fp, "[Match]\n");
    fprintf(fp, "Name=%s\n", interface);
    fprintf(fp, "[Link]\n");
    fprintf(fp, "MACAddress=%s\n", mac);
    fprintf(fp, "[Network]\n");
    fprintf(fp, "DHCP=ipv4\n");
    fclose(fp);
    // 备份原配置
    char bak_cmd[256];
    snprintf(bak_cmd, sizeof(bak_cmd), "cp %s %s.bak 2>/dev/null", conf_path, conf_path);
    system(bak_cmd);

    // 替换正式配置文件
    if (rename(tmp_conf, conf_path) != 0)
    {
        P_LOG_ERROR("rename conf failed %s → %s", tmp_conf, conf_path);
        remove(tmp_conf);
        return;
    }

    // 通知systemd-networkd重载配置，热生效，不需要重启网卡
    int ret = system("networkctl reload");
    (void)ret;
    P_LOG_INFO("systemd-networkd reload done, %s DHCP ip: %s mac:%s", interface, mac);

    printf("DHCP IP config for %s completed:\n", interface);
    printf("  MAC: %s\n", mac);
}
