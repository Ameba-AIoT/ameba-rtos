#include "ameba_soc.h"
#include "spdio_api.h"
#include "ameba_inic.h"
static const char *const TAG = "SPDIO";

/** @addtogroup Ameba_Mbed_API
 * @{
 */

/** @addtogroup MBED_SPDIO
 *  @brief      MBED_SPDIO driver modules.
 *  @{
 */

/** @defgroup MBED_SPDIO_Exported_Constants MBED_SPDIO Exported Constants
 * @{
 */

#define SPDIO_IRQ_PRIORITY			INT_PRI_MIDDLE
#define SPDIO_TX_BD_BUF_SZ_UNIT		64
#define SPDIO_RX_BD_FREE_TH			5
#define SPDIO_MIN_RX_BD_SEND_PKT	2
#define SPDIO_MAX_RX_BD_BUF_SIZE	16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

#define SDIO_INIT_INT_MASK			(SDIO_WIFI_BIT_H2C_DMA_OK | SDIO_WIFI_BIT_C2H_DMA_OK | \
									SDIO_WIFI_BIT_H2C_BUS_RES_FAIL | SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT | \
									SDIO_NOTIFY_TYPE_INT)

#define SDIO_NOTIFY_TYPE_INT		(SDIO_WIFI_BIT_H2C_MSG_INT | SDIO_WIFI_BIT_RPWM1_INT | \
									SDIO_WIFI_BIT_RPWM2_INT | SDIO_WIFI_BIT_HOST_WAKE_CPU_INT | \
									SDIO_WIFI_BIT_H2C_SUS_REQ)

/** @}*/


/** @defgroup MBED_SPDIO_Exported_Constants MBED_SPDIO Exported Constants
 * @{
 */

struct spdio_t *g_spdio_priv = NULL;
SPDIO_ADAPTER gSPDIODev;

rtos_sema_t xSDIOIrqSema = NULL;;			/* Semaphore for SDIO RX, use to wakeup the SDIO RX task */
rtos_task_t xSDIOIrqTaskHandle = NULL;	/* The handle of the SDIO Task special for RX, can be used to delete the task */

/**
 * @}
 */

/** @defgroup MBED_SPDIO_Exported_Functions MBED_SPDIO Exported Functions
 * @{
 */

static s8 spdio_device_rx_done_cb(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf, u8 *pdata, u16 pktsize, u8 type)
{
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;

	if (obj) {
		return obj->device_rx_done_cb(obj, buf, pdata, pktsize, type);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio rx done callback function is null!");
	}

	return RTK_SUCCESS;
}

static s8 spdio_device_tx_done_cb(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf)
{
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;
	if (obj) {
		return obj->device_tx_done_cb(obj, buf);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio tx done callback function is null!");
	}
	return RTK_SUCCESS;
}

static s8 spdio_rpwm_cb(PSPDIO_ADAPTER pSPDIODev)
{
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;

	u16 rpwm2 = SDIO_RPWM2_Get(SDIO_WIFI);

	if (obj) {
		return obj->rpwm_cb(obj, rpwm2);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio rpwm callback function is null!");
	}
	return RTK_SUCCESS;
}

/**
 * @brief Spdio write function.
 * @param obj Pointer to a initialized spdio_t structure.
 * @param pbuf Pointer to a spdio_buf_t structure which carries the payload.
 * @retval RTK_SUCCESS or RTK_FAIL.
 */
s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf)
{
	PSPDIO_ADAPTER pgSDIODev = obj->priv;
	return SPDIO_DeviceTx(pgSDIODev, pbuf);
}

/**
 * @brief Get example settings for spdio obj.
 * @param obj Pointer to a spdio_t structure which will be initialized with an example settings.
 * @return None
 */
void spdio_structinit(struct spdio_t *obj)
{
	obj->device_rx_bufsz = SPDIO_RX_BUFSZ_ALIGN(2048 + 24); // extra 24 bytes for sdio header
	obj->host_tx_bd_num = 24;
	obj->host_rx_bd_num = 24;
	obj->priv = NULL;
	obj->rx_buf = NULL;
	obj->device_rx_done_cb = NULL;
	obj->device_tx_done_cb = NULL;
}

/**
 * @brief Trigger SDIO read packet when have free skb.
 */
void spdio_trigger_rx_handle(void)
{
	PSPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	if (pSPDIODev->WaitForDeviceRxbuf) {
		rtos_sema_give(xSDIOIrqSema);
	}
}

/**
 * @brief SPDIO device interrupt service routine, including
 * 		- Read and clean interrupt status.
 * 		- Wake up the SDIO task to handle the IRQ event.
 * @param pData Pointer to SDIO device data structure.
 * @retval 0
 */
u32 SPDIO_IRQ_Handler(void *pData)
{
	UNUSED(pData);
	// PSPDIO_ADAPTER pSPDIODev = pData;
	InterruptDis(SDIO_WIFI_IRQ);
	rtos_sema_give(xSDIOIrqSema);

	return 0;
}

static void SPDIO_IRQ_Handler_BH(void *pData)
{
	PSPDIO_ADAPTER pSPDIODev = pData;
	u16 IntStatus;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(xSDIOIrqSema, RTOS_MAX_TIMEOUT);

		IntStatus = SDIO_INTStatus(SDIO_WIFI);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s:ISRStatus=0x%x\n", __FUNCTION__, IntStatus);
		SDIO_INTClear(SDIO_WIFI, IntStatus); // clean the ISR

		if (IntStatus & SDIO_NOTIFY_TYPE_INT) {
			SPDIO_Notify_INT(IntStatus);
		}

		if (IntStatus & SDIO_WIFI_BIT_C2H_DMA_OK) {
			SPDIO_Recycle_Rx_BD(pSPDIODev, spdio_device_tx_done_cb);
		}

		if (IntStatus & SDIO_WIFI_BIT_H2C_MSG_INT) {
			SDIO_H2C_MSG_Get(SDIO_WIFI);
		}

		if (IntStatus & SDIO_WIFI_BIT_H2C_DMA_OK || pSPDIODev->WaitForDeviceRxbuf) {
			SPDIO_TxBd_DataReady_DeviceRx(pSPDIODev, spdio_device_rx_done_cb);
		}
		if (IntStatus & SDIO_WIFI_BIT_TXBD_H2C_OVF) {
			pSPDIODev->TxOverFlow = 1;
		}

		if (IntStatus & SDIO_WIFI_BIT_RPWM2_INT) {
			spdio_rpwm_cb(pSPDIODev);
		}
		InterruptEn(SDIO_WIFI_IRQ, SPDIO_IRQ_PRIORITY);

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s @2 IntStatus=0x%x\n", __FUNCTION__, IntStatus);
	}
}

void SPDIO_Board_Init(void)
{
	u8 idx;
	/* Be consistent with order of SDIO_PAD[][]. */
	char *sdio_pin[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};

	assert_param(SDIO_Pin_Grp <= 0x6);

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_PAD[SDIO_Pin_Grp][idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_PAD[SDIO_Pin_Grp][idx], PINMUX_FUNCTION_SDIO_SLV);
		RTK_LOGI(TAG, "SDIO_%s --> P%c%d\n", sdio_pin[idx], 'A' + PORT_NUM(SDIO_PAD[SDIO_Pin_Grp][idx]), PIN_NUM(SDIO_PAD[SDIO_Pin_Grp][idx]));
	}
}

void SPDIO_Buffer_free(PSPDIO_ADAPTER pSPDIODev)
{
	if (pSPDIODev->pRXBDHdl) {
		rtos_mem_free((u8 *)pSPDIODev->pRXBDHdl);
		pSPDIODev->pRXBDHdl = NULL;
	}

	if (pSPDIODev->pRXBDAddr) {
		rtos_mem_free((u8 *)pSPDIODev->pRXBDAddr);
		pSPDIODev->pRXBDAddr = NULL;
	}

	if (pSPDIODev->pTXBDHdl) {
		rtos_mem_free((u8 *)pSPDIODev->pTXBDHdl);
		pSPDIODev->pTXBDHdl = NULL;
	}

	if (pSPDIODev->pTXBDAddr) {
		rtos_mem_free(pSPDIODev->pTXBDAddr);
		pSPDIODev->pTXBDAddr = NULL;
	}

	if (pSPDIODev->pRXDESCAddr) {
		rtos_mem_free(pSPDIODev->pRXDESCAddr);
		pSPDIODev->pRXDESCAddr = NULL;
	}

	if (xSDIOIrqSema) {
		rtos_sema_delete(xSDIOIrqSema);
		xSDIOIrqSema = NULL;
	}

	if (xSDIOIrqTaskHandle) {
		rtos_task_delete(xSDIOIrqTaskHandle);
		xSDIOIrqTaskHandle = NULL;
	}
}

/**
 * @brief Initialize spdio interface.
 * @param obj Pointer to a spdio_t structure which should be initialized by user,
 * 		and which will be used to initialize spdio interface.
 * 		- obj->host_rx_bd_num: spdio write bd number, needs 2 bd for one transaction.
 * 		- obj->host_tx_bd_num: spdio read bd number.
 * 		- obj->device_rx_bufsz: spdio read buffer size.
 * 		- obj->rx_buf: spdio read buffer array.
 * @return None
 */
void spdio_init(struct spdio_t *obj)
{
	PSPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	int ret;

	if (obj == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio obj is NULL, spdio init failed!\n");
		return;
	}

	if ((obj->host_tx_bd_num == 0) || (obj->device_rx_bufsz == 0) || (obj->device_rx_bufsz % 64)
		|| (obj->host_rx_bd_num == 0) || (obj->host_rx_bd_num % 2) || (obj->rx_buf == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio obj resource isn't correctly inited, spdio init failed!\n");
		return;
	}

	g_spdio_priv = obj;

	RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO_Device_Init==>\n");

	pSPDIODev->spdio_priv = (void *)obj;
	obj->priv = (void *)pSPDIODev;

	pSPDIODev->host_rx_bd_num = obj->host_rx_bd_num;
	pSPDIODev->host_tx_bd_num = obj->host_tx_bd_num;
	pSPDIODev->device_rx_bufsz = obj->device_rx_bufsz;

	// initial TX BD and RX BD
	pSPDIODev->pTXBDAddr = (SPDIO_TX_BD *)rtos_mem_malloc(obj->host_tx_bd_num * sizeof(SPDIO_TX_BD));
	if (NULL == pSPDIODev->pTXBDAddr) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Malloc for TX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pSPDIODev->pTXBDHdl = (SPDIO_TX_BD_HANDLE *)rtos_mem_zmalloc(obj->host_tx_bd_num * sizeof(SPDIO_TX_BD_HANDLE));
	if (NULL == pSPDIODev->pTXBDHdl) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Malloc for TX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}
	SDIO_TxBdHdl_Init(pSPDIODev->pTXBDHdl, pSPDIODev->pTXBDAddr, obj->rx_buf, obj->host_tx_bd_num);

	pSPDIODev->pRXBDAddr = (SPDIO_RX_BD *)rtos_mem_malloc(obj->host_rx_bd_num * sizeof(SPDIO_RX_BD));
	if (NULL == pSPDIODev->pRXBDAddr) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Malloc for RX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pSPDIODev->pRXBDHdl = (SPDIO_RX_BD_HANDLE *)rtos_mem_zmalloc(obj->host_rx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
	if (NULL == pSPDIODev->pRXBDHdl) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Malloc for RX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pSPDIODev->pRXDESCAddr = (INIC_RX_DESC *)rtos_mem_zmalloc(obj->host_rx_bd_num * sizeof(INIC_RX_DESC));
	if (NULL == pSPDIODev->pRXDESCAddr) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Malloc for RX_DESC Err!!\n");
		goto SDIO_INIT_ERR;
	}
	SDIO_RxBdHdl_Init(pSPDIODev->pRXBDHdl, pSPDIODev->pRXBDAddr, pSPDIODev->pRXDESCAddr, obj->host_rx_bd_num);

	rtos_sema_create(&(xSDIOIrqSema), 0, RTOS_SEMA_MAX_COUNT);
	if (NULL == xSDIOIrqSema) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init Create IRQ Semaphore Err!!\n");
		goto SDIO_INIT_ERR;
	}
	ret = rtos_task_create((rtos_task_t *)&xSDIOIrqTaskHandle, "SPDIO_IRQ_TASK", SPDIO_IRQ_Handler_BH, (void *)pSPDIODev, 1024 * 4, 1);
	if (RTK_SUCCESS != ret) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Device_Init: Create IRQ Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}

	SPDIO_Board_Init();

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDD, APBPeriph_SDD_CLOCK, ENABLE);

	SPDIO_Device_Init(pSPDIODev);

	// Update the power state indication
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, ENABLE);

	/* enable the interrupt */
	InterruptRegister((IRQ_FUN)SPDIO_IRQ_Handler, SDIO_WIFI_IRQ, (u32)pSPDIODev, SPDIO_IRQ_PRIORITY);
	InterruptEn(SDIO_WIFI_IRQ, SPDIO_IRQ_PRIORITY);

	return;

SDIO_INIT_ERR:
	SPDIO_Buffer_free(obj->priv);
}

/**
 * @brief Deinitialize spdio interface.
 * @param obj Pointer to a spdio_t structure which is already initialized
 * @return None
 */
void spdio_deinit(struct spdio_t *obj)
{
	if (obj == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "spdio obj is NULL, spdio deinit failed\n");
		return;
	}

	// Indicate the Host that Ameba is inactive
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, DISABLE);

	SPDIO_Device_DeInit();

	InterruptDis(SDIO_WIFI_IRQ);
	InterruptUnRegister(SDIO_WIFI_IRQ);

	SPDIO_Buffer_free(obj->priv);

	g_spdio_priv = NULL;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
