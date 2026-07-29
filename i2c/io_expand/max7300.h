#ifndef __MAX7300_H__
#define __MAX7300_H__

#include <stdint.h>

typedef enum
{
    E_LED_01_CTRL = 0,
    E_LED_02_CTRL = 1,
    E_LED_03_CTRL = 2,
    E_LED_04_CTRL = 3,
    E_LED_05_CTRL = 4,
    E_LED_06_CTRL = 5,
    E_LED_07_CTRL = 6,
    E_LED_08_CTRL = 7,
    E_LED_A = 8,
    E_LED_B = 9,
} eI2CLEDSequenct_t;

#define C_PORT_CONFG_START_ADDR 0x09

#define C_SIGNAL_PORT_CTRL_START_ADDR 0x24
#define C_CONFIGURATION_ADDR 0x04

#define C_MAX7300_NORMAL_OPERATION 0x01
#define C_MAX7300_PORT_INPUT_CFG 0xAA
#define C_MAX7300_PORT_OUTPUT_CFG 0x55

#define MAX7300_IO_INPUT 0
#define MAX7300_IO_OUTPUT 1

#define LED_OFF 0x07
#define LED_GREEN 0x06
#define LED_RED 0x05
#define LED_BLUE 0x03
#define LED_YELLOW (LED_RED & LED_GREEN)
#define LED_MAGENTA (LED_RED & LED_BLUE)
#define LED_CYAN (LED_GREEN & LED_BLUE)
#define LED_WHITE (LED_RED & LED_GREEN & LED_BLUE)

int max7300_init_all_output_high(const char *i2cDevPath, uint8_t slaveAddr);
int max7300_set_all_io_level(const char *i2cDevPath, uint8_t slaveAddr, uint8_t level);
int max7300_set_all_io_direction(const char *i2cDevPath, uint8_t slaveAddr, uint8_t direction);
int max7300_set_single_io(const char *i2cDevPath, uint8_t slaveAddr, uint8_t port, uint8_t level);
int max7300_set_led_color(const char *i2cDevPath, uint8_t slaveAddr, uint8_t led, uint8_t color);

#endif
