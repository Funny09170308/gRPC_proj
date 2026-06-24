#include "spi2_test.h"
#include "spi_func.h"
#include "../platform_log/platform_log.h"

#define spi2_dev_path
#define BUFFER_SIZE1 12

static int fd = -1;
uint8_t ReadBuffer1[BUFFER_SIZE1] = {0};
uint8_t WriteBuffer1[BUFFER_SIZE1] = {0};

void spi2_init(void)
{
    fd = spi_open_device(SPI2_DEV_PATH);
}

void spi2_test(void)
{
    WriteBuffer1[0] = 0x0a + 0x80;//read
	WriteBuffer1[1] = 0x00;
	WriteBuffer1[2] = 0x00;
	WriteBuffer1[3] = 0x00;
    spi_transfer_data(fd, ReadBuffer1, WriteBuffer1, 4);
    usleep(500) ;
    for (uint8_t index = 0; index < BUFFER_SIZE1; ++index)
    {
        P_LOG_INFO("ReadBuffer1[%d] = %d\n", index, ReadBuffer1[index]);
    }

	if (ReadBuffer1[3] == 0xe6) {
		P_LOG_INFO("spi_init_ok\n\r");
	}else{
		P_LOG_INFO("spi_init_fail\n\r");
	}
}

void spi2_close(void)
{
    close(fd);
}
