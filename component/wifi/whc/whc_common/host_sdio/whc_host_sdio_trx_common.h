#ifndef __WHC_HOST_SDIO_TRX_H__
#define __WHC_HOST_SDIO_TRX_H__

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
 *   - WHC_MALLOC(sz) / WHC_FREE(ptr)
 *   - WHC_SEM_TAKE(sem)                 : block on the RX wake sema object
 *   - WHC_HOST_SDIO_RX_INT_DISABLE(p)   : mask device RX_REQ int (or no-op)
 *   - WHC_HOST_SDIO_RX_INT_RESTORE(p)   : restore device RX_REQ int (or no-op)
 *   - WHC_HOST_SDIO_RECV_PKTS(pbuf)     : handle a WHC_WIFI_EVT_RECV_PKTS buffer
 *   - WHC_HOST_SDIO_RX_DEFAULT(pbuf)    : handle any other event buffer
 *   - hal: sdio_read8 / sdio_local_read / sdio_read_port
 *   - SDIO_BLOCK_SIZE and the SDIO register map (whc_host_sdio_reg.h)
 */
void whc_host_sdio_recv_data_process(void);

/* Shared RX dispatch: routes one received buffer by event type via the
 * per-port WHC_HOST_SDIO_RECV_PKTS / WHC_HOST_SDIO_RX_DEFAULT macros. */
int whc_host_sdio_recv_process(uint8_t *pbuf);

/* per-port hook: deliver a non-RECV_PKTS buffer to the user/app layer */
void whc_host_pkt_rx_to_user(uint8_t *pbuf);

/* Shared HISR dispatch (one pass): read/clear HISR, handle TX-avail and
 * RX-request. Called by each port's rtw_sdio_interrupt_handler wrapper.
 * Additional required port macros:
 *   - WHC_SEM_GIVE(sem)              : give a sema handle/object
 *   - WHC_HOST_SDIO_HISR_CLEAR(p)    : clear serviced HISR bits (or no-op)
 */
void whc_host_sdio_isr_process(struct whc_sdio *priv);

#endif /* __WHC_HOST_SDIO_TRX_H__ */
