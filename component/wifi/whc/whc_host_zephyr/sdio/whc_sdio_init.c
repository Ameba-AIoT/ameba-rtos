#include "rtw_whc_common.h"
#include "../../whc_common/host_sdio/whc_host_sdio_init_common.h"

extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;
struct whc_sdio whc_sdio_priv = {0};

K_THREAD_STACK_DEFINE(whc_sdio_rx_req_stack, WIFI_STACK_SIZE_RX_REQ_TASK);
static struct k_thread ameba_host_rx_req_thread;
/* defined in ameba_wifi.c in zephyr sdk */
void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
int (*rx_callback_ptr)(uint8_t idx, void *buffer, uint16_t len);

K_THREAD_STACK_DEFINE(sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE);
struct k_thread sdio_polling_thread;

uint32_t whc_sdio_host_enable_func(struct whc_sdio *priv)
{
	//TODO set block size SDIO_BLOCK_SIZE
	priv->func = &sdio_func1;
	priv->block_transfer_len = SDIO_BLOCK_SIZE;

	return TRUE;
}

void whc_sdio_host_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	u32 Interval = 10;
	struct whc_sdio *priv = &whc_sdio_priv;

	while (1) {
		/* check int status */
		whc_sdio_host_isr_process(priv);
		whc_msleep(Interval);
	}
}

static void whc_sdio_host_init_drv(void)
{
	k_sem_init(&whc_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.txbd_wq), 0, SEMA_MAX_COUNT);
	k_mutex_init(&whc_sdio_priv.lock);

#ifndef WHC_SDIO_INT_MODE
	k_thread_create(&sdio_polling_thread,
					sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE,
					whc_sdio_host_polling_task,
					NULL, NULL, NULL,
					K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
#endif

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

	if (whc_sdio_host_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();
}
