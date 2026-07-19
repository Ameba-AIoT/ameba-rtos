#ifndef __RTW_SDIO_DRVIO_H__
#define __RTW_SDIO_DRVIO_H__

#define rtw_read8(priv, addr) sdio_read8((priv), (addr))
#define rtw_read16(priv, addr) sdio_read16((priv), (addr))
#define rtw_read32(priv, addr) sdio_read32((priv), (addr))
#define rtw_read_port(priv, addr, cnt, mem) sdio_read_port((priv), (addr), (cnt), (mem))

#define  rtw_write8(priv, addr, val) sdio_write8((priv), (addr), (val))
#define  rtw_write16(priv, addr, val) sdio_write16((priv), (addr), (val))
#define  rtw_write32(priv, addr, val) sdio_write32((priv), (addr), (val))

#define rtw_write_port(priv, addr, cnt, mem) sdio_write_port((priv), (addr), (cnt), (mem))


/* low-level primitives provided by the zephyr SDIO glue */
int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata);
int sdio_write_byte(void *func, uint32_t addr, uint8_t pdata);
int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);
int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);

u8 sdio_read8(struct whc_sdio *priv, u32 addr);
u16 sdio_read16(struct whc_sdio *priv, u32 addr);
u32 sdio_read32(struct whc_sdio *priv, u32 addr);
u32 sdio_read_port(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *mem);
s32 sdio_write8(struct whc_sdio *priv, u32 addr, u8 val);
s32 sdio_write16(struct whc_sdio *priv, u32 addr, u16 val);
s32 sdio_write32(struct whc_sdio *priv, u32 addr, u32 val);
u32 sdio_write_port(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *mem);

#endif

