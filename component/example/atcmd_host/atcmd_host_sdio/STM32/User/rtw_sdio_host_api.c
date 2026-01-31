#include "sdio_api_ext.h"
#include "stdio.h"
#include "string.h"

extern size_t xFreeBytesRemaining;

u8 g_sdio_tx_buf[3000];

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

s32 sdio_read_common(uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	s32 err;
	uint8_t *ptmpbuf;
	uint32_t n, retry = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	if (cnt == 1) {
		err = sdio_readb(addr, pbuf);

		if (err) {
			int j;
			err = 0;
			for (j = 0; j < SD_IO_TRY_CNT; j++) {
				err = sdio_readb(addr, pbuf);
				if (!err) {
					break;
				}
			}
			if (j == SD_IO_TRY_CNT) {
				printf("%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
				return err;
			}
		}
	} else {
		n = _RND4(cnt);
		ptmpbuf = (uint8_t *)pvPortMalloc(n);
		if (!ptmpbuf) {
			return (-1);
		}

		while (1) {
			err = sd_read(addr, n, ptmpbuf);
			if (!err) {
				memcpy(pbuf, ptmpbuf, cnt);
				break;
			} else {
				/* retry to reduce impact of bus err */
				if (retry++ > 10) {
					break;
				}
			}
		}

		if (ptmpbuf) {
			vPortFree(ptmpbuf);
		}
	}

	return err;

}

uint32_t sdio_read_port(uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	s32 err;
	static u8 SdioRxFIFOCnt = 0;

	sdio_get_cmdaddr(addr, SdioRxFIFOCnt++, &addr);

	cnt = _RND4(cnt);

	if (cnt > 64) {
		cnt = _RND(cnt, 64);
	}

	err = sd_read(addr, cnt, mem);
	if (err) {
		return FALSE;
	}
	return TRUE;
}

uint8_t *sdio_read_rxfifo(uint32_t size)
{
	uint32_t allocsize, ret;
	uint32_t retry = 0;
	uint8_t *pbuf = NULL;

	allocsize = _RND(size, SDIO_BLOCK_SIZE);

	pbuf = (uint8_t *)pvPortMalloc(allocsize);
	if (pbuf == NULL) {
		printf("%s: Alloc skb rx buf Err, alloc_sz %d free %d !!\n\r", __func__, allocsize, xFreeBytesRemaining);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
		if (ret == TRUE) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				vPortFree(pbuf);
				printf("%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	return pbuf;
}

int sd_read(uint32_t addr, uint32_t cnt, void *pdata)
{
	return sdio_cmd53_wrap(BUS_READ, addr, cnt, pdata);
}

rtw_result_t sdio_readb(uint32_t addr, uint8_t *out)
{
	if (out == NULL) {
		return RTW_BADARG;
	}

	RTW_VERIFY_RESULT(rtw_sdio_cmd52(BUS_READ, USER_FUNC, addr, NULL, RESPONSE_NEEDED, out));
	return RTW_SUCCESS;
}


void sdio_host_send_to_dev(u8 *buf, u32 len)
{
	u32 txsize = len + SIZE_TX_DESC;
	u8 *txbuf = g_sdio_tx_buf;

	memcpy(txbuf + SIZE_TX_DESC, buf, len);

	sdio_send_data(txbuf, txsize, NULL);
}

void sdio_send_data(uint8_t *buf, uint32_t len, void *pskb)
{
	(void) pskb;
	struct INIC_TX_DESC *ptxdesc;

	ptxdesc = (struct INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	sdio_write_port(SDIO_TX_FIFO_DOMAIN_ID, len, buf);
}

s32 sdio_local_write(uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	s32 err;
	uint8_t *ptmpbuf;

	if (addr & 0x3) {
		printf("%s, address must be 4 bytes alignment\n", __FUNCTION__);
	}

	if (cnt & 0x3) {
		printf("%s, size must be the multiple of 4 \n", __FUNCTION__);
	}

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	ptmpbuf = (uint8_t *)pvPortMalloc(cnt);
	if (!ptmpbuf) {
		return (-1);
	}

	memcpy(ptmpbuf, pbuf, cnt);

	err = sd_write(addr, cnt, ptmpbuf);

	if (ptmpbuf) {
		vPortFree(ptmpbuf);
	}

	return err;
}

uint32_t sdio_write_port(uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	s32 err;
	cnt = _RND4(cnt);

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	if (cnt > 64) {
		cnt = _RND(cnt, 64);
	}

	err = sd_write(addr, cnt, mem);
	if (err) {
		printf("%s, error=%d\n", __func__, err);
		return FALSE;
	}
	return TRUE;
}

int sd_write(uint32_t addr, uint32_t cnt, void *pdata)
{
	return sdio_cmd53_wrap(BUS_WRITE, addr, cnt, pdata);
}

rtw_result_t sdio_writeb(uint32_t addr, uint8_t value)
{
	RTW_VERIFY_RESULT(rtw_sdio_cmd52(BUS_WRITE, USER_FUNC, addr, value, RESPONSE_NEEDED, NULL));
	return RTW_SUCCESS;
}

rtw_result_t sdio_cmd53_wrap(rtw_sdio_transfer_direction_t direction, uint32_t addr, uint16_t byte_cnt, uint8_t *data)
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
		RTW_VERIFY_RESULT(rtw_sdio_cmd53(direction, USER_FUNC, SDIO_BLOCK_MODE, addr, size, buf, RESPONSE_NEEDED, NULL));
		remainder -= size;
		buf += size;
	}

	/* Write the remainder using byte mode. */
	while (remainder > 0) {
		// TODO: replace sdio_max_byte_size(func) with MAX_BYTE_SIZE

		size = remainder > MAX_BYTE_SIZE ? MAX_BYTE_SIZE : remainder;
		RTW_VERIFY_RESULT(rtw_sdio_cmd53(direction, USER_FUNC, SDIO_BYTE_MODE, addr, size, buf, RESPONSE_NEEDED, NULL));
		remainder -= size;
		buf += size;
	}
	return RTW_SUCCESS;
}