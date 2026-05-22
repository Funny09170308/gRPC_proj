#ifndef APP_H
#define APP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_SERVER_NUM          1
#define APP_MAX_CLIENTS         16
#define APP_SERVER_PORT         9090
#define APP_PKT_MAGIC           0x55AA5A5Au
#define APP_MAX_PAYLOAD_SIZE    (200 * 1024)         /* 单包最大负载，需与上位机保持一致 */
#define APP_MAX_READ_REQ_SIZE   (4 * 1024 * 1024)    /* 单次回读总长度上限 */
#define APP_SOCKET_BACKLOG      16

#pragma pack(push, 1)
typedef struct
{
    uint32_t magic;         /* 固定 0x55AA5A5A */
    uint32_t cmd;           /* 见 app_pkt_cmd_e */
    uint32_t chip_id;       /* 子卡号 */
    uint32_t total_packets; /* 总包数 */
    uint32_t packet_index;  /* 当前包序号 */
    uint32_t payload_len;   /* 当前包 payload 字节数 */
    uint64_t addr;          /* PCIe 起始地址 */
	uint32_t type;          /* 数据类型，用于区分AWG和QA*/
} app_pkt_hdr_t;

typedef struct
{
    uint32_t magic;
    uint32_t cmd;           /* ACK / ERR */
    int32_t  status;        /* 见 app_pkt_status_e */
    uint32_t detail;        /* 额外信息：包序号/错误码/总包数等 */
} app_ack_t;
#pragma pack(pop)

typedef enum
{
    APP_PKT_CMD_WRITE    = 1,
    APP_PKT_CMD_READ_REQ = 2,
    APP_PKT_CMD_READ_RSP = 3,
    APP_PKT_CMD_ACK      = 4,
    APP_PKT_CMD_ERR      = 5,
} app_pkt_cmd_e;

typedef enum
{
    APP_PKT_STATUS_OK              = 0,
    APP_PKT_STATUS_BAD_MAGIC       = -1,
    APP_PKT_STATUS_BAD_CMD         = -2,
    APP_PKT_STATUS_BAD_LENGTH      = -3,
    APP_PKT_STATUS_RECV_FAIL       = -4,
    APP_PKT_STATUS_SEND_FAIL       = -5,
    APP_PKT_STATUS_CTX_UNAVAILABLE = -6,
    APP_PKT_STATUS_INTERNAL_ERR    = -7,
} app_pkt_status_e;

void *rpc_thread(void *arg);
void *tcp_thread(void *arg);
void task_creat(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */

