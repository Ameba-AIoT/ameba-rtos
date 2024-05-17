#ifndef __RTW_SDIO_DRVIO_H__
#define __RTW_SDIO_DRVIO_H__

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
#endif

