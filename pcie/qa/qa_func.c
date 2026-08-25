#include "qa_func.h"
#include "../pcie_func.h"
#include "../rf/rf_ctrl.h"
#include "../../device_info.h"
#include "../../i2c/io_expand/max7300.h"
#include "../../lib/include/platform_log/platform_log.h"
#include <unistd.h>

static QAUserReg_t s_QAUserRegs;

void QAConfigRegisterInit(void)
{
    s_QAUserRegs.m_soft_trig = C_USER_SPACE_CONFIG_OFFSET + 0 * 4;
    s_QAUserRegs.m_trigger_mode = C_USER_SPACE_CONFIG_OFFSET + 1 * 4;
    s_QAUserRegs.m_trigger_source = C_USER_SPACE_CONFIG_OFFSET + 3 * 4;
    uint8_t index = 0;
    for (uint8_t u8ChIndex = 0; u8ChIndex < CARD_LOCAL_OUT_CH_NUM; ++u8ChIndex)
    {
        // DAC参数寄存器配置
        for (index = 0; index < SIGNAL_MODULE_NUM; ++index)
        {
            s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_en[index] = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0000 + index * 0x10) * 4;
            P_LOG_DEBUG("Channel %d DAC ch %d DAC_CFG_REG addr-m_dac_en = %#x", u8ChIndex, index, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_en[index]);
            s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_freq[index] = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0002 + index * 0x10) * 4;
            P_LOG_DEBUG("Channel %d DAC ch %d DAC_CFG_REG addr-m_dac_freq = %#x", u8ChIndex, index, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_freq[index]);
            s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_phase[index] = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0004 + index * 0x10) * 4;
            P_LOG_DEBUG("Channel %d DAC ch %d DAC_CFG_REG addr-m_dac_phase = %#x", u8ChIndex, index, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_phase[index]);
            s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_amp[index] = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0005 + index * 0x10) * 4;
            P_LOG_DEBUG("Channel %d DAC ch %d DAC_CFG_REG addr-m_dac_amp = %#x", u8ChIndex, index, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_amp[index]);
        }
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_data_len = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0006) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_data_len = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_data_len);
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_times = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0007) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_play_times = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_times);
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_delay = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0008) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_play_delay = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_delay);
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_wait_calc_delay = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x0009) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_wait_calc_delay = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_wait_calc_delay);
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_mode = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x000A) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_play_mode = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_play_mode);
        s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_start_play = C_USER_SPACE_CONFIG_OFFSET + (DAC_CONFIG_BASE + u8ChIndex * 0x0100 + 0x000B) * 4;
        P_LOG_DEBUG("Channel %d DAC DAC_CFG_REG addr-m_dac_start_play = %#x", u8ChIndex, s_QAUserRegs.m_DAC_config_reg[u8ChIndex].m_dac_start_play);
    }

    s_QAUserRegs.m_device_id = C_USER_SPACE_CONFIG_OFFSET + (SYSTEM_CONFIG_BASE + 0x0000) * 4;
    s_QAUserRegs.m_adc_mem_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0000 * 4;
    s_QAUserRegs.m_adc_corr_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0001 * 4;
    s_QAUserRegs.m_dac_play_status = C_USER_SPACE_CONFIG_OFFSET + STATUS_BASE + 0x0002 * 4;

    s_QAUserRegs.m_dac_ram_base[0] = 0x1cc0000000;
    s_QAUserRegs.m_dac_ram_base[1] = 0x1cc1000000;
    s_QAUserRegs.m_dac_ram_base[2] = 0x1cc2000000;
    s_QAUserRegs.m_dac_ram_base[3] = 0x1cc3000000;

    s_QAUserRegs.m_dac_ram_base[4] = 0x1cc4000000;
    s_QAUserRegs.m_dac_ram_base[5] = 0x1cc5000000;
    s_QAUserRegs.m_dac_ram_base[6] = 0x1cc6000000;
    s_QAUserRegs.m_dac_ram_base[7] = 0x1cc7000000;

    /*
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
        s_QAUserRegs.m_decision_cnt_reg[0].m_decision_cnt[index] = C_USER_SPACE_CONFIG_OFFSET + ADC1_RES_BASE + index * 4;
        s_QAUserRegs.m_decision_cnt_reg[1].m_decision_cnt[index] = C_USER_SPACE_CONFIG_OFFSET + ADC2_RES_BASE + index * 4;
    }
    */
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
    switch (localch)
    {
    case 1:
        phych = 0;
        break;
    case 2:
        phych = 1;
        break;
    case 3:
        phych = 2;
        break;
    case 4:
        phych = 3;
        break;
    case 5:
        phych = 4;
        break;
    case 6:
        phych = 5;
        break;
    case 7:
        phych = 6;
        break;
    case 8:
        phych = 7;
        break;
    default:
        P_LOG_ERROR("no suppurt ch: %d\r\n", localch);
        break;
    }
    return phych;
}

int adc_localch_to_phych(uint32_t localch)
{
    int phych;
    switch (localch)
    {
    case 1:
        phych = 0;
        break;
    case 2:
        phych = 1;
        break;
    case 3:
        phych = 2;
        break;
    case 4:
        phych = 3;
        break;
    default:
        P_LOG_ERROR("no suppurt ch: %d\r\n", localch);
        break;
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
    phase_word = (uint32_t)(config.m_DAC_Phase / 360 * pow(2, 25));
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
    if (param.m_play_mode == E_DAC_AWG_TRIG_PLAY || param.m_play_mode == E_DAC_AWG_CONT_PLAY)
    {
        param.m_data_len = param.m_data_len / 32;
        P_LOG_DEBUG("play_mode: %s, write data len: %d.", "AWG", param.m_data_len);
    }
    else if (param.m_play_mode == E_DAC_DDS_TRIG_PLAY || param.m_play_mode == E_DAC_DDS_CONT_PLAY)
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

uint8_t reverse8bit(uint8_t data)
{
    uint8_t res = 0;
    for (int i = 0; i < 8; i++)
    {
        res <<= 1;         // 结果左移腾出最低位
        res |= (data & 1); // 取出data最低位放到res
        data >>= 1;        // 原数据右移，处理下一位
    }
    return res;
}

void qa_set_rf_da_atten(uint32_t logical_ch, float attenVal)
{
    uint8_t chip_id, local_ch;
    logical_ch = logical_ch;
    get_qa_out_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
    int phych;
    phych = dac_localch_to_phych(local_ch);
    uint8_t setAttenVal = (uint8_t)(attenVal / C_ATTEN_STEP);
    P_LOG_DEBUG("Set atten total value:%d", setAttenVal);
    uint8_t setStage1AttenVal = C_ATTEN_SET_MIN_VAL;
    uint8_t setStage2AttenVal = 0;
    if ((setAttenVal >= 0) && (setAttenVal <= C_STAGE_ATTEN_SET_MAX_VAL))
    {
        setStage1AttenVal = reverse8bit(setAttenVal);
        setStage2AttenVal = 0;
        P_LOG_DEBUG("Set RF channel %d atten stage 1:%#x(origin:%#x)...%f",
                    phych,
                    setStage1AttenVal,
                    setAttenVal,
                    attenVal);
    }
    else
    {
        P_LOG_ERROR("Set RF atten ERROR!...%f, %d", attenVal, setAttenVal);
        return;
    }
    set_ch_atten(phych, C_SET_ATTEN_0, setStage1AttenVal);
}

void qa_set_rf_ad_atten(uint32_t logical_ch, float attenVal)
{
    uint8_t chip_id, local_ch;
    logical_ch = 5 - logical_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
    int phych;
    phych = adc_localch_to_phych(local_ch) + 4;
    uint8_t setAttenVal = (uint8_t)(attenVal / C_ATTEN_STEP);
    P_LOG_DEBUG("Set atten total value:%d", setAttenVal);
    uint8_t stage1AttenVal = C_ATTEN_SET_MIN_VAL;
    uint8_t stage2AttenVal = 0;
    uint8_t setStage1AttenVal = reverse8bit(stage1AttenVal);
    uint8_t setStage2AttenVal = reverse8bit(stage2AttenVal);
    if ((setAttenVal >= 0) && (setAttenVal <= C_STAGE_ATTEN_SET_MAX_VAL))
    {
        stage1AttenVal = setAttenVal;
        stage2AttenVal = 0;
        setStage1AttenVal = reverse8bit(stage1AttenVal);
        setStage2AttenVal = reverse8bit(stage2AttenVal);
        P_LOG_DEBUG("Only use stage 1. Set RF channel %d atten stage 1:%#x(origin:%#x), stage 2:%#x(origin:%#x)...%f",
                    phych,
                    setStage1AttenVal,
                    stage1AttenVal,
                    setStage2AttenVal,
                    stage2AttenVal,
                    attenVal);
    }
    else if ((setAttenVal > C_STAGE_ATTEN_SET_MAX_VAL) && (setAttenVal <= C_ATTEN_SET_MAX_VAL))
    {
        stage1AttenVal = C_STAGE_ATTEN_SET_MAX_VAL;
        stage2AttenVal = setAttenVal - C_STAGE_ATTEN_SET_MAX_VAL;
        setStage1AttenVal = reverse8bit(stage1AttenVal);
        setStage2AttenVal = reverse8bit(stage2AttenVal);
        P_LOG_DEBUG("Use both stage. Set RF channel %d atten stage 1:%#x(origin:%#x), stage 2:%#x(origin:%#x)...%f",
                    phych,
                    setStage1AttenVal,
                    stage1AttenVal,
                    setStage2AttenVal,
                    stage2AttenVal,
                    attenVal);
    }
    else
    {
        P_LOG_ERROR("Set RF atten ERROR!...%f, %d", attenVal, setAttenVal);
        return;
    }
    set_ch_atten(phych, C_SET_ATTEN_0, setStage1AttenVal);
    usleep(100);
    set_ch_atten(phych, C_SET_ATTEN_1, setStage2AttenVal);
}

#define C_DIGITAL_ATTEN_CH_START_BIT 8
#define C_DIGITAL_ATTEN_CH_BIT_WIDTH 8
#define C_DIGITAL_ATTEN_VALUE_START_BIT 0
#define C_DIGITAL_ATTEN_VALUE_BIT_WIDTH 5
#define C_DIGITAL_ATTEN_LATCH_START_BIT 16
#define C_DIGITAL_ATTEN_LATCH_BIT_WIDTH 1

void qa_set_digital_atten(uint32_t logical_ch, uint8_t attenVal)
{
    uint8_t chip_id, local_ch;
    get_qa_in_route(logical_ch, &chip_id, &local_ch);
    P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
    int phych;
    phych = adc_localch_to_phych(local_ch) + 1;
    uint32_t regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
                      C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
                      C_FIELD_PACK(0, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
    regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
             C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
             C_FIELD_PACK(1, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
    regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
             C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
             C_FIELD_PACK(0, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
    usleep(1);
    regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
             C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
             C_FIELD_PACK(0, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
    regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
             C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
             C_FIELD_PACK(1, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
    regVal = C_FIELD_PACK(phych, C_DIGITAL_ATTEN_CH_START_BIT, C_DIGITAL_ATTEN_CH_BIT_WIDTH) |
             C_FIELD_PACK(attenVal, C_DIGITAL_ATTEN_VALUE_START_BIT, C_DIGITAL_ATTEN_VALUE_BIT_WIDTH) |
             C_FIELD_PACK(0, C_DIGITAL_ATTEN_LATCH_START_BIT, C_DIGITAL_ATTEN_LATCH_BIT_WIDTH);
    P_LOG_DEBUG("Write digital atten value: %d to channel %d, reg value%#x", attenVal, phych, regVal);
    xdma_write_user_space(chip_id, C_USER_SPACE_CONFIG_OFFSET + (4 * 4), regVal);
}
