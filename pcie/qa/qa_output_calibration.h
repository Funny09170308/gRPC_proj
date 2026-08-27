#ifndef __QA_OUTPUT_CALIBRATION_H__
#define __QA_OUTPUT_CALIBRATION_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define QA_OUTPUT_CAL_DEFAULT_DIR "/root/app/verify_param"

/* User-facing power scale correction: old 8 dBm command measured 10 dBm. */
#define QA_OUTPUT_CAL_POWER_OFFSET_DB 2.0

    /*
     * Load the newest Baseband_Output_CH<n>*.csv for every output channel.
     * Call once during program startup, after the PCIe board has been detected.
     * Returns the number of successfully loaded channel tables (negative on a
     * directory-level error).
     */
    int qa_output_calibration_init(const char *directory);

    /* Release all tables. It is also safe to call before reloading the tables. */
    void qa_output_calibration_deinit(void);

    /* True when a usable table was loaded for this 1-based logical channel. */
    int qa_output_calibration_is_loaded(uint32_t logical_ch);

    /* Number of frequency/power points loaded for a 1-based logical channel. */
    size_t qa_output_calibration_point_count(uint32_t logical_ch);

    /*
     * Convert the user-facing actual output power to the hardware attenuation.
     * Frequency is in Hz and power is in dBm. Linear interpolation is used
     * between measured frequencies; values outside the table use the nearest
     * endpoint. The result is rounded to the attenuator's 0.25 dB step.
     */
    int qa_output_calibration_lookup(uint32_t logical_ch,
                                     double frequency_hz,
                                     double output_power_dbm,
                                     float *attenuation_db);

    /* Lookup and immediately program the QA DAC RF attenuator. */
    int qa_output_calibration_set(uint32_t logical_ch,
                                  double frequency_hz,
                                  double output_power_dbm,
                                  float *applied_attenuation_db);

    /*
     * Use this interface for CSV files measured with DAC RF attenuation
     * explicitly set to 0 dB. No legacy 2 dB power-scale offset is applied.
     */
    int qa_output_calibration_set_zero_atten_reference(
        uint32_t logical_ch,
        double frequency_hz,
        double output_power_dbm,
        float *applied_attenuation_db);

#ifdef __cplusplus
}
#endif

#endif /* __QA_OUTPUT_CALIBRATION_H__ */
