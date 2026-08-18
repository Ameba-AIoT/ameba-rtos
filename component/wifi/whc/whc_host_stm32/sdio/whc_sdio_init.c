#include "rtw_whc_common.h"

struct whc_sdio whc_sdio_priv = {0};

static void whc_sdio_host_irqhdl_task(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		rtos_sema_take(whc_sdio_priv.host_irq, MUTEX_WAIT_TIMEOUT);
		whc_sdio_host_isr_process(priv);
	}
}

/* called by sdio int hdl in stm32 sdk */
void whc_sdio_host_irq_sema_give(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	//SDIO->MASK &= ~SDIO_MASK_SDIOITIE;
	sdio_disable_data1_irq();
#endif
	rtos_sema_give(priv->host_irq); /* ignore failure since there is nothing that can be done about it in a ISR */
}

uint32_t whc_sdio_host_enable_func(struct whc_sdio *priv)
{
	//TODO set block size SDIO_BLOCK_SIZE
	priv->block_transfer_len = SDIO_BLOCK_SIZE;

	return TRUE;
}

void whc_sdio_host_polling_task(void *arg1, void *arg2, void *arg3)
{
	u32 Interval = 100;
	struct whc_sdio *priv = &whc_sdio_priv;

	printf("sdio polling every %dms \n", Interval);

	while (1) {
		// polling int reg
		whc_sdio_host_isr_process(priv);
		vTaskDelay(Interval);
	}
}

void whc_sdio_host_init_drv(void)
{
	//printf("init sdio sema \r\n");
	rtos_mutex_create(&whc_sdio_priv.lock);
	rtos_mutex_create(&whc_sdio_priv.host_send);
	rtos_sema_create(&whc_sdio_priv.host_irq, 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.txbd_wq), 0, SEMA_MAX_COUNT);

	/* should higher than polling, polling 7 */
	/* rx task */
	if (rtos_task_create(NULL, ((const char *)"whc_host_sdio_recv_data_process"), whc_host_sdio_recv_data_process, NULL, WIFI_STACK_SIZE_RX_REQ_TASK,
						 0 + 6) != SUCCESS) {
		printf("create whc_host_sdio_recv_data_process fail \n");
	}

#ifndef WHC_SDIO_INT_MODE
	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), whc_sdio_host_polling_task, NULL, SDIO_POLLING_STACK_SIZE,
						 0 + 7) != SUCCESS) {
		printf("create sdioPollingTask fail \n");
	}
#else
	if (rtos_task_create(NULL, ((const char *)"whc_sdio_host_irqhdl_task"), whc_sdio_host_irqhdl_task, NULL, SDIO_POLLING_STACK_SIZE,
						 0 + 7) != SUCCESS) {
		printf("create sdio_int_hal_task fail \n");
	}
#endif
}

/**
 * @brief  to initialize the whc host.
 * @param  none.
 * @return none.
 */
void whc_host_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (whc_sdio_priv.whc_host_init_done == 1) {
		return;
	}

	/* lock for sdio hw lock */
	sdio_init_hwlock();

	if (whc_sdio_host_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();

	whc_sdio_priv.whc_host_init_done = 1;
}