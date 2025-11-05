#ifndef _SECONDARY_FLASH_H
#define _SECONDARY_FLASH_H
#ifdef __cplusplus
extern "C" {
#endif
#include "PinNames.h"

#define SCLK_FREQ  20000000 //50M. Baudrate to be set should be less than or equal to half of the SPI IpClk(100M).



#if defined (CONFIG_AMEBASMART)
#define SPI0_MOSI				PB_29
#define SPI0_MISO				PB_30
#define SPI0_SCLK				PB_31
#define SPI0_CS				PC_0

#elif defined (CONFIG_AMEBALITE)
#define SPI0_MOSI  _PA_29
#define SPI0_MISO  _PA_30
#define SPI0_SCLK  _PA_28
#define SPI0_CS  _PA_31

#elif defined (CONFIG_AMEBADPLUS)
#define SPI0_MOSI  _PA_27
#define SPI0_MISO  _PA_28
#define SPI0_SCLK  _PA_26
#define SPI0_CS  _PA_12

#else
#define SPI0_MOSI  _PA_27
#define SPI0_MISO  _PA_28
#define SPI0_SCLK  _PA_26
#define SPI0_CS  _PA_12

#endif

/* GD */
#define CMD_WRITE_ENABLE		0x06
#define CMD_WRITE_DISABLE		0x04
#define CMD_READ_STATUS			0x05
#define CMD_READ_STATUS2		0x35
#define CMD_READ_DEVICE_ID		0x9F
#define CMD_READ_DATA			0x03
#define CMD_FAST_READ_DATA		0x0B
#define CMD_PAGE_PROGRAM		0x02
#define CMD_FAST_PAGE_PROGRAM	0xF2
#define CMD_SECTOR_ERASE		0x20
#define CMD_BLOCK_ERASE_32K		0x52
#define CMD_BLOCK_ERASE_64K		0xD8

#define SECTOR_SIZE				4096
#define FLASH_PAGE_SIZE				256

#define WIP_BIT					(1 << 0)
#define WEL_BIT					(1 << 1)

#define MAX_RX_SIZE				PAGE_SIZE
#define MAX_TX_SIZE				PAGE_SIZE

#define CMD_LENGTH_ONE			1			// Command length is 1 byte
#define CMD_LENGTH_TWO			2			// Command length is 2 byte
#define CMD_LENGTH_FOUR 		4			// Command length is 4 byte
#define CMD_LENGTH_FIVE 		5			// Command length is 5 byte

#if (defined CONFIG_AMEBASMART) && !(defined CACHE_LINE_SIZE)
#define CACHE_LINE_SIZE CACHE_L1_DCACHE_LINE_SIZE
#endif


void secondary_flash_spi_init(int hz);
void secondary_flash_get_id(void);
int32_t secondary_flash_erase_sector(uint32_t address);
int32_t secondary_flash_write_stream(uint32_t address, uint32_t length, char *tx_buffer);
int32_t secondary_flash_fast_read_stream(uint32_t address, uint32_t length, char *rx_buffer);
int32_t secondary_flash_read_stream(uint32_t address, uint32_t length, char *rx_buffer);
#ifdef __cplusplus
}
#endif
#endif