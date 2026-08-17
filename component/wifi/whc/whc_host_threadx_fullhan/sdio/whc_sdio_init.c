#include "rtw_whc_common.h"
#include "../../whc_common/host_sdio/whc_host_sdio_init_common.h"

struct whc_sdio whc_sdio_priv = {0};

static void whc_sdio_host_irqhdl_task(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		/* now rt_sema_release in sdio_gpio_int_hdl in rtk_wifi_adapter */
		rtos_sema_take(whc_sdio_priv.host_irq, SEMA_WAIT_TIMEOUT);
		whc_sdio_host_isr_process(priv);
	}
}

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
	uint8_t val;
	//need double check
	val  = SDIO_BLOCK_SIZE & 0xFF;
	sd_cmd52_f0_write(priv, 0x110, 1, &val);
	val = (SDIO_BLOCK_SIZE >> 8) & 0xFF;
	sd_cmd52_f0_write(priv, 0x111, 1, &val);
	printf("set blk size %d \r\n", SDIO_BLOCK_SIZE);
	priv->block_transfer_len = SDIO_BLOCK_SIZE;

	return TRUE;
}

void whc_sdio_host_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;

	uint32_t Interval = 10;
	struct whc_sdio *priv = &whc_sdio_priv;
	printf("sdio polling every %d \n", (int)Interval);

	while (1) {
		// polling int reg
		whc_sdio_host_isr_process(priv);
		rt_thread_mdelay(Interval);
	}
}

void whc_sdio_host_init_drv(void)
{
	rtos_sema_create(&(whc_sdio_priv.host_send), 1, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_irq), 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_send_block_sema), 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	rtos_mutex_create(&(whc_sdio_priv.lock));

	/* should higher than polling, polling 7 */
	if (rtos_task_create(NULL, ((const char *)"whc_host_sdio_recv_data_process"), (rtos_task_function_t)whc_host_sdio_recv_data_process, NULL,
						 WIFI_STACK_SIZE_RX_REQ_TASK, 0 + 6) != 0) {
		printf("create whc_host_sdio_recv_data_process fail \n");
	}

#ifndef WHC_SDIO_INT_MODE
	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), (rtos_task_function_t)whc_sdio_host_polling_task, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
	}
#else
	if (rtos_task_create(NULL, ((const char *)"sdio_int_hal_task"), (rtos_task_function_t)whc_sdio_host_irqhdl_task, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
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

	if (priv->whc_host_init_done == 1) {
		return;
	}
	sdio_init_hwlock();

	if (whc_sdio_host_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();
	lwip_module_init();

	priv->whc_host_init_done = 1;

	/* tell dev host type rtos */
	whc_host_set_host();
	/* init wifi when sdio done */
	whc_host_wifi_on();
}
