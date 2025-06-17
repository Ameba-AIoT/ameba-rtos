#include "ameba_soc.h"
#include "whc_host.h"

#define MAX_BLOCKS		0x1FF
#define MAX_BYTE_SIZE	512

extern SD_HdlTypeDef hsd0;
rtos_mutex_t sdio_lock;

int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata)
{
	(void)func;
	int ret = RTK_SUCCESS;

	if (pdata == NULL) {
		return RTK_FAIL;
	}

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "fail %s %d \r\n", __func__, __LINE__);
		return RTK_FAIL;
	}

	if (SD_IO_RW_Direct(&hsd0, BUS_READ, SDIO_FUNC1, addr, NULL, pdata) != SD_OK) {
		ret = RTK_FAIL;
	}

	rtos_mutex_give(sdio_lock);
	return ret;
}

int sdio_write_byte(void *func, uint32_t addr, uint8_t data)
{
	(void)func;
	int ret = RTK_SUCCESS;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "fail %s %d \r\n", __func__, __LINE__);
		return RTK_FAIL;
	}

	if (SD_IO_RW_Direct(&hsd0, BUS_WRITE, SDIO_FUNC1, addr, data, NULL) != SD_OK) {
		ret = RTK_FAIL;
	}

	rtos_mutex_give(sdio_lock);
	return ret;
}

int sdio_io_rw_ext_helper(u8 dir, u32 addr, u16 byte_cnt, u8 *data)
{
	u16 remainder = byte_cnt;
	u8 *buf = data;
	u16 size;

	/* Do the bulk of the transfer using block mode (if supported). */
	while (remainder >= SD_BLOCK_SIZE) {
		u32 blocks;

		blocks = remainder / SD_BLOCK_SIZE;

		if (blocks > MAX_BLOCKS) {
			blocks = MAX_BLOCKS;
		}

		size = blocks * SD_BLOCK_SIZE;

		if (dir == BUS_WRITE) { // write
			if (SD_IO_WriteBlocks(SDIO_FUNC1, addr, buf, blocks) != SD_OK) {
				return RTK_FAIL;
			}
		} else { // read
			if (SD_IO_ReadBlocks(SDIO_FUNC1, addr, buf, blocks) != SD_OK) {
				return RTK_FAIL;
			}
		}

		remainder -= size;
		buf += size;
	}

	/* Write the remainder using byte mode. */
	while (remainder > 0) {

		size = remainder > MAX_BYTE_SIZE ? MAX_BYTE_SIZE : remainder;

		if (dir == BUS_WRITE) { // write
			if (SD_IO_WriteBytes(SDIO_FUNC1, addr, buf, size) != SD_OK) {
				return RTK_FAIL;
			}
		} else { // read
			if (SD_IO_ReadBytes(SDIO_FUNC1, addr, buf, size) != SD_OK) {
				return RTK_FAIL;
			}
		}
		remainder -= size;
		buf += size;
	}
	return RTK_SUCCESS;
}

int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	(void)func;
	int ret = RTK_SUCCESS;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "fail %s %d \r\n", __func__, __LINE__);
		return RTK_FAIL;
	}

	if (sdio_io_rw_ext_helper(BUS_READ, addr, len, pdata) == RTK_FAIL) {
		ret = RTK_FAIL;
	}

	rtos_mutex_give(sdio_lock);
	return ret;
}

int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	(void)func;
	int ret = RTK_SUCCESS;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "fail %s %d \r\n", __func__, __LINE__);
		return RTK_FAIL;
	}

	if (sdio_io_rw_ext_helper(BUS_WRITE, addr, len, pdata) == RTK_FAIL) {
		ret = RTK_FAIL;
	}

	rtos_mutex_give(sdio_lock);
	return ret;
}

int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = RTK_SUCCESS;
	uint32_t i;

	for (i = 0; i < cnt; i++) {
		err = sdio_read_byte(priv->func, addr + i, pdata + i);
		if (err == RTK_FAIL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
			break;
		}
	}

	if (err == RTK_FAIL) {
		int j;

		err = RTK_SUCCESS;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				err = sdio_read_byte(priv->func, addr + i, pdata + i);
				if (err == RTK_FAIL) {
					RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
					break;
				}
			}

			if (err == RTK_SUCCESS) {
				break;
			} else {
				//TODO err
			}
		}

		if (j == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	return err;
}

int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = RTK_SUCCESS;
	uint32_t i;


	for (i = 0; i < cnt; i++) {
		err = sdio_write_byte(priv->func, addr + i, pdata[i]);
		if (err == RTK_FAIL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr + i, pdata[i]);
			break;
		}
	}

	if (err == RTK_FAIL) {
		int j;

		err = RTK_SUCCESS;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				err = sdio_write_byte(priv->func, addr + i, pdata[i]);
				if (err) {
					RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s(): sdio_writeb failed!\n", __func__);
					break;
				}
			}

			if (err == RTK_SUCCESS) {
				break;
			}
		}

		if (j == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	return err;
}

uint8_t sd_read8(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint8_t v = RTK_SUCCESS;

	*err = sdio_read_byte(priv->func, addr, &v);

	if (err && (*err == RTK_FAIL)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x\n", __func__, *err, addr);
	}

	return v;
}

uint16_t sd_read16(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint16_t v = RTK_SUCCESS;
	uint8_t data[2] = {0};

	*err = sdio_read_fifo(priv->func, addr, data, 2);
	v = (data[0] << 8) | data[1];

	if (err && (*err == RTK_FAIL)) {
		int i;

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = RTK_SUCCESS;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			*err = sdio_read_fifo(priv->func, addr, data, 2);
			v = (data[0] << 8) | data[1];

			if (*err == RTK_SUCCESS) {
				break;
			} else {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, (data[0] << 8) | data[1], i);

			}
		}

		if (i == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}

	return  v;
}

uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint32_t v = RTK_SUCCESS;
	uint8_t data[4] = {0};

	if (err) {
		*err = sdio_read_fifo(priv->func, addr, data, 4);
	}

	v = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

	if (err && (*err == RTK_FAIL)) {
		int i;

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = RTK_SUCCESS;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			*err = sdio_read_fifo(priv->func, addr, data, 4);
			v = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

			if (*err == RTK_SUCCESS) {
				break;
			} else {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
			}
		}

		if (i == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}

	return  v;
}

void sd_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err)
{
	*err = sdio_write_byte(priv->func, addr, v);

	if (err && (*err == RTK_FAIL)) {
		int i;

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = RTK_SUCCESS;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			*err = sdio_write_byte(priv->func, addr, v);

			if (*err == RTK_SUCCESS) {
				break;
			} else {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
			}
		}

		if (i == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}

}


void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, int *err)
{
	uint8_t data[4] = {0};

	data[0] = (v >> 24) & 0xff;
	data[1] = (v >> 16) & 0xff;
	data[2] = (v >> 8) & 0xff;
	data[3] = v & 0xff;

	*err = sdio_write_fifo(priv->func, addr, data, 4);
	if (err && (*err == RTK_FAIL)) {
		int i;

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x val=0x%08x\n", __func__, *err, addr, v);

		*err = RTK_SUCCESS;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			*err = sdio_write_fifo(priv->func, addr, data, 4);
			if (*err == RTK_SUCCESS) {
				break;
			} else {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);

			}
		}

		if (i == SD_IO_TRY_CNT) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, *err, addr, v, i);
		} else {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: (%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}
}

int sd_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = RTK_SUCCESS;
	u32 i;
	u8 *pbuf;
	if (unlikely((cnt == 1) || (cnt == 2))) {
		pbuf = (u8 *)pdata;

		for (i = 0; i < cnt; i++) {
			err = sdio_read_byte(priv->func, addr + i, pbuf + i);

			if (err == RTK_FAIL) {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x\n", __func__, err, addr);
				break;
			}
		}
		return err;
	}

	err = sdio_read_fifo(priv->func, addr, (uint8_t *)pdata, cnt);
	if (err == RTK_FAIL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
	}

	return err;
}

int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = RTK_SUCCESS;
	u32 i;
	u8 *pbuf;

	if (unlikely((cnt == 1) || (cnt == 2))) {
		pbuf = (u8 *)pdata;

		for (i = 0; i < cnt; i++) {
			err = sdio_write_byte(priv->func, addr + i, pbuf[i]);
			if (err == RTK_FAIL) {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr, *(pbuf + i));
				break;
			}
		}

		return err;
	}

	err = sdio_write_fifo(priv->func, addr, pdata, cnt);
	if (err == RTK_FAIL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: FAIL(%d)! ADDR=0x%x Size=%d(%d)\n", __func__, err, addr, cnt, cnt);
	}

	return err;
}

void sdio_get_cmdaddr(uint8_t DomainID, uint32_t Param, uint32_t *pCmdAddr)
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
static uint8_t sdio_get_domainid(uint32_t addr)
{
	uint8_t domainId;
	uint16_t pseudoId;

	pseudoId = (uint16_t)(addr >> 16);
	switch (pseudoId) {
	case 0x1025:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		break;

	case 0x1031:
		domainId = SDIO_TX_FIFO_DOMAIN_ID;
		break;

	case 0x1034:
		domainId = SDIO_RX_FIFO_DOMAIN_ID;
		break;

	default:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		break;
	}

	return domainId;
}

uint32_t sdio_convert_to_cmdaddr(const uint32_t addr, uint8_t *pdeviceId, uint16_t *poffset)
{
	uint8_t domainId;
	uint16_t offset;
	uint32_t ftaddr;

	domainId = sdio_get_domainid(addr);
	offset = 0;

	switch (domainId) {
	case SDIO_LOCAL_DOMAIN_ID:
		offset = addr & SDIO_LOCAL_MSK;
		break;

	case SDIO_TX_FIFO_DOMAIN_ID:
		offset = addr & SDIO_TX_FIFO_MSK;
		break;

	case SDIO_RX_FIFO_DOMAIN_ID:
		offset = addr & SDIO_RX_FIFO_MSK;
		break;

	default:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		offset = addr & SDIO_LOCAL_MSK;
		break;
	}
	ftaddr = (domainId << 13) | offset;

	if (pdeviceId) {
		*pdeviceId = domainId;
	}
	if (poffset) {
		*poffset = offset;
	}

	return ftaddr;
}

uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint8_t val = RTK_SUCCESS;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	sd_cmd52_read(priv, ftaddr, 1, (uint8_t *)&val);

	return val;
}

uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr)
{
	u32 ftaddr;
	u16 val = RTK_SUCCESS;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 2, (u8 *)&val);

	return val;
}

uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr)
{
	u32 ftaddr;
	u32 val = RTK_SUCCESS;
	u8 shift;
	int err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		val = sd_read32(priv, ftaddr, &err);
	} else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtos_mem_zmalloc(8);
		if (NULL == ptmpbuf) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return RTK_FAIL;
		}

		ftaddr &= ~(u16)0x3;
		sd_read(priv, ftaddr, 8, ptmpbuf);
		memcpy(&val, ptmpbuf + shift, 4);
		val = le32_to_cpu(val);

		rtos_mem_free(ptmpbuf);
	}

	return val;
}

int sdio_readN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	int err = RTK_SUCCESS;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_read(priv, ftaddr, cnt, pbuf);
	} else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = (u8 *)rtos_mem_zmalloc(n);
		if (NULL == ptmpbuf) {
			return RTK_FAIL;
		}
		err = sd_read(priv, ftaddr, n, ptmpbuf);
		if (err == RTK_SUCCESS) {
			memcpy(pbuf, ptmpbuf + shift, cnt);
		}
		rtos_mem_free(ptmpbuf);
	}

	return err;
}

void sdio_read_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *rmem)
{
	sdio_readN(priv, addr, cnt, rmem);
}

uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err;

	sdio_get_cmdaddr(addr, priv->SdioRxFIFOCnt++, &addr);

	cnt = _RND4(cnt);

	err = sd_read(priv, addr, cnt, mem);

	return err;
}


int sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	int err = RTK_SUCCESS;
	u32 ftaddr;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_write8(priv, ftaddr, val, &err);

	return err;
}

int sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	int err = RTK_SUCCESS;
	u32 ftaddr;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	err = sd_cmd52_write(priv, ftaddr, 2, (u8 *)&val);

	return err;
}

int sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	int err = RTK_SUCCESS;
	uint32_t ftaddr;
	uint16_t offset;
	uint8_t deviceId;
	uint8_t shift;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;

	if (shift == 0) {
		sd_write32(priv, ftaddr, val, &err);
	} else {
		err = sd_cmd52_write(priv, ftaddr, 4, (u8 *)&val);
	}

	return err;
}

int sdio_writeN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	uint8_t deviceId;
	uint16_t offset;
	uint32_t ftaddr;
	uint8_t shift;
	int err = RTK_SUCCESS;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_write(priv, ftaddr, cnt, pbuf);
	} else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = (u8 *)rtos_mem_zmalloc(n);
		if (NULL == ptmpbuf) {
			return RTK_FAIL;
		}
		err = sd_read(priv, ftaddr, 4, ptmpbuf);
		if (err) {
			rtos_mem_free(ptmpbuf);
			return err;
		}
		memcpy(ptmpbuf + shift, pbuf, cnt);
		err = sd_write(priv, ftaddr, n, ptmpbuf);
		rtos_mem_free(ptmpbuf);
	}
	return err;
}

void sdio_write_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *wmem)
{
	sdio_writeN(priv, addr, cnt, wmem);

}

uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err = RTK_SUCCESS;
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)mem;

	cnt = _RND4(cnt);

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	err = sd_write(priv, addr, cnt, mem);

	if (err == RTK_FAIL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s, error=%d\n", __func__, err);
		return 1;
	}

	return err;
}

/*
 * align address to 4 bytes in future.
 */
int sdio_local_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	int err = RTK_SUCCESS;
	uint8_t *ptmpbuf;
	uint32_t n, retry = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	n = RND4(cnt);
	ptmpbuf = (u8 *)rtos_mem_zmalloc(n);
	if (!ptmpbuf) {
		return RTK_FAIL;
	}

	while (1) {
		err = sd_read(priv, addr, n, ptmpbuf);
		if (err == RTK_SUCCESS) {
			memcpy(pbuf, ptmpbuf, cnt);
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry ++ > 10) {
				break;
			}
		}
	}

	if (ptmpbuf) {
		rtos_mem_free(ptmpbuf);
	}

	return err;
}

/*
 * align address to 4 bytes in future.
 */
int sdio_local_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	int err = RTK_SUCCESS;
	uint8_t *ptmpbuf;

	if (addr & 0x3) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s, address must be 4 bytes alignment\n", __FUNCTION__);
	}
	if (cnt  & 0x3) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s, size must be the multiple of 4 \n", __FUNCTION__);
	}

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	ptmpbuf = (u8 *)rtos_mem_zmalloc(cnt);
	if (!ptmpbuf) {
		return RTK_FAIL;
	}

	memcpy(ptmpbuf, pbuf, cnt);

	err = sd_write(priv, addr, cnt, ptmpbuf);

	if (ptmpbuf) {
		rtos_mem_free(ptmpbuf);
	}
	return err;
}

uint32_t sdio_cmd53_read4byte_local(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t val = 0;
	int err = RTK_SUCCESS;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	val = sd_read32(priv, addr, &err);
	return val;
}

uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr)
{
	uint8_t val = RTK_SUCCESS;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 1, &val);

	return val;
}

void sdio_enable_data1_irq(void)
{
	SDIO_ConfigNormIntSts(hsd0.Instance, SDIOHOST_BIT_CARD_INT_STATUS_EN, ENABLE);
	SDIO_ConfigNormIntSig(hsd0.Instance, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);
}

void sdio_disable_data1_irq(void)
{
	SDIO_ConfigNormIntSts(hsd0.Instance, SDIOHOST_BIT_CARD_INT_STATUS_EN, DISABLE);
	SDIO_ConfigNormIntSig(hsd0.Instance, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, DISABLE);
}









