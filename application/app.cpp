#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "app.h"
#include "../public/public.h"
#include "../pcie/pcie_func.h"
#include "../pcie/common_func.h"
#include "../grpc/grpc_server.h"
#include "../param_mgr/param_mgr.h"
#include "../platform_log/platform_log.h"

/*
 * 这里故意屏蔽底层 dma_xxx 的返回值差异。
 * 如果你底层已经有明确返回值，可在这里自行补充错误判定。
 */
static int app_dma_write_data(uint32_t chip_id, uint64_t addr, uint32_t len, const uint8_t *buf)
{
    dma_write_data((int)chip_id, addr, len, (uint8_t *)buf);
    return 0;
}

static int app_dma_read_data(uint32_t chip_id, uint64_t addr, uint32_t len, uint8_t *buf)
{
    dma_read_data((int)chip_id, addr, len, buf);
    return 0;
}

void task_creat(void)
{
    pthread_t grpcServer_tid;
    if (pthread_create(&grpcServer_tid, NULL, RunServer, NULL) != 0)
    {
        P_LOG_ERROR("create required thread failed\n");
        return;
    }
    pthread_detach(grpcServer_tid);
    status_led_ctrl(1);
    P_LOG_INFO("Task init finish!\r\n");
}

void slave_card_detect(void)
{
    PcieBoardInfo *g_pcie_board_info = get_pcie_board_info();
    int cardCount = get_subcard_count();
    // TODO:此检测方式不支持混插, 否则会误报
    if (g_pcie_board_info->awg_board_num != 0)
    {
        if (g_pcie_board_info->awg_board_num != 4)
        {
            status_led_ctrl(0);
        }
    }
    else if (g_pcie_board_info->qa_board_num != 0)
    {
        if (g_pcie_board_info->qa_board_num != 2)
        {
            status_led_ctrl(0);
        }
    }
}
