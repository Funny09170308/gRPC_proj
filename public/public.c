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

#if 0
/* çćâćç­ĺŻéâĺčżĺśďźć¨Ąć Python str(double)ďź
 * ç­çĽďźäť 17 ä˝ććć°ĺ­ĺžä¸čŻ %.Ngďźč˝ round-tripďźstrtod==ĺĺźďźĺ°ąćĽĺć´ç­çă
 */
static void double_to_shortest(char out[64], double x)
{
    snprintf(out, 64, "%.17g", x);
    char cand[64];
    for (int n = 16; n >= 1; --n)
    {
        snprintf(cand, sizeof(cand), "%.*g", n, x);
        char *endp = NULL;
        double back = strtod(cand, &endp);
        if (endp && *endp == '\0' && back == x)
        {
            strcpy(out, cand);
        }
        else
        {
            break;
        }
    }
}
#endif

#if 0
/* č§Łćĺčżĺś m[.n][eÂąk] -> ç˛žçĄŽĺć° num/denďźç­äťˇ Decimal(str(alpha))ă
 * čżéĺčŽž alpha >= 0ă
 */
static void parse_decimal_fraction(const char *s, unsigned __int128 *unum, unsigned __int128 *uden)
{
    while (*s == ' ' || *s == '\t')
        ++s;
    if (*s == '+')
        ++s;
    char buf[128];
    strncpy(buf, s, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;

    char *e = strchr(buf, 'e');
    if (!e)
        e = strchr(buf, 'E');
    int exp10 = 0;
    if (e)
    {
        exp10 = atoi(e + 1);
        *e = 0;
    }

    char digits[64];
    int di = 0;
    int frac_len = 0;
    for (const char *p = buf; *p; ++p)
    {
        if (*p == '.')
        {
            for (const char *q = p + 1; *q; ++q)
                if (*q >= '0' && *q <= '9')
                    ++frac_len;
            continue;
        }
        if (*p >= '0' && *p <= '9')
            digits[di++] = *p;
    }
    if (di == 0)
        digits[di++] = '0';
    digits[di] = 0;

    long k = (long)frac_len - (long)exp10; // den = 10^k
    unsigned __int128 num = 0;
    for (int i = 0; i < di; i++)
        num = num * 10 + (unsigned)(digits[i] - '0');

    unsigned __int128 den = 1;
    if (k >= 0)
    {
        for (long i = 0; i < k; i++)
            den *= 10;
    }
    else
    {
        for (long i = 0; i < -k; i++)
            num *= 10;
    }

    *unum = num;
    *uden = den;
}

/* čŽĄçŽ Q = int(Decimal(str(alpha)) * 2**96)ă
 * ç¨âäşčżĺśéżé¤ćłâďźQ = floor((num/den)*2^96)ă
 */
static unsigned __int128 decimal_times_2p96_to_u128(const char *str_alpha)
{
    unsigned __int128 num, den;
    parse_decimal_fraction(str_alpha, &num, &den);

    unsigned __int128 ip = num / den;  // ć´ć°é¨ĺ
    unsigned __int128 rem = num % den; // ä˝ć°
    unsigned __int128 Q = ip << 96;    // ip * 2^96

    for (int i = 0; i < 96; i++)
    {
        rem <<= 1;
        unsigned bit = 0;
        if (rem >= den)
        {
            rem -= den;
            bit = 1;
        }
        Q = (Q << 1) | bit;
    }
    return Q;
}

int alpha_to_hi_lo(double alpha, uint64_t *hi, uint64_t *lo)
{
    if (!hi || !lo)
        return -1;

    /* ĺşĺŽä¸ş C ĺşĺä¸éťčŽ¤čĺĽďźéżĺ locale/čĺĽç­çĽĺ˝ąĺč§Łć */
    setlocale(LC_NUMERIC, "C");
    fesetround(FE_TONEAREST);

    /* 1) çćä¸ Python ä¸č´çâćç­ĺŻéâĺčżĺś */
    char s[64];
    double_to_shortest(s, alpha);

    /* ĺŻéďźć e ćšć EďźĺŞćŻćžç¤şéŁć źďźä¸ĺ˝ąĺć°ĺźďź
       for (char *p=s; *p; ++p) if (*p=='e') *p='E';
    */

    /* 2) čŽĄçŽ Q = int(Decimal(str(alpha)) * 2**96) */
    unsigned __int128 Q = decimal_times_2p96_to_u128(s);

    /* 3) ććéŤ/ä˝ 64 ä˝ */
    *hi = (uint64_t)(Q >> 64);
    *lo = (uint64_t)Q;
    return 0;
}


/**
 * @brief čŽĄçŽä¸éśä˝éćť¤ćł˘ĺ¨çłťć° alpha
 * @param tau ćśé´ĺ¸¸ć°ďźç§ďź
 * @return alpha çłťć°
 */
double calc_alpha(double tau)
{
    // ä¸ Python ä¸č´ďźf_s = 4e6
    const long double fs_base = 40e6L;    // 40 MHz
    const long double dn = 10.0L;         // ä¸éć ˇĺ ĺ­
    const long double f_s = fs_base / dn; // 4 MHz
    const long double f_c = 1.0L / (2.0L * M_PI * (long double)tau);
    const long double gama = 2.0L * M_PI * f_c / f_s;

    long double alphaL;

    if (gama > 0.001L)
    {
        const long double gama_cos = cosl(gama);
        // ĺ Python ĺŽĺ¨ä¸č´çčĄ¨čžžĺź
        alphaL = gama_cos - 1.0L + sqrtl(gama_cos * gama_cos - 4.0L * gama_cos + 3.0L);
    }
    else
    {
        alphaL = gama / (1.0L + gama);
    }

    double alpha = (double)alphaL;

    // ćĺ°ä¸¤ç§ć źĺźďźäžżäşčˇ Python ĺŻšćŻ
    printf("alpha = %.17g (%.17f)\n", alpha, alpha);

    return alpha;
}

int get_alpha_to_reg(uint64_t *alpha_high, uint64_t *alpha_low, double tau)
{
    double alpha;
    uint64_t alpha_h, alpha_l;

    alpha = calc_alpha(tau);

    if (alpha_to_hi_lo(alpha, &alpha_h, &alpha_l) == 0)
    { /*ćľčŻä¸éşythonäťŁç çťćä¸č´*/
        printf("alpha_hi = 0x%llX\n", (unsigned long long)alpha_h);
        printf("alpha_lo = 0x%016llX\n", (unsigned long long)alpha_l);
        // çťĺć´ä˝ 128 ä˝ĺĺ­čżĺśďźĺŻéďź
        printf("alpha128 = 0x%llX%016llX\n", (unsigned long long)alpha_h, (unsigned long long)alpha_l);
    }
    else
    {
        fprintf(stderr, "alpha_to_hi_lo failed\n");
    }

    *alpha_high = alpha_h;
    *alpha_low = alpha_l;

    return 0;
}
#endif

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

    // 1) ćŁćĽéç˝ŽćäťśćŻĺŚĺˇ˛ä¸ş DHCP
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

    // čŻťĺMACĺ°ĺ
    if (fgets(mac_out, 18, pipe) == NULL)
    {
        pclose(pipe);
        P_LOG_ERROR("eth instance %s mac get error.", instance);
        return;
    }
    pclose(pipe);

    // ĺťé¤ć˘čĄçŹŚ
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
