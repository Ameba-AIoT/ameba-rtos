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

#define VERIFY_RESULT(x)                                                       \
{                                                                              \
	wwd_result_t verify_result;                                                \
	verify_result = (x);                                                       \
	if (verify_result != WWD_SUCCESS) {                                            \
		printf("fail: func=%s line=%d %d\n", __FUNCTION__, __LINE__, x);           \
		return verify_result;                                                  \
	}                                                                          \
}

u8 sdio_read8(struct whc_sdio *priv, u32 addr);
u16 sdio_read16(struct whc_sdio *priv, u32 addr);
u32 sdio_read32(struct whc_sdio *priv, u32 addr);
u32 sdio_read_port(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *mem);
s32 sdio_write8(struct whc_sdio *priv, u32 addr, u8 val);
s32 sdio_write16(struct whc_sdio *priv, u32 addr, u16 val);
s32 sdio_write32(struct whc_sdio *priv, u32 addr, u32 val);
u32 sdio_write_port(struct whc_sdio *priv, u32 addr, u32 cnt, u8 *mem);

#endif


