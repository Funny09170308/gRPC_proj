#ifndef __LNAWG_FUNCTION__
#define __LNAWG_FUNCTION__

#include <stdio.h>
#include <stdint.h>

// 通道数
#define LNAWG_CH_DDS_NUM 4
#define LNAWG_CHANNEL_NUM 2

#define AWG_REG_BASE_ADDR 0x10000
#define AWG_CH_MODE_AWG 1
#define AWG_CH_MODE_DDS 2
#define AWG_CH_MODE_CHIRP_OUT 3

#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum
    {
        CHANNEL_1_MODE = AWG_REG_BASE_ADDR + (1 * 4),
        CHANNEL_0_MODE = AWG_REG_BASE_ADDR + (2 * 4),

        CHANNEL_1_OUTPUT_EN = AWG_REG_BASE_ADDR + (3 * 4),
        CHANNEL_0_OUTPUT_EN = AWG_REG_BASE_ADDR + (4 * 4),

        CHANNEL_1_SEQUENCE_CNT = AWG_REG_BASE_ADDR + (5 * 4),
        CHANNEL_0_SEQUENCE_CNT = AWG_REG_BASE_ADDR + (6 * 4),

        CHANNEL_1_LOOP_CNT = AWG_REG_BASE_ADDR + (7 * 4),
        CHANNEL_0_LOOP_CNT = AWG_REG_BASE_ADDR + (8 * 4),

        DDS_1_EN = AWG_REG_BASE_ADDR + (9 * 4),
        DDS_1_PART_1_FREQUENCE = AWG_REG_BASE_ADDR + (10 * 4),
        DDS_1_PART_2_FREQUENCE = AWG_REG_BASE_ADDR + (11 * 4),
        DDS_1_PART_3_FREQUENCE = AWG_REG_BASE_ADDR + (12 * 4),
        DDS_1_PART_4_FREQUENCE = AWG_REG_BASE_ADDR + (13 * 4),

        DDS_1_PART_1_PHASE = AWG_REG_BASE_ADDR + (14 * 4),
        DDS_1_PART_2_PHASE = AWG_REG_BASE_ADDR + (15 * 4),
        DDS_1_PART_3_PHASE = AWG_REG_BASE_ADDR + (16 * 4),
        DDS_1_PART_4_PHASE = AWG_REG_BASE_ADDR + (17 * 4),

        DDS_1_PART_1_AMP = AWG_REG_BASE_ADDR + (18 * 4),
        DDS_1_PART_2_AMP = AWG_REG_BASE_ADDR + (19 * 4),
        DDS_1_PART_3_AMP = AWG_REG_BASE_ADDR + (20 * 4),
        DDS_1_PART_4_AMP = AWG_REG_BASE_ADDR + (21 * 4),

        DDS_0_EN = AWG_REG_BASE_ADDR + (22 * 4),
        DDS_0_PART_1_FREQUENCE = AWG_REG_BASE_ADDR + (23 * 4),
        DDS_0_PART_2_FREQUENCE = AWG_REG_BASE_ADDR + (24 * 4),
        DDS_0_PART_3_FREQUENCE = AWG_REG_BASE_ADDR + (25 * 4),
        DDS_0_PART_4_FREQUENCE = AWG_REG_BASE_ADDR + (26 * 4),

        DDS_0_PART_1_PHASE = AWG_REG_BASE_ADDR + (27 * 4),
        DDS_0_PART_2_PHASE = AWG_REG_BASE_ADDR + (28 * 4),
        DDS_0_PART_3_PHASE = AWG_REG_BASE_ADDR + (29 * 4),
        DDS_0_PART_4_PHASE = AWG_REG_BASE_ADDR + (30 * 4),

        DDS_0_PART_1_AMP = AWG_REG_BASE_ADDR + (31 * 4),
        DDS_0_PART_2_AMP = AWG_REG_BASE_ADDR + (32 * 4),
        DDS_0_PART_3_AMP = AWG_REG_BASE_ADDR + (33 * 4),
        DDS_0_PART_4_AMP = AWG_REG_BASE_ADDR + (34 * 4),

        CHANNEL_1_OFFSET_SET = AWG_REG_BASE_ADDR + (35 * 4),
        CHANNEL_0_OFFSET_SET = AWG_REG_BASE_ADDR + (36 * 4),

        CHANNEL_1_RANGE_SET = AWG_REG_BASE_ADDR + (38 * 4),
        CHANNEL_0_RANGE_SET = AWG_REG_BASE_ADDR + (39 * 4),

        DDS_1_PART_1_LEN = AWG_REG_BASE_ADDR + (40 * 4),
        DDS_1_PART_2_LEN = AWG_REG_BASE_ADDR + (41 * 4),
        DDS_1_PART_3_LEN = AWG_REG_BASE_ADDR + (42 * 4),
        DDS_1_PART_4_LEN = AWG_REG_BASE_ADDR + (43 * 4),

        DDS_1_PART_1_TRIG_DELAY = AWG_REG_BASE_ADDR + (44 * 4),
        DDS_1_PART_2_TRIG_DELAY = AWG_REG_BASE_ADDR + (45 * 4),
        DDS_1_PART_3_TRIG_DELAY = AWG_REG_BASE_ADDR + (46 * 4),
        DDS_1_PART_4_TRIG_DELAY = AWG_REG_BASE_ADDR + (47 * 4),

        DDS_0_PART_1_LEN = AWG_REG_BASE_ADDR + (48 * 4),
        DDS_0_PART_2_LEN = AWG_REG_BASE_ADDR + (49 * 4),
        DDS_0_PART_3_LEN = AWG_REG_BASE_ADDR + (50 * 4),
        DDS_0_PART_4_LEN = AWG_REG_BASE_ADDR + (51 * 4),

        DDS_0_PART_1_TRIG_DELAY = AWG_REG_BASE_ADDR + (52 * 4),
        DDS_0_PART_2_TRIG_DELAY = AWG_REG_BASE_ADDR + (53 * 4),
        DDS_0_PART_3_TRIG_DELAY = AWG_REG_BASE_ADDR + (54 * 4),
        DDS_0_PART_4_TRIG_DELAY = AWG_REG_BASE_ADDR + (55 * 4),

        CHANNEL_0AND1_EXT_SOURCE = AWG_REG_BASE_ADDR + (56 * 4),

        DDS_1_PART_1_DELT_X = AWG_REG_BASE_ADDR + (57 * 4),
        DDS_1_PART_2_DELT_X = AWG_REG_BASE_ADDR + (58 * 4),
        DDS_1_PART_3_DELT_X = AWG_REG_BASE_ADDR + (59 * 4),
        DDS_1_PART_4_DELT_X = AWG_REG_BASE_ADDR + (60 * 4),

        DDS_0_PART_1_DELT_X = AWG_REG_BASE_ADDR + (61 * 4),
        DDS_0_PART_2_DELT_X = AWG_REG_BASE_ADDR + (62 * 4),
        DDS_0_PART_3_DELT_X = AWG_REG_BASE_ADDR + (63 * 4),
        DDS_0_PART_4_DELT_X = AWG_REG_BASE_ADDR + (64 * 4),

    } AWGConfigRegOffset_t;

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
        uint32_t m_len_addr[LNAWG_CH_DDS_NUM];
        uint32_t m_trig_delay_addr[LNAWG_CH_DDS_NUM];
        uint32_t m_freq_addr[LNAWG_CH_DDS_NUM];
        uint32_t m_phase_addr[LNAWG_CH_DDS_NUM];
        uint32_t m_amp_addr[LNAWG_CH_DDS_NUM];
        uint32_t m_delt_x_addr[LNAWG_CH_DDS_NUM];
    } DDSAddrMap_t;

    LocalParam *get_local_param_instance();

    void set_awg_ch_run(int32_t logical_ch, int32_t state);
    uint32_t get_awg_ch_run(int32_t logical_ch);

    void set_awg_ch_mode(int32_t logical_ch, int32_t mode);
    uint32_t get_awg_ch_mode(int32_t logical_ch);

    void set_awg_ch_ext_src(int32_t logical_ch, int32_t source);
    uint32_t get_awg_ch_ext_src(int32_t logical_ch);

    void set_dev_trig(DevTrigCtrl devtrig);
    void set_dev_trig_state(int32_t state);
    void send_software_trig();

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
    ChirpOutParam_t get_chirp_out_param(int32_t logical_ch);

    void set_awg_dds_enable(int32_t logical_ch, uint32_t enable);
    uint32_t get_awg_dds_enable(int32_t logical_ch);

#ifdef __cplusplus
}
#endif

#endif // __LNAWG_FUNCTION__
