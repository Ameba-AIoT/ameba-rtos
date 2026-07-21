#include "rtw_whc_common.h"
#include "../../whc_common/host_sdio/whc_host_sdio_init_common.h"
extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;

void rtw_sdio_polling_timer_expiry(struct k_timer *dummy);

//timer for sdio int polling
K_SEM_DEFINE(sdio_polling_sem, 0, SEMA_MAX_COUNT);
K_TIMER_DEFINE(sdio_polling_timer, rtw_sdio_polling_timer_expiry, NULL);

//TODO check real stack size
#define SDIO_POLLING_STACK_SIZE 1024
K_THREAD_STACK_DEFINE(sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE);
struct k_thread sdio_polling_thread;

static void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	whc_host_sdio_isr_process(priv);
}

uint32_t rtw_sdio_enable_func(struct whc_sdio *priv)
{
	//int err = 0;

	//TODO set block size SDIO_BLOCK_SIZE

	priv->func = &sdio_func1;
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}


void rtw_sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;

	while (1) {
		// wait for sema from timer
		k_sem_take(&sdio_polling_sem, K_FOREVER);
		rtw_sdio_interrupt_handler();
	}
}

void rtw_sdio_polling_timer_expiry(struct k_timer *dummy)
{
	(void)dummy;
	k_sem_give(&sdio_polling_sem);
}

void rtw_sdio_polling_init(void)
{
	u32 Interval = 10;
	printf("sdio polling every %dms \n", Interval);

	k_thread_create(&sdio_polling_thread,
					sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE,
					rtw_sdio_polling_task,
					NULL, NULL, NULL,
					K_PRIO_PREEMPT(7), 0, K_NO_WAIT);

	//TODO check period to achieve best tp
	k_timer_start(&sdio_polling_timer, K_MSEC(Interval), K_MSEC(Interval));
}


