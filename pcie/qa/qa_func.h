#ifndef __QA_FUNC_H__
#define __QA_FUNC_H__

#include <stdint.h>
#include "../common_func.h"
#define CARD_LOCAL_CH_NUM 2
#define SIGNAL_MODULE_NUM 16

// ============ XDMA-user space偏移地址 ============//
#define SYSTEM_CONFIG_BASE 0x0000 * 4
#define DAC1_CONFIG_BASE 0x0400 * 4
#define DAC2_CONFIG_BASE 0x0500 * 4

#define ADC1_CONFIG_BASE 0x0800 * 4
#define ADC2_CONFIG_BASE 0x0900 * 4

#define ADC1_SAMPLE_BASE 0x0C00 * 4
#define ADC2_SAMPLE_BASE 0x0C10 * 4

#define STATUS_BASE 0x1000 * 4
#define ADC1_RES_BASE 0x1200 * 4
#define ADC2_RES_BASE 0x1300 * 4



#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        E_TRIGGER_SOURCE_CTRL = C_USER_SPACE_CONFIG_OFFSET + 1 * 4,
        E_TRIGGER_SOURCE_SEL = C_USER_SPACE_CONFIG_OFFSET + 3 * 4,
    } QAUserSpaceReg_t;

    typedef struct
    {
        uint32_t m_dac_en[SIGNAL_MODULE_NUM];
        uint32_t m_dac_freq[SIGNAL_MODULE_NUM];
        uint32_t m_dac_phase[SIGNAL_MODULE_NUM];
        uint32_t m_dac_amp[SIGNAL_MODULE_NUM];
        uint32_t m_dac_data_len;
        uint32_t m_dac_play_times;
        uint32_t m_dac_play_delay;
        uint32_t m_dac_wait_calc_delay;
        uint32_t m_dac_play_mode;
        uint32_t m_dac_start_play;
    } DACConfigReg_t;

    typedef struct
    {
        uint32_t m_adc_en[SIGNAL_MODULE_NUM];
        uint32_t m_adc_freq[SIGNAL_MODULE_NUM];
        uint32_t m_adc_phase[SIGNAL_MODULE_NUM];
        uint32_t m_adc_data_len;
        uint32_t m_adc_play_times;
        uint32_t m_adc_play_delay;
        uint32_t m_adc_play_mode;
        uint32_t m_adc_start_play;
    } ADCConfigReg_t;

    typedef struct
    {
        uint32_t m_de_real[SIGNAL_MODULE_NUM];
        uint32_t m_de_imag[SIGNAL_MODULE_NUM];
        uint32_t m_de_value_l[SIGNAL_MODULE_NUM];
        uint32_t m_de_value_h[SIGNAL_MODULE_NUM];
        uint32_t m_de_result;
    } DEConfigReg_t;

    typedef struct
    {
        uint32_t m_sample_start;
        uint32_t m_sample_mode;
        uint32_t m_sample_length;
        uint32_t m_sample_delay;
        uint32_t m_sample_num;
    } SampleConfigReg_t;

    typedef struct
    {
        uint32_t m_decision_cnt[SIGNAL_MODULE_NUM];
    } decisionCntReg_t;

    typedef struct
    {
        uint32_t m_soft_trig;
        uint32_t m_trigger_mode;
        uint32_t m_trigger_source;
        DACConfigReg_t m_DAC_config_reg[CARD_LOCAL_CH_NUM];
        ADCConfigReg_t m_ADC_config_reg[CARD_LOCAL_CH_NUM];
        DEConfigReg_t m_DE_config_reg[CARD_LOCAL_CH_NUM];
        SampleConfigReg_t m_sample_config_reg[CARD_LOCAL_CH_NUM];
        decisionCntReg_t m_decision_cnt_reg[CARD_LOCAL_CH_NUM];
        uint32_t m_device_id;
        uint32_t m_adc_mem_status;  // adc采集状态
        uint32_t m_adc_corr_status; // adc解调状态
        uint32_t m_dac_play_status; // dac播放状态
        uint64_t m_dac_ram_base[CARD_LOCAL_CH_NUM];
        uint64_t m_adc_data_save_base[CARD_LOCAL_CH_NUM];
        uint64_t m_adc_decision_res_base[CARD_LOCAL_CH_NUM];
        uint64_t m_adc_ddr_base[CARD_LOCAL_CH_NUM];
    } QAUserReg_t;

    typedef struct
    {
        float m_DAC_freq;
        float m_DAC_Phase;
        float m_DAC_AMP;
    } DACDDSConfig_t;

    typedef struct
    {
        uint32_t m_data_len;
        uint32_t m_play_times;
        uint32_t m_play_delay;
        uint32_t m_wait_calc_delay;
        uint32_t m_play_mode;
    } DACDDSPlayParam_t;
    typedef struct
    {
        float m_adc_freq;
        float m_adc_phase;
    } ADCConfig_t;

    typedef struct
    {
        uint32_t m_adc_data_len;
        uint32_t m_adc_play_times;
        uint32_t m_adc_play_delay;
        uint32_t m_adc_play_mode;
    } ADCPlayParam_t;

    /// @brief 初始化QA卡件寄存器偏移
    void QAConfigRegisterInit(void);

    /// @brief 触发源设置(0:内触发;1:外触发;)
    void qa_trigger_source_ctrl(uint32_t logical_ch, uint32_t source);
    /// @brief dac dds配置
    void qa_dac_dds_config(uint32_t logical_ch, uint32_t index, DACDDSConfig_t config);
    /// @brief dac播放参数
    void qa_dac_play_param(uint32_t logical_ch, DACDDSPlayParam_t param);
    /// @brief dac启停
    void qa_dac_start_stop_play(uint32_t logical_ch, uint32_t en);
    /// @brief 采样参数配置
    void qa_sample_config(uint32_t logical_ch, SampleConfigReg_t config);
    /// @brief 采样启停
    void qa_sample_start_stop(uint32_t logical_ch, uint32_t en);
    /// @brief adc参数配置
    void qa_adc_config(uint32_t logical_ch, uint32_t index, ADCConfig_t config);
    /// @brief adc播放参数
    void qa_adc_play_param(uint32_t logical_ch, ADCPlayParam_t param);
    /// @brief adc启停
    void qa_adc_start_stop_play(uint32_t logical_ch, uint32_t en);
    /// @brief 回读采样数据
    int qa_get_sample_data(uint32_t logical_ch, uint64_t getLength, uint8_t *buffer);
    /// @brief 回读解调数据
    int qa_get_ddr_data(uint32_t logical_ch, uint64_t getLength, uint8_t *buffer);
    /// @brief 软触发
    void qa_soft_trig_signal(uint32_t logical_ch);

    int qa_get_sample_state(uint32_t logical_ch);
    int qa_get_demo_state(uint32_t logical_ch);

    void start_gain_trig();

    void qa_trig_source_init(void);

#ifdef __cplusplus
}
#endif
#endif // __QA_FUNC_H__
