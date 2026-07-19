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


int sdio_read_byte(void *func, uint32_t addr, uint8_t *pdata);
int sdio_write_byte(void *func, uint32_t addr, uint8_t pdata);
int sdio_read_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);
int sdio_write_fifo(void *func, uint32_t addr, uint8_t *pdata, int len);
uint8_t sdio_read8(struct whc_sdio *priv, uint32_t addr);
uint16_t sdio_read16(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_read32(struct whc_sdio *priv, uint32_t addr);
uint32_t sdio_read_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);
int sdio_write8(struct whc_sdio *priv, uint32_t addr, uint8_t val);
int sdio_write16(struct whc_sdio *priv, uint32_t addr, uint16_t val);
int sdio_write32(struct whc_sdio *priv, uint32_t addr, uint32_t val);
uint32_t sdio_write_port(struct whc_sdio *priv, uint32_t addr, uint32_t cnt, uint8_t *mem);

void sdio_enable_data1_irq(void);
void sdio_disable_data1_irq(void);

#endif


