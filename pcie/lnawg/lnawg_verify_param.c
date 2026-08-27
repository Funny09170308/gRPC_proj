#include "lnawg_verify_param.h"
#include "../../platform_log/platform_log.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 4096U
#define CSV_COLS 3U
#define SOURCE_NAME_SIZE 256U

static LnawgVerifyChannelKB s_kb[LNAWG_VERIFY_PARAM_CHANNEL_COUNT];
static int s_valid[LNAWG_VERIFY_PARAM_CHANNEL_COUNT][LNAWG_VERIFY_PARAM_RANGE_COUNT];
static char s_source_name[LNAWG_VERIFY_PARAM_CHANNEL_COUNT]
                         [LNAWG_VERIFY_PARAM_RANGE_COUNT][SOURCE_NAME_SIZE];

static char *trim(char *s)
{
    char *end;
    while (isspace((unsigned char)*s))
        ++s;
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        --end;
    *end = '\0';
    return s;
}

static size_t split(char *line, char **fields, size_t capacity)
{
    size_t n = 0;
    char *p = line;
    while (n < capacity)
    {
        fields[n++] = trim(p);
        p = strchr(p, ',');
        if (p == NULL)
            break;
        *p++ = '\0';
    }
    return n;
}

static int parse_source_name(const char *text, size_t *ch, size_t *range)
{
    const char *p;
    const char *range_text;
    char *end;
    unsigned long ch_num, range_num;
    for (p = text; p[0] != '\0' && p[1] != '\0'; ++p)
    {
        const char range_tag[] = "_range";
        size_t tag_index;
        if (tolower((unsigned char)p[0]) != 'c' ||
            tolower((unsigned char)p[1]) != 'h')
            continue;
        errno = 0;
        ch_num = strtoul(p + 2, &end, 10);
        if (errno || end == p + 2 || ch_num < 1UL ||
            ch_num > (unsigned long)LNAWG_VERIFY_PARAM_CHANNEL_COUNT)
            continue;
        for (tag_index = 0U; range_tag[tag_index] != '\0'; ++tag_index)
            if (tolower((unsigned char)end[tag_index]) != range_tag[tag_index])
                break;
        if (range_tag[tag_index] != '\0')
            continue;
        errno = 0;
        range_text = end + tag_index;
        range_num = strtoul(range_text, &end, 10);
        if (errno || end == range_text ||
            range_num >= LNAWG_VERIFY_PARAM_RANGE_COUNT ||
            (*end != '\0' && *end != '_' && *end != '.'))
            continue;
        *ch = (size_t)(ch_num - 1UL);
        *range = (size_t)range_num;
        return 0;
    }
    return -1;
}

static int parse_double(const char *text, double *value)
{
    char *end;
    errno = 0;
    *value = strtod(text, &end);
    while (isspace((unsigned char)*end))
        ++end;
    return errno == 0 && end != text && *end == '\0' && isfinite(*value) ? 0 : -1;
}

static LnawgVerifyKB *at(LnawgVerifyChannelKB *channel, size_t range)
{
    return range == 0U ? &channel->direct : &channel->range_3v;
}

int lnawg_verify_param_init(void)
{
    LnawgVerifyChannelKB loaded[LNAWG_VERIFY_PARAM_CHANNEL_COUNT];
    int result = lnawg_verify_param_read_all(
        loaded, LNAWG_VERIFY_PARAM_CHANNEL_COUNT);
    if (result != 0)
        P_LOG_ERROR("LNAWG KB startup load failed, result=%d", result);
    else
        P_LOG_INFO("LNAWG KB coefficients are ready in memory");
    return result;
}

int lnawg_verify_param_read_all(LnawgVerifyChannelKB *channels, size_t count)
{
    const char path[] = LNAWG_VERIFY_PARAM_DIR "/" LNAWG_VERIFY_PARAM_FILE;
    FILE *fp;
    char line[LINE_SIZE];
    char *fields[CSV_COLS];
    LnawgVerifyChannelKB parsed[LNAWG_VERIFY_PARAM_CHANNEL_COUNT] = {0};
    char parsed_names[LNAWG_VERIFY_PARAM_CHANNEL_COUNT]
                     [LNAWG_VERIFY_PARAM_RANGE_COUNT][SOURCE_NAME_SIZE] = {{{0}}};
    int found[LNAWG_VERIFY_PARAM_CHANNEL_COUNT][LNAWG_VERIFY_PARAM_RANGE_COUNT] = {{0}};
    if (channels == NULL || count != LNAWG_VERIFY_PARAM_CHANNEL_COUNT)
        return -1;
    fp = fopen(path, "r");
    if (fp == NULL)
        return -2;
    /* The first row is a title row: file name, K coefficient, B coefficient. */
    if (!fgets(line, sizeof(line), fp))
    {
        fclose(fp);
        return -3;
    }
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        size_t ch, range;
        LnawgVerifyKB *item;
        if (trim(line)[0] == '\0')
            continue;
        if (split(line, fields, CSV_COLS) != CSV_COLS ||
            parse_source_name(fields[0], &ch, &range) != 0 || found[ch][range])
        {
            fclose(fp);
            return -3;
        }
        item = at(&parsed[ch], range);
        if (parse_double(fields[1], &item->k) || parse_double(fields[2], &item->b))
        {
            fclose(fp);
            return -3;
        }
        snprintf(parsed_names[ch][range], SOURCE_NAME_SIZE, "%s", fields[0]);
        found[ch][range] = 1;
    }
    fclose(fp);
    for (size_t ch = 0; ch < LNAWG_VERIFY_PARAM_CHANNEL_COUNT; ++ch)
        for (size_t range = 0; range < LNAWG_VERIFY_PARAM_RANGE_COUNT; ++range)
        {
            if (!found[ch][range])
                return -3;
            s_valid[ch][range] = 1;
        }
    memcpy(channels, parsed, sizeof(parsed));
    memcpy(s_kb, parsed, sizeof(parsed));
    memcpy(s_source_name, parsed_names, sizeof(parsed_names));
    P_LOG_INFO("Loaded LNAWG direct/3V KB coefficients from %s", path);
    return 0;
}

int lnawg_verify_param_write_all(const LnawgVerifyChannelKB *channels, size_t count)
{
    const char path[] = LNAWG_VERIFY_PARAM_DIR "/" LNAWG_VERIFY_PARAM_FILE;
    const char tmp[] = LNAWG_VERIFY_PARAM_DIR "/" LNAWG_VERIFY_PARAM_FILE ".tmp";
    FILE *fp;
    int failed;
    if (channels == NULL || count != LNAWG_VERIFY_PARAM_CHANNEL_COUNT)
        return -1;
    for (size_t ch = 0; ch < count; ++ch)
        if (!isfinite(channels[ch].direct.k) || !isfinite(channels[ch].direct.b) ||
            !isfinite(channels[ch].range_3v.k) || !isfinite(channels[ch].range_3v.b))
            return -1;
    fp = fopen(tmp, "w");
    if (fp == NULL)
        return -2;
    fprintf(fp, "文件名,K系数,B系数\n");
    for (size_t ch = 0; ch < count; ++ch)
    {
        for (size_t range = 0; range < LNAWG_VERIFY_PARAM_RANGE_COUNT; ++range)
        {
            const LnawgVerifyKB *item = range == 0U ? &channels[ch].direct
                                                    : &channels[ch].range_3v;
            if (s_source_name[ch][range][0] != '\0')
                fprintf(fp, "%s", s_source_name[ch][range]);
            else
                fprintf(fp, "ch%u_range%u", (unsigned)(ch + 1U), (unsigned)range);
            fprintf(fp, ",%.17g,%.17g\n", item->k, item->b);
        }
    }
    failed = ferror(fp) != 0;
    if (fflush(fp) != 0)
        failed = 1;
    if (fclose(fp) != 0)
        failed = 1;
    if (failed)
    {
        remove(tmp);
        return -2;
    }
    if (rename(tmp, path) != 0)
    {
        remove(tmp);
        return -2;
    }
    memcpy(s_kb, channels, sizeof(s_kb));
    for (size_t ch = 0; ch < LNAWG_VERIFY_PARAM_CHANNEL_COUNT; ++ch)
        for (size_t range = 0; range < LNAWG_VERIFY_PARAM_RANGE_COUNT; ++range)
            s_valid[ch][range] = 1;
    return 0;
}

int lnawg_verify_param_get_kb(uint32_t logical_ch, LnawgVerifyRange range,
                              double *k, double *b)
{
    size_t ch;
    LnawgVerifyKB *item;
    if (logical_ch < 1U || logical_ch > LNAWG_VERIFY_PARAM_CHANNEL_COUNT ||
        (range != LNAWG_VERIFY_RANGE_DIRECT && range != LNAWG_VERIFY_RANGE_3V) ||
        k == NULL || b == NULL)
        return -1;
    ch = logical_ch - 1U;
    if (!s_valid[ch][range])
        return -2;
    item = at(&s_kb[ch], (size_t)range);
    *k = item->k;
    *b = item->b;
    return 0;
}

int lnawg_verify_param_set_kb(uint32_t logical_ch, LnawgVerifyRange range,
                              double k, double b)
{
    size_t ch;
    LnawgVerifyKB *item;
    if (logical_ch < 1U || logical_ch > LNAWG_VERIFY_PARAM_CHANNEL_COUNT ||
        (range != LNAWG_VERIFY_RANGE_DIRECT && range != LNAWG_VERIFY_RANGE_3V) ||
        !isfinite(k) || !isfinite(b))
        return -1;
    ch = logical_ch - 1U;
    item = at(&s_kb[ch], (size_t)range);
    item->k = k;
    item->b = b;
    s_valid[ch][range] = 1;
    return 0;
}

int lnawg_verify_param_get_direct_kb(uint32_t ch, double *k, double *b)
{
    return lnawg_verify_param_get_kb(ch, LNAWG_VERIFY_RANGE_DIRECT, k, b);
}

int lnawg_verify_param_get_3v_kb(uint32_t ch, double *k, double *b)
{
    return lnawg_verify_param_get_kb(ch, LNAWG_VERIFY_RANGE_3V, k, b);
}
