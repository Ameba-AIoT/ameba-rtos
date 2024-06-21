#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "sdio_type.h"

extern sdio_adapter adapter;

int loopback_send(sdio_adapter *padapter, u8 *pbuf, u32 size)
{
	u32	polling_num = 0;
	tx_desc *ptxdesc = (tx_desc *)pbuf;
	static u32 loopback_send_cnt = 0;

query_free_page:
	// check if hardware tx fifo page is enough
	if (_FALSE == rtw_sdio_query_free_txbd(padapter, ptxdesc->bus_agg_num)) {
		polling_num++;
		if ((polling_num % 60) == 0) {//or 80
			//RTK_PRINTF("%s: FIFO starvation!(%d) len=%d agg=%d\n",
			//	__func__, n, sz, ptxdesc->bus_agg_num);
			rtw_msleep_os(1);
		}

		rtw_update_free_txbd(padapter);
		goto query_free_page;
	}

	if (padapter->surprise_remove == _TRUE) {
		RTK_PRINTF("%s: bSurpriseRemoved(wirte port)\n", __FUNCTION__);
		return -1;
	}

	//Check TX Descriptor information, fault TX descriptor can driver device crash
	if (((ptxdesc->offset + ptxdesc->txpktsize) > padapter->tx_max_size) || ((u32)(pbuf) % 4)) {
		RTK_PRINTF("%s: PKT SIZE ERROR OFFSET(%d) PKTSIZE(%d)\n", __FUNCTION__, ptxdesc->offset, ptxdesc->txpktsize);
	}

	rtw_write_txfifo(padapter, size, pbuf);
	RTK_PRINTF("Send successed [%d]!, sz=%d\n", ++loopback_send_cnt, size);


	return _SUCCESS;
}

int loopback_start(sdio_adapter *padapter)
{
	tx_desc *ptxdesc;
	u8 *loopback_buf = NULL;
	int ret = _SUCCESS;
	int i;

	loopback_buf = padapter->tx_buffer;

	ptxdesc = (tx_desc *)loopback_buf;
	ptxdesc->offset = (sizeof(tx_desc));
	ptxdesc->bus_agg_num = 1;
	ptxdesc->txpktsize = LOOPBACK_SIZE - ptxdesc->offset;

#ifdef CHECK_DATA
	for (i = 0; i < ptxdesc->txpktsize; i++) {
		*((u8 *)(loopback_buf + ptxdesc->offset + i)) = i % 256;
	}
#endif

	ret = loopback_send(padapter, loopback_buf, LOOPBACK_SIZE);

	return ret;
}


void loopback_recv(sdio_adapter *padapter, u32 size)
{
	u32 ret;
	static u32 loopback_recv_cnt = 0;
	int i;
	u8 *ppkt;


	ppkt = padapter->rx_buffer;

	ret = rtw_read_rxfifo(padapter, size, ppkt);
	if (ret) {
		RTK_PRINTF("%s: read port FAIL!\n", __FUNCTION__);
	}
	RTK_PRINTF("recv successed [%d], size=%d!\n", ++loopback_recv_cnt, size);


#ifdef CHECK_DATA
	for (i = 0; i < size - 16; i++) {
		if (ppkt[i + 16] != i % 256) {
			RTK_PRINTF("fail: %d, %d, %d\n", i, ppkt[i], i % 256);
		}
	}
#endif

}

thread_return rtw_sdio_loopback_thread(thread_context context)
{
	s32 ret;
	sdio_adapter *padapter = &adapter;

	ret = _SUCCESS;

	RTK_PRINTF("start %s\n", __FUNCTION__);

	do {
		if (rtw_thread_should_stop() == _TRUE) {
			break;
		}
		ret = loopback_start(padapter);
		rtw_msleep_os(200);
	} while (_SUCCESS == ret);

	RTK_PRINTF("exit %s\n", __FUNCTION__);

	rtw_thread_exit();
	return 0;
}

void rtw_sdio_loopback_start(sdio_adapter *padapter)
{
	struct task_struct *task = NULL;

	RTK_PRINTF("%s\n", __FUNCTION__);

	if (rtw_create_task_ex(&task,  "RTW_LOOPBACK_THREAD", rtw_sdio_loopback_thread, NULL) == _FAIL) {
		RTK_PRINTF("%s: start loopback send thread FAIL!!\n", __FUNCTION__);
		return;
	}

	padapter->LPThread = task;
}

void rtw_sdio_loopback_stop(sdio_adapter *padapter)
{
	if (padapter->LPThread) {
		rtw_delete_task_ex(padapter->LPThread);
		padapter->LPThread = NULL;
	}
}

