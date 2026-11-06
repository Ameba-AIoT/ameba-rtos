// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 */
#ifndef __WHC_GSPI_DRVIO_H__
#define __WHC_GSPI_DRVIO_H__

#define _RND(sz, r)	((((sz) + ((r) - 1)) / (r)) * (r))
#define _RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0 : 1)) << 2)

/* Register access. Every op is one CS-low GSPI frame. */
u8  gspi_read8(struct whc_gspi *priv, u32 addr);
u16 gspi_read16(struct whc_gspi *priv, u32 addr);
u32 gspi_read32(struct whc_gspi *priv, u32 addr);
s32 gspi_write8(struct whc_gspi *priv, u32 addr, u8 val);
s32 gspi_write16(struct whc_gspi *priv, u32 addr, u16 val);
s32 gspi_write32(struct whc_gspi *priv, u32 addr, u32 val);

/* FIFO access.
 * gspi_write_port: src points at [INIC_TX_DESC | payload], cnt bytes.
 * gspi_read_port:  fb is a frame buffer with GSPI_FIFO_FRAME_LEN(cnt) bytes;
 *                  on return the payload sits at fb + GSPI_CMD_PHASE_LEN.
 */
u32 gspi_write_port(struct whc_gspi *priv, u8 *src, u32 cnt);
u32 gspi_read_port(struct whc_gspi *priv, u8 *fb, u32 cnt);

/* Keep the rtw_* spelling the shared WHC code and SDIO/SPI drivers use. */
#define rtw_read8(priv, addr)		gspi_read8((priv), (addr))
#define rtw_read16(priv, addr)		gspi_read16((priv), (addr))
#define rtw_read32(priv, addr)		gspi_read32((priv), (addr))
#define rtw_write8(priv, addr, val)	gspi_write8((priv), (addr), (val))
#define rtw_write16(priv, addr, val)	gspi_write16((priv), (addr), (val))
#define rtw_write32(priv, addr, val)	gspi_write32((priv), (addr), (val))

#endif /* __WHC_GSPI_DRVIO_H__ */
