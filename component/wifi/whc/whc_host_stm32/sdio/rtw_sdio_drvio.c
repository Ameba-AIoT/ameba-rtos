#include "whc_host.h"
#include "wwd_constants.h"                  /* For wwd_result_t */
#include "wwd_sdio_interface.h"

#define USER_BLK_SZ SDIO_64B_BLOCK
#define BASIC_FUNC 0	// func0
#define USER_FUNC 1		// func1: SDIO_WIFI

#define MAX_BLOCKS 511
#define MAX_BYTE_SIZE 511

rtos_mutex_t sdio_lock;

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

#define VERIFY_RESULT(x)                                                       \
{                                                                              \
	wwd_result_t verify_result;                                                \
	verify_result = (x);                                                       \
	if (verify_result != WWD_SUCCESS) {                                            \
		printf("fail: func=%s line=%d %d\n", __FUNCTION__, __LINE__, x);           \
		return verify_result;                                                  \
	}                                                                          \
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


/*
 * Return:
 *	0		Success
 *	others	Fail
 */
int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{

	int err = 0;
	uint32_t i;


	for (i = 0; i < cnt; i++) {
		err = sdio_read_byte(priv->func, addr + i, pdata + i);
		if (err) {
			printf("%s(): sdio_readb failed, err:%d!\n", __func__, err);
			break;
		}
	}

	if (err) {
		int j;

		err = 0;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				err = sdio_read_byte(priv->func, addr + i, pdata + i);
				if (err) {
					printf("%s(): sdio_readb failed, err:%d!\n", __func__, err);
					break;
				}
			}
			if (err == 0) {
				break;
			}
		}

		if (j == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	return err;
}

/*
 * Return:
 *	0		Success
 *	others	Fail
 */
int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata)
{
	int err = 0;
	uint32_t i;


	for (i = 0; i < cnt; i++) {
		err = sdio_write_byte(priv->func, addr + i, pdata[i]);
		if (err) {
			printf("%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr + i, pdata[i]);
			break;
		}
	}

	if (err) {
		int j;

		err = 0;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				err = sdio_write_byte(priv->func, addr + i, pdata[i]);
				if (err) {
					printf("%s(): sdio_writeb failed!\n", __func__);
					break;
				}
			}
			if (err == 0) {
				break;
			}
		}

		if (j == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	return err;
}

uint8_t sd_read8(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint8_t v = 0;
	int state;

	state = sdio_read_byte(priv->func, addr, &v);

	if (state) {
		printf("%s: FAIL!(%d) addr=0x%05x\n", __func__, state, addr);
	}

	if (err) {
		*err = state;
	}

	return v;
}

uint16_t sd_read16(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint16_t v = 0;
	uint8_t data[2] = {0};
	int state;

	state = sdio_read_fifo(priv->func, addr, data, 2);
	v = (data[0] << 8) | data[1];

	if (state) {
		int i;

		printf("%s: (%d) addr=0x%05x, val=0x%x\n", __func__, state, addr, v);

		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			state = sdio_read_fifo(priv->func, addr, data, 2);
			v = (data[0] << 8) | data[1];
			if (state == 0) {
				break;
			}
		}

		if (i == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, state, addr, v, i);
		}
	}

	if (err) {
		*err = state;
	}

	return  v;
}

uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err)
{
	uint32_t v = 0;
	uint8_t data[4] = {0};
	int state;

	state = sdio_read_fifo(priv->func, addr, data, 4);
	v = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

	if (state) {
		int i;
		printf("%s: (%d) addr=0x%05x, val=0x%x\n", __func__, state, addr, v);

		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			state = sdio_read_fifo(priv->func, addr, data, 4);
			v = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
			if (state == 0) {
				break;
			}
		}

		if (i == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, state, addr, v, i);
		}
	}

	if (err) {
		*err = state;
	}

	return  v;
}

void sd_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err)
{
	int state;

	state = sdio_write_byte(priv->func, addr, v);

	if (state) {
		int i;

		printf("%s: (%d) addr=0x%05x, val=0x%x\n", __func__, state, addr, v);

		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			state = sdio_write_byte(priv->func, addr, v);
			if (state == 0) {
				break;
			}
		}

		if (i == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, state, addr, v, i);
		}
	}

	if (err) {
		*err = state;
	}
}

void sd_write16(struct whc_sdio *priv, uint32_t addr, uint16_t v, int *err)
{
	uint8_t data[2] = {0};
	int state;

	data[0] = (v >> 8) & 0xff;
	data[1] = v & 0xff;

	state = sdio_write_fifo(priv->func, addr, data, 2);
	if (state) {
		int i;

		printf("%s: (%d) addr=0x%05x, val=0x%x\n", __func__, state, addr, v);

		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			state = sdio_write_fifo(priv->func, addr, data, 2);
			if (state == 0) {
				break;
			}
		}

		if (i == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, state, addr, v, i);
		}
	}

	if (err) {
		*err = state;
	}
}

void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, s32 *err)
{
	uint8_t data[4] = {0};
	int state;

	data[0] = (v >> 24) & 0xff;
	data[1] = (v >> 16) & 0xff;
	data[2] = (v >> 8) & 0xff;
	data[3] = v & 0xff;

	state = sdio_write_fifo(priv->func, addr, data, 4);
	if (state) {
		int i;

		printf("%s: (%d) addr=0x%05x val=0x%08x\n", __func__, state, addr, v);

		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			state = sdio_write_fifo(priv->func, addr, data, 4);
			if (state == 0) {
				break;
			}
		}

		if (i == SD_IO_TRY_CNT) {
			printf("%s: FAIL!(%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, state, addr, v, i);
		} else {
			printf("%s: (%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, state, addr, v, i);
		}
	}

	if (err) {
		*err = state;
	}
}

/*
 * Use CMD53 to read data from SDIO device.
 *
 * Parameters:
 *	psdio	pointer of SDIO_DATA
 *	addr	address to read
 *	cnt		amount to read
 *	pdata	pointer to put data, this should be a "DMA:able scratch buffer"!
 *
 * Return:
 *	0		Success
 *	others	Fail
 */
int sd_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = -1;

	if ((cnt == 1) || (cnt == 2)) {
		uint32_t i;
		uint8_t *pbuf = (uint8_t *)pdata;

		for (i = 0; i < cnt; i++) {
			err = sdio_read_byte(priv->func, addr + i, pbuf + i);
			if (err) {
				printf("%s: FAIL!(%d) addr=0x%05x\n", __func__, err, addr);
				break;
			}
		}
		return err;
	}

	err = sdio_read_fifo(priv->func, addr, (uint8_t *)pdata, cnt);
	if (err) {
		printf("%s: FAIL(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
#ifdef DBG_STACK_TRACE
		dump_stack();
#endif
	} else {
		//dev_dbg(&priv->func->dev, "%s: SUCCESS(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
	}

	return err;
}

/*
 * Use CMD53 to write data to SDIO device.
 *
 * Parameters:
 *  psdio	pointer of SDIO_DATA
 *  addr	address to write
 *  cnt		amount to write
 *  pdata	data pointer, this should be a "DMA:able scratch buffer"!
 *
 * Return:
 *  0		Success
 *  others	Fail
 */
int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata)
{
	int err = -1;


	if ((cnt == 1) || (cnt == 2)) {
		uint32_t i;
		uint8_t *pbuf = (uint8_t *)pdata;

		for (i = 0; i < cnt; i++) {
			err = sdio_write_byte(priv->func, addr + i, pbuf[i]);
			if (err) {
				printf("%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr, pbuf[i]);
				break;
			}
		}

		return err;
	}

	err = sdio_write_fifo(priv->func, addr, (uint8_t *)pdata, cnt);
	if (err) {
		printf("%s: FAIL(%d)! ADDR=0x%x Size=%d(%d)\n", __func__, err, addr, cnt, cnt);
	}

	return err;
}

//
// Description:
//	The following mapping translates domain ID + offset to the SDIO command address
//	for local registers, TX FIFO, and RX FIFO domains.
static void sdio_get_cmdaddr(
	uint8_t				DomainID,
	uint32_t				Param,
	uint32_t			*pCmdAddr
)
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

static uint32_t sdio_convert_to_cmdaddr(const uint32_t addr, uint8_t *pdeviceId, uint16_t *poffset)
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
	uint8_t val;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	sd_cmd52_read(priv, ftaddr, 1, (uint8_t *)&val);

	return val;
}

uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint16_t val;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 2, (uint8_t *)&val);


	return val;
}

uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t ftaddr;
	uint32_t val;
	uint8_t shift;
//	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		val = sd_read32(priv, ftaddr, NULL);
	} else {
		uint8_t *ptmpbuf;

		ptmpbuf = (uint8_t *)WHC_MALLOC(8);
		if (NULL == ptmpbuf) {
			printf("%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return FALSE;
		}

		ftaddr &= ~(uint16_t)0x3;
		sd_read(priv, ftaddr, 8, ptmpbuf);
		memcpy(&val, ptmpbuf + shift, 4);

		WHC_FREE(ptmpbuf);
	}

	return val;
}

s32 sdio_readN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{

	uint8_t deviceId;
	uint16_t offset;
	uint32_t ftaddr;
	uint8_t shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_read(priv, ftaddr, cnt, pbuf);
	} else {
		uint8_t *ptmpbuf;
		uint32_t n;

		ftaddr &= ~(uint16_t)0x3;
		n = cnt + shift;
		ptmpbuf = (uint8_t *)WHC_MALLOC(n);
		if (NULL == ptmpbuf) {
			return -1;
		}
		err = sd_read(priv, ftaddr, n, ptmpbuf);
		if (!err) {
			memcpy(pbuf, ptmpbuf + shift, cnt);
		}
		WHC_FREE(ptmpbuf);
	}

	return err;
}

void sdio_read_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *rmem)
{
	s32 err;

	err = sdio_readN(priv, addr, cnt, rmem);
}

/*
 * Description:
 *	Read from RX FIFO
 *	Round read size to block size,
 *	and make sure data transfer will be done in one command.
 *
 * Parameters:
 *	func		a pointer of sdio func
 *	addr		port ID
 *	cnt			size to read
 *	rmem		address to put data
 *
 * Return:
 *	TRUE(1)		Success
 *	FALSE(0)		Fail
 */

uint32_t sdio_read_port(
	struct whc_sdio *priv,
	uint32_t addr,
	uint32_t cnt,
	uint8_t *mem)
{
	s32 err;

	sdio_get_cmdaddr(addr, priv->SdioRxFIFOCnt++, &addr);

	cnt = _RND4(cnt);

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	err = sd_read(priv, addr, cnt, mem);
	if (err) {
		return FALSE;
	}
	return TRUE;
}


s32 sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val)
{
	uint32_t ftaddr;
	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_write8(priv, ftaddr, val, &err);

	return err;
}

s32 sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val)
{
	uint32_t ftaddr;
	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	err = sd_cmd52_write(priv, ftaddr, 2, (uint8_t *)&val);

	return err;
}

s32 sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val)
{
	uint8_t deviceId;
	uint16_t offset;
	uint32_t ftaddr;
	uint8_t shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;

	if (shift == 0) {
		sd_write32(priv, ftaddr, val, &err);
	} else {
		err = sd_cmd52_write(priv, ftaddr, 4, (uint8_t *)&val);
	}

	return err;
}


s32 sdio_writeN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf)
{

	uint8_t deviceId;
	uint16_t offset;
	uint32_t ftaddr;
	uint8_t shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_write(priv, ftaddr, cnt, pbuf);
	} else {
		uint8_t *ptmpbuf;
		uint32_t n;

		ftaddr &= ~(uint16_t)0x3;
		n = cnt + shift;
		ptmpbuf = (uint8_t *)WHC_MALLOC(n);
		if (NULL == ptmpbuf) {
			return -1;
		}
		err = sd_read(priv, ftaddr, 4, ptmpbuf);
		if (err) {
			WHC_FREE(ptmpbuf);
			return err;
		}
		memcpy(ptmpbuf + shift, pbuf, cnt);
		err = sd_write(priv, ftaddr, n, ptmpbuf);
		WHC_FREE(ptmpbuf);
	}

	return err;
}

void sdio_write_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *wmem)
{
	sdio_writeN(priv, addr, cnt, wmem);
}

/*
 * Description:
 *	Write to TX FIFO
 *	Align write size block size,
 *	and make sure data could be written in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to write
 *	wmem		data pointer to write
 *
 * Return:
 *	TRUE(1)		Success
 *	FALSE(0)		Fail
 */
uint32_t sdio_write_port(
	struct whc_sdio *priv,
	uint32_t addr,
	uint32_t cnt,
	uint8_t *mem)
{
	s32 err;
	cnt = _RND4(cnt);

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	err = sd_write(priv, addr, cnt, mem);
	if (err) {
		printf("%s, error=%d\n", __func__, err);
		return FALSE;
	}
	return TRUE;
}

/*
 * align address to 4 bytes in future.
 */
s32 sdio_local_read(
	struct whc_sdio *priv,
	uint32_t		addr,
	uint32_t		cnt,
	uint8_t		*pbuf)
{

	s32 err;
	uint8_t *ptmpbuf;
	uint32_t n, retry = 0;
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	n = RND4(cnt);
	ptmpbuf = (uint8_t *)WHC_MALLOC(n);
	if (!ptmpbuf) {
		return (-1);
	}

	while (1) {

		err = sd_read(priv, addr, n, ptmpbuf);

		if (!err) {
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
		WHC_FREE(ptmpbuf);
	}

	return err;
}


uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr)
{

	uint8_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 1, &val);

	return val;
}

uint16_t sdio_cmd52_read2byte_local(struct whc_sdio *priv, uint32_t addr)
{

	uint16_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 2, (uint8_t *)&val);


	return val;
}

uint32_t sdio_cmd52_read4byte_local(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 4, (uint8_t *)&val);

	return val;
}

uint32_t sdio_cmd53_read4byte_local(struct whc_sdio *priv, uint32_t addr)
{
	uint32_t val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	val = sd_read32(priv, addr, NULL);

	return val;
}

void sdio_cmd52_write1byte_local(struct whc_sdio *priv, uint32_t addr, uint8_t v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_write(priv, addr, 1, &v);
}

void sdio_cmd52_write2byte_local(struct whc_sdio *priv, uint32_t addr, uint16_t v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_write(priv, addr, 2, (uint8_t *)&v);
}

void sdio_cmd52_write4byte_local(struct whc_sdio *priv, uint32_t addr, uint32_t v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_write(priv, addr, 4, (uint8_t *)&v);
}