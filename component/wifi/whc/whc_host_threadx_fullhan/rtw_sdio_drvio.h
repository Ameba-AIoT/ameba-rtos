#ifndef __RTW_SDIO_DRVIO_H__
#define __RTW_SDIO_DRVIO_H__

#ifndef _RND
#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)
inline static uint32_t _RND4(uint32_t sz)
{

	uint32_t	val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;

}

inline static uint32_t _RND8(uint32_t sz)
{

	uint32_t	val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;

}

inline static uint32_t _RND128(uint32_t sz)
{

	uint32_t	val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;

}
#endif

#define rtw_read8(priv, addr) sdio_read8((priv), (addr))
#define rtw_read16(priv, addr) sdio_read16((priv), (addr))
#define rtw_read_port(priv, addr, cnt, mem) sdio_read_port((priv), (addr), (cnt), (mem))

#define  rtw_write8(priv, addr, val) sdio_write8((priv), (addr), (val))
#define  rtw_write16(priv, addr, val) sdio_write16((priv), (addr), (val))
#define  rtw_write32(priv, addr, val) sdio_write32((priv), (addr), (val))

#define rtw_write_port(priv, addr, cnt, mem) sdio_write_port((priv), (addr), (cnt), (mem))


int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata);
int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata);
uint8_t sd_f0_read8(struct whc_sdio *priv, uint32_t addr, int *err);
void sd_f0_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err);
uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err);
void sd_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err);
void sd_write16(struct whc_sdio *priv, uint32_t addr, uint16_t v, int *err);
void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, int *err);
int sd_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, u8 *pdata);
int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, u8 *pdata);

uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr);
uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr);
void sdio_read_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *rmem);
uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);
uint8_t sdio_f0_read8(struct whc_sdio *priv, uint32_t addr);
int sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val);
int sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val);
int sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val);
int sdio_writeN(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf);
void sdio_write_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *wmem);
uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);
uint32_t sdio_local_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf);
uint32_t sdio_local_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf);
uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr);
uint16_t sdio_cmd52_read2byte_local(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_cmd52_read4byte_local(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_cmd53_read4byte_local(struct whc_sdio *priv, uint32_t addr);
void sdio_cmd52_write1byte_local(struct whc_sdio *priv, uint32_t addr, uint8_t v);
void sdio_cmd52_write2byte_local(struct whc_sdio *priv, uint32_t addr, uint16_t v);
void sdio_cmd52_write4byte_local(struct whc_sdio *priv, uint32_t addr, uint32_t v);
int sd_cmd52_f0_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata);

#endif


