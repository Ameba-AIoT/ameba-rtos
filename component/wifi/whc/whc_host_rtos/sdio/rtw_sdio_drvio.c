#include "whc_host.h"

rtos_mutex_t sdio_lock;

int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata)
{
	(void)func;
	(void)addr;
	(void)pdata;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGE(TAG_WLAN_INIC, "fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	rtos_mutex_give(sdio_lock);
	return 0;
}

int sdio_write_byte(void *func, uint32_t addr, uint8_t pdata)
{
	(void)func;
	(void)addr;
	(void)pdata;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGE(TAG_WLAN_INIC, "fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	rtos_mutex_give(sdio_lock);
	return 0;
}


int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	(void)func;
	(void)addr;
	(void)pdata;
	(void)len;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGE(TAG_WLAN_INIC, "fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}

	rtos_mutex_give(sdio_lock);
	return 0;
}

int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len)
{
	(void)func;
	(void)addr;
	(void)pdata;
	(void)len;

	if (RTK_SUCCESS != rtos_mutex_take(sdio_lock, 500)) {
		RTK_LOGE(TAG_WLAN_INIC, "fail %s %d \r\n", __func__, __LINE__);
		return 1;
	}


	rtos_mutex_give(sdio_lock);

	return 0;
}

int rtw_inc_and_chk_continual_io_error(struct whc_sdio *priv)
{
	(void)priv;

	return FALSE;
}

void rtw_reset_continual_io_error(struct whc_sdio *priv)
{
	(void)priv;

	return;
}


int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{

	int err = 0;
	(void)priv;
	(void)addr;
	(void)pdata;
	(void)cnt;

	return err;
}

int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = 0;
	(void)priv;
	(void)addr;
	(void)pdata;
	(void)cnt;

	return err;
}

uint8_t sd_read8(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint8_t v = 0;
	(void)priv;
	(void)addr;
	(void)err;

	return v;
}

uint16_t sd_read16(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint16_t v = 0;
	(void)priv;
	(void)addr;
	(void)err;

	return  v;
}

uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint32_t v = 0;
	(void)priv;
	(void)addr;
	(void)err;

	return  v;
}

void sd_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err)
{
	(void)priv;
	(void)addr;
	(void)err;
	(void)v;

}

void sd_write16(struct whc_sdio *priv, uint32_t addr, uint16_t v, int *err)
{
	(void)priv;
	(void)addr;
	(void)v;
	(void)err;

}

void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, s32 *err)
{
	(void)priv;
	(void)addr;
	(void)v;
	(void)err;
}

int sd_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = -1;

	(void)priv;
	(void)addr;
	(void)pdata;
	(void)cnt;

	return err;
}

int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = -1;

	(void)priv;
	(void)addr;
	(void)pdata;
	(void)cnt;

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
	uint8_t val = 0;
	(void)priv;
	(void)addr;

	return val;
}

uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr)
{
	uint16_t val = 0;
	(void)priv;
	(void)addr;

	return val;
}

uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr)
{
	(void)priv;
	(void)addr;
	uint32_t val = 0;

	return val;
}

s32 sdio_readN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	s32 err = 0;

	(void)priv;
	(void)addr;
	(void)cnt;
	(void)pbuf;

	return err;
}

void sdio_read_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *rmem)
{
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)rmem;
}

uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)mem;

	return 1;
}


s32 sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	s32 err = 0;

	(void)priv;
	(void)addr;
	(void)val;

	return err;
}

s32 sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	s32 err = 0;

	(void)priv;
	(void)addr;
	(void)val;

	return err;
}

s32 sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	s32 err = 0;

	(void)priv;
	(void)addr;
	(void)val;

	return err;
}

s32 sdio_writeN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{

	s32 err = 0;

	(void)priv;
	(void)addr;
	(void)cnt;
	(void)pbuf;

	return err;
}

void sdio_write_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *wmem)
{
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)wmem;
}

uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)mem;

	return 1;
}

/*
 * align address to 4 bytes in future.
 */
s32 sdio_local_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	s32 err = 0;
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)pbuf;
	return err;
}

/*
 * align address to 4 bytes in future.
 */
s32 sdio_local_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	s32 err = 0;
	(void)priv;
	(void)addr;
	(void)cnt;
	(void)pbuf;

	return err;
}

uint32_t sdio_cmd53_read4byte_local(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t val = 0;
	(void)priv;
	(void)addr;

	return val;
}

uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr)
{

	uint8_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 1, &val);

	return val;
}

void sdio_enable_data1_irq(void)
{

}

void sdio_disable_data1_irq(void)
{

}









