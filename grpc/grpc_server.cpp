#include "grpc_server.h"
#include "../pcie/pcie_func.h"
#include "../application/app.h"
#include "../pcie/qa/qa_func.h"
#include "../pcie/common_func.h"
#include "../param_mgr/param_mgr.h"
#include "../pcie/lnawg/lnawg_func.h"
#include "../platform_log/platform_log.h"
#include <string>
#include <grpcpp/grpcpp.h>

using namespace std;

static uint8_t *m_dmaResvBuffer = static_cast<uint8_t *>(malloc(STREAM_CHUNK_SIZE));
extern PcieBoardInfo g_pcie_board_info;
void *RunServer(void *arg)
{
    string server_address("0.0.0.0:50051");
    CommonCMDServiceImpl CommCMDService;
    LNAWGCMDServiceImpl LNAWGCMDService;
    QACMDServiceImpl QACMDService;
    ServerBuilder builder;
    builder.SetMaxReceiveMessageSize(50 * 1024 * 1024);
    builder.SetMaxSendMessageSize(50 * 1024 * 1024);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&CommCMDService);
    builder.RegisterService(&LNAWGCMDService);
    builder.RegisterService(&QACMDService);
    unique_ptr<Server> server(builder.BuildAndStart());
    P_LOG_INFO("Server listening on %s\n", server_address.c_str());
    server->Wait();
    return nullptr;
}

Status CommonCMDServiceImpl::SetDebugParam(ServerContext *context,
                                           const DebugParamRequest *request,
                                           ParamResponse *response)
{
    uint32_t level = request->debug_level();
    P_LOG_DEBUG(" Received SetDebugParam, level = %#x.",
                level);
    plog_set_level(level);
    response->set_success(true);
    return Status::OK;
}

Status CommonCMDServiceImpl::SetRegValue(ServerContext *context,
                                         const RegSetRequest *request,
                                         ParamResponse *response)
{
    uint64_t regAddr = request->regaddr();
    uint32_t regVal = request->regval();
    P_LOG_DEBUG(" Received SetRegValue, address = %#llx, value = %u(Hex:%#x).",
                regAddr,
                regVal,
                regVal);
    common_reg_data_set(regAddr, regVal);
    response->set_success(true);
    return Status::OK;
}

Status CommonCMDServiceImpl::GetRegValue(ServerContext *context,
                                         const RegGetRequest *request,
                                         RegValueResponse *response)
{
    uint64_t regAddr = request->regaddr();
    P_LOG_DEBUG(" Received GetRegValue, address = %#llx.",
                regAddr);
    uint32_t value = common_reg_data_get(regAddr);
    response->set_regvalue(value);
    return Status::OK;
}

Status CommonCMDServiceImpl::SetPCIERegValue(ServerContext *context,
                                             const PCIERegSetRequest *request,
                                             ParamResponse *response)
{
    uint32_t chip = request->chip();
    uint64_t regAddr = request->regaddr();
    uint32_t regVal = request->regval();
    P_LOG_DEBUG(" Received SetPCIERegValue, chip = %u, address = %#llx, value = %u(Hex:%#x).",
                chip,
                regAddr,
                regVal,
                regVal);
    xdma_write_user_space(static_cast<int>(chip), regAddr, regVal);
    response->set_success(true);
    return Status::OK;
}

Status CommonCMDServiceImpl::GetPCIERegValue(ServerContext *context,
                                             const PCIERegGetRequest *request,
                                             PCIERegValueResponse *response)
{
    uint32_t chip = request->chip();
    uint64_t regAddr = request->regaddr();
    P_LOG_DEBUG(" Received GetPCIERegValue, chip = %u, address = %#llx.",
                chip,
                regAddr);
    uint32_t value = common_pcie_user_reg_data_get(chip, regAddr);
    response->set_pcieregvalue(value);
    return Status::OK;
}

Status CommonCMDServiceImpl::SetNetCfgAddr(ServerContext *context,
                                           const SetNetCfgRequest *request,
                                           ParamResponse *response)
{
    uint32_t dhcpEn = request->dhcpen();
    string ipAddr = request->ipaddr();
    string gateWay = request->gateway();
    string macAddr = request->macaddr();
    P_LOG_DEBUG("Set Net Config: dhcpen = %s, ip = %s, gateway = %s, macaddr = %s",
                (dhcpEn == 1) ? "Enable" : "Disable",
                ipAddr.c_str(),
                gateWay.c_str(),
                macAddr.c_str());
    response->set_success(true);
    return Status::OK;
}

Status CommonCMDServiceImpl::GetNetCfgAddr(ServerContext *context,
                                           const GetNetCfgRequest *request,
                                           NetCfgResponse *response)
{
    P_LOG_DEBUG("Get Net Config");
    systemConfig_t *deviceCfg;
    get_device_config(&deviceCfg);
    uint32_t dhcpEn = deviceCfg->dhcpen;
    char ipAddr[IP_STR_LEN];
    char gateWay[IP_STR_LEN];
    char macAddr[MAC_STR_LEN];
    memcpy(ipAddr, deviceCfg->ip, IP_STR_LEN);
    memcpy(gateWay, deviceCfg->gw, IP_STR_LEN);
    memcpy(macAddr, deviceCfg->mac, MAC_STR_LEN);
    response->set_dhcpen(dhcpEn);
    response->set_ipaddr(ipAddr);
    response->set_gateway(gateWay);
    response->set_macaddr(macAddr);
    return Status::OK;
}

Status CommonCMDServiceImpl::SetDevTrigParam(ServerContext *context,
                                             const SetTrigParamRequest *request,
                                             ParamResponse *response)
{
    DevTrigCtrl devTrig;
    devTrig.trigger_source = request->trigsource();
    devTrig.trigger_us = request->trigcontinue();
    devTrig.trigger_times = request->trigtimes();
    devTrig.trigger_continue = request->trigperiod();
    devTrig.trigger_delay = request->trigdelay();
    P_LOG_DEBUG("SetDevTrigParam: Set device trigger param");
    set_dev_trig(devTrig);
    return Status::OK;
}

Status CommonCMDServiceImpl::SetTrigStart(ServerContext *context,
                                          const SetTrigStartRequest *request,
                                          ParamResponse *response)
{
    uint32_t state = request->state();
    if (state)
    {
        set_dev_trig_state(1);
        P_LOG_DEBUG("send soft trigger...\r\n");
        send_software_trig();
    }
    else
    {
        set_dev_trig_state(0);
    }
    return Status::OK;
}

Status CommonCMDServiceImpl::StreamDataSet(ServerContext *context,
                                           ServerReader<SetStreamDataRequest> *reader,
                                           SetStreamResult *response)
{
    P_LOG_INFO("Client connect the stream port: StreamDataSet");

    uint32_t chip = 0;
    uint32_t streamID = 0;
    uint32_t totalResvBytes = 0;
    SetStreamDataRequest request;

    uint8_t subid = 0;
    uint8_t local_ch = 0;
    bool route_inited = false;

    while (reader->Read(&request))
    {
        // 解包当前包
        chip = request.chip();
        streamID = request.streamid();
        uint64_t startAddr = request.startaddr();
        uint32_t currentPackBytes = request.currentpackbytes();
        const string &data = request.data();

        // 累计总长度
        totalResvBytes += currentPackBytes;

        if (!route_inited)
        {
            if (streamID == DEVICE_TYPE_LNAWG)
            {
                get_awg_route(chip, &subid, &local_ch);
            }
            else if (streamID == DEVICE_TYPE_QA)
            {
                get_qa_out_route(chip, &subid, &local_ch);
            }
            route_inited = true;
        }

        int ret = dma_write_data(
            subid,
            startAddr,
            currentPackBytes,
            (uint8_t *)data.data());
        if (ret != 0)
        {
            P_LOG_ERROR("dma_write_data deploy failed! pack_index = %u, address = %#llx, length = %u",
                        request.currentpackage(), startAddr, currentPackBytes);
            response->set_resvtotal(0);
            return Status::OK;
        }

        P_LOG_DEBUG("deploy succeed! pack_index = %u/%u,  address = 0x%llx, length = %u",
                    request.currentpackage(), request.totalpackages(),
                    startAddr, currentPackBytes);
    }

    P_LOG_INFO("StreamDataSet All packs deploy succeed! %u", totalResvBytes);
    response->set_resvtotal(totalResvBytes);
    return Status::OK;
}

Status CommonCMDServiceImpl::StreamDataGet(ServerContext *context,
                                           const GetStreamDataRequest *request,
                                           grpc::ServerWriter<GetStreamResult> *writer)
{
    uint32_t logicCh = request->chip();
    uint32_t streamID = request->streamid();
    uint64_t startAddr = request->startaddr();
    uint32_t requestLen = request->requestlen();
    uint8_t subid, local_ch;
    if (streamID == DEVICE_TYPE_LNAWG)
    {
        get_awg_route(logicCh, &subid, &local_ch);
    }
    else if (streamID == DEVICE_TYPE_QA)
    {
        get_qa_in_route(logicCh, &subid, &local_ch);
    }

    P_LOG_INFO("StreamDataGet: logic_ch = %d, chip=%u, startAddr=0x%llx, requestLen=%u",
               logicCh, subid, (unsigned long long)startAddr, requestLen);

    // 边界检查
    if (requestLen == 0)
    {
        P_LOG_ERROR("StreamDataGet: request length is 0");
        return Status(grpc::INVALID_ARGUMENT, "Request length cannot be 0");
    }

    // 计算总包数
    uint32_t totalPackages = (requestLen + STREAM_CHUNK_SIZE - 1) / STREAM_CHUNK_SIZE;
    uint32_t currentPackage = 0;
    uint64_t currentAddr = startAddr;
    uint32_t remainingLen = requestLen;
    memset(m_dmaResvBuffer, 0, sizeof(m_dmaResvBuffer));
    uint8_t *dmaBuf = m_dmaResvBuffer;
    dma_read_data(subid, startAddr, requestLen, dmaBuf);
    while (remainingLen > 0)
    {
        // 计算当前包的大小
        uint32_t currentPackBytes = min(STREAM_CHUNK_SIZE, remainingLen);
        // 构造响应包
        GetStreamResult result;
        result.set_streamid(E_AWG_DATA);
        result.set_totalpackages(totalPackages);
        result.set_currentpackage(currentPackage);
        result.set_currentpackbytes(currentPackBytes);
        result.set_startaddr(currentAddr);
        result.set_data(dmaBuf, currentPackBytes);
        if (!writer->Write(result))
        {
            P_LOG_ERROR("StreamDataGet: Failed to write package %u/%u, client disconnected",
                        currentPackage, totalPackages);
            return Status::CANCELLED;
        }
        P_LOG_DEBUG("StreamDataGet: Sent package %u/%u, addr=0x%llx, bytes=%u",
                    currentPackage, totalPackages, (unsigned long long)currentAddr, currentPackBytes);
        dmaBuf += currentPackBytes;
        currentAddr += currentPackBytes;
        remainingLen -= currentPackBytes;
        currentPackage++;
    }
    P_LOG_INFO("StreamDataGet: All %u packages sent successfully, total bytes=%u",
               totalPackages, requestLen);
    return Status::OK;
}

Status CommonCMDServiceImpl::GetIDN(ServerContext *context,
                                    const GetIDNRequest *request,
                                    GetIDNResponse *response)
{
    systemConfig_t *deviceCfg;
    get_device_config(&deviceCfg);
    static char version_ext[STR_SMALL_LEN * 2];
    snprintf(version_ext, sizeof(version_ext), "%s,%s", deviceCfg->version, deviceCfg->mac);
    response->set_idn(std::string(deviceCfg->vendors) + "," +
                      std::string(deviceCfg->model) + "," +
                      std::string(deviceCfg->serial) + "," +
                      std::string(version_ext));
    return Status::OK;
}

Status CommonCMDServiceImpl::GetDeviceInfo(ServerContext *context,
                                           const GetDeviceInfoRequest *request,
                                           GetDeviceInfoResponse *response)
{
    P_LOG_DEBUG("CommonCMDServiceImpl::GetDeviceInfo");
    response->set_awgchcount((uint32_t)g_pcie_board_info.awg_ch_num);
    response->set_qainchcount((uint32_t)g_pcie_board_info.qa_in_ch_num);
    response->set_qaoutchcount((uint32_t)g_pcie_board_info.qa_out_ch_num);
    response->clear_qasubtype();
    for (int i = 0; i < BOARD_NUM_MAX; i++)
    {
        uint32_t val = g_pcie_board_info.qa_sub_type[i];
        if (val == 0)
            break;
        response->add_qasubtype(val);
    }
    return Status::OK;
}

// -------------------------------------LNAWGCMDServiceImpl------------------------------------------------------
Status LNAWGCMDServiceImpl::SetMode(ServerContext *context,
                                    const ModeSetRequest *request,
                                    ParamResponse *response)
{
    set_awg_ch_mode(request->logical_ch(), request->mode());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetMode(ServerContext *context,
                                    const ModeGetRequest *request,
                                    ModeGetResponse *response)
{
    uint32_t mode = get_awg_ch_mode(request->logical_ch());
    response->set_mode(mode);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetRun(ServerContext *context,
                                   const RunSetRequest *request,
                                   ParamResponse *response)
{
    set_awg_ch_run(request->logical_ch(), request->state());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetRun(ServerContext *context,
                                   const RunGetRequest *request,
                                   RunGetResponse *response)
{
    uint32_t state = get_awg_ch_run(request->logical_ch());
    if (state == 0xFFFFFFFFU)
    {
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Get run state failed: invalid mode or channel");
    }
    response->set_state(state);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetExtSource(ServerContext *context,
                                         const ExtSourceSetRequest *request,
                                         ParamResponse *response)
{
    set_awg_ch_ext_src(request->logical_ch(), request->source());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetExtSource(ServerContext *context,
                                         const ExtSourceGetRequest *request,
                                         ExtSourceGetResponse *response)
{
    uint32_t source = get_awg_ch_ext_src(request->logical_ch());
    response->set_source(source);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetRange(ServerContext *context,
                                     const RangeSetRequest *request,
                                     ParamResponse *response)
{
    set_awg_ch_range(request->logical_ch(), request->range());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetRange(ServerContext *context,
                                     const RangeGetRequest *request,
                                     RangeGetResponse *response)
{
    uint32_t range = get_awg_ch_range(request->logical_ch());
    response->set_range(range);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetOffset(ServerContext *context,
                                      const OffsetSetRequest *request,
                                      ParamResponse *response)
{
    set_awg_ch_offset(request->logical_ch(), request->offset());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetOffset(ServerContext *context,
                                      const OffsetGetRequest *request,
                                      OffsetGetResponse *response)
{
    double offset = get_awg_ch_offset(request->logical_ch());
    response->set_offset(offset);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetSegmentCount(ServerContext *context,
                                            const SegmentCountSetRequest *request,
                                            ParamResponse *response)
{
    set_awg_ch_segment_count(request->logical_ch(), request->segment_count());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetSegmentCount(ServerContext *context,
                                            const SegmentCountGetRequest *request,
                                            SegmentCountGetResponse *response)
{
    uint32_t segment_count = get_awg_ch_segment_count(request->logical_ch());
    response->set_segment_count(segment_count);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetLoopCount(ServerContext *context,
                                         const LoopCountSetRequest *request,
                                         ParamResponse *response)
{
    set_awg_ch_segment_loop(request->logical_ch(), request->loop_count());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetLoopCount(ServerContext *context,
                                         const LoopCountGetRequest *request,
                                         LoopCountGetResponse *response)
{
    uint32_t loop_count = get_awg_ch_segment_loop(request->logical_ch());
    response->set_loop_count(loop_count);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetDDSParam(ServerContext *context,
                                        const DDSParamSetRequest *request,
                                        ParamResponse *response)
{
    DDSConfigParam_t config_param;
    config_param.m_index = request->config().index();
    config_param.m_len = request->config().len();
    config_param.m_trig_delay = request->config().trig_delay();
    config_param.m_freq = request->config().freq();
    config_param.m_phase = request->config().phase();
    config_param.m_AMP = request->config().amp();
    set_awg_dds_config(request->logical_ch(), config_param);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetDDSParam(ServerContext *context,
                                        const DDSParamGetRequest *request,
                                        DDSParamGetResponse *response)
{
    DDSConfigParam_t config_param = get_awg_dds_config(request->logical_ch(), request->index());
    // response->set_config->set_index(config_param.m_index);
    // response->set_config->set_len(config_param.m_len);
    // response->set_config->set_trig_delay(config_param.m_trig_delay);
    // response->set_config->set_freq(config_param.m_freq);
    // response->set_config->set_phase(config_param.m_phase);
    // response->set_config->set_amp(config_param.m_AMP);
    auto *cfg = response->mutable_config();
    cfg->set_index(config_param.m_index);
    cfg->set_len(config_param.m_len);
    cfg->set_trig_delay(config_param.m_trig_delay);
    cfg->set_freq(config_param.m_freq);
    cfg->set_phase(config_param.m_phase);
    cfg->set_amp(config_param.m_AMP);
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetChirpOutParam(ServerContext *context,
                                             const ChirpOutParamSetRequest *request,
                                             ParamResponse *response)
{
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetChirpOutParam(ServerContext *context,
                                             const ChirpOutParamGetRequest *request,
                                             ChirpOutParamGetResponse *response)
{
    return Status::OK;
}

Status LNAWGCMDServiceImpl::SetDDSEnable(ServerContext *context,
                                         const DDSEnableSetRequest *request,
                                         ParamResponse *response)
{
    set_awg_dds_enable(request->logical_ch(), request->enable());
    return Status::OK;
}

Status LNAWGCMDServiceImpl::GetDDSEnable(ServerContext *context,
                                         const DDSEnableGetRequest *request,
                                         DDSEnableGetResponse *response)
{
    uint32_t enable = get_awg_dds_enable(request->logical_ch());
    response->set_enable(enable);
    return Status::OK;
}

Status QACMDServiceImpl::SetTrigSour(ServerContext *context,
                                     const SetTrigSourRequest *request,
                                     ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t source = request->source();
    P_LOG_DEBUG("SetTrigSour: Set qa trigger source...%d, %d.", ch, source);
    qa_trigger_source_ctrl(ch, source);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::SetSoftTrig(ServerContext *context,
                                     const SetSoftTrigRequest *request,
                                     ParamResponse *response)
{
    uint32_t ch = request->logicch();
    P_LOG_DEBUG("SetSoftTrig: Set soft trigger once...%d.", ch);
    qa_soft_trig_signal(ch);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::SetDACDDSConfig(ServerContext *context,
                                         const SetDACDDSConfigRequest *request,
                                         ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t index = request->index();
    DACDDSConfig_t DACDDSCfg;
    DACDDSCfg.m_DAC_freq = request->dacfreq();
    DACDDSCfg.m_DAC_Phase = request->dacphase();
    DACDDSCfg.m_DAC_AMP = request->dacamp();
    P_LOG_DEBUG("SetDACDDSConfig: Set qa dac dds config...%d, %d.", ch, index);
    qa_dac_dds_config(ch, index, DACDDSCfg);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetDACDDSConfig(ServerContext *context,
                                         const GetDACDDSConfigRequest *request,
                                         GetDACDDSConfigResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetDACPlayParam(ServerContext *context,
                                         const SetDACPlayParamRequest *request,
                                         ParamResponse *response)
{
    uint32_t ch = request->logicch();
    DACDDSPlayParam_t DACPlayParam;
    DACPlayParam.m_data_len = request->datalen();
    DACPlayParam.m_play_times = request->playtimes();
    DACPlayParam.m_play_delay = request->playdelay();
    DACPlayParam.m_play_mode = request->playmode();
    P_LOG_DEBUG("SetDACPlayParam: Set qa dac play param...%d.", ch);
    qa_dac_play_param(ch, DACPlayParam);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetDACPlayParam(ServerContext *context,
                                         const GetDACPlayParamRequest *request,
                                         GetDACPlayParamResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetDACStartStop(ServerContext *context,
                                         const SetDACStartStopRequest *request,
                                         ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t en = request->chenable();
    P_LOG_DEBUG("SetSampleParam: Set qa dac start stop...%d, %d.", ch, en);
    qa_dac_start_stop_play(ch, en);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetDACStartStop(ServerContext *context,
                                         const GetDACStartStopRequest *request,
                                         GetDACStartStopResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetSampleParam(ServerContext *context,
                                        const SetSampleParamRequest *request,
                                        ParamResponse *response)
{
    uint32_t ch = request->logicch();
    SampleConfigReg_t sampleCfg;
    sampleCfg.m_sample_length = request->samplelength();
    sampleCfg.m_sample_delay = request->sampledelay();
    sampleCfg.m_sample_num = request->samplenum();
    P_LOG_DEBUG("SetSampleParam: Set qa sample config...%d.", ch);
    qa_sample_config(ch, sampleCfg);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetSampleParam(ServerContext *context,
                                        const GetSampleParamRequest *request,
                                        GetSampleParamResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetSampleStartStop(ServerContext *context,
                                            const SetSampleStartStopRequest *request,
                                            ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t en = request->chenable();
    P_LOG_DEBUG("SetSampleStartStop: Set qa sample start stop...%d, %d.", ch, en);
    qa_sample_start_stop(ch, en);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetSampleStartStop(ServerContext *context,
                                            const GetSampleStartStopRequest *request,
                                            GetSampleStartStopResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetADCConfig(ServerContext *context,
                                      const SetADCConfigRequest *request,
                                      ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t index = request->index();
    ADCConfig_t ADCCfg;
    ADCCfg.m_adc_freq = request->adcfreq();
    ADCCfg.m_adc_phase = request->adcphase();
    P_LOG_DEBUG("SetADCConfig: Set qa adc config...%d, %d.", ch, index);
    qa_adc_config(ch, index, ADCCfg);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetADCConfig(ServerContext *context,
                                      const GetADCConfigRequest *request,
                                      GetADCConfigResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetADCPlayParam(ServerContext *context,
                                         const SetADCPlayParamRequest *request,
                                         ParamResponse *response)
{
    uint32_t ch = request->logicch();
    ADCPlayParam_t ADCPlayParam;
    ADCPlayParam.m_adc_data_len = request->datalen();
    ADCPlayParam.m_adc_play_times = request->playtimes();
    ADCPlayParam.m_adc_play_delay = request->playdelay();
    ADCPlayParam.m_adc_play_mode = request->playmode();
    qa_adc_play_param(ch, ADCPlayParam);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetADCPlayParam(ServerContext *context,
                                         const GetADCPlayParamRequest *request,
                                         GetADCPlayParamResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::SetADCStartStop(ServerContext *context,
                                         const SetADCStartStopRequest *request,
                                         ParamResponse *response)
{
    uint32_t ch = request->logicch();
    uint32_t en = request->chenable();
    P_LOG_DEBUG("SetADCStartStop: Set qa adc start stop...%d, %d.", ch, en);
    qa_adc_start_stop_play(ch, en);
    response->set_success(true);
    return Status::OK;
}

Status QACMDServiceImpl::GetADCStartStop(ServerContext *context,
                                         const GetADCStartStopRequest *request,
                                         GetADCStartStopResponse *response)
{
    uint32_t ch = request->logicch();
    return Status::OK;
}

Status QACMDServiceImpl::GetSampleState(ServerContext *context,
                                        const GetSampleStateRequest *request,
                                        ParamResponse *response)
{
    uint32_t ch = request->logicch();
    P_LOG_DEBUG("GetSampleState: Get sample state...%d, %d.", ch);
    if (qa_get_sample_state(ch) < 0)
    {
        response->set_success(false);
    }
    else
    {
        response->set_success(true);
    }
    return Status::OK;
}
Status QACMDServiceImpl::GetDemodeState(ServerContext *context,
                                        const GetDemodeStateRequest *request,
                                        ParamResponse *response)
{
    uint32_t ch = request->logicch();
    P_LOG_DEBUG("GetDemodeState: Get demod state...%d, %d.", ch);
    if (qa_get_demo_state(ch) < 0)
    {
        response->set_success(false);
    }
    else
    {
        response->set_success(true);
    }
    return Status::OK;
}
