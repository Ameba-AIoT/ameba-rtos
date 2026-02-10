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


s32 sd_cmd52_read(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pdata);
s32 sd_cmd52_write(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pdata);
u8 sd_read8(struct inic_sdio *priv, u32 addr, s32 *err);
u8 sd_f0_read8(struct inic_sdio *priv, u32 addr, s32 *err);
void sd_f0_write8(struct inic_sdio *priv, u32 addr, u8 v, s32 *err);
u16 sd_read16(struct inic_sdio *priv, u32 addr, s32 *err);
u32 sd_read32(struct inic_sdio *priv, u32 addr, s32 *err);
void sd_write8(struct inic_sdio *priv, u32 addr, u8 v, s32 *err);
void sd_write16(struct inic_sdio *priv, u32 addr, u16 v, s32 *err);
void sd_write32(struct inic_sdio *priv, u32 addr, u32 v, s32 *err);
s32 sd_read(struct inic_sdio *priv, u32 addr, u32 cnt, void *pdata);
s32 sd_write(struct inic_sdio *priv, u32 addr, u32 cnt, void *pdata);

u8 sdio_read8(struct inic_sdio *priv, u32 addr);
u16 sdio_read16(struct inic_sdio *priv, u32 addr);
u32 sdio_read32(struct inic_sdio *priv, u32 addr);
void sdio_read_mem(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *rmem);
u32 sdio_read_port(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *mem);
u8 sdio_f0_read8(struct inic_sdio *priv, u32 addr);
s32 sdio_write8(struct inic_sdio *priv, u32 addr, u8 val);
s32 sdio_write16(struct inic_sdio *priv, u32 addr, u16 val);
s32 sdio_write32(struct inic_sdio *priv, u32 addr, u32 val);
s32 sdio_writeN(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pbuf);
void sdio_write_mem(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *wmem);
u32 sdio_write_port(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *mem);
s32 sdio_local_read(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pbuf);
s32 sdio_local_write(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pbuf);
u8 sdio_cmd52_read1byte_local(struct inic_sdio *priv, u32 addr);
u16 sdio_cmd52_read2byte_local(struct inic_sdio *priv, u32 addr);
u32 sdio_cmd52_read4byte_local(struct inic_sdio *priv, u32 addr);
u32 sdio_cmd53_read4byte_local(struct inic_sdio *priv, u32 addr);
void sdio_cmd52_write1byte_local(struct inic_sdio *priv, u32 addr, u8 v);
void sdio_cmd52_write2byte_local(struct inic_sdio *priv, u32 addr, u16 v);
void sdio_cmd52_write4byte_local(struct inic_sdio *priv, u32 addr, u32 v);

#endif

