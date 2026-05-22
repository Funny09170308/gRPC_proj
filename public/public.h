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

int get_alpha_to_reg(uint64_t *alpha_high, uint64_t *alpha_low, double tau);

int sync_filter_params(uint32_t fin, uint32_t *M1, uint32_t *M2, uint32_t *Nm);

char *skip_prefix(char *str);
void trim_trailing_whitespace(char *str);

void set_static_ip(const char *interface, const char *ip, const char *gateway, const char *netmask, const char *mac);
int start_dhcp_ip(void);
void get_mac_address_ifconfig(const char *instance, char *mac_out);
void print_binary_u8(uint8_t data);

int sys_gpio_export(int gpio_num);
int sys_gpio_set_direction(int gpio_num, const char *direction);
int sys_gpio_set_value(int gpio_num, int value);

#endif
