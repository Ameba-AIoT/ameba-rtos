#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/crc32.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/rtnetlink.h>
#include <linux/fs.h>


#include <linux/moduleparam.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/errno.h>

#include "sdio_type.h"

sdio_adapter adapter;


void dump_reg(void)
{
	sdio_adapter *padapter = &adapter;

	RTK_PRINTF("SDIO_REG_TX_CTRL(%x)(1B): %x\n", SDIO_REG_TX_CTRL, rtw_read8(padapter, SDIO_REG_TX_CTRL));
	RTK_PRINTF("SDIO_REG_STATIS_RECOVERY_TIMOUT(%x)(2B): %x\n", SDIO_REG_STATIS_RECOVERY_TIMOUT, rtw_read16(padapter, SDIO_REG_STATIS_RECOVERY_TIMOUT));
	RTK_PRINTF("SDIO_REG_32K_TRANS_IDLE_TIME(%x)(2B): %x\n", SDIO_REG_32K_TRANS_IDLE_TIME, rtw_read16(padapter, SDIO_REG_32K_TRANS_IDLE_TIME));
	RTK_PRINTF("SDIO_REG_HIMR(%x)(4B): %x\n", SDIO_REG_HIMR, rtw_read32(padapter, SDIO_REG_HIMR));
	RTK_PRINTF("SDIO_REG_HISR(%x)(4B): %x\n", SDIO_REG_HISR, rtw_read32(padapter, SDIO_REG_HISR));
	RTK_PRINTF("SDIO_REG_RX0_REQ_LEN(%x)(4B): %x\n", SDIO_REG_RX0_REQ_LEN, rtw_read32(padapter, SDIO_REG_RX0_REQ_LEN));
	RTK_PRINTF("SDIO_REG_FREE_TXBD_NUM(%x)(2B): %x\n", SDIO_REG_FREE_TXBD_NUM, rtw_read16(padapter, SDIO_REG_FREE_TXBD_NUM));
	RTK_PRINTF("SDIO_REG_TX_SEQNUM(%x)(1B): %x\n", SDIO_REG_TX_SEQNUM, rtw_read8(padapter, SDIO_REG_TX_SEQNUM));
	RTK_PRINTF("SDIO_REG_HCPWM(%x)(1B): %x\n", SDIO_REG_HCPWM, rtw_read8(padapter, SDIO_REG_HCPWM));
	RTK_PRINTF("SDIO_REG_HCPWM2(%x)(2B): %x\n", SDIO_REG_HCPWM2, rtw_read16(padapter, SDIO_REG_HCPWM2));
	RTK_PRINTF("SDIO_REG_H2C_MSG(%x)(4B): %x\n", SDIO_REG_H2C_MSG, rtw_read32(padapter, SDIO_REG_H2C_MSG));
	RTK_PRINTF("SDIO_REG_C2H_MSG(%x)(4B): %x\n", SDIO_REG_C2H_MSG, rtw_read32(padapter, SDIO_REG_C2H_MSG));
	RTK_PRINTF("SDIO_REG_H2C_MSG_EXT(%x)(4B): %x\n", SDIO_REG_H2C_MSG_EXT, rtw_read32(padapter, SDIO_REG_H2C_MSG_EXT));
	RTK_PRINTF("SDIO_REG_C2H_MSG_EXT(%x)(4B): %x\n", SDIO_REG_C2H_MSG_EXT, rtw_read32(padapter, SDIO_REG_C2H_MSG_EXT));
	RTK_PRINTF("SDIO_REG_HRPWM(%x)(1B): %x\n", SDIO_REG_HRPWM, rtw_read8(padapter, SDIO_REG_HRPWM));
	RTK_PRINTF("SDIO_REG_HRPWM2(%x)(2B): %x\n", SDIO_REG_HRPWM2, rtw_read16(padapter, SDIO_REG_HRPWM2));
	RTK_PRINTF("SDIO_REG_CPU_IND(%x)(1B): %x\n", SDIO_REG_CPU_IND, rtw_read8(padapter, SDIO_REG_CPU_IND));
	RTK_PRINTF("SDIO_REG_CMD_IN2RSP_TIME(%x)(2B): %x\n", SDIO_REG_CMD_IN2RSP_TIME, rtw_read16(padapter, SDIO_REG_CMD_IN2RSP_TIME));
	RTK_PRINTF("SDIO_REG_ERR_FLAG(%x)(1B): %x\n", SDIO_REG_ERR_FLAG, rtw_read8(padapter, SDIO_REG_ERR_FLAG));
	RTK_PRINTF("SDIO_REG_DATA_ERRCNT(%x)(1B): %x\n", SDIO_REG_DATA_ERRCNT, rtw_read8(padapter, SDIO_REG_DATA_ERRCNT));
	RTK_PRINTF("SDIO_REG_CRC_ERR_INDEX(%x)(1B): %x\n", SDIO_REG_CRC_ERR_INDEX, rtw_read8(padapter, SDIO_REG_CRC_ERR_INDEX));
	RTK_PRINTF("SDIO_REG_AVAI_BD_NUM_TH_L(%x)(4B): %x\n", SDIO_REG_AVAI_BD_NUM_TH_L, rtw_read32(padapter, SDIO_REG_AVAI_BD_NUM_TH_L));
	RTK_PRINTF("SDIO_REG_AVAI_BD_NUM_TH_H(%x)(4B): %x\n", SDIO_REG_AVAI_BD_NUM_TH_H, rtw_read32(padapter, SDIO_REG_AVAI_BD_NUM_TH_H));
	RTK_PRINTF("SDIO_REG_RX_AGG_CFG(%x)(2B): %x\n", SDIO_REG_RX_AGG_CFG, rtw_read16(padapter, SDIO_REG_RX_AGG_CFG));


	RTK_PRINTF("SDIO_REG_RXBD_NUM(%x)(2B): %x\n", SDIO_REG_RXBD_NUM, rtw_read16(padapter, SDIO_REG_RXBD_NUM));
	RTK_PRINTF("SDIO_REG_CPU_INT_MASK(%x)(2B): %x\n", SDIO_REG_CPU_INT_MASK, rtw_read16(padapter, SDIO_REG_CPU_INT_MASK));
	RTK_PRINTF("SDIO_REG_CPU_INT(%x)(2B): %x\n", SDIO_REG_CPU_INT, rtw_read16(padapter, SDIO_REG_CPU_INT));
	RTK_PRINTF("SDIO_REG_CCPWM(%x)(1B): %x\n", SDIO_REG_CCPWM, rtw_read8(padapter, SDIO_REG_CCPWM));
	RTK_PRINTF("SDIO_REG_CCPWM2(%x)(1B): %x\n", SDIO_REG_CCPWM2, rtw_read8(padapter, SDIO_REG_CCPWM2));
	RTK_PRINTF("SDIO_REG_AHB_DMA_CTRL(%x)(4B): %x\n", SDIO_REG_AHB_DMA_CTRL, rtw_read32(padapter, SDIO_REG_AHB_DMA_CTRL));
	RTK_PRINTF("SDIO_REG_TXBUF_UNIT_SZ(%x)(1B): %x\n", SDIO_REG_TXBUF_UNIT_SZ, rtw_read8(padapter, SDIO_REG_TXBUF_UNIT_SZ));
	RTK_PRINTF("SDIO_REG_FREE_RXBD_CNT(%x)(2B): %x\n", SDIO_REG_FREE_RXBD_CNT, rtw_read16(padapter, SDIO_REG_FREE_RXBD_CNT));

}

void sd_int_dpc(sdio_adapter *padapter)
{
	u32 rx_len_rdy;

	if (padapter->sdio_hisr & SDIO_HISR_AVAL_INT) {
		rtw_update_free_txbd(padapter);
	}

	if (padapter->sdio_hisr & SDIO_HISR_RX_REQUEST) {
		padapter->sdio_hisr ^= SDIO_HISR_RX_REQUEST;

		do {
			//Sometimes rx length will be zero. driver need to use cmd53 read again.
			if (padapter->rx_len == 0) {

				//validate RX_LEN_RDY before reading RX0_REQ_LEN
				rx_len_rdy = rtw_read32(padapter, SDIO_REG_RX0_REQ_LEN);
				if (rx_len_rdy & SDIO_RX_REQ_LEN_RDY) {
					padapter->rx_len = rtw_read32(padapter, SDIO_REG_RX0_REQ_LEN) & SDIO_RX_REQ_LEN_MSK;
				}
			}

			if (padapter->rx_len) {
				loopback_recv(padapter, padapter->rx_len);
				padapter->rx_len = 0;
			} else {
				break;
			}
		} while (1);
	}

	if (padapter->sdio_hisr & SDIO_HISR_CPWM1) {
		RTK_PRINTF("HCPWM: %x\n", rtw_read8(padapter, SDIO_REG_HCPWM));
	}

	if (padapter->sdio_hisr & SDIO_HISR_CPWM2) {
		RTK_PRINTF("HCPWM2: %x\n", rtw_read16(padapter, SDIO_REG_HCPWM2));
	}

	if (padapter->sdio_hisr & SDIO_HISR_C2H_MSG_INT) {
		RTK_PRINTF("C2H_MSG: %x\n", rtw_read32(padapter, SDIO_REG_C2H_MSG));
		RTK_PRINTF("C2H_MSG_EXT: %x\n", rtw_read32(padapter, SDIO_REG_C2H_MSG_EXT));
	}
}


static void rtw_sdio_irq(struct sdio_func *func)
{
	sdio_adapter *padapter = &adapter;
	u8 rx_len_rdy;
	u32 v32;

	if (func == NULL) {
		RTK_PRINTF("%s: func is NULL!\n", __FUNCTION__);
		return;
	}

	if ((padapter->surprise_remove == TRUE)) {
		RTK_PRINTF("%s: surprise remove!\n", __FUNCTION__);
		return;
	}

	padapter->sdio_hisr = rtw_read32(padapter, SDIO_REG_HISR);

	//RTK_PRINTF("hisr: %x\n", padapter->sdio_hisr);

	if (padapter->sdio_hisr & SDIO_HISR_TXBD_OVF) {
		RTK_PRINTF("TXBD overflow error\n");
	}

	//rx length is valide only when RX_REQ_LEN_RDY(BIT31) is 1
	rx_len_rdy = rtw_read32(padapter, SDIO_REG_RX0_REQ_LEN);
	if (rx_len_rdy & SDIO_RX_REQ_LEN_RDY) {
		padapter->rx_len = rtw_read32(padapter, SDIO_REG_RX0_REQ_LEN) & SDIO_RX_REQ_LEN_MSK;
	}

	if (padapter->sdio_hisr & padapter->sdio_himr) {
		padapter->sdio_hisr &= padapter->sdio_himr;
		// clear HISR
		v32 = padapter->sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (v32) {
			rtw_write32(padapter, SDIO_REG_HISR, v32);
		}

		sd_int_dpc(padapter);
	} else {
		RTK_PRINTF("Interrupt mismatch error! hisr(%x), himr(%x)\n", padapter->sdio_hisr, padapter->sdio_himr);
	}
}

#if 0
static u32 rtk_sdio_config_rx_agg(sdio_adapter *padapter)
{
	// to confirm, for rx aggregation
	u8	valueTimeout;
	u8	valueBDCount;
	u16	valueRXBDMax;

	//DMA timeout
	valueTimeout = 0x06;
	//AGG BD threshold
	valueBDCount = 10 * 2; //SDIO needs 2 BD to send one rx packet

	//get rx bd max num
	valueRXBDMax = rtw_read16(padapter, SDIO_REG_RXBD_NUM);

	if (valueBDCount > valueRXBDMax) {
		RTK_PRINTF("%s: HALINIT FAILED! RX_AGG_NUM(%d) > RXBD_NUM(%d)\n", __FUNCTION__, valueBDCount, valueRXBDMax);
		return _FAIL;
	}

	rtw_write8(padapter, SDIO_REG_RX_AGG_CFG + 1, valueTimeout);
	rtw_write8(padapter, SDIO_REG_RX_AGG_CFG, valueBDCount);

	//enable rx aggregation
	rtw_write16(padapter, SDIO_REG_RX_AGG_CFG, rtw_read16(padapter, SDIO_REG_RX_AGG_CFG) | SDIO_RX_AGG_EN);

	return _SUCCESS;
}
#endif

static int rtk_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	sdio_adapter *padapter = &adapter;
	int i;
	u16 fw_ready;
	int ret;

	RTK_PRINTF("%s():++\n", __FUNCTION__);
	RTK_PRINTF("vendor=0x%04x device=0x%04x class=0x%02x\n", func->vendor, func->device, func->class);

	memset((void *)padapter, 0, sizeof(sdio_adapter));

	padapter->func = func;

	padapter->tx_buffer = rtw_malloc(TRX_BUFFER_SIZE);
	if (padapter->tx_buffer == NULL) {
		RTK_PRINTF("memory allocate for loopback tx buffer failed!\n");
		goto exit1;
	}

	padapter->rx_buffer = rtw_malloc(TRX_BUFFER_SIZE);
	if (padapter->rx_buffer == NULL) {
		RTK_PRINTF("memory allocate for loopback rx buffer failed!\n");
		goto exit2;
	}

	/* init char dev */
	ret = mychardev_init();
	if (ret) {
		RTK_PRINTF("memory allocate for loopback rx buffer failed!\n");
		goto exit3;
	}

	/* enable func */
	rtw_enable_function(func);

	/* config blocksize */
	padapter->block_transfer_len = 512;
	rtw_set_block_size(func, 512);

	/* wakeup sdio device */
	//rtw_write8(padapter, SDIO_REG_HRPWM, BIT1|BIT0);

#ifdef FW_DOWNLOAD
	/* image download, amebadplus not support yet */
	//RL7005_FirmwareDownload(padapter);
#endif

	padapter->stop_trx = TRUE;
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(padapter, SDIO_REG_CPU_IND);
		if (fw_ready & SDIO_SYSTEM_TRX_RDY_IND) {
			padapter->stop_trx = FALSE;
			break;
		}
		rtw_msleep_os(10);
	}
	if (i == 100) {
		RTK_PRINTF("%s: Wait Device Firmware Ready Timeout!!SDIO_REG_CPU_IND @ 0x%04x\n", __FUNCTION__, fw_ready);
		goto exit3;
	}

	/* register irq */
	rtw_register_irq(func, rtw_sdio_irq);

	/* configure interrupt */
	rtw_write32(padapter, SDIO_REG_HISR, 0xFFFFFFFF);
	rtw_write32(padapter, SDIO_REG_HIMR, 0);
	padapter->sdio_himr = MASK_SDIO_HIMR;

	/* configure rx agg */
	//rtk_sdio_config_rx_agg(padapter);

	/* update free txbd */
	rtw_update_free_txbd(padapter);

	padapter->tx_max_size = rtw_read8(padapter, SDIO_REG_TXBUF_UNIT_SZ) * 64 - 32;
	padapter->txbd_size = rtw_read16(padapter, SDIO_REG_TXBD_NUM);

	padapter->surprise_remove = FALSE;

	rtw_write16(padapter, 0x0, rtw_read16(padapter, 0) | BIT1);

	rtw_enable_interrupt(padapter);

	printk("probe sdio device success\n");

	//dump_reg();
	return 0;
exit3:
	mychardev_exit();
	rtw_mfree(padapter->rx_buffer);
exit2:
	rtw_mfree(padapter->tx_buffer);
exit1:
	rtw_disable_function(func);
	return -ENODEV;
}

static void rtk_sdio_remove(struct sdio_func *func)
{
	sdio_adapter *padapter = &adapter;
	rtw_release_irq(func);
	rtw_disable_function(func);
	padapter->surprise_remove = TRUE;
	mychardev_exit();
	rtw_mfree(padapter->rx_buffer);
	rtw_mfree(padapter->tx_buffer);
	RTK_PRINTF("%s():++\n", __FUNCTION__);
}

static const struct sdio_device_id sdio_ids[] = {
	{ SDIO_DEVICE(0x024c, 0x8721)}, 							//amebad
	{ SDIO_DEVICE(0x024c, 0x8722)}, 							//amebadplus
#if defined(RTK_SDIO_WIFI) && (RTK_SDIO_WIFI == 1)
	{ SDIO_DEVICE(0x024c, 0x8720), .class = SDIO_CLASS_WLAN}, 	//6973 or 7005 WIFI
#elif defined(RTK_SDIO_BT) && (RTK_SDIO_BT == 1)
	{ SDIO_DEVICE(0x024c, 0x8720), .class = SDIO_CLASS_BT_A}, 	//6973 or 7005 BT
#endif
};

static struct sdio_driver rtk_sdio_driver = {
	.probe	= rtk_sdio_probe,
	.remove	= rtk_sdio_remove,
	.name	= "RTKSDIO",
	.id_table	= sdio_ids,
};

static int __init rtk_sdio_init_module(void)
{
	int ret = 0;

	RTK_PRINTF("rtk_sdio module init start\n");

	ret = rtw_register_driver(&rtk_sdio_driver);
	if (ret != 0) {
		RTK_PRINTF("sdio register driver Failed!\n");
	}

	RTK_PRINTF("rtk_sdio module init ret=%d\n", ret);
	return ret;
}

static void __exit rtk_sdio_cleanup_module(void)
{
	RTK_PRINTF("rtk_sdio module exit start\n");

	rtw_unregister_driver(&rtk_sdio_driver);

	RTK_PRINTF("rtk_sdio module exit success\n");
}

module_init(rtk_sdio_init_module);
module_exit(rtk_sdio_cleanup_module);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek SDIO");
MODULE_LICENSE("GPL");
MODULE_VERSION("RTK_SDIO_1_0");

