#include "qa_func.h"
#include "../pcie_func.h"
#include "../../device_info.h"
#include "../../i2c/io_expand/max7300.h"
#include "../../platform_log/platform_log.h"
#include <unistd.h>

static QAUserReg_t s_QAUserRegs;

void QAConfigRegisterInit(void)
{
    s_QAUserRegs.m_soft_trig = C_USER_SPACE_CONFIG_OFFSET + 0 * 4;
    s_QAUserRegs.m_trigger_mode = C_USER_SPACE_CONFIG_OFFSET + 1 * 4;
    s_QAUserRegs.m_trigger_source = C_USER_SPACE_CONFIG_OFFSET + 3 * 4;
    uint8_t index = 0;
    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_DAC_config_reg[0].m_dac_en[index] = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[0].m_dac_freq[index] = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + (0x0002 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[0].m_dac_phase[index] = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + (0x0004 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[0].m_dac_amp[index] = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + (0x0005 + index * 0x10) * 4;
    }
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_data_len = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x0006 * 4;
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_play_times = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x0007 * 4;
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_play_delay = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x0008 * 4;
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_wait_calc_delay = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x0009 * 4;
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_play_mode = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x000A * 4;
    s_QAUserRegs.m_DAC_config_reg[0].m_dac_start_play = C_USER_SPACE_CONFIG_OFFSET + DAC1_CONFIG_BASE + 0x000B * 4;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_DAC_config_reg[1].m_dac_en[index] = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[1].m_dac_freq[index] = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + (0x0002 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[1].m_dac_phase[index] = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + (0x0004 + index * 0x10) * 4;
        s_QAUserRegs.m_DAC_config_reg[1].m_dac_amp[index] = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + (0x0005 + index * 0x10) * 4;
    }
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_data_len = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x0006 * 4;
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_play_times = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x0007 * 4;
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_play_delay = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x0008 * 4;
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_wait_calc_delay = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x0009 * 4;
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_play_mode = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x000A * 4;
    s_QAUserRegs.m_DAC_config_reg[1].m_dac_start_play = C_USER_SPACE_CONFIG_OFFSET + DAC2_CONFIG_BASE + 0x000B * 4;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_ADC_config_reg[0].m_adc_en[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_ADC_config_reg[0].m_adc_freq[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + (0x0001 + index * 0x10) * 4;
        s_QAUserRegs.m_ADC_config_reg[0].m_adc_phase[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + (0x0004 + index * 0x10) * 4;
    }

    s_QAUserRegs.m_ADC_config_reg[0].m_adc_data_len = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + 0x0005 * 4;
    s_QAUserRegs.m_ADC_config_reg[0].m_adc_play_times = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + 0x0006 * 4;
    s_QAUserRegs.m_ADC_config_reg[0].m_adc_play_delay = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + 0x0007 * 4;
    s_QAUserRegs.m_ADC_config_reg[0].m_adc_play_mode = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + 0x0008 * 4;
    s_QAUserRegs.m_ADC_config_reg[0].m_adc_start_play = C_USER_SPACE_CONFIG_OFFSET + ADC1_CONFIG_BASE + 0x0009 * 4;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_ADC_config_reg[1].m_adc_en[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_ADC_config_reg[1].m_adc_freq[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + (0x0001 + index * 0x10) * 4;
        s_QAUserRegs.m_ADC_config_reg[1].m_adc_phase[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + (0x0004 + index * 0x10) * 4;
    }

    s_QAUserRegs.m_ADC_config_reg[1].m_adc_data_len = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + 0x0005 * 4;
    s_QAUserRegs.m_ADC_config_reg[1].m_adc_play_times = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + 0x0006 * 4;
    s_QAUserRegs.m_ADC_config_reg[1].m_adc_play_delay = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + 0x0007 * 4;
    s_QAUserRegs.m_ADC_config_reg[1].m_adc_play_mode = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + 0x0008 * 4;
    s_QAUserRegs.m_ADC_config_reg[1].m_adc_start_play = C_USER_SPACE_CONFIG_OFFSET + ADC2_CONFIG_BASE + 0x0009 * 4;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_DE_config_reg[0].m_de_real[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[0].m_de_imag[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + (0x0001 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[0].m_de_value_l[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + (0x0002 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[0].m_de_value_h[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + (0x0003 + index * 0x10) * 4;
    }
    s_QAUserRegs.m_DE_config_reg[0].m_de_result = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + 0x0004;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_DE_config_reg[1].m_de_real[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + (0x0000 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[1].m_de_imag[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + (0x0001 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[1].m_de_value_l[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + (0x0002 + index * 0x10) * 4;
        s_QAUserRegs.m_DE_config_reg[1].m_de_value_h[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + (0x0003 + index * 0x10) * 4;
    }
    s_QAUserRegs.m_DE_config_reg[1].m_de_result = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + 0x0004 * 4;

    s_QAUserRegs.m_sample_config_reg[0].m_sample_start = C_USER_SPACE_CONFIG_OFFSET + ADC1_SAMPLE_BASE + 0x0000 * 4;
    s_QAUserRegs.m_sample_config_reg[0].m_sample_mode = C_USER_SPACE_CONFIG_OFFSET + ADC1_SAMPLE_BASE + 0x0001 * 4;
    s_QAUserRegs.m_sample_config_reg[0].m_sample_length = C_USER_SPACE_CONFIG_OFFSET + ADC1_SAMPLE_BASE + 0x0002 * 4;
    s_QAUserRegs.m_sample_config_reg[0].m_sample_delay = C_USER_SPACE_CONFIG_OFFSET + ADC1_SAMPLE_BASE + 0x0003 * 4;
    s_QAUserRegs.m_sample_config_reg[0].m_sample_num = C_USER_SPACE_CONFIG_OFFSET + ADC1_SAMPLE_BASE + 0x0004 * 4;

    s_QAUserRegs.m_sample_config_reg[1].m_sample_start = C_USER_SPACE_CONFIG_OFFSET + ADC2_SAMPLE_BASE + 0x0000 * 4;
    s_QAUserRegs.m_sample_config_reg[1].m_sample_mode = C_USER_SPACE_CONFIG_OFFSET + ADC2_SAMPLE_BASE + 0x0001 * 4;
    s_QAUserRegs.m_sample_config_reg[1].m_sample_length = C_USER_SPACE_CONFIG_OFFSET + ADC2_SAMPLE_BASE + 0x0002 * 4;
    s_QAUserRegs.m_sample_config_reg[1].m_sample_delay = C_USER_SPACE_CONFIG_OFFSET + ADC2_SAMPLE_BASE + 0x0003 * 4;
    s_QAUserRegs.m_sample_config_reg[1].m_sample_num = C_USER_SPACE_CONFIG_OFFSET + ADC2_SAMPLE_BASE + 0x0004 * 4;

    for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
    {
        s_QAUserRegs.m_decision_cnt_reg[0].m_decision_cnt[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + index * 4;
        s_QAUserRegs.m_decision_cnt_reg[1].m_decision_cnt[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + index * 4;
    }

    s_QAUserRegs.m_device_id = C_USER_SPACE_CONFIG_OFFSET + SYSTEM_CONFIG_BASE + 0x0000 * 4;
    s_QAUserRegs.m_adc_mem_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0000 * 4;
    s_QAUserRegs.m_adc_corr_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0001 * 4;
    s_QAUserRegs.m_dac_play_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0002 * 4;

    s_QAUserRegs.m_adc_data_save_base[0] = 0x1cc2000000;
    s_QAUserRegs.m_adc_data_save_base[1] = 0x1cc6000000;
    s_QAUserRegs.m_adc_decision_res_base[0] = 0x1cc4000000;
    s_QAUserRegs.m_adc_decision_res_base[1] = 0x1cc8000000;
    s_QAUserRegs.m_dac_ram_base[0] = 0x1cc0000000;
    s_QAUserRegs.m_dac_ram_base[1] = 0x1cca000000;
    s_QAUserRegs.m_adc_ddr_base[0] = 0x1c80000000;
    s_QAUserRegs.m_adc_ddr_base[1] = 0x1cA0000000;
}

void qa_trigger_source_ctrl(uint32_t logical_ch, uint32_t source)
{
    uint8_t chip_id, local_ch;
    get_qa_out_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    source = (source == 1) ? 1 : 0;
    P_LOG_DEBUG("QA: set trigger: %s", (source) ? "OUT" : "IN");
    if (1 == source) // 外部触发
    {
        xdma_write_user_space(chip_id, s_QAUserRegs.m_trigger_mode, 0);
        xdma_write_user_space(chip_id, s_QAUserRegs.m_trigger_source, 1);
    }
    else // 软触发
    {
        xdma_write_user_space(chip_id, s_QAUserRegs.m_trigger_mode, 0);
        xdma_write_user_space(chip_id, s_QAUserRegs.m_trigger_source, 0);
    }
}

int dac_localch_to_phych(uint32_t localch)
{
    int phych;

    //[CH3]:FPGA-CH1 [CH2]:FPGA-CH2
    if (localch == 3)
    {
        phych = 0;
    }
    else if (localch == 2)
    {
        phych = 1;
    }
    else
    {
        P_LOG_ERROR("no suppurt ch: %d\r\n", localch);
    }

    return phych;
}

int adc_localch_to_phych(uint32_t localch)
{
    int phych;

    //[CH4]:FPGA-CH1 [CH1]:FPGA-CH2
    if (localch == 4)
    {
        phych = 0;
    }
    else if (localch == 1)
    {
        phych = 1;
    }
    else
    {
        P_LOG_ERROR("no suppurt ch: %d\r\n", localch);
    }

    return phych;
}

void qa_dac_start_stop_play(uint32_t logical_ch, uint32_t en)
{
    uint8_t chip_id, local_ch;
    get_qa_out_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
    int phych;
    P_LOG_DEBUG("QA: Set chip: %d dac ch: %d enable status: %d", chip_id, local_ch, en);

    phych = dac_localch_to_phych(local_ch);

    en = (en == 1) ? 1 : 0;
    if (en)
    {
        set_qa_ch_led_status(logical_ch, LED_GREEN);
    }
    else
    {
        set_qa_ch_led_status(logical_ch, LED_OFF);
    }
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_start_play, en);
}

void qa_dac_dds_config(uint32_t logical_ch, uint32_t index, DACDDSConfig_t config)
{
    uint8_t chip_id, local_ch;
    get_qa_out_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    uint32_t freq_word, phase_word, amp_word, phych;
    P_LOG_DEBUG("QA: Set chip: %d dac ch: %d dds config.", chip_id, local_ch);

    qa_dac_start_stop_play(logical_ch, 0); // 停止输出

    freq_word = (uint32_t)(config.m_DAC_freq / 4000e6 * pow(2, 25));
    phase_word = (uint32_t)(config.m_DAC_Phase / 180 * pow(2, 25));
    amp_word = (uint32_t)(config.m_DAC_AMP * pow(2, 32));

    P_LOG_DEBUG("freq: %d value: %d(%#x) write to dds index: %d", config.m_DAC_freq, freq_word, freq_word, index);
    P_LOG_DEBUG("phase: %d value: %d(%#x) write to dds index: %d", config.m_DAC_Phase, phase_word, phase_word, index);
    P_LOG_DEBUG("amp: %d value: %d(%#x) write to dds index: %d", config.m_DAC_AMP, amp_word, amp_word, index);

    phych = dac_localch_to_phych(local_ch);

    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_freq[index], freq_word);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_phase[index], phase_word);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_amp[index], amp_word);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_en[index], 0);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_en[index], 1);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_en[index], 0);
}

void qa_dac_play_param(uint32_t logical_ch, DACDDSPlayParam_t param)
{
    uint8_t chip_id, local_ch;
    get_qa_out_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    uint32_t phych;

    P_LOG_DEBUG("QA: Set chip: %d local ch: %d dds play param.", chip_id, local_ch);
    P_LOG_DEBUG("data_len: %d.", param.m_data_len);
    P_LOG_DEBUG("play_times: %d.", param.m_play_times);
    P_LOG_DEBUG("play_delay: %d.", param.m_play_delay);
    P_LOG_DEBUG("play_mode: %d.", param.m_play_mode);
    phych = dac_localch_to_phych(local_ch);
    if (param.m_play_mode == 1 || param.m_play_mode == 2)
    {
        param.m_data_len = param.m_data_len / 32;
        P_LOG_DEBUG("play_mode: %s, write data len: %d.", "AWG", param.m_data_len);
    }
    else if (param.m_play_mode == 3 || param.m_play_mode == 4)
    {
        param.m_data_len = param.m_data_len / 16;
        P_LOG_DEBUG("play_mode: %s, write data len: %d.", "DDS", param.m_data_len);
    }
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_data_len, param.m_data_len);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_play_times, param.m_play_times);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_play_delay, param.m_play_delay);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_wait_calc_delay, 0x1);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_DAC_config_reg[phych].m_dac_play_mode, param.m_play_mode);
}

void qa_sample_start_stop(uint32_t logical_ch, uint32_t en)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    int phych;
    P_LOG_DEBUG("QA: Set chip: %d sample ch: %d enable status: %d", chip_id, local_ch, en);

    phych = adc_localch_to_phych(local_ch);

    en = (en == 1) ? 1 : 0;

    xdma_write_user_space(chip_id, s_QAUserRegs.m_sample_config_reg[phych].m_sample_start, en);
}

void qa_sample_config(uint32_t logical_ch, SampleConfigReg_t config)
{
    PcieBoardInfo *board_info = get_pcie_board_info();
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    int phych;

    P_LOG_DEBUG("QA: Set chip: %d dac ch: %d sample config.", chip_id, local_ch);
    P_LOG_DEBUG("sample_length: %d.", config.m_sample_length);
    P_LOG_DEBUG("sample_delay: %d.", config.m_sample_delay);
    P_LOG_DEBUG("sample_num: %d.", config.m_sample_num);

    phych = adc_localch_to_phych(local_ch);
    if (board_info->qa_sub_type[chip_id] == 4) // 4G板卡采样长度
    {
        config.m_sample_length = config.m_sample_length / 8;
    }
    else if (board_info->qa_sub_type[chip_id] == 8) // 8G板卡采样长度
    {
        config.m_sample_length = config.m_sample_length / 16;
    }
    xdma_write_user_space(chip_id, s_QAUserRegs.m_sample_config_reg[phych].m_sample_mode, 1);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_sample_config_reg[phych].m_sample_length, config.m_sample_length);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_sample_config_reg[phych].m_sample_delay, config.m_sample_delay);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_sample_config_reg[phych].m_sample_num, config.m_sample_num);
}

void qa_adc_start_stop_play(uint32_t logical_ch, uint32_t en)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    int phych;

    P_LOG_DEBUG("QA: Set chip: %d adc ch: %d enable status: %d", chip_id, local_ch, en);
    phych = adc_localch_to_phych(local_ch);

    en = (en == 1) ? 1 : 0;

    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_start_play, en);
    if (en)
    {
        start_gain_trig();
    }
}

void qa_adc_config(uint32_t logical_ch, uint32_t index, ADCConfig_t config)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    int phych;

    P_LOG_DEBUG("QA: Set chip: %d adc ch: %d adc config.", chip_id, local_ch);
    uint32_t freq_word, phase_word;
    qa_adc_start_stop_play(logical_ch, 0);
    freq_word = (uint32_t)(config.m_adc_freq / 2000e6 * pow(2, 25));
    phase_word = (uint32_t)(config.m_adc_phase / 180 * pow(2, 25));

    P_LOG_DEBUG("freq: %.f value: %d(%#x) write to adc index: %d", config.m_adc_freq, freq_word, freq_word, index);
    P_LOG_DEBUG("phase: %.f value: %d(%#x) write to adc index: %d", config.m_adc_phase, phase_word, phase_word, index);

    phych = adc_localch_to_phych(local_ch);

    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_freq[index], freq_word);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_phase[index], phase_word);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_en[index], 0);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_en[index], 1);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_en[index], 0);
}

void qa_adc_play_param(uint32_t logical_ch, ADCPlayParam_t param)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    int phych;

    P_LOG_DEBUG("QA: Set chip: %d adc ch: %d adc play param.", chip_id, local_ch);
    P_LOG_DEBUG("adc_data_len: %d.", param.m_adc_data_len);
    P_LOG_DEBUG("adc_play_times: %d.", param.m_adc_play_times);
    P_LOG_DEBUG("adc_play_delay: %d.", param.m_adc_play_delay);
    P_LOG_DEBUG("adc_play_mode: %d.", param.m_adc_play_mode);

    phych = adc_localch_to_phych(local_ch);

    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_data_len, param.m_adc_data_len);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_play_times, param.m_adc_play_times);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_play_delay, param.m_adc_play_delay);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_ADC_config_reg[phych].m_adc_play_mode, param.m_adc_play_mode);
}

int qa_get_sample_data(uint32_t logical_ch, uint64_t getLength, uint8_t *buffer)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    if (buffer == NULL)
    {
        return -2;
    }
    int rtn = -1, count = 10000;
    uint32_t readVal = 0;
    do
    {
        rtn = xdma_read_user_space(chip_id, s_QAUserRegs.m_adc_mem_status, readVal);
        if ((readVal & 0x03) == 0x3)
        {
            P_LOG_DEBUG("Sampling finished, exit waiting.");
            break;
        }
    } while (count--);
    if (count <= 0 || rtn != 0)
    {
        P_LOG_ERROR("Wait for adc memry status overtime!");
        return -3;
    }
    dma_read_data(chip_id, s_QAUserRegs.m_adc_data_save_base[local_ch], getLength, buffer);
}

int qa_get_sample_state(uint32_t logical_ch)
{
    int ret;
    int count = 10000;
    uint32_t readVal = 0;
    uint8_t chip_id = 0;
    uint8_t local_ch = 0;

    ret = get_qa_in_route(logical_ch, &chip_id, &local_ch);

    if (ret != 0)
    {
        P_LOG_ERROR("get_qa_in_route failed, logical_ch=%u\n", logical_ch);
        return -1;
    }

    P_LOG_DEBUG("logical_ch=%u, chip_id=%u, local_ch=%u\n",
                logical_ch, chip_id, local_ch);

    while (count-- > 0)
    {
        ret = xdma_read_user_space(chip_id, s_QAUserRegs.m_adc_mem_status, &readVal);
        if (ret != 0)
        {
            P_LOG_ERROR("xdma_read_user_space failed, chip_id=%u\n", chip_id);
            return -1;
        }

        if ((readVal & 0x03) == 0x03)
        {
            P_LOG_DEBUG("Sampling finished\n");
            return 0;
        }

        usleep(100);
    }

    P_LOG_ERROR("Sampling wait timeout, logical_ch=%u, chip_id=%u\n",
                logical_ch, chip_id);
    return -1;
}

int qa_get_demo_state(uint32_t logical_ch)
{
    int ret;
    int count = 10000;
    uint32_t readVal = 0;
    uint8_t chip_id = 0;
    uint8_t local_ch = 0;

    ret = get_qa_in_route(logical_ch, &chip_id, &local_ch);

    if (ret != 0)
    {
        P_LOG_ERROR("get_qa_in_route failed, logical_ch=%u\n", logical_ch);
        return -1;
    }

    P_LOG_DEBUG("logical_ch=%u, chip_id=%u, local_ch=%u\n",
                logical_ch, chip_id, local_ch);

    while (count-- > 0)
    {
        ret = xdma_read_user_space(chip_id, s_QAUserRegs.m_adc_corr_status, &readVal);
        if (ret != 0)
        {
            P_LOG_ERROR("xdma_read_user_space failed, chip_id=%u\n", chip_id);
            return -1;
        }

        if ((readVal & 0x03) == 0x03)
        {
            P_LOG_DEBUG("demo finished\n");
            return 0;
        }

        usleep(100);
    }

    P_LOG_ERROR("demo wait timeout, logical_ch=%u, chip_id=%u\n", logical_ch, chip_id);

    return -1;
}

int qa_get_ddr_data(uint32_t logical_ch, uint64_t getLength, uint8_t *buffer)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);

    if (buffer == NULL)
    {
        return -2;
    }
    int rtn = -1, count = 10000;
    uint32_t readVal = 0;
    do
    {
        rtn = xdma_read_user_space(chip_id, s_QAUserRegs.m_adc_corr_status, readVal);
        if ((readVal & 0x03) == 0x3)
        {
            P_LOG_DEBUG("DDR data analize finished, exit waiting.");
            break;
        }
    } while (count--);
    if (count <= 0 || rtn != 0)
    {
        P_LOG_ERROR("Wait for adc corr status overtime!");
        return -3;
    }
    dma_read_data(chip_id, s_QAUserRegs.m_adc_decision_res_base[local_ch], getLength * 16 * 16, buffer);
}

void qa_soft_trig_signal(uint32_t logical_ch)
{
    int ret;
    uint32_t readVal = 0;
    uint8_t chip_id = 0;
    uint8_t local_ch = 0;
    ret = get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("QA: Set chip: %d trigger once.", chip_id);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_soft_trig, 0);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_soft_trig, 1);
    xdma_write_user_space(chip_id, s_QAUserRegs.m_soft_trig, 0);
}

void start_gain_trig()
{
    common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 0);
    usleep(100);
    common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 1);
    usleep(100);
    common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 0);
}

void qa_trig_source_init(void)
{
    PcieBoardInfo *pBoardInfo = NULL;
    pBoardInfo = get_pcie_board_info();
    if (pBoardInfo == NULL)
    {
        P_LOG_ERROR("get pcie board info error\r\n");
        return;
    }
    // qa subscard default out trigger
    for (uint8_t i = 0; i < pBoardInfo->qa_out_ch_num; i++)
    {
        qa_trigger_source_ctrl(i, 0x1); // 0x01:外部触发源
        // TODO: FPGA修复AXI4跨时钟域访问BUG后可删除该延时
        usleep(10);
    }
}
