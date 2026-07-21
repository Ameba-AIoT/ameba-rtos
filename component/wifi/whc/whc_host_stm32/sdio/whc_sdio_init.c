#include "rtw_whc_common.h"

extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;

void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		rtos_sema_take(whc_sdio_priv.host_irq, MUTEX_WAIT_TIMEOUT);
		whc_host_sdio_isr_process(priv);
	}
}

/* called by sdio int hdl in stm32 sdk */
void whc_sdio_irq_sema_give(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	//SDIO->MASK &= ~SDIO_MASK_SDIOITIE;
	sdio_disable_data1_irq();
#endif
	rtos_sema_give(priv->host_irq); /* ignore failure since there is nothing that can be done about it in a ISR */
}

uint32_t rtw_sdio_enable_func(struct whc_sdio *priv)
{
	//TODO set block size SDIO_BLOCK_SIZE
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}

void sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	u32 Interval = 100;
	struct whc_sdio *priv = &whc_sdio_priv;

	printf("sdio polling every %dms \n", Interval);

	while (1) {
		// polling int reg
		whc_host_sdio_isr_process(priv);
		vTaskDelay(Interval);
	}
}

