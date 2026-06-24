#ifndef LMK04828_DRV_H
#define LMK04828_DRV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
    #endif

    struct adireg
    {
        unsigned char addr_h;
        unsigned char addr_l;
        unsigned char value;
        unsigned char rev;
    };

    struct adfreg
    {
        unsigned char addr_h1;
        unsigned char addr_h;
        unsigned char addr_l;
        unsigned char value;
    };

    struct tireg
    {
        unsigned int addr_data;
    };

    int lmk04828_default_init(int type);
    int lmk04828_internel_init(int type);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LMK04828_DRV_H
