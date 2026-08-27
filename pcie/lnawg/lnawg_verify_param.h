#ifndef __LNAWG_VERIFY_PARAM_H__
#define __LNAWG_VERIFY_PARAM_H__

#include <stddef.h>
#include <stdint.h>
#include "lnawg_func.h"
#include "../../device_info.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LNAWG_VERIFY_PARAM_DIR "/root/app/verify_param"
#define LNAWG_VERIFY_PARAM_FILE "lnawg_verify_param.csv"
#define LNAWG_VERIFY_PARAM_CHANNEL_COUNT (C_LNAWG_CHANNEL_NUM * CHIP_NUM)
#define LNAWG_VERIFY_PARAM_RANGE_COUNT 2U

typedef enum {
    LNAWG_VERIFY_RANGE_DIRECT = 0,
    LNAWG_VERIFY_RANGE_3V = 1
} LnawgVerifyRange;

typedef struct { double k; double b; } LnawgVerifyKB;
typedef struct {
    LnawgVerifyKB direct;
    LnawgVerifyKB range_3v;
} LnawgVerifyChannelKB;

/*
 * CSV format:
 * file-name,K-coefficient,B-coefficient
 * ch1_range0_any-text.csv,1.0,0.0
 * ch1_range1_any-text.csv,2.0,0.0
 *
 * Only the case-insensitive ch<n>_range<0|1> part of the first column is
 * interpreted. All text before/after it is ignored.
 */
/* Load the complete CSV into the internal cache once during program startup. */
int lnawg_verify_param_init(void);

int lnawg_verify_param_read_all(LnawgVerifyChannelKB *channels,
                                size_t channel_count);
int lnawg_verify_param_write_all(const LnawgVerifyChannelKB *channels,
                                 size_t channel_count);
int lnawg_verify_param_get_kb(uint32_t logical_ch, LnawgVerifyRange range,
                              double *k, double *b);
int lnawg_verify_param_set_kb(uint32_t logical_ch, LnawgVerifyRange range,
                              double k, double b);
int lnawg_verify_param_get_direct_kb(uint32_t logical_ch,
                                     double *k, double *b);
int lnawg_verify_param_get_3v_kb(uint32_t logical_ch,
                                 double *k, double *b);

#ifdef __cplusplus
}
#endif
#endif
