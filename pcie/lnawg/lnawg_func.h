#ifndef __LNAWG_FUNCTION__
#define __LNAWG_FUNCTION__

#include <stdio.h>
#include <stdint.h>

// 通道数
#define C_LNAWG_CH_DDS_NUM 4
#define C_LNAWG_CHANNEL_NUM 4

#define C_AWG_REG_BASE_ADDR 0x10000
#define C_AWG_CH_MODE_AWG 1
#define C_AWG_CH_MODE_DDS 2
#define C_AWG_CH_MODE_CHIRP_OUT 3
#define C_AWG_CH_MODE_PARAM_WAVE 4 // 参数化波形

#define C_SYSTEM_BASE_ADDR 0x000
#define C_DDS_BASE_ADDR 0x200
#define C_DAC_BASE_ADDR 0x600
#define C_FB_BASE_ADDR 0xA00


typedef enum
{
    E_RANGE_DIRECT = 0,
    E_RANGE_3V = 1,
    E_RANGE_HIGH_Z = 2,
    E_RANGE_GND = 3,
} eAWGChRangeContext_t;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _DEV_TRIG_CTRL
    {
        uint32_t trigger_source;   /* 0: int, 1: out */
        uint32_t trigger_us;       /* -> trig_period */
        uint32_t trigger_times;    /* -> trig_count */
        uint32_t trigger_continue; /* -> trig_continue */
        uint32_t trigger_delay;    /* -> trig_delay */
    } DevTrigCtrl;

    // save temp param to local
    typedef struct _LOCAL_PARAM
    {
        int32_t awgch_mode[8]; // 1:AWG 2:DDS
        DevTrigCtrl dev_trig_ctrl;
    } LocalParam;

    typedef struct
    {
        uint32_t m_index;
        uint32_t m_len;
        uint32_t m_trig_delay;
        double m_freq;
        double m_phase;
        double m_amp;
    } DDSConfigParam_t;

    typedef struct
    {
        uint32_t m_index;
        uint32_t m_len;
        uint32_t m_trig_delay;
        double m_freq_start;
        double m_freq_end;
        double m_phase;
        double m_amp;
    } ChirpOutParam_t;

    typedef struct
    {
        uint32_t m_en;
        uint32_t m_mode;
        uint32_t m_len_addr[C_LNAWG_CH_DDS_NUM];
        uint32_t m_trig_delay_addr[C_LNAWG_CH_DDS_NUM];
        uint32_t m_freq_addr[C_LNAWG_CH_DDS_NUM];
        uint32_t m_phase_addr[C_LNAWG_CH_DDS_NUM];
        uint32_t m_amp_addr[C_LNAWG_CH_DDS_NUM];
        uint32_t m_delt_x_addr[C_LNAWG_CH_DDS_NUM];
    } DDSAddrMap_t;

    typedef struct
    {
        uint32_t m_fb_en;
        uint32_t m_rx_idelay_tapout;
        uint32_t m_rx_fb_res;
        uint32_t m_rx_clear;
        uint32_t m_rx_idealy_tapin;
    } feedbackConfig_t;
    typedef struct
    {
        uint32_t m_device_id;
        uint32_t m_start_sync;
        uint32_t m_start_dac_config;
        uint32_t m_dac_output_rst;
        uint32_t m_temp;
        uint32_t m_ext_source;
        uint32_t m_ch_mode[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_en[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_seq_cnt[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_loop_cnt[C_LNAWG_CHANNEL_NUM];
        uint32_t m_switch_flag[C_LNAWG_CHANNEL_NUM];
        DDSAddrMap_t m_ch_dds_config[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_offset[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_range[C_LNAWG_CHANNEL_NUM];
        feedbackConfig_t m_fb_config[C_LNAWG_CHANNEL_NUM];
        uint32_t m_ch_wave_base_addr[C_LNAWG_CHANNEL_NUM];
    } AWGUserReg_t;

    LocalParam *get_local_param_instance();
    void AWGConfigRegisterInit(void);
    void set_awg_ch_run(int32_t logical_ch, int32_t state);
    uint32_t get_awg_ch_run(int32_t logical_ch);

    void set_awg_ch_mode(int32_t logical_ch, int32_t mode);
    uint32_t get_awg_ch_mode(int32_t logical_ch);

    void set_awg_ch_ext_src(int32_t logical_ch, int32_t source);
    uint32_t get_awg_ch_ext_src(int32_t logical_ch);

    void set_dev_trig(DevTrigCtrl devtrig);
    void set_dev_trig_state(int32_t state);
    void send_software_trig();

    void set_awg_ch_out_range(int32_t logical_ch, int32_t range);
    uint32_t get_awg_ch_out_range(int32_t logical_ch);

    void set_awg_ch_range(int32_t logical_ch, int32_t range);
    uint32_t get_awg_ch_range(int32_t logical_ch);
    void set_awg_ch_offset(int32_t logical_ch, double offset);
    double get_awg_ch_offset(int32_t logical_ch);

    void set_awg_ch_segment_count(int32_t logical_ch, int32_t segcnt);
    uint32_t get_awg_ch_segment_count(int32_t logical_ch);

    void set_awg_ch_segment_loop(int32_t logical_ch, int32_t segloop);
    uint32_t get_awg_ch_segment_loop(int32_t logical_ch);

    void set_awg_dds_config(int32_t logical_ch, DDSConfigParam_t config);
    DDSConfigParam_t get_awg_dds_config(int32_t logical_ch, uint32_t index);

    void lnawg_trig_source_init(void);

    void set_chirp_out_param(int32_t logical_ch, ChirpOutParam_t param);
    ChirpOutParam_t get_chirp_out_param(int32_t logical_ch, uint32_t index);

    void set_awg_dds_enable(int32_t logical_ch, uint32_t enable);
    uint32_t get_awg_dds_enable(int32_t logical_ch);

    void set_awg_feadback_enable(int32_t logical_ch, uint32_t enable);
    uint32_t get_awg_feadback_enable(int32_t logical_ch);

#ifdef __cplusplus
}
#endif

#endif // __LNAWG_FUNCTION__
