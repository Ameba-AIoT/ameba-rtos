#include "rtw_whc_common.h"
#include "../../whc_common/host_sdio/whc_host_sdio_init_common.h"

extern struct whc_sdio whc_sdio_priv;

void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		/* now rt_sema_release in sdio_gpio_int_hdl in rtk_wifi_adapter */
		rtos_sema_take(whc_sdio_priv.host_irq, SEMA_WAIT_TIMEOUT);
		whc_host_sdio_isr_process(priv);
	}
}

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
	uint8_t val;
	//need double check
	val  = SDIO_BLOCK_SIZE & 0xFF;
	sd_cmd52_f0_write(priv, 0x110, 1, &val);
	val = (SDIO_BLOCK_SIZE >> 8) & 0xFF;
	sd_cmd52_f0_write(priv, 0x111, 1, &val);
	printf("set blk size %d \r\n", SDIO_BLOCK_SIZE);
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}

void sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;

	uint32_t Interval = 10;
	struct whc_sdio *priv = &whc_sdio_priv;
	printf("sdio polling every %d \n", (int)Interval);

	while (1) {
		// polling int reg
		whc_host_sdio_isr_process(priv);
		rt_thread_mdelay(Interval);
	}
}
