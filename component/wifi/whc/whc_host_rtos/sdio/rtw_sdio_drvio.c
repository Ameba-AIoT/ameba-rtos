#include "ameba_soc.h"
#include "whc_host.h"

#define MAX_BLOCKS		0x1FF
#define MAX_BYTE_SIZE	512

extern SD_HdlTypeDef hsd0;
rtos_mutex_t sdio_lock;

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

/* SD_IO_RW_Direct cmd52 read write */
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

/* SD_IO_WriteBlocks cmd53 read write */
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
	u32 ftaddr;
	u16 val = 0;
	u32 i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	for (i = 0; i < 2; i++) {
		sdio_read_byte(priv->func, ftaddr + i, (u8 *)(&val) + i);
	}
	return val;
}

uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr)
{
	u32 ftaddr;
	u32 val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	/* CMD53 4-byte read; little-endian host places byte@lowest-addr as LSB */
	sdio_read_fifo(priv->func, ftaddr, (uint8_t *)&val, 4);
	return val;
}

uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err;

	sdio_get_cmdaddr(addr, 0, &addr);
	cnt = _RND4(cnt);
	err = sdio_read_fifo(priv->func, addr, mem, cnt);
	return err;
}


int sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	int err = RTK_SUCCESS;
	u32 ftaddr;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	err = sdio_write_byte(priv->func, ftaddr, val);
	return err;
}

int sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	int err = RTK_SUCCESS;
	u32 ftaddr;
	u32 i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);

	for (i = 0; i < 2; i++) {
		err = sdio_write_byte(priv->func,  ftaddr + i,  *((u8 *)(&val) + i));
		if (err) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s(): sdio_write_byte failed, err:%d!\n", __func__, err);
		}
	}

	return err;
}

/* should always 4B aligned */
int sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	int err = RTK_SUCCESS;
	uint32_t ftaddr;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	err = sdio_write_fifo(priv->func, ftaddr, (u8 *)&val, 4);

	return err;
}

uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err = RTK_SUCCESS;

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	} else {
		cnt = _RND4(cnt);
	}

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	err = sdio_write_fifo(priv->func, addr, mem, cnt);

	if (err == RTK_FAIL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%s, error=%d\n", __func__, err);
	}

	return err;
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
