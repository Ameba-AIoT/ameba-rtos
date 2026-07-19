#include "whc_host.h"
#include "wwd_constants.h"                  /* For wwd_result_t */
#include "wwd_sdio_interface.h"

#define USER_BLK_SZ SDIO_64B_BLOCK
#define BASIC_FUNC 0	// func0
#define USER_FUNC 1		// func1: SDIO_WIFI

#define MAX_BLOCKS 511
#define MAX_BYTE_SIZE 511

rtos_mutex_t sdio_lock;

// Description:
//	The following mapping translates domain ID + offset to the SDIO command address
//	for local registers, TX FIFO, and RX FIFO domains.
static void sdio_get_cmdaddr(uint8_t DomainID, uint32_t Param, uint32_t *pCmdAddr)
{
	switch (DomainID) {
	case SDIO_LOCAL_DOMAIN_ID:
		*pCmdAddr = ((SDIO_LOCAL_DOMAIN_ID << 13) | (Param & SDIO_LOCAL_MSK));
		break;

	case SDIO_TX_FIFO_DOMAIN_ID:
		*pCmdAddr = ((SDIO_TX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_TX_FIFO_MSK));
		break;

	case SDIO_RX_FIFO_DOMAIN_ID:
		*pCmdAddr = ((SDIO_RX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_RX_FIFO_MSK));
		break;

	default:
		break;
	}
}

/*
* Read a byte from SDIO address addr into pdata under the sdio_lock.
* @return 0: success
* @return 1: mutex take timeout
*/
int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata)
{
	if (SUCCESS != rtos_mutex_take(sdio_lock, 1000)) {
		printf("fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	sdio_readb(addr, pdata);
	rtos_mutex_give(sdio_lock);

	return 0;
}

int sdio_write_byte(void *func, uint32_t addr, uint8_t pdata)
{
	if (SUCCESS != rtos_mutex_take(sdio_lock, 1000)) {
		printf("fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	sdio_writeb(addr, pdata);
	rtos_mutex_give(sdio_lock);

	return 0;
}


int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	if (SUCCESS != rtos_mutex_take(sdio_lock, 1000)) {
		printf("fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}
	sdio_memcpy_fromio(addr, len, pdata);

	rtos_mutex_give(sdio_lock);

	return 0;
}

int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	if (SUCCESS != rtos_mutex_take(sdio_lock, 1000)) {
		printf("fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	sdio_memcpy_toio(addr, len, pdata);
	rtos_mutex_give(sdio_lock);

	return 0;
}

int mmc_io_rw_direct(wwd_bus_transfer_direction_t direction, uint32_t addr, uint8_t value, uint8_t *out)
{
	return wwd_bus_sdio_cmd52(direction, USER_FUNC, addr, value, RESPONSE_NEEDED, out);
}

int sdio_readb(uint32_t addr, uint8_t *out)
{
	if (out == NULL) {
		return WWD_BADARG;
	}

	return mmc_io_rw_direct(BUS_READ, addr, NULL, out);
}

int sdio_writeb(uint32_t addr, uint8_t value)
{
	/* if raw = 0, out should be NULL */
	return mmc_io_rw_direct(BUS_WRITE, addr, value, NULL);
}

int sdio_io_rw_ext_helper(wwd_bus_transfer_direction_t direction, uint32_t addr, uint16_t byte_cnt, uint8_t *data)
{
	uint16_t remainder = byte_cnt;
	uint8_t *buf = data;
	uint16_t size;

	/* Do the bulk of the transfer using block mode (if supported). */
	while (remainder > USER_BLK_SZ) {
		uint32_t blocks;

		blocks = remainder / USER_BLK_SZ;

		if (blocks > MAX_BLOCKS) {
			blocks = MAX_BLOCKS;
		}

		size = blocks * USER_BLK_SZ;

		VERIFY_RESULT(wwd_bus_sdio_cmd53(direction, USER_FUNC, SDIO_BLOCK_MODE, addr, size, buf, RESPONSE_NEEDED, NULL));

		remainder -= size;
		buf += size;

		// todo: if local region, no inc; if tx/rx fifo, inc
#ifdef TODO
		if (incr_addr) {
			addr += size;
		}
#endif
	}

	/* Write the remainder using byte mode. */
	while (remainder > 0) {
		// TODO: replace sdio_max_byte_size(func) with MAX_BYTE_SIZE

		size = remainder > MAX_BYTE_SIZE ? MAX_BYTE_SIZE : remainder;

		VERIFY_RESULT(wwd_bus_sdio_cmd53(direction, USER_FUNC, SDIO_BYTE_MODE, addr, size, buf, RESPONSE_NEEDED, NULL));

		remainder -= size;
		buf += size;

		// todo: if local region, no inc; if tx/rx fifo, inc
#ifdef TODO
		if (incr_addr) {
			addr += size;
		}
#endif
	}
	return WWD_SUCCESS;
}

int sdio_memcpy_fromio(uint32_t addr, uint16_t byte_cnt, uint8_t *data)
{
	return sdio_io_rw_ext_helper(BUS_READ, addr, byte_cnt, data);
}

int sdio_memcpy_toio(uint32_t addr, uint16_t byte_cnt, uint8_t *data)
{
	return sdio_io_rw_ext_helper(BUS_WRITE, addr, byte_cnt, data);
}

uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint8_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	sdio_read_byte(priv->func, ftaddr, &val);
	return val;
}

uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint16_t val = 0;
	uint32_t i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	for (i = 0; i < 2; i++) {
		sdio_read_byte(priv->func, ftaddr + i, (uint8_t *)(&val) + i);
	}
	return val;
}

uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint32_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	/* CMD53 4-byte read; little-endian host places byte@lowest-addr as LSB */
	sdio_read_fifo(priv->func, ftaddr, (uint8_t *)&val, 4);
	return val;
}

/* Read from RX FIFO; round size to block size so it finishes in one command. */
uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	s32 err;

	cnt = _RND4(cnt);
	if (cnt > priv->block_transfer_len) {
		cnt = _RND(cnt, priv->block_transfer_len);
	}

	sdio_get_cmdaddr(addr, 0, &addr);

	err = sdio_read_fifo(priv->func, addr, mem, cnt);
	if (err) {
		return FALSE;
	}
	return TRUE;
}

s32 sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	uint32_t ftaddr;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	return sdio_write_byte(priv->func, ftaddr, val);
}

s32 sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	uint32_t ftaddr;
	s32 err = 0;
	uint32_t i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	for (i = 0; i < 2; i++) {
		err = sdio_write_byte(priv->func, ftaddr + i, *((uint8_t *)(&val) + i));
		if (err) {
			printf("%s: sdio_write_byte fail, err:%d\n", __func__, err);
		}
	}
	return err;
}

/* addr should always be 4-byte aligned */
s32 sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	uint32_t ftaddr;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	return sdio_write_fifo(priv->func, ftaddr, (uint8_t *)&val, 4);
}

/* Write to TX FIFO; align size to block size so it finishes in one command. */
uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	s32 err;

	cnt = _RND4(cnt);
	if (cnt > priv->block_transfer_len) {
		cnt = _RND(cnt, priv->block_transfer_len);
	}

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	err = sdio_write_fifo(priv->func, addr, mem, cnt);
	if (err) {
		printf("%s, error=%d\n", __func__, err);
		return FALSE;
	}
	return TRUE;
}
