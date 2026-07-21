#ifndef __WHC_HOST_SDIO_TRX_COMMON_H__
#define __WHC_HOST_SDIO_TRX_COMMON_H__

#include <stdint.h>

struct whc_sdio;

/* Shared SDIO host RX bottom-half, compiled into each whc host port
 * (stm32 / threadx / zephyr).
 *
 * whc_host_sdio_recv_data_process() is the RX task entry: it blocks on the
 * device RX-ready wake, polls the RX FIFO and hands each buffer to the
 * per-port whc_host_sdio_recv_process().
 *
 * Each port must provide the following, reachable via "rtw_whc_common.h":
 *   - struct whc_sdio        (members used here: sdio_himr, dev_state,
 *                             host_recv_wake)
 *   - global instance        : struct whc_sdio whc_sdio_priv;
 *   - whc_malloc(sz) / whc_free(ptr)
 *   - WHC_HOST_SDIO_RECV_PKTS(pbuf)     : handle a WHC_WIFI_EVT_RECV_PKTS buffer
 *   - hal: sdio_read8 / sdio_read_port
 *   - SDIO_BLOCK_SIZE and the SDIO register map (whc_host_sdio_reg.h)
 */
void whc_host_sdio_recv_data_process(void);

/* Shared RX dispatch: routes one received buffer by event type */
int whc_host_sdio_recv_process(uint8_t *pbuf);

/* per-port hook: deliver a non-RECV_PKTS buffer to the user/app layer */
void whc_host_pkt_rx_to_user(uint8_t *pbuf);

/* Shared HISR dispatch (one pass): read/clear HISR, handle TX-avail and
 * RX-request. Called by each port's rtw_sdio_interrupt_handler wrapper.
 * Additional required port macros:
 *   - whc_sem_give(sem)              : give a sema handle/object
 */
void whc_host_sdio_isr_process(struct whc_sdio *priv);

/* Shared SDIO host TX entry: fill TX descriptor and push buf to the device
 * TX FIFO, blocking on the TXBD-avail sema/poll until a slot is free.
 * Additional required port macros:
 *   - whc_mutex_take(m, t) / whc_mutex_give(m)
 *   - whc_sem_take_timeout(s, t)
 *   - whc_msleep(ms)
 * plus rtw_sdio_query_txbd_status() and hal rtw_write_port(). */
void whc_host_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb);

#endif /* __WHC_HOST_SDIO_TRX_COMMON_H__ */
