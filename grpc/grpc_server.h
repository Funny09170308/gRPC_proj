#ifndef __GRPC_SERVER_H__
#define __GRPC_SERVER_H__

#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/sync_stream.h>
#include <grpcpp/server_context.h>

#include "../proto_cc_gen/common_cmd.pb.h"      // 消息类型
#include "../proto_cc_gen/common_cmd.grpc.pb.h" // 服务接口
#include "../proto_cc_gen/qa_cmd.pb.h"          // 消息类型
#include "../proto_cc_gen/qa_cmd.grpc.pb.h"     // 服务接口
#include "../proto_cc_gen/lnawg_cmd.pb.h"       // 消息类型
#include "../proto_cc_gen/lnawg_cmd.grpc.pb.h"  // 服务接口

using namespace std;
using namespace grpc;
using namespace silicon_based;

typedef enum
{
    E_AWG_DATA = 1,
} StreamID;

constexpr uint32_t STREAM_CHUNK_SIZE = 4 * 1024 * 1024 * 10;

class CommonCMDServiceImpl final : public CommonCMDService::Service
{
public:
    Status GetIDN(ServerContext *context,
                  const GetIDNRequest *request,
                  GetIDNResponse *response) override;
    Status GetDeviceInfo(ServerContext *context,
                         const GetDeviceInfoRequest *request,
                         GetDeviceInfoResponse *response) override;
    Status SetDebugParam(ServerContext *context,
                         const DebugParamRequest *request,
                         ParamResponse *response) override;
    Status SetRegValue(ServerContext *context,
                       const RegSetRequest *request,
                       ParamResponse *response) override;
    Status GetRegValue(ServerContext *context,
                       const RegGetRequest *request,
                       RegValueResponse *response) override;
    Status SetPCIERegValue(ServerContext *context,
                           const PCIERegSetRequest *request,
                           ParamResponse *response) override;
    Status GetPCIERegValue(ServerContext *context,
                           const PCIERegGetRequest *request,
                           PCIERegValueResponse *response) override;
    Status StreamDataSet(ServerContext *context,
                         ServerReader<SetStreamDataRequest> *reader,
                         SetStreamResult *response) override;
    Status StreamDataGet(ServerContext *context,
                         const GetStreamDataRequest *request,
                         ServerWriter<GetStreamResult> *writer) override;
    Status SetNetCfgAddr(ServerContext *context,
                         const SetNetCfgRequest *request,
                         ParamResponse *response) override;
    Status GetNetCfgAddr(ServerContext *context,
                         const GetNetCfgRequest *request,
                         NetCfgResponse *response) override;
    Status SetDevTrigParam(ServerContext *context,
                           const SetTrigParamRequest *request,
                           ParamResponse *response) override;
    Status SetTrigStart(ServerContext *context,
                        const SetTrigStartRequest *request,
                        ParamResponse *response) override;

private:
    vector<uint8_t> m_resvBuffer;
};

class LNAWGCMDServiceImpl final : public LNAWGCMDService::Service
{
public: // QA function
    Status SetMode(ServerContext *context,
                   const ModeSetRequest *request,
                   ParamResponse *response) override;

    Status GetMode(ServerContext *context,
                   const ModeGetRequest *request,
                   ModeGetResponse *response) override;

    Status SetRun(ServerContext *context,
                  const RunSetRequest *request,
                  ParamResponse *response) override;

    Status GetRun(ServerContext *context,
                  const RunGetRequest *request,
                  RunGetResponse *response) override;

    Status SetExtSource(ServerContext *context,
                        const ExtSourceSetRequest *request,
                        ParamResponse *response) override;

    Status GetExtSource(ServerContext *context,
                        const ExtSourceGetRequest *request,
                        ExtSourceGetResponse *response) override;

    Status SetRange(ServerContext *context,
                    const RangeSetRequest *request,
                    ParamResponse *response) override;

    Status GetRange(ServerContext *context,
                    const RangeGetRequest *request,
                    RangeGetResponse *response) override;

    Status SetOffset(ServerContext *context,
                     const OffsetSetRequest *request,
                     ParamResponse *response) override;

    Status GetOffset(ServerContext *context,
                     const OffsetGetRequest *request,
                     OffsetGetResponse *response) override;

    Status SetSegmentCount(ServerContext *context,
                           const SegmentCountSetRequest *request,
                           ParamResponse *response) override;

    Status GetSegmentCount(ServerContext *context,
                           const SegmentCountGetRequest *request,
                           SegmentCountGetResponse *response) override;

    Status SetLoopCount(ServerContext *context,
                        const LoopCountSetRequest *request,
                        ParamResponse *response) override;

    Status GetLoopCount(ServerContext *context,
                        const LoopCountGetRequest *request,
                        LoopCountGetResponse *response) override;

    Status SetDDSParam(ServerContext *context,
                       const DDSParamSetRequest *request,
                       ParamResponse *response) override;

    Status GetDDSParam(ServerContext *context,
                       const DDSParamGetRequest *request,
                       DDSParamGetResponse *response) override;

    Status SetChirpOutParam(ServerContext *context,
                            const ChirpOutParamSetRequest *request,
                            ParamResponse *response) override;

    Status GetChirpOutParam(ServerContext *context,
                            const ChirpOutParamGetRequest *request,
                            ChirpOutParamGetResponse *response) override;

    Status SetDDSEnable(ServerContext *context,
                        const DDSEnableSetRequest *request,
                        ParamResponse *response) override;

    Status GetDDSEnable(ServerContext *context,
                        const DDSEnableGetRequest *request,
                        DDSEnableGetResponse *response) override;
};

class QACMDServiceImpl final : public QACMDService::Service
{
public: // QA function
    Status SetTrigSour(ServerContext *context,
                       const SetTrigSourRequest *request,
                       ParamResponse *response) override;
    Status SetSoftTrig(ServerContext *context,
                       const SetSoftTrigRequest *request,
                       ParamResponse *response) override;
    Status SetDACDDSConfig(ServerContext *context,
                           const SetDACDDSConfigRequest *request,
                           ParamResponse *response) override;
    Status GetDACDDSConfig(ServerContext *context,
                           const GetDACDDSConfigRequest *request,
                           GetDACDDSConfigResponse *response) override;
    Status SetDACPlayParam(ServerContext *context,
                           const SetDACPlayParamRequest *request,
                           ParamResponse *response) override;
    Status GetDACPlayParam(ServerContext *context,
                           const GetDACPlayParamRequest *request,
                           GetDACPlayParamResponse *response) override;
    Status SetDACStartStop(ServerContext *context,
                           const SetDACStartStopRequest *request,
                           ParamResponse *response) override;
    Status GetDACStartStop(ServerContext *context,
                           const GetDACStartStopRequest *request,
                           GetDACStartStopResponse *response) override;
    Status SetSampleParam(ServerContext *context,
                          const SetSampleParamRequest *request,
                          ParamResponse *response) override;
    Status GetSampleParam(ServerContext *context,
                          const GetSampleParamRequest *request,
                          GetSampleParamResponse *response) override;
    Status SetSampleStartStop(ServerContext *context,
                              const SetSampleStartStopRequest *request,
                              ParamResponse *response) override;
    Status GetSampleStartStop(ServerContext *context,
                              const GetSampleStartStopRequest *request,
                              GetSampleStartStopResponse *response) override;
    Status SetADCConfig(ServerContext *context,
                        const SetADCConfigRequest *request,
                        ParamResponse *response) override;
    Status GetADCConfig(ServerContext *context,
                        const GetADCConfigRequest *request,
                        GetADCConfigResponse *response) override;
    Status SetADCPlayParam(ServerContext *context,
                           const SetADCPlayParamRequest *request,
                           ParamResponse *response) override;
    Status GetADCPlayParam(ServerContext *context,
                           const GetADCPlayParamRequest *request,
                           GetADCPlayParamResponse *response) override;
    Status SetADCStartStop(ServerContext *context,
                           const SetADCStartStopRequest *request,
                           ParamResponse *response) override;
    Status GetADCStartStop(ServerContext *context,
                           const GetADCStartStopRequest *request,
                           GetADCStartStopResponse *response) override;
};

#ifdef __cplusplus
extern "C"
{
#endif

    void *RunServer(void *arg);

#ifdef __cplusplus
}
#endif

#endif // __GRPC_SERVER_H__
