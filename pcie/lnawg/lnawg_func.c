#include "lnawg_func.h"
#include "../pcie_func.h"
#include "../common_func.h"
#include "../../device_info.h"
#include "../../i2c/i2c_func.h"
#include "../../i2c/io_expand/max7300.h"
#include "../../lib/include/platform_log/platform_log.h"

static AWGUserReg_t s_AWGUserReg;

void AWGConfigRegisterInit(void)
{
	uint8_t chIndex = 0, DDSConfigIndex = 0;
	s_AWGUserReg.m_device_id = C_USER_SPACE_CONFIG_OFFSET + (C_SYSTEM_BASE_ADDR + 0x0) * 4;
	s_AWGUserReg.m_start_sync = C_USER_SPACE_CONFIG_OFFSET + (C_SYSTEM_BASE_ADDR + 0x1) * 4;
	s_AWGUserReg.m_start_dac_config = C_USER_SPACE_CONFIG_OFFSET + (C_SYSTEM_BASE_ADDR + 0x2) * 4;
	s_AWGUserReg.m_dac_output_rst = C_USER_SPACE_CONFIG_OFFSET + (C_SYSTEM_BASE_ADDR + 0x3) * 4;
	s_AWGUserReg.m_temp = C_USER_SPACE_CONFIG_OFFSET + (C_SYSTEM_BASE_ADDR + 0x7) * 4;
	for (chIndex = 0; chIndex < C_LNAWG_CHANNEL_NUM; ++chIndex)
	{
		// AWG波形参数
		s_AWGUserReg.m_ch_mode[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x0) * 4;
		s_AWGUserReg.m_ch_en[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x1) * 4;
		s_AWGUserReg.m_ch_seq_cnt[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x2) * 4;
		s_AWGUserReg.m_ch_loop_cnt[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x3) * 4;
		s_AWGUserReg.m_switch_flag[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x4) * 4;
		s_AWGUserReg.m_ch_range[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x5) * 4;
		s_AWGUserReg.m_ch_offset[chIndex] = C_AWG_REG_BASE_ADDR + (C_SYSTEM_BASE_ADDR + (chIndex * 0x10) + 0x8) * 4;

		s_AWGUserReg.m_ch_dds_config[chIndex].m_en = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (chIndex * 0x10) + 0x8) * 4;
		for (DDSConfigIndex = 0; DDSConfigIndex < C_LNAWG_CH_DDS_NUM; ++DDSConfigIndex)
		{
			s_AWGUserReg.m_ch_dds_config[chIndex].m_freq_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x1) * 4;
			s_AWGUserReg.m_ch_dds_config[chIndex].m_phase_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x2) * 4;
			s_AWGUserReg.m_ch_dds_config[chIndex].m_amp_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x3) * 4;
			s_AWGUserReg.m_ch_dds_config[chIndex].m_len_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x4) * 4;
			s_AWGUserReg.m_ch_dds_config[chIndex].m_trig_delay_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x5) * 4;
			s_AWGUserReg.m_ch_dds_config[chIndex].m_delt_x_addr[DDSConfigIndex] = C_AWG_REG_BASE_ADDR + (C_DDS_BASE_ADDR + (DDSConfigIndex * 0x10) + 0x6) * 4;
		}
	}

	// 反馈配置地址
	s_AWGUserReg.m_fb_config.m_fb_en = C_FB_BASE_ADDR;

	// 波形下载基地址
	s_AWGUserReg.m_ch_wave_base_addr[0] = 0x1800000000;
	s_AWGUserReg.m_ch_wave_base_addr[1] = 0x1880000000;
	s_AWGUserReg.m_ch_wave_base_addr[2] = 0x1900000000;
	s_AWGUserReg.m_ch_wave_base_addr[3] = 0x1980000000;
}

static DDSAddrMap_t s_ddsAddrMapCtx[C_LNAWG_CHANNEL_NUM] = {
	[0] = {
		.m_en = DDS_0_EN,
		.m_mode = CHANNEL_0_MODE,
		.m_len_addr = {DDS_0_PART_1_LEN, DDS_0_PART_2_LEN, DDS_0_PART_3_LEN, DDS_0_PART_4_LEN},
		.m_trig_delay_addr = {DDS_0_PART_1_TRIG_DELAY, DDS_0_PART_2_TRIG_DELAY, DDS_0_PART_3_TRIG_DELAY, DDS_0_PART_4_TRIG_DELAY},
		.m_freq_addr = {DDS_0_PART_1_FREQUENCE, DDS_0_PART_2_FREQUENCE, DDS_0_PART_3_FREQUENCE, DDS_0_PART_4_FREQUENCE},
		.m_phase_addr = {DDS_0_PART_1_PHASE, DDS_0_PART_2_PHASE, DDS_0_PART_3_PHASE, DDS_0_PART_4_PHASE},
		.m_amp_addr = {DDS_0_PART_1_AMP, DDS_0_PART_2_AMP, DDS_0_PART_3_AMP, DDS_0_PART_4_AMP},
		.m_delt_x_addr = {DDS_0_PART_1_DELT_X, DDS_0_PART_2_DELT_X, DDS_0_PART_3_DELT_X, DDS_0_PART_4_DELT_X},
	},
	[1] = {
		.m_en = DDS_1_EN,
		.m_mode = CHANNEL_1_MODE,
		.m_len_addr = {DDS_1_PART_1_LEN, DDS_1_PART_2_LEN, DDS_1_PART_3_LEN, DDS_1_PART_4_LEN},
		.m_trig_delay_addr = {DDS_1_PART_1_TRIG_DELAY, DDS_1_PART_2_TRIG_DELAY, DDS_1_PART_3_TRIG_DELAY, DDS_1_PART_4_TRIG_DELAY},
		.m_freq_addr = {DDS_1_PART_1_FREQUENCE, DDS_1_PART_2_FREQUENCE, DDS_1_PART_3_FREQUENCE, DDS_1_PART_4_FREQUENCE},
		.m_phase_addr = {DDS_1_PART_1_PHASE, DDS_1_PART_2_PHASE, DDS_1_PART_3_PHASE, DDS_1_PART_4_PHASE},
		.m_amp_addr = {DDS_1_PART_1_AMP, DDS_1_PART_2_AMP, DDS_1_PART_3_AMP, DDS_1_PART_4_AMP},
		.m_delt_x_addr = {DDS_1_PART_1_DELT_X, DDS_1_PART_2_DELT_X, DDS_1_PART_3_DELT_X, DDS_1_PART_4_DELT_X},
	},
};

LocalParam gLocalParam;

LocalParam *get_local_param_instance()
{
	return &gLocalParam;
}

void set_awg_ch_run(int32_t logical_ch, int32_t state)
{
	int32_t chmode;
	uint8_t chip_id;
	uint8_t local_ch;
	chmode = gLocalParam.awgch_mode[logical_ch - 1];
	get_awg_route(logical_ch, &chip_id, &local_ch);

	if (chmode == C_AWG_CH_MODE_AWG)
	{
		if (state)
		{
			set_awg_ch_led_status(logical_ch, LED_GREEN);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_en[local_ch - 1], 1);
		}
		else
		{
			set_awg_ch_led_status(logical_ch, LED_OFF);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_en[local_ch - 1], 0);
		}
		P_LOG_DEBUG("AWG MODE\r\n");
	}
	else if (chmode == C_AWG_CH_MODE_DDS || chmode == C_AWG_CH_MODE_CHIRP_OUT)
	{
		if (state)
		{
			// run
			set_awg_ch_led_status(logical_ch, LED_GREEN);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, 1);
		}
		else
		{
			// stop
			set_awg_ch_led_status(logical_ch, LED_OFF);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, 0);
		}
		P_LOG_DEBUG("DDS MODE\r\n");
	}
	else if (chmode == C_AWG_CH_MODE_PARAM_WAVE)
	{
		if (state)
		{
			// run
			set_awg_ch_led_status(logical_ch, LED_GREEN);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_en[local_ch - 1], 1);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, 1);
		}
		else
		{
			// stop
			set_awg_ch_led_status(logical_ch, LED_OFF);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_en[local_ch - 1], 0);
			xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, 0);
		}
		P_LOG_DEBUG("WAVE PARAM MODE\r\n");
	}
}

uint32_t get_awg_ch_run(int32_t logical_ch)
{
	int32_t chmode;
	uint8_t chip_id;
	uint8_t local_ch;

	chmode = gLocalParam.awgch_mode[logical_ch - 1];
	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t state;
	if (chmode == C_AWG_CH_MODE_AWG)
	{
		xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_en[local_ch - 1], &state);
	}
	else if (chmode == C_AWG_CH_MODE_DDS || chmode == C_AWG_CH_MODE_CHIRP_OUT)
	{
		xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, &state);
	}
	else
	{
		P_LOG_ERROR("Invalid channel mode! channel mode must be in [1,2,3], current mode = %d\r\n", chmode);
		return 0xFFFFFFFFU; // 设置一个值，表示异常值
	}
	P_LOG_DEBUG("Success to get awg ch run state, ch:%d, state:%d", logical_ch, state);
	return state;
}

void set_awg_ch_mode(int32_t logical_ch, int32_t mode)
{
	uint8_t chip_id;
	uint8_t local_ch;

	LocalParam *plp = NULL;

	plp = get_local_param_instance();

	get_awg_route(logical_ch, &chip_id, &local_ch);

	P_LOG_DEBUG("set_awg_ch_mode mode: %d", mode);

	set_awg_ch_run(logical_ch, 0); // stop before set mode
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_mode[local_ch - 1], mode);
	set_awg_ch_segment_count(logical_ch, 1);

	// save mode
	plp->awgch_mode[logical_ch - 1] = mode;
	P_LOG_DEBUG("Success to set_awg_ch_mode");
}

uint32_t get_awg_ch_mode(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;
	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t mode;
	set_awg_ch_run(logical_ch, 0); // stop before get mode
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_mode[local_ch - 1], &mode);

	return mode;
}

void set_awg_ch_ext_src(int32_t logical_ch, int32_t source)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ext_source, source);
}

uint32_t get_awg_ch_ext_src(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;
	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t ext_src;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ext_source, &ext_src);
	return ext_src;
}

void set_dev_trig(DevTrigCtrl devtrig)
{
	int i;
	PcieBoardInfo *pBoardInfo = NULL;
	pBoardInfo = get_pcie_board_info();
	if (pBoardInfo == NULL)
	{
		P_LOG_ERROR("get pcie board info error\r\n");
		return;
	}
	// 参数同步到本地
	gLocalParam.dev_trig_ctrl.trigger_source = devtrig.trigger_source;
	gLocalParam.dev_trig_ctrl.trigger_us = devtrig.trigger_us;
	gLocalParam.dev_trig_ctrl.trigger_times = devtrig.trigger_times;
	gLocalParam.dev_trig_ctrl.trigger_continue = devtrig.trigger_continue;
	gLocalParam.dev_trig_ctrl.trigger_delay = devtrig.trigger_delay;

	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (0 * 4), devtrig.trigger_source);
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (1 * 4), (uint32_t)((devtrig.trigger_us - 1) * 25));
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (2 * 4), devtrig.trigger_times);
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (3 * 4), devtrig.trigger_continue);
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (4 * 4), (uint32_t)((devtrig.trigger_delay) * 25));
}

// 1:enable 0:disable
void set_dev_trig_state(int32_t state)
{
	if (state)
		common_reg_data_set(SOFT_TIRGGER_BASEADDR + (6 * 4), 0);
	else
		common_reg_data_set(SOFT_TIRGGER_BASEADDR + (6 * 4), 1);
}

void send_software_trig()
{
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 0);
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 1);
	common_reg_data_set(SOFT_TIRGGER_BASEADDR + (5 * 4), 0);
}

void set_awg_ch_out_range(int32_t logical_ch, int32_t range)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	if (range == E_RANGE_DIRECT)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 1);

		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000100);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000010);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0100000);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 0);
	}
	else if (range == E_RANGE_3V)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 1);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000001);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b1000000);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b1001000);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b1100000);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b1000000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 0);
	}
	else if (range == E_RANGE_HIGH_Z)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 1);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0100101);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 0);
	}
	else if (range == E_RANGE_GND)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 1);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0010101);
		usleep(20000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0b0000000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_switch_flag[local_ch - 1], 0);
	}
	else
	{
	}
}

uint32_t get_awg_ch_out_range(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	// todo 不知道怎么读
	uint32_t range;

	return range;
}

void set_awg_ch_range(int32_t logical_ch, int32_t range)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	if (range == 0)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0x25);
		usleep(600000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0);
	}
	else if (range == 3)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0x2A);
		usleep(800000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 64);
	}
	else if (range == 5)
	{
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 0x1A);
		usleep(600000);
		xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_range[local_ch - 1], 64);
	}
	else
	{
	}
}

uint32_t get_awg_ch_range(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	// todo 不知道怎么读
	uint32_t range;

	return range;
}

void set_awg_ch_offset(int32_t logical_ch, double offset)
{
	uint8_t chip_id;
	uint8_t local_ch;
	int32_t value;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	value = (int32_t)lround(offset * 32767.0);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_offset[local_ch - 1], value);
}

double get_awg_ch_offset(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;
	int32_t value;

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_offset[local_ch - 1], &value);
	double offset = (double)value / 32767.0;

	return offset;
}

void set_awg_ch_segment_count(int32_t logical_ch, int32_t segcnt)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_seq_cnt[local_ch - 1], segcnt);
}

uint32_t get_awg_ch_segment_count(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t seg_cnt;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_seq_cnt[local_ch - 1], &seg_cnt);
	return seg_cnt;
}

void set_awg_ch_segment_loop(int32_t logical_ch, int32_t segloop)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_loop_cnt[local_ch - 1], segloop);
}

uint32_t get_awg_ch_segment_loop(int32_t logical_ch)
{
	uint8_t chip_id;
	uint8_t local_ch;

	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t loop_cnt;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_loop_cnt[local_ch - 1], &loop_cnt);
	return loop_cnt;
}

void set_awg_dds_config(int32_t logical_ch, DDSConfigParam_t config)
{
	int8_t chip_id;
	int8_t local_ch;

	uint32_t freq_val = (uint32_t)config.m_freq / 2e9 * pow(2, 32);
	uint32_t phase_val = (uint32_t)config.m_phase / 360 * pow(2, 32);
	uint32_t amp_val = (uint32_t)65535 * config.m_amp;
	uint32_t set_index = config.m_index;
	// TODO:delay/len必须>16, 且是16的整数倍
	P_LOG_DEBUG("set_awg_dds_config: logical_ch=%d, freq=%.2f, phase=%.2f, amp=%.2f, index=%d\r\n",
				logical_ch, config.m_freq, config.m_phase, config.m_amp, config.m_index);
	get_awg_route(logical_ch, &chip_id, &local_ch);
	P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
	P_LOG_DEBUG("freq_val: %d(0x%08x), phase_val: %d(0x%08x), amp_val: %d(0x%08x)", freq_val, freq_val, phase_val, phase_val, amp_val, amp_val);

	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_len_addr[set_index], config.m_len);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_trig_delay_addr[set_index], config.m_trig_delay);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_freq_addr[set_index], freq_val);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_phase_addr[set_index], phase_val);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_amp_addr[set_index], amp_val);
}

DDSConfigParam_t get_awg_dds_config(int32_t logical_ch, uint32_t index)
{
	int8_t chip_id;
	int8_t local_ch;
	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t freq_val, phase_val, amp_val, len, trig_delay;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_len_addr[index], &len);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_trig_delay_addr[index], &trig_delay);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_freq_addr[index], &freq_val);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_phase_addr[index], &phase_val);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_amp_addr[index], &amp_val);

	// 将double变为uint32_t
	DDSConfigParam_t config;
	config.m_index = index;
	config.m_len = len;
	config.m_trig_delay = trig_delay;
	config.m_freq = (double)freq_val * 2.4e9 / pow(2, 32);
	config.m_phase = (double)phase_val * 360.0 / pow(2, 32);
	config.m_amp = (double)amp_val / 65535;

	return config;
}

void lnawg_trig_source_init(void)
{
	PcieBoardInfo *pBoardInfo = NULL;
	pBoardInfo = get_pcie_board_info();
	if (pBoardInfo == NULL)
	{
		P_LOG_ERROR("get pcie board info error\r\n");
		return;
	}
	// awg subscard default out trigger
	for (uint8_t i = 0; i < pBoardInfo->awg_ch_num; i++)
	{
		set_awg_ch_ext_src(i + 1, 0x01); // 0x01:外部触发源
		// TODO: FPGA修复AXI4跨时钟域访问BUG后可删除该延时
		usleep(10);
	}
}

void set_chirp_out_param(int32_t logical_ch, ChirpOutParam_t config)
{
	uint8_t chip_id;  // 子卡id
	uint8_t local_ch; // 通道数

	get_awg_route(logical_ch, &chip_id, &local_ch);
	P_LOG_DEBUG("Analize physcial ch: %d, to chip: %d, local ch:%d", logical_ch, chip_id, local_ch);
	uint32_t freq_start_val = (uint32_t)(config.m_freq_start / 2.4e9 * pow(2, 32));
	uint32_t freq_end_val = (uint32_t)(config.m_freq_end / 2.4e9 * pow(2, 32));
	uint32_t delt_x_val = (freq_end_val - freq_start_val) / config.m_len;
	uint32_t phase_val = (uint32_t)(config.m_phase / 360 * pow(2, 32));
	uint32_t amp_val = (uint32_t)65535 * config.m_amp;
	uint32_t set_index = config.m_index;
	P_LOG_DEBUG(
		"freq_start_val: %d(0x%08x), freq_end_val: %d(0x%08x), phase_val: %d(0x%08x), amp_val: %d(0x%08x), len: %d(0x%08x),"
		"trig_delay: %d(0x%08x), delt_x_val: %d(0x%08x)",
		freq_start_val, freq_end_val, phase_val, amp_val, config.m_len, config.m_trig_delay, delt_x_val);

	// 设置频率
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_freq_addr[set_index], freq_start_val);
	// 设置相位
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_phase_addr[set_index], phase_val);
	// 设置幅度
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_amp_addr[set_index], amp_val);
	// 设置长度
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_len_addr[set_index], config.m_len);
	// 设置触达时延
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_trig_delay_addr[set_index], config.m_trig_delay);
	// 设置delt x
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_delt_x_addr[set_index], delt_x_val);
}

ChirpOutParam_t get_chirp_out_param(int32_t logical_ch, uint32_t index)
{
	int8_t chip_id;
	int8_t local_ch;
	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t freq_start, phase, amp, len, trig_delay, delt_x;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_freq_addr[index], &freq_start);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_phase_addr[index], &phase);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_amp_addr[index], &amp);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_len_addr[index], &len);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_trig_delay_addr[index], &trig_delay);
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_delt_x_addr[index], &delt_x);

	ChirpOutParam_t config;
	config.m_index = index;
	config.m_len = len;
	config.m_trig_delay = trig_delay;
	config.m_freq_start = (double)freq_start * 2.4e9 / pow(2, 32);
	config.m_freq_end = (double)(delt_x * len + config.m_freq_start) * 2.4e9 / pow(2, 32);
	config.m_phase = (double)phase * 360.0 / pow(2, 32);
	config.m_amp = (double)amp / 65535;
	return config;
}

void set_awg_dds_enable(int32_t logical_ch, uint32_t enable)
{
	uint8_t chip_id;  // 子卡id
	uint8_t local_ch; // 通道数

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, enable);
}

uint32_t get_awg_dds_enable(int32_t logical_ch)
{
	uint8_t chip_id;  // 子卡id
	uint8_t local_ch; // 通道数

	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t enable;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_ch_dds_config[local_ch - 1].m_en, &enable);
	return enable;
}

void set_awg_feadback_enable(int32_t logical_ch, uint32_t enable)
{
	uint8_t chip_id;  // 子卡id
	uint8_t local_ch; // 通道数

	get_awg_route(logical_ch, &chip_id, &local_ch);
	xdma_write_user_space(chip_id, s_AWGUserReg.m_fb_config.m_fb_en, enable);
}

uint32_t get_awg_feadback_enable(int32_t logical_ch)
{
	uint8_t chip_id;  // 子卡id
	uint8_t local_ch; // 通道数

	get_awg_route(logical_ch, &chip_id, &local_ch);

	uint32_t enable;
	xdma_read_user_space(chip_id, s_AWGUserReg.m_fb_config.m_fb_en, &enable);
	return enable;
}
