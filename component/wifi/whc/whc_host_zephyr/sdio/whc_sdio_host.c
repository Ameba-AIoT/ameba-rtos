#include "rtw_whc_common.h"
#include "whc_sdio_host_app.h"

struct whc_sdio whc_sdio_priv = {0};

#define whc_host_rx_req_task_prio	2

#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)

K_THREAD_STACK_DEFINE(whc_sdio_rx_req_stack, WIFI_STACK_SIZE_RX_REQ_TASK);

static struct k_thread ameba_host_rx_req_thread;

void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
int (*rx_callback_ptr)(uint8_t idx, void *buffer, uint16_t len);

static void whc_host_sdio_init_drv(void)
{
	k_sem_init(&whc_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.host_recv_done), 1, 1);
	k_sem_init(&(whc_sdio_priv.txbd_wq), 0, SEMA_MAX_COUNT);
	k_mutex_init(&whc_sdio_priv.lock);

	/* init host<->dev synchronous request/return */
	k_sem_init(&(whc_sdio_priv.api_ret_sema), 0, SEMA_MAX_COUNT);
	k_mutex_init(&(whc_sdio_priv.send_mutex));

	k_thread_create(&ameba_host_rx_req_thread, whc_sdio_rx_req_stack,
					WIFI_STACK_SIZE_RX_REQ_TASK,
					(k_thread_entry_t)whc_host_sdio_recv_data_process, NULL, NULL, NULL,
					whc_host_rx_req_task_prio, K_USER,
					K_NO_WAIT);
}

/**
 * @brief  to initialize the whc sdio host.
 * @param  none.
 * @return none.
 */
void whc_host_sdio_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_host_sdio_init_drv();
}

void whc_host_sdio_recv_pkts(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	uint32_t len = msg_info->data_len;

	// direct up to zephyr network, free after done
	if (rx_callback_ptr) {
		rx_callback_ptr(msg_info->wlan_idx, data, len);
	}

	WHC_FREE(buf);
}

