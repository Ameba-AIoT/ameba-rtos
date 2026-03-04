#include "rtw_whc_common.h"

//sdio_drv_creg_read  -- cmd52
//sdio_drv_read_write  -- cmd53
typedef void *HSDC;
//cmd  read
extern int sdio_drv_creg_read(HSDC handle, int addr, int fn, unsigned int *resp);
//cmd52 write
extern int sdio_drv_creg_write(HSDC handle, int addr, int fn, unsigned char data, unsigned int *resp);
//cmd53 read
extern int sdio_drv_read(HSDC handle, unsigned int addr, unsigned int fn, unsigned int bcnt, unsigned int bsize, unsigned char *buf);
//cmd53 write
extern int sdio_drv_write(HSDC handle, unsigned int addr, unsigned int fn, unsigned int bcnt, unsigned int bsize, unsigned char *buf);

uint32_t sdio_convert_to_cmdaddr(const uint32_t addr, uint8_t *pdeviceId, uint16_t *poffset);

//swap buffer for read，DMA cache line need invalidate
__attribute__((aligned(64))) uint8_t read_buf[64];

static u8 sdio_get_domainid(u32 addr)
{
	u8 domainId;
	u16 pseudoId;

	pseudoId = (u16)(addr >> 16);
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

// Description:
//	The following mapping is for SDIO host local register space.
static void sdio_get_cmdaddr(u8	DomainID, u32 Param, u32 *pCmdAddr)
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

// func for handler
int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	unsigned int err = 0;
	uint32_t i;
	unsigned int resp;
	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}

	for (i = 0; i < cnt; i++) {
		err = sdio_drv_creg_read(priv->func, addr + i, 1, &resp);
		if (err) {
			printf("%s(): sdio_drv_creg_read failed, err:%d!\n", __func__, err);
			break;
		} else {
			pdata[i] = (uint8_t)resp;
		}
	}
	rtos_mutex_give(priv->hw_lock);
	return err;
}

int sd_cmd52_f0_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = 0;
	uint32_t i;
	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	for (i = 0; i < cnt; i++) {
		err = sdio_drv_creg_write(priv->func, addr + i, 0, pdata[i], NULL);
		if (err) {
			printf("%s: FAIL!(%d) addr=0x%x val=0x%02x\n", __func__, err, (unsigned int)(addr + i), pdata[i]);
			break;
		}
	}
	rtos_mutex_give(priv->hw_lock);

	return err;
}


int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = 0;
	uint32_t i;
	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	for (i = 0; i < cnt; i++) {
		err = sdio_drv_creg_write(priv->func, addr + i, 1, pdata[i], NULL);
		if (err) {
			printf("%s: FAIL!(%d) addr=0x%x val=0x%02x\n", __func__, err, (unsigned int)(addr + i), pdata[i]);
			break;
		}
	}
	rtos_mutex_give(priv->hw_lock);

	return err;
}

uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint32_t v = 0;
	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	*err = sdio_drv_read(priv->func, addr, 1, 1, 4, (unsigned char *)&read_buf);

	rtos_mutex_give(priv->hw_lock);

	if (*err) {
		printf("%s: FAIL!(%d) addr=0x%x\n", __func__, *err, (unsigned int)addr);
	} else {
		memcpy((u8 *)&v, read_buf, 4);
	}
	return v;
}

void sd_write8(struct whc_sdio *priv, uint32_t addr, u8 v, int *err)
{
	int ret;

	ret = sd_cmd52_write(priv, addr, 1, &v);

	if (err) {
		*err = ret;
	}
}

void sd_write16(struct whc_sdio *priv, uint32_t addr, u16 v, int *err)
{
	int ret;

	ret =  sd_cmd52_write(priv, addr, 2, (u8 *)&v);
	if (err) {
		*err = ret;
	}
}

void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, int *err)
{
	int ret;
	ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return;
	}
	ret = sdio_drv_write(priv->func, addr, 1, 1, 4, (unsigned char *)&v);
	if (err) {
		*err = ret;
	}
	rtos_mutex_give(priv->hw_lock);

}

uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr)
{
	u8 val = 0;
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 1, &val);
	return val;
}

#if 1
int sd_read(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *pdata)
{
	u32 remain_size, bcnt = 0;
	int err;

	if ((cnt == 1) || (cnt == 2)) {
		err = sd_cmd52_read(priv, addr, cnt, pdata);
		return err;
	}

	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	//remain_size =  cnt & (priv->block_transfer_len - 1);
	//bcnt = (cnt - remain_size) / (priv->block_transfer_len);
	remain_size = cnt;
	while (remain_size >= priv->block_transfer_len) {
		err = sdio_drv_read(priv->func, addr, 1, 1, priv->block_transfer_len, pdata + bcnt * priv->block_transfer_len);
		bcnt++;
		remain_size -= priv->block_transfer_len;
	}

	if (remain_size > 0) {
		err = sdio_drv_read(priv->func, addr, 1, 1, remain_size, pdata + bcnt * priv->block_transfer_len);
	}

	rtos_mutex_give(priv->hw_lock);

	return err;
}
#else
int sd_read(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *pdata)
{
	u32 remain_size, bcnt = 0;
	int err;

	if ((cnt == 1) || (cnt == 2)) {
		err = sd_cmd52_read(priv, addr, cnt, pdata);
		return err;
	}

	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}

	remain_size = cnt;
	if (remain_size >= priv->block_transfer_len) {
		bcnt = remain_size / priv->block_transfer_len;
		err = sdio_drv_read(priv->func, addr, 1, bcnt, priv->block_transfer_len, pdata);
		remain_size -= bcnt * priv->block_transfer_len;
	}

	if (remain_size > 0) {
		err = sdio_drv_read(priv->func, addr, 1, 1, remain_size, pdata + bcnt * priv->block_transfer_len);
	}

	rtos_mutex_give(priv->hw_lock);

	return err;
}
#endif
uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr)
{
	uint8_t val = 0;
	uint32_t ftaddr;
	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 1, (uint8_t *)&val);

	return val;
}

uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr)
{
	uint16_t val = 0;
	uint32_t ftaddr;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 2, (uint8_t *)&val);

	return val;
}

uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err;

	sdio_get_cmdaddr(addr, priv->SdioRxFIFOCnt++, &addr);
	cnt = _RND4(cnt);
	// if (cnt >  64) {
	//  	cnt = _RND(cnt, 64);
	// }

	err = sd_read(priv, addr, cnt, mem);
	if (err) {
		return 0;
	}
	return 1;
}

uint32_t sdio_local_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{
	int err = 0;
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	err = sdio_drv_read(priv->func, addr, 1, 1, cnt, (unsigned char *)&read_buf);
	rtos_mutex_give(priv->hw_lock);

	if (err) {
		printf("%s: FAIL!(%d) addr=0x%x\n", __func__, err, (unsigned int)addr);
	} else {
		memcpy(pbuf, read_buf, 4);
	}
	return err;
}

int sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	u32 ftaddr;
	int err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	err = sd_cmd52_write(priv, ftaddr, 1, &val);
	return err;
}


int sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	u32 ftaddr;
	int err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	err = sd_cmd52_write(priv, ftaddr, 2, (u8 *)&val);
	return err;
}

int sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	uint32_t ftaddr;
	u8 deviceId;
	u16 offset;
	uint8_t shift;
	int err;

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

#if 1
int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	u32 remain_size, bcnt = 0;
	int err;
	if ((cnt == 1) || (cnt == 2)) {
		err = sd_cmd52_write(priv, addr, cnt, pdata);
		return err;
	}
	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	//uint32_t tick_start, tick_total;

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}
	//tick_start = rt_tick_get();
	remain_size = cnt;
	while (remain_size >= priv->block_transfer_len) {
		err = sdio_drv_write(priv->func, addr, 1, 1, priv->block_transfer_len, pdata + bcnt * priv->block_transfer_len);
		bcnt++;
		remain_size -= priv->block_transfer_len;
	}

	if (remain_size > 0) {
		err = sdio_drv_write(priv->func, addr, 1, 1, remain_size, pdata + bcnt * priv->block_transfer_len);
	}
	//tick_total = rt_tick_get() - tick_start;

	//printf("sw %d\r\n", tick_total);
	rtos_mutex_give(priv->hw_lock);

	return err;
}
#else
int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	u32 remain_size, bcnt = 0;
	int err;

	if ((cnt == 1) || (cnt == 2)) {
		err = sd_cmd52_write(priv, addr, cnt, pdata);
		return err;
	}

	int ret = rtos_mutex_take(priv->hw_lock, 1000);

	if (ret != 0) {
		printf("take hw lock fail %s\r\n", __func__);
		return -1;
	}

	remain_size = cnt;
	if (remain_size >= priv->block_transfer_len) {
		bcnt = remain_size / priv->block_transfer_len;
		err = sdio_drv_write(priv->func, addr, 1, bcnt, priv->block_transfer_len, pdata);
		remain_size -= bcnt * priv->block_transfer_len;
	}

	if (remain_size > 0) {
		err = sdio_drv_write(priv->func, addr, 1, 1, remain_size, pdata + bcnt * priv->block_transfer_len);
	}

	rtos_mutex_give(priv->hw_lock);

	return err;
}
#endif

uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err = 0;
	cnt = _RND4(cnt);

	// if (cnt >  priv->block_transfer_len) {
	//  	cnt = _RND(cnt,  priv->block_transfer_len);
	// }
	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	err = sd_write(priv, addr, cnt, mem);
	if (err) {
		printf("%s, error=%d\n", __func__, err);
	}

	return err;

}

uint32_t sdio_local_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem)
{
	int err = 0;
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	err = sd_write(priv, addr, cnt, mem);
	if (err) {
		printf("%s, error=%d\n", __func__, err);
	}

	return err;
}

uint32_t sdio_convert_to_cmdaddr(uint32_t addr, uint8_t *pdeviceId, uint16_t *poffset)
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
