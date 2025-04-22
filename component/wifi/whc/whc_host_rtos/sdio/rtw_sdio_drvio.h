#ifndef __RTW_SDIO_DRVIO_H__
#define __RTW_SDIO_DRVIO_H__

#ifndef _RND
#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)
inline static u32 _RND4(u32 sz)
{

	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;

}

inline static u32 _RND8(u32 sz)
{

	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;

}

inline static u32 _RND128(u32 sz)
{

	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;

}
#endif

#define rtw_read8(priv, addr) sdio_read8((priv), (addr))
#define rtw_read16(priv, addr) sdio_read16((priv), (addr))
#define rtw_read32(priv, addr) sdio_read32((priv), (addr))
#define rtw_read_mem(priv, addr, cnt, mem) sdio_read_mem((priv), (addr), (cnt), (mem))
#define rtw_read_port(priv, addr, cnt, mem) sdio_read_port((priv), (addr), (cnt), (mem))

#define  rtw_write8(priv, addr, val) sdio_write8((priv), (addr), (val))
#define  rtw_write16(priv, addr, val) sdio_write16((priv), (addr), (val))
#define  rtw_write32(priv, addr, val) sdio_write32((priv), (addr), (val))
#define  rtw_writeN(priv, addr, length, data) sdio_writeN((priv), (addr), (length), (data))

#define rtw_write_port(priv, addr, cnt, mem) sdio_write_port((priv), (addr), (cnt), (mem))


int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata);
int sdio_write_byte(void *func, uint32_t addr, uint8_t pdata);
int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);
int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);
int rtw_inc_and_chk_continual_io_error(struct whc_sdio *priv);
void rtw_reset_continual_io_error(struct whc_sdio *priv);
int sd_cmd52_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata);
int sd_cmd52_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pdata);
uint8_t sd_read8(struct whc_sdio *priv, uint32_t addr, int *err);
uint16_t sd_read16(struct whc_sdio *priv, uint32_t addr, int *err);
uint32_t sd_read32(struct whc_sdio *priv, uint32_t addr, int *err);
void sd_write8(struct whc_sdio *priv, uint32_t addr, uint8_t v, int *err);
void sd_write16(struct whc_sdio *priv, uint32_t addr, uint16_t v, int *err);
void sd_write32(struct whc_sdio *priv, uint32_t addr, uint32_t v, s32 *err);
int sd_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata);
int sd_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, void *pdata);
uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr);
uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr);
void sdio_read_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *rmem);
uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);
s32 sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val);
s32 sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val);
s32 sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val);
void sdio_write_mem(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *wmem);
uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);
s32 sdio_local_read(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf);
s32 sdio_local_write(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *pbuf);
uint8_t sdio_cmd52_read1byte_local(struct whc_sdio *priv, uint32_t addr);

uint32_t sdio_cmd53_read4byte_local(struct whc_sdio *priv, uint32_t addr);
void sdio_enable_data1_irq(void);
void sdio_disable_data1_irq(void);

#endif


