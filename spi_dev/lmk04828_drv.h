#ifndef LMK04828_DRV_H
#define LMK04828_DRV_H

int lmk04828_reario_init();
int lmk04828_awg_init();

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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LMK04828_DRV_H
