#include "led.h"

#include <stdio.h>
#include <fcntl.h>
#define LED_PATH "/sys/class/gpio/gpioxxx"

int write_led(int value) {
    FILE *fp = fopen(LED_PATH, "w");
    if (!fp) {
        perror("fopen LED");
        return -1;
    }
    fprintf(fp, "%d", value);
    fclose(fp);
    return 0;
}
