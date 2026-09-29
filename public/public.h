#ifndef _PUBLIC_H_
#define _PUBLIC_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <locale.h>
#include <fenv.h>

void printhex(void *buffer, int size, int linecnt);

void set_static_ip(const char *interface, const char *ip, const char *gateway, const char *netmask, const char *mac);
void start_dhcp_ip(const char *interface, const char *mac);
void get_mac_address_ifconfig(const char *instance, char *mac_out);
void print_binary_u8(uint8_t data);

int sys_gpio_export(int gpio_num);
int sys_gpio_set_direction(int gpio_num, const char *direction);
int sys_gpio_set_value(int gpio_num, int value);

#endif
