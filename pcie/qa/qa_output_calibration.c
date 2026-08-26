#include "qa_output_calibration.h"

#include "qa_func.h"
#include "../pcie_func.h"
#include "../rf/rf_ctrl.h"
#include "../../lib/include/platform_log/platform_log.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define QA_CAL_MAX_CHANNELS 64U
#define QA_CAL_LINE_SIZE 4096U

typedef struct
{
    double frequency_mhz;
    double measured_power_dbm;
} QAOutputCalPoint;

typedef struct
{
    QAOutputCalPoint *points;
    size_t count;
    char source_file[512];
} QAOutputCalTable;

static QAOutputCalTable s_tables[QA_CAL_MAX_CHANNELS];

static int case_equal(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        if (tolower((unsigned char)*left++) != tolower((unsigned char)*right++))
            return 0;
    }
    return *left == '\0' && *right == '\0';
}

static char *trim(char *text)
{
    char *end;
    while (isspace((unsigned char)*text))
        ++text;
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]))
        --end;
    *end = '\0';
    return text;
}

static int split_csv(char *line, char **fields, int capacity)
{
    int count = 0;
    char *cursor = line;
    while (count < capacity)
    {
        fields[count++] = trim(cursor);
        cursor = strchr(cursor, ',');
        if (cursor == NULL)
            break;
        *cursor++ = '\0';
    }
    return count;
}

static int point_compare(const void *left, const void *right)
{
    const QAOutputCalPoint *a = (const QAOutputCalPoint *)left;
    const QAOutputCalPoint *b = (const QAOutputCalPoint *)right;
    return (a->frequency_mhz > b->frequency_mhz) -
           (a->frequency_mhz < b->frequency_mhz);
}

static int parse_channel(const char *name)
{
    const char prefix[] = "Baseband_Output_CH";
    const char *p;
    char *end;
    long channel;
    if (strncmp(name, prefix, sizeof(prefix) - 1U) != 0)
        return -1;
    /* *_Cal_* is an already calibrated verification result, not raw input. */
    if (strstr(name, "_Cal_") != NULL)
        return -1;
    p = name + sizeof(prefix) - 1U;
    errno = 0;
    channel = strtol(p, &end, 10);
    if (errno != 0 || end == p || channel < 1 || channel > QA_CAL_MAX_CHANNELS)
        return -1;
    if (strstr(end, ".csv") == NULL)
        return -1;
    return (int)channel;
}

static int load_table(const char *path, QAOutputCalTable *table)
{
    FILE *file = fopen(path, "r");
    char line[QA_CAL_LINE_SIZE];
    char *fields[64];
    int field_count;
    int frequency_col = -1;
    int power_col = -1;
    size_t capacity = 0;
    QAOutputCalPoint *points = NULL;

    if (file == NULL)
        return -1;
    if (fgets(line, sizeof(line), file) == NULL)
        goto fail;
    field_count = split_csv(line, fields, 64);
    for (int i = 0; i < field_count; ++i)
    {
        /* Match the columns actually used by power_calibration.py. */
        if (frequency_col < 0 && case_equal(fields[i], "target_freq_mhz"))
            frequency_col = i;
        if (power_col < 0 && case_equal(fields[i], "measured_power_dbm"))
            power_col = i;
    }
    /* Also accept a compact two-column measurement file. */
    for (int i = 0; i < field_count; ++i)
    {
        if (frequency_col < 0 && strstr(fields[i], "freq") != NULL)
            frequency_col = i;
        if (power_col < 0 && strstr(fields[i], "power") != NULL)
            power_col = i;
    }
    if (frequency_col < 0 || power_col < 0)
        goto fail;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *freq_end;
        char *power_end;
        double frequency;
        double power;
        field_count = split_csv(line, fields, 64);
        if (frequency_col >= field_count || power_col >= field_count)
            continue;
        errno = 0;
        frequency = strtod(fields[frequency_col], &freq_end);
        power = strtod(fields[power_col], &power_end);
        if (errno != 0 || freq_end == fields[frequency_col] ||
            power_end == fields[power_col] || !isfinite(frequency) ||
            !isfinite(power))
            continue;
        if (table->count == capacity)
        {
            size_t new_capacity = capacity == 0 ? 128U : capacity * 2U;
            QAOutputCalPoint *resized = (QAOutputCalPoint *)realloc(
                points, new_capacity * sizeof(*points));
            if (resized == NULL)
                goto fail;
            points = resized;
            capacity = new_capacity;
        }
        points[table->count].frequency_mhz = frequency;
        points[table->count].measured_power_dbm = power;
        ++table->count;
    }
    fclose(file);
    if (table->count == 0)
    {
        free(points);
        return -1;
    }
    qsort(points, table->count, sizeof(*points), point_compare);
    table->points = points;
    snprintf(table->source_file, sizeof(table->source_file), "%s", path);
    return 0;

fail:
    fclose(file);
    free(points);
    table->count = 0;
    return -1;
}

void qa_output_calibration_deinit(void)
{
    for (size_t i = 0; i < QA_CAL_MAX_CHANNELS; ++i)
    {
        free(s_tables[i].points);
        memset(&s_tables[i], 0, sizeof(s_tables[i]));
    }
}

int qa_output_calibration_init(const char *directory)
{
    DIR *dir;
    struct dirent *entry;
    char selected[QA_CAL_MAX_CHANNELS][512] = {{0}};
    time_t selected_time[QA_CAL_MAX_CHANNELS] = {0};
    int loaded = 0;

    qa_output_calibration_deinit();
    if (directory == NULL || directory[0] == '\0')
        directory = QA_OUTPUT_CAL_DEFAULT_DIR;
    dir = opendir(directory);
    if (dir == NULL)
    {
        P_LOG_WARNING("QA output calibration directory not available: %s", directory);
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        int channel = parse_channel(entry->d_name);
        char path[512];
        struct stat status;
        size_t index;
        if (channel < 1)
            continue;
        index = (size_t)(channel - 1);
        if (snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name) >=
            (int)sizeof(path))
            continue;
        if (stat(path, &status) != 0 || !S_ISREG(status.st_mode))
            continue;
        if (selected[index][0] == '\0' || status.st_mtime > selected_time[index])
        {
            snprintf(selected[index], sizeof(selected[index]), "%s", path);
            selected_time[index] = status.st_mtime;
        }
    }
    closedir(dir);

    for (size_t i = 0; i < QA_CAL_MAX_CHANNELS; ++i)
    {
        if (selected[i][0] == '\0')
            continue;
        if (load_table(selected[i], &s_tables[i]) == 0)
        {
            ++loaded;
            P_LOG_INFO("Loaded QA OUT%u calibration: %u points from %s",
                       (unsigned)(i + 1U), (unsigned)s_tables[i].count,
                       s_tables[i].source_file);
        }
        else
        {
            P_LOG_ERROR("Invalid QA output calibration CSV: %s", selected[i]);
        }
    }
    return loaded;
}

int qa_output_calibration_is_loaded(uint32_t logical_ch)
{
    return logical_ch >= 1U && logical_ch <= QA_CAL_MAX_CHANNELS &&
           s_tables[logical_ch - 1U].count > 0U;
}

size_t qa_output_calibration_point_count(uint32_t logical_ch)
{
    return qa_output_calibration_is_loaded(logical_ch)
               ? s_tables[logical_ch - 1U].count
               : 0U;
}

int qa_output_calibration_lookup(uint32_t logical_ch,
                                 double frequency_hz,
                                 double output_power_dbm,
                                 float *attenuation_db)
{
    QAOutputCalTable *table;
    double frequency_mhz;
    double calibrated_power;
    double attenuation;
    size_t low;
    size_t high;

    if (attenuation_db == NULL || !isfinite(frequency_hz) ||
        !isfinite(output_power_dbm) || frequency_hz < 0.0)
        return -1;
    if (!qa_output_calibration_is_loaded(logical_ch))
        return -2;
    table = &s_tables[logical_ch - 1U];
    frequency_mhz = frequency_hz / 1.0e6;
    if (frequency_mhz <= table->points[0].frequency_mhz)
        calibrated_power = table->points[0].measured_power_dbm;
    else if (frequency_mhz >= table->points[table->count - 1U].frequency_mhz)
        calibrated_power = table->points[table->count - 1U].measured_power_dbm;
    else
    {
        low = 0U;
        high = table->count - 1U;
        while (high - low > 1U)
        {
            size_t middle = low + (high - low) / 2U;
            if (table->points[middle].frequency_mhz <= frequency_mhz)
                low = middle;
            else
                high = middle;
        }
        if (table->points[high].frequency_mhz == table->points[low].frequency_mhz)
            calibrated_power = table->points[low].measured_power_dbm;
        else
        {
            double ratio = (frequency_mhz - table->points[low].frequency_mhz) /
                           (table->points[high].frequency_mhz -
                            table->points[low].frequency_mhz);
            calibrated_power = table->points[low].measured_power_dbm +
                               ratio * (table->points[high].measured_power_dbm -
                                        table->points[low].measured_power_dbm);
        }
    }

    /* Actual output = calibrated zero-attenuation power - attenuation. */
    attenuation = calibrated_power - output_power_dbm;
    attenuation = floor(attenuation / C_ATTEN_STEP + 0.5) * C_ATTEN_STEP;
    if (attenuation < 0.0)
        return -3; /* Requested power is higher than this channel can produce. */
    if (attenuation > C_STAGE_ATTEN_SET_MAX_VAL * C_ATTEN_STEP)
        return -4; /* DAC output path has one 31.75 dB attenuation stage. */
    *attenuation_db = (float)attenuation;
    return 0;
}

int qa_output_calibration_set(uint32_t logical_ch,
                              double frequency_hz,
                              double output_power_dbm,
                              float *applied_attenuation_db)
{
    float attenuation;
    int result = qa_output_calibration_lookup(logical_ch, frequency_hz,
                                              output_power_dbm, &attenuation);
    P_LOG_DEBUG("qa_output_calibration_set: logical_ch=%u, frequency_hz=%f, output_power_dbm=%f, attenuation=%f, result=%d",
                logical_ch, frequency_hz, output_power_dbm, attenuation, result);
    if (result != 0)
        return result;
    qa_set_rf_da_atten(logical_ch, attenuation);
    if (applied_attenuation_db != NULL)
        *applied_attenuation_db = attenuation;
    return 0;
}
