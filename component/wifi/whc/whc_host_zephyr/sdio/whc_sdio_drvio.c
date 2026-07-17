
#include <zephyr/sys/atomic.h>
#include <zephyr/sd/sdio.h>
#include "rtw_whc_common.h"

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
