/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_core.h"
#include "usbd_pcd.h"

/* Private defines -----------------------------------------------------------*/

/* Do USB power on flow after USB download done/aborted */
#define USB_PWR_ON_EN	       1
#define USB_PWR_OFF_EN         1

/* Enable In Token prediction debug */
#define DEBUG_IN_TOKEN_PREDICT 0

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the PCD interrupt.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
static HAL_Status USB_PCD_InitInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	if (hpcd->ISR_Installed) {
		usb_hal_driver.disable_interrupt();
		usb_hal_driver.unregister_irq_handler();
	}
	usb_hal_driver.register_irq_handler(USBD_IRQHandler, USBD_IRQ_PRI);
	usb_hal_driver.enable_interrupt(USBD_IRQ_PRI);
	hpcd->ISR_Installed = 1;
	return HAL_OK;
}

/**
  * @brief  Deinitializes the PCD interrupt.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
static HAL_Status USB_PCD_DeInitInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	if (hpcd->ISR_Installed) {
		usb_hal_driver.disable_interrupt();
		usb_hal_driver.unregister_irq_handler();
		hpcd->ISR_Installed = 0;
	}
	return HAL_OK;
}

/**
  * @brief  Handle PCD reset interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleResetIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 i;

	USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

	usb_hal_flush_tx_fifo(0x10U);

	for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
		USB_INEP(i)->DIEPINT = 0xFFFFU; // 0xFB7FU;
		USB_INEP(i)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
		USB_OUTEP(i)->DOEPINT = 0xFFFFU; // 0xFB7FU;
		USB_OUTEP(i)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
	}
	if (hpcd->ded_tx_fifo_en == 0) {
		USB_ResetLearnQueue(hpcd);
	}

	USB_DEVICE->DAINTMSK |= 0x10001U;

	USB_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_STUPM |
						   USB_OTG_DOEPMSK_XFRCM |
						   USB_OTG_DOEPMSK_EPDM |
						   USB_OTG_DOEPMSK_OTEPSPRM |
						   USB_OTG_DOEPMSK_NAKM;

	USB_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_TOM |
						   USB_OTG_DIEPMSK_XFRCM |
						   USB_OTG_DIEPMSK_EPDM |
						   USB_OTG_DIEPMSK_INEPNMM;

	/* Set Default Address to 0 */
	USB_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;

	/* setup EP0 to receive SETUP packets */
	USB_EP0_OutStart(hpcd);

}

/**
  * @brief  Handle PCD enum done interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleEnumDoneIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USB_ActivateSetup(hpcd);

	hpcd->Init.speed = USB_GetDevSpeed(hpcd);

	/* Set USB Turnaround time */
	USB_SetTurnaroundTime(hpcd);

	USBD_SetSpeed(hpcd->pData, hpcd->Init.speed);

	/* Reset Device */
	USBD_Reset(hpcd->pData);
}

/**
  * @brief  Handle PCD suspend interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleSuspendIntr(USB_PCD_HandleTypeDef *hpcd)
{
	if ((USB_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS) {
		USBD_Suspend(hpcd->pData);
	}
}

/**
  * @brief  process EP OUT setup packet received interrupt.
  * @param  hpcd : PCD handle
  * @param  epnum : endpoint number
  * @retval HAL status
  */
static HAL_Status USB_PCD_HandleEPOutSetupPacketIntr(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
	u32 gSNPSiD = USB_GLOBAL->GSNPSID;
	u32 DoepintReg = USB_OUTEP(epnum)->DOEPINT;

	if (hpcd->Init.dma_enable == 1U) {
		/* StupPktRcvd = 1 pending setup packet int */
		if ((gSNPSiD > USB_OTG_CORE_ID_300A) &&
			((DoepintReg & USB_OTG_DOEPINT_STPKTRX) == USB_OTG_DOEPINT_STPKTRX)) {
			USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STPKTRX);
		}
	} else {
		if ((gSNPSiD == USB_OTG_CORE_ID_310A) &&
			((DoepintReg & USB_OTG_DOEPINT_STPKTRX) == USB_OTG_DOEPINT_STPKTRX)) {
			USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STPKTRX);
		}
	}

	/* Inform the upper layer that a setup packet is available */
	USBD_SetupStage(hpcd->pData, (u8 *) hpcd->Setup);

	if ((gSNPSiD > USB_OTG_CORE_ID_300A) && (hpcd->Init.dma_enable == 1U)) {
		USB_EP0_OutStart(hpcd);
	}

	return HAL_OK;
}

/**
  * @brief  process EP OUT transfer complete interrupt.
  * @param  hpcd : PCD handle
  * @param  epnum : endpoint number
  * @retval HAL status
  */
static HAL_Status USB_PCD_HandleEPOutXfrCompleteIntr(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
	u32 gSNPSiD = USB_GLOBAL->GSNPSID;
	u32 DoepintReg = USB_OUTEP(epnum)->DOEPINT;

	if (hpcd->Init.dma_enable == 1U) {
		if ((DoepintReg & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP) { /* Class C */
			/* StupPktRcvd = 1 this is a setup packet */
			if ((gSNPSiD > USB_OTG_CORE_ID_300A) &&
				((DoepintReg & USB_OTG_DOEPINT_STPKTRX) == USB_OTG_DOEPINT_STPKTRX)) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STPKTRX);
			}

			/* Inform the upper layer that a setup packet is available */
			USBD_SetupStage(hpcd->pData, (u8 *) hpcd->Setup);

			USB_EP0_OutStart(hpcd);
			USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
		} else if ((DoepintReg & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR) { /* Class E */
			USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
		} else if ((DoepintReg & (USB_OTG_DOEPINT_STUP | USB_OTG_DOEPINT_OTEPSPR)) == 0U) {
			/* StupPktRcvd = 1 this is a setup packet */
			if ((gSNPSiD > USB_OTG_CORE_ID_300A) &&
				((DoepintReg & USB_OTG_DOEPINT_STPKTRX) == USB_OTG_DOEPINT_STPKTRX)) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STPKTRX);
			} else {
				/* out data packet received over EP0 */
				hpcd->OUT_ep[epnum].xfer_count =
					hpcd->OUT_ep[epnum].maxpacket -
					(USB_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);

				hpcd->OUT_ep[epnum].xfer_buff += hpcd->OUT_ep[epnum].maxpacket;

				USBD_DataOutStage(hpcd->pData, (u8)epnum, hpcd->OUT_ep[epnum].xfer_buff);

				if ((epnum == 0U) && (hpcd->OUT_ep[epnum].xfer_len == 0U)) {
					/* this is ZLP, so prepare EP0 for next setup */
					USB_EP0_OutStart(hpcd);
				}
			}
		} else {
			/* ... */
		}
	} else {
		if (gSNPSiD == USB_OTG_CORE_ID_310A) {
			/* StupPktRcvd = 1 this is a setup packet */
			if ((DoepintReg & USB_OTG_DOEPINT_STPKTRX) == USB_OTG_DOEPINT_STPKTRX) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STPKTRX);
			} else {
				if ((DoepintReg & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR) {
					USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
				}
				USBD_DataOutStage(hpcd->pData, (u8)epnum, hpcd->OUT_ep[epnum].xfer_buff);
			}
		} else {
			USBD_DataOutStage(hpcd->pData, (u8)epnum, hpcd->OUT_ep[epnum].xfer_buff);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Handle PCD OUT EP interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleOutEpIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 ep_intr;
	u32 epint;
	u32 epnum = 0U;

	/* Read in the device interrupt bits */
	ep_intr = USB_ReadDevAllOutEpInterrupt(hpcd);
	while ((ep_intr != 0U) && (epnum < hpcd->Init.dev_endpoints)) {
		if ((ep_intr & 0x1U) != 0U) {
			epint = USB_ReadDevOutEPInterrupt(hpcd, (u8)epnum);

			if ((epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);
				USB_PCD_HandleEPOutXfrCompleteIntr(hpcd, epnum);
			}

			if ((epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP) {
				/* Class B setup phase done for previous decoded setup */
				USB_PCD_HandleEPOutSetupPacketIntr(hpcd, epnum);
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
			}

			if ((epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);
			}

			/* Clear Status Phase Received interrupt */
			if ((epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR) {
				if (hpcd->Init.dma_enable == 1U) {
					USB_EP0_OutStart(hpcd);
				}
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
			}

			/* Clear OUT NAK interrupt */
			if ((epint & USB_OTG_DOEPINT_NAK) == USB_OTG_DOEPINT_NAK) {
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_NAK);
			}
		}
		epnum++;
		ep_intr >>= 1U;
	}
}

/**
  * @brief  Handle PCD IN EP disable interrupt.
  * @param  hpcd: PCD handle
  * @param  epnum : endpoint number
  * @retval void
  */
static void USB_PCD_HandleInEpDisableIntr(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
#if USBD_DMA_ENABLE
	u32 reg;

	reg = USB_INEP(epnum)->DIEPCTL;

	if ((hpcd->start_predict == 0)
		|| ((reg & USB_OTG_DIEPCTL_EPTYP_0) != 0)) {  // INT or ISOC
		// TODO: Clear the Global IN NP NAK and restart transfer
		return;
	}

	if (hpcd->start_predict > 2)  {
		hpcd->start_predict--;
		return;
	}

	hpcd->start_predict--;

	if (hpcd->start_predict == 1)  {
		// TODO: predict nextep
		hpcd->start_predict = 0;
	}
#else
	UNUSED(hpcd);
	UNUSED(epnum);
#endif
}

/**
  * @brief  Write TxFIFO when EP TxFIFO emptry interrupt occurs
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval HAL status
  */
static HAL_Status USB_PCD_WriteEmptyTxFifo(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
	USB_EPTypeDef *ep;
	u32 len;
	u32 len32b;
	u32 msk;

	ep = &hpcd->IN_ep[epnum];

	if (ep->xfer_count > ep->xfer_len) {
		return HAL_ERR_PARA;
	}

	len = ep->xfer_len - ep->xfer_count;

	if (len > ep->maxpacket) {
		len = ep->maxpacket;
	}

	len32b = (len + 3U) / 4U;

	while (((USB_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >= len32b) &&
		   (ep->xfer_count < ep->xfer_len) && (ep->xfer_len != 0U)) {
		/* Write the FIFO */
		len = ep->xfer_len - ep->xfer_count;

		if (len > ep->maxpacket) {
			len = ep->maxpacket;
		}
		len32b = (len + 3U) / 4U;

		usb_hal_write_packet(ep->xfer_buff, (u8)epnum, (u16)len);

		ep->xfer_buff  += len;
		ep->xfer_count += len;
	}

	if (ep->xfer_len <= ep->xfer_count) {
		if (hpcd->ded_tx_fifo_en) {
			msk = (0x1UL << (epnum & EP_ADDR_MSK));
			USB_DEVICE->DIEPEMPMSK &= ~msk;
		} else {
			USB_GLOBAL->GINTMSK &= ~USB_OTG_GINTMSK_NPTXFEM;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Handle PCD IN EP interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleInEpIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 ep_intr;
	u32 epint;
	u32 msk;
	u32 epnum = 0U;

	/* Read in the device interrupt bits */
	ep_intr = USB_ReadDevAllInEpInterrupt(hpcd);
	while ((ep_intr != 0U) && (epnum < hpcd->Init.dev_endpoints)) {
		if ((ep_intr & 0x1U) != 0U) { /* In ITR */
			epint = USB_ReadDevInEPInterrupt(hpcd, (u8)epnum);

			if ((epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC) {
				if (hpcd->ded_tx_fifo_en) {
					msk = 0x1UL << (u32)epnum;
					USB_DEVICE->DIEPEMPMSK &= ~msk;
				} else {
					USB_GLOBAL->GINTMSK &= ~USB_OTG_GINTMSK_NPTXFEM;
				}

				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);

				if (hpcd->Init.dma_enable == 1U) {
					hpcd->IN_ep[epnum].xfer_buff += hpcd->IN_ep[epnum].maxpacket;
				}

				USBD_DataInStage(hpcd->pData, (u8)epnum, hpcd->IN_ep[epnum].xfer_buff);

				if (hpcd->Init.dma_enable == 1U) {
					/* this is ZLP, so prepare EP0 for next setup */
					if ((epnum == 0U) && (hpcd->IN_ep[epnum].xfer_len == 0U)) {
						/* prepare to rx more setup packets */
						USB_EP0_OutStart(hpcd);
					}
				}
			}
			if ((epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC) {
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_TOC);
			}
			if ((epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE) {
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_ITTXFE);
			}
			if ((epint & USB_OTG_DIEPINT_INEPNM) == USB_OTG_DIEPINT_INEPNM) {
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNM);
			}
			if ((epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE) {
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNE);
			}
			if ((epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD) {
				USB_PCD_HandleInEpDisableIntr(hpcd, epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_EPDISD);
			}
			if ((epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE) {
				USB_PCD_WriteEmptyTxFifo(hpcd, epnum);
			}
		}
		epnum++;
		ep_intr >>= 1U;
	}
}

/**
  * @brief  Handle PCD RxFIFO non-empty interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleRxFifoNonEmptyIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USB_EPTypeDef *ep;
	u32 reg;
	u32 xfer_cnt;
	u8 ep_num;

	USB_PCD_MASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);

	reg = USB_GLOBAL->GRXSTSP;
	xfer_cnt = (reg & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
	ep_num = (u8)(reg & USB_OTG_GRXSTSP_EPNUM);

	ep = &hpcd->OUT_ep[ep_num];

	if (((reg & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_DATA_UPDT) {
		if (xfer_cnt != 0U) {
			usb_hal_read_packet(ep->xfer_buff, ep_num, (u16)(xfer_cnt & 0xFFFF));

			ep->xfer_buff += xfer_cnt;
			ep->xfer_count += xfer_cnt;
		}
	} else if (((reg & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_SETUP_UPDT) {
		usb_hal_read_packet((u8 *)hpcd->Setup, ep_num, 8U);
		ep->xfer_count += xfer_cnt;
	}

	USB_PCD_UNMASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);
}

/**
  * @brief  Get IN EP number sequence as per IN Token learn queue
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_GetEpNumOfInTokens(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	u32 i;
	u32 idx;
	u32 cnt;
	int start;
	int end;
	int sort_done;
	u32 dtknqr1;
	u32 dtknqr2;
	u8 intkn_seq[USB_IN_TOKEN_QUEUE_DEPTH];
	u8 seq[USB_MAX_ENDPOINTS];
	u8 temp;

	dtknqr1 = USB_DEVICE->DTKNQR1;
	dtknqr2 = USB_DEVICE->DTKNQR2;

	USB_GLOBAL->GRSTCTL |= USB_OTG_GRSTCTL_INTKNQFLSH;

	if ((dtknqr1 & USB_OTG_DTKNQR1_WRAPBIT) != 0) {
		start = (int)(dtknqr1 & USB_OTG_DTKNQR1_INTKNWPTR_Msk);
		end = start - 1;
		if (end < 0) {
			end = USB_IN_TOKEN_QUEUE_DEPTH - 1;
		}
		cnt = USB_IN_TOKEN_QUEUE_DEPTH;
	} else {
		start = 0;
		end = (int)(dtknqr1 & USB_OTG_DTKNQR1_INTKNWPTR_Msk) - 1;
		if (end < 0) {
			end = 0;
		}
		cnt = end - start + 1;
	}

	for (i = 0; i < hpcd->Init.dev_endpoints; i++) {
		seq[i] = i + 31;
	}

	for (i = 0; i < 6; i++) {
		intkn_seq[i] = (dtknqr1 >> (8 + i * 4)) & 0xF;
	}

	for (i = 6; i < USB_IN_TOKEN_QUEUE_DEPTH; i++) {
		intkn_seq[i] = (dtknqr2 >> ((i - 6) * 4)) & 0xF;
	}

#if DEBUG_IN_TOKEN_PREDICT
	for (i = 0; i < USB_IN_TOKEN_QUEUE_DEPTH; i++) {
		//RTL_DBG("Raw INTKN[%d]=EP%d\n", i, intkn_seq[i]);
	}

	//RTL_DBG("Start=%d end=%d\n", start, end);
#endif

	i = 0;
	idx = start;
	do {
		seq[intkn_seq[idx]] = i;
		idx++;
		i++;
		if (idx == USB_IN_TOKEN_QUEUE_DEPTH) {
			idx = 0;
		}
	} while (i < cnt);

#if DEBUG_IN_TOKEN_PREDICT
	for (i = 0; i < hpcd->Init.dev_endpoints; i++) {
		//RTL_DBG("Unsorted SEQ[%d]=%d\n", i, seq[i]);
	}
#endif

	sort_done = 0;
	while (!sort_done) {
		sort_done = 1;
		for (i = 0; i < hpcd->Init.dev_endpoints - 1; i++) {
			if (seq[i] > seq[i + 1]) {
				temp = seq[i];
				seq[i] = seq[i + 1];
				seq[i + 1] = temp;
				sort_done = 0;
			}
		}
	}

#if DEBUG_IN_TOKEN_PREDICT
	for (i = 0; i < hpcd->Init.dev_endpoints; i++) {
		//RTL_DBG("Sorted SEQ[%d]=%d\n", i, seq[i]);
	}
#endif

	for (i = 0; i < hpcd->Init.dev_endpoints; i++) {
		idx = start + i;
		if (seq[i] < 31) {
			idx = start + seq[i];
			if (idx >= USB_IN_TOKEN_QUEUE_DEPTH) {
				idx = idx % USB_IN_TOKEN_QUEUE_DEPTH;
			}
			hpcd->in_ep_sequece[i] = intkn_seq[idx];
		} else {
			hpcd->in_ep_sequece[i] = 0xFF;
		}
	}

#if DEBUG_IN_TOKEN_PREDICT
	for (i = 0; i < hpcd->Init.dev_endpoints; i++) {
		//RTL_DBG("INTKN[%d]=%d\n", i, hpcd->in_ep_sequece[i]);
	}
#endif
}

#if 0
/**
  * @brief  Get last IN EP number as per IN Token learn queue
  * @param  hpcd: PCD handle
  * @retval Last IN EP number in IN Token learn queue
  */
static u32 USB_PCD_GetEpNumOfLastInToken(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	int idx;
	u32 epnum;
	u32 dtknqr1;
	u32 dtknqr2;

	dtknqr1 = USB_DEVICE->DTKNQR1;
	dtknqr2 = USB_DEVICE->DTKNQR2;

	idx = (int)(dtknqr1 & USB_OTG_DTKNQR1_INTKNWPTR_Msk) - 1;
	dtknqr1 = (dtknqr1 >> USB_OTG_DTKNQR1_EPTKN_Pos) & USB_OTG_DTKNQR1_EPTKN_Msk;

	if (idx == -1) {
		if ((dtknqr1 & USB_OTG_DTKNQR1_WRAPBIT) != 0) {
			epnum = (dtknqr2 >> 4) & 0xF;
		} else {
			epnum = 0xFF;
		}
	} else if (idx > 5) {
		epnum = (dtknqr2 >> (4 * (idx - 6))) & 0xF;
	} else {
		epnum = (dtknqr1 >> (4 * idx)) & 0xF;
	}

	return epnum;
}
#endif

/**
  * @brief  Handle PCD EP non-periodical TxFIFO empty interrupt.
  * @param  hpcd: PCD handle
  * @param  epnum endpoint number
  * @retval void
  */
static void USB_PCD_HandleEpNPTxFifoEmptyIntr(USB_PCD_HandleTypeDef *hpcd, u8 epnum)
{
	USB_EPTypeDef *ep;
	u32 len;
	u32 reg;

	ep = &hpcd->IN_ep[epnum];

	if (ep->is_periodic) {
		return;
	}

	if ((ep->xfer_len == 0) && (ep->is_zlp == 0)) {
#if DEBUG_IN_TOKEN_PREDICT
		//RTL_DBG("NPTxFEmp: token received for EP%d when TxFIFO is not ready\n", epnum);
#endif
		return;
	}

	if ((ep->xfer_count > ep->xfer_len) || ((ep->xfer_len > 0) && (ep->xfer_buff == NULL))) {
#if DEBUG_IN_TOKEN_PREDICT
		//RTL_DBG("NPTxFEmp: EP%d invalid parameter xfer_count=%d xfer_len=%d xfer_buff=%p\n", epnum, ep->xfer_count, ep->xfer_len,
		ep->xfer_buff);
#endif
		return;
	}

	len = ep->xfer_len - ep->xfer_count;
	if (len > ep->maxpacket) {
		len = ep->maxpacket;
	}

	////RTL_DBG("NPTxFEmp: EP%d xfer_len=%d xfer_count=%d len=%d\n", epnum, ep->xfer_len, ep->xfer_count, len);

	/* While there is space in the queue and space in the FIFO and
	 * More data to tranfer, Write packets to the Tx FIFO */
	reg = USB_GLOBAL->GNPTXSTS;

	while (((reg & USB_OTG_GNPTXSTS_NPTQXSAV_Msk) > 0)
		   && ((reg & USB_OTG_GNPTXSTS_NPTXFSAV_Msk) >= ((len + 3U) / 4U))
		   && (ep->xfer_count < ep->xfer_len)) {
		usb_hal_write_packet(ep->xfer_buff, ep->num, (u16)len);
		ep->xfer_count += len;
		ep->xfer_buff += len;
		len = ep->xfer_len - ep->xfer_count;
		if (len > ep->maxpacket) {
			len = ep->maxpacket;
		}
		reg = USB_GLOBAL->GNPTXSTS;
	}
}

/**
  * @brief  Handle PCD non-periodical TxFIFO empty interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleNPTxFifoEmptyIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u8 epnum;
	int i;

	USB_PCD_GetEpNumOfInTokens(hpcd);

	for (i = (int)hpcd->Init.dev_endpoints - 1; i >= 0; i--) {
		epnum = hpcd->in_ep_sequece[i];
		if (epnum != 0xFF) {
			if (epnum < hpcd->Init.dev_endpoints) {
				USB_PCD_HandleEpNPTxFifoEmptyIntr(hpcd, epnum);
			}
		}
	}
}

/**
  * @brief  Handle PCD EP mismatch interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleEpMismatchIntr(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	// TODO
}

/**
  * @brief  Handle PCD wakeup interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
static void USB_PCD_HandleWakeupIntr(USB_PCD_HandleTypeDef *hpcd)
{
	/* Clear the Remote Wake-up Signaling */
	USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

	if (hpcd->LPM_State == LPM_L1) {
		hpcd->LPM_State = LPM_L0;
		// FIXME: Do nothing
	} else {
		USBD_Resume(hpcd->pData);
	}
}

/**
  * @brief  Handle SOF interrupt
  * @param  hpcd PCD handle
  * @retval None
  */
static void USB_PCD_HandleSofIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USBD_SOF(hpcd->pData);
}

/**
  * @brief  Handle SRQ interrupt
  * @param  hpcd PCD handle
  * @retval None
  */
static void USB_PCD_HandleSrqIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USBD_DevConnected(hpcd->pData);
}

/**
  * @brief  Handle OTG interrupt
  * @param  hpcd PCD handle
  * @retval None
  */
static void USB_PCD_HandleOtgIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 reg = USB_GLOBAL->GOTGINT;

	if ((reg & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET) {
		//RTL_DBG("GOTGINT_SEDET\n");
		USBD_DevDisconnected(hpcd->pData);
	}

	USB_GLOBAL->GOTGINT |= reg;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the USB_CfgTypeDef and initialize the associated handle.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_Init(USB_PCD_HandleTypeDef *hpcd)
{
	u8 i;

	/* Check the PCD handle allocation */
	if (hpcd == NULL) {
		return HAL_ERR_PARA;
	}

#if USB_PWR_ON_EN
	if (hpcd->State == HAL_PCD_STATE_RESET) {
		/* Allocate lock resource and initialize it */
		hpcd->Lock = USB_HAL_UNLOCKED;

		usb_hal_driver.deinit();

		/* SoC specific init */
		if (usb_hal_driver.init(USB_OTG_MODE_DEVICE) != HAL_OK) {
			RTK_LOGW(NOTAG, "USB power on fail\n");
			hpcd->State = HAL_PCD_STATE_ERROR;
			return HAL_ERR_HW;
		}
	}
#endif

	hpcd->State = HAL_PCD_STATE_BUSY;
	hpcd->PHY_Initialized = 0;
	hpcd->ISR_Installed = 0;

	/* Disable the Interrupts */

	usb_hal_disable_global_interrupt();

	/*Init the Core (common init.) */
	if (usb_hal_core_init(hpcd->Init.dma_enable) != HAL_OK) {
		RTK_LOGW(NOTAG, "USB core init fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}

	/* Force Device Mode*/
	usb_hal_set_otg_mode(USB_OTG_MODE_DEVICE);

	/* Init endpoints structures */
	for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
		/* Init ep structure */
		hpcd->IN_ep[i].is_in = 1U;
		hpcd->IN_ep[i].num = i;
		hpcd->IN_ep[i].tx_fifo_num = i;
		hpcd->IN_ep[i].is_periodic = 0;
		/* Control until ep is activated */
		hpcd->IN_ep[i].type = EP_TYPE_CTRL;
		hpcd->IN_ep[i].maxpacket = 0U;
		hpcd->IN_ep[i].xfer_buff = 0U;
		hpcd->IN_ep[i].xfer_len = 0U;
		hpcd->IN_ep[i].is_zlp = 0U;
	}

	for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
		hpcd->OUT_ep[i].is_in = 0U;
		hpcd->OUT_ep[i].num = i;
		/* Control until ep is activated */
		hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
		hpcd->OUT_ep[i].maxpacket = 0U;
		hpcd->OUT_ep[i].xfer_buff = 0U;
		hpcd->OUT_ep[i].xfer_len = 0U;
		hpcd->OUT_ep[i].is_zlp = 0U;
	}

	/* Init Device */
	if (USB_DevInit(hpcd) != HAL_OK) {
		RTK_LOGW(NOTAG, "USB device init fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}

	hpcd->USB_Address = 0U;
	hpcd->State = HAL_PCD_STATE_READY;
	USB_DevDisconnect(hpcd);

#ifndef CONFIG_SUPPORT_USB_NO_PHY
	if (usb_hal_calibrate(USB_OTG_MODE_DEVICE) != HAL_OK) {
		RTK_LOGW(NOTAG, "USB calibration fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}
#endif

#ifdef CONFIG_AMEBAGREEN2
	USB_PCD_SetRxFiFo(hpcd, 644); // For CTRL OUT and BULK OUT
	USB_PCD_SetTxFiFo(hpcd, 0, 32); // For CTRL IN
	USB_PCD_SetTxFiFo(hpcd, 1, 16);
	USB_PCD_SetTxFiFo(hpcd, 2, 256); // For BULK IN
	USB_PCD_SetTxFiFo(hpcd, 3, 16);
	USB_PCD_SetTxFiFo(hpcd, 4, 32); // For INTR IN, unused
	USB_PCD_SetTxFiFo(hpcd, 5, 16);
#elif defined(CONFIG_AMEBASMARTPLUS)
	USB_PCD_SetRxFiFo(hpcd, 898); // For CTRL OUT and BULK OUT
	USB_PCD_SetTxFiFo(hpcd, 0, 32); // For CTRL IN
	USB_PCD_SetTxFiFo(hpcd, 1, 256); // For BULK IN
	USB_PCD_SetTxFiFo(hpcd, 2, 32); // For INTR IN, unused
	USB_PCD_SetTxFiFo(hpcd, 3, 16);
	USB_PCD_SetTxFiFo(hpcd, 4, 16);
	USB_PCD_SetTxFiFo(hpcd, 5, 16);
#elif defined(CONFIG_AMEBAL2)
	USB_PCD_SetRxFiFo(hpcd, 660); // For CTRL OUT and BULK OUT
	USB_PCD_SetTxFiFo(hpcd, 0, 32); // For CTRL IN
	USB_PCD_SetTxFiFo(hpcd, 1, 256); // For BULK IN
	USB_PCD_SetTxFiFo(hpcd, 2, 32); // For INTR IN, unused
	USB_PCD_SetTxFiFo(hpcd, 3, 16);
	USB_PCD_SetTxFiFo(hpcd, 4, 16);
#endif

	USB_PCD_InitInterrupt(hpcd);

	return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_DeInit(USB_PCD_HandleTypeDef *hpcd)
{
	HAL_Status result = HAL_OK;

	/* Check the PCD handle allocation */
	if (hpcd == NULL) {
		return HAL_ERR_PARA;
	}

	USB_PCD_DeInitInterrupt(hpcd);

	hpcd->State = HAL_PCD_STATE_BUSY;

	/* Stop Device */
	result = USB_PCD_Stop(hpcd);

#if USB_PWR_OFF_EN
	/* DeInit the low level hardware: CLOCK, NVIC.*/
	usb_hal_driver.deinit();
#endif

	hpcd->State = HAL_PCD_STATE_RESET;

	return result;
}

/**
  * @brief  Start the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_Start(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	USB_DevConnect(hpcd);
	usb_hal_enable_global_interrupt();
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Stop the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_Stop(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);

	usb_hal_disable_global_interrupt();

	if (USB_StopDevice(hpcd) != HAL_OK) {
		USB_HAL_UNLOCK(hpcd);
		return HAL_ERR_HW;
	}

	USB_DevDisconnect(hpcd);
	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Connect the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_DevConnect(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	USB_DevConnect(hpcd);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Disconnect the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_Status USB_PCD_DevDisconnect(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	USB_DevDisconnect(hpcd);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Set the USB Device address.
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
HAL_Status USB_PCD_SetAddress(USB_PCD_HandleTypeDef *hpcd, u8 address)
{
	USB_HAL_LOCK(hpcd);
	hpcd->USB_Address = address;
	USB_SetDevAddress(hpcd, address);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  ep_mps endpoint max packet size
  * @param  ep_type endpoint type
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_Open(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u16 ep_mps, u8 ep_type)
{
	HAL_Status  ret = HAL_OK;
	USB_EPTypeDef *ep;

	if ((ep_addr & 0x80U) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
		ep->is_periodic = (ep_type == EP_TYPE_ISOC) || ((ep_type == EP_TYPE_INTR) && (hpcd->Init.is_ptx_intr != 0U) && (hpcd->ded_tx_fifo_en == 0U));
	} else {
		ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 0U;
	}

	ep->num = ep_addr & EP_ADDR_MSK;
	ep->maxpacket = ep_mps;
	ep->type = ep_type;

	if (ep->is_in != 0U) {
		/* Assign a Tx FIFO */
		ep->tx_fifo_num = USB_GetTxFifoNum(hpcd, ep);
	}
	/* Set initial data PID. */
	if (ep_type == EP_TYPE_BULK) {
		ep->data_pid_start = 0U;
	}

	USB_HAL_LOCK(hpcd);
	USB_ActivateEndpoint(hpcd, ep);
	USB_HAL_UNLOCK(hpcd);

	return ret;
}

/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_Close(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_EPTypeDef *ep;

	if ((ep_addr & 0x80U) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
		ep->is_periodic = 0;
	} else {
		ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 0U;
	}
	ep->num   = ep_addr & EP_ADDR_MSK;

	USB_HAL_LOCK(hpcd);
	USB_DeactivateEndpoint(hpcd, ep);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Receive an amount of data.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the reception buffer
  * @param  len amount of data to be received
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_Receive(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u8 *pBuf, u32 len)
{
	USB_EPTypeDef *ep;

	ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];

	/*setup and start the Xfer */
	ep->xfer_buff = pBuf;
	ep->xfer_len = len;
	ep->xfer_count = 0U;
	ep->is_in = 0U;
	ep->is_zlp = (len == 0) ? 1 : 0;
	ep->num = ep_addr & EP_ADDR_MSK;

	if (hpcd->Init.dma_enable == 1U) {
		ep->dma_addr = (u32)pBuf;
	}

	if ((ep_addr & EP_ADDR_MSK) == 0U) {
		USB_EP0StartXfer(hpcd, ep);
	} else {
		USB_EPStartXfer(hpcd, ep);
	}

	return HAL_OK;
}

/**
  * @brief  Get Received Data Size
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval Data Size
  */
u32 USB_PCD_EP_GetRxCount(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	return hpcd->OUT_ep[ep_addr & EP_ADDR_MSK].xfer_count;
}

/**
  * @brief  Send an amount of data
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the transmission buffer
  * @param  len amount of data to be sent
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_Transmit(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u8 *pBuf, u32 len)
{
	USB_EPTypeDef *ep;

	ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];

	/*setup and start the Xfer */
	ep->xfer_buff = pBuf;
	ep->xfer_len = len;
	ep->xfer_count = 0U;
	ep->is_in = 1U;
	ep->num = ep_addr & EP_ADDR_MSK;
	ep->is_zlp = (len == 0) ? 1 : 0;

	if (hpcd->Init.dma_enable == 1U) {
		ep->dma_addr = (u32)pBuf;
	}

	if ((ep_addr & EP_ADDR_MSK) == 0U) {
		USB_EP0StartXfer(hpcd, ep);
	} else {
		USB_EPStartXfer(hpcd, ep);
	}

	return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_SetStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_EPTypeDef *ep;

	if (((u32)ep_addr & EP_ADDR_MSK) > hpcd->Init.dev_endpoints) {
		return HAL_ERR_PARA;
	}

	if ((0x80U & ep_addr) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
	} else {
		ep = &hpcd->OUT_ep[ep_addr];
		ep->is_in = 0U;
	}

	ep->is_stall = 1U;
	ep->num = ep_addr & EP_ADDR_MSK;

	USB_HAL_LOCK(hpcd);

	USB_EPSetStall(hpcd, ep);
	if ((ep_addr & EP_ADDR_MSK) == 0U) {
		USB_EP0_OutStart(hpcd);
	}
	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_ClrStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_EPTypeDef *ep;

	if (((u32)ep_addr & 0x0FU) > hpcd->Init.dev_endpoints) {
		return HAL_ERR_PARA;
	}

	if ((0x80U & ep_addr) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
	} else {
		ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 0U;
	}

	ep->is_stall = 0U;
	ep->num = ep_addr & EP_ADDR_MSK;

	USB_HAL_LOCK(hpcd);
	USB_EPClearStall(hpcd, ep);
	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_Status USB_PCD_EP_Flush(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_HAL_LOCK(hpcd);

	if ((ep_addr & 0x80U) == 0x80U) {
		usb_hal_flush_tx_fifo((u32)ep_addr & EP_ADDR_MSK);
	} else {
		usb_hal_flush_rx_fifo();
	}

	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Return the PCD handle state.
  * @param  hpcd PCD handle
  * @retval HAL state
  */
USB_PCD_StateTypeDef USB_PCD_GetState(USB_PCD_HandleTypeDef *hpcd)
{
	return hpcd->State;
}

/**
  * @brief  Set Tx FIFO
  * @param  hpcd PCD handle
  * @param  fifo The number of Tx fifo
  * @param  size Fifo size
  * @retval HAL status
  */
HAL_Status USB_PCD_SetTxFiFo(USB_PCD_HandleTypeDef *hpcd, u8 fifo, u16 size)
{
	UNUSED(hpcd);
	u8 i;
	u32 Tx_Offset;

	/*  TXn min size = 16 words. (n  : Transmit FIFO index)
	    When a TxFIFO is not used, the Configuration should be as follows:
	        case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
	       --> Txm can use the space allocated for Txn.
	       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
	       --> Txn should be configured with the minimum space of 16 words
	   The FIFO is used optimally when used TxFIFOs are allocated in the top
	       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
	   When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

	Tx_Offset = USB_GLOBAL->GRXFSIZ;

	if (fifo == 0U) {
		USB_GLOBAL->GNPTXFSIZ = ((u32)size << 16) | Tx_Offset;
	} else {
		Tx_Offset += (USB_GLOBAL->GNPTXFSIZ) >> 16;
		for (i = 0U; i < (fifo - 1U); i++) {
			Tx_Offset += (USB_GLOBAL->DPTXFSIZ_DIEPTXF[i] >> 16);
		}

		/* Multiply Tx_Size by 2 to get higher performance */
		USB_GLOBAL->DPTXFSIZ_DIEPTXF[fifo - 1U] = ((u32)size << 16) | Tx_Offset;
	}

	return HAL_OK;
}

/**
  * @brief  Set Rx FIFO
  * @param  hpcd PCD handle
  * @param  size Size of Rx fifo
  * @retval HAL status
  */
HAL_Status USB_PCD_SetRxFiFo(USB_PCD_HandleTypeDef *hpcd, u16 size)
{
	UNUSED(hpcd);

	USB_GLOBAL->GRXFSIZ = size;

	return HAL_OK;
}

/**
  * @brief  Handles PCD interrupt request.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
void USB_PCD_IRQHandler(USB_PCD_HandleTypeDef *hpcd)
{
	/* ensure that we are in device mode */
	if (usb_hal_get_otg_mode() == USB_OTG_MODE_DEVICE) {
		/* avoid spurious interrupt */
		if (USB_ReadInterrupts() == 0U) {
			return;
		}

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_MMIS)) {
			/* incorrect mode, acknowledge the interrupt */
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_MMIS);
		}

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_IEPINT)) {
			USB_PCD_HandleInEpIntr(hpcd);
		}

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_OEPINT)) {
			USB_PCD_HandleOutEpIntr(hpcd);
		}

		/* Handle Resume Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_WKUINT)) {
			USB_PCD_HandleWakeupIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_WKUINT);
		}

		/* Handle Suspend Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_USBSUSP)) {
			//RTL_DBG("USBSUSP\n");
			USB_PCD_HandleSuspendIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_USBSUSP);
		}
		/* Handle Reset Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_USBRST)) {
			//RTL_DBG("USBRST\n");
			USB_PCD_HandleResetIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_USBRST);
		}

		/* Handle Enumeration done Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_ENUMDNE)) {
			USB_PCD_HandleEnumDoneIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_ENUMDNE);
		}

		/* Handle RxQLevel Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_RXFLVL)) {
			USB_PCD_HandleRxFifoNonEmptyIntr(hpcd);
		}

		/* Handle NP TxFIFO Empty Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_NPTXFE)) {
			USB_PCD_HandleNPTxFifoEmptyIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_NPTXFE);
		}

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_EPMIS)) {
			//RTL_DBG("EPMIS\n");
			USB_PCD_HandleEpMismatchIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_EPMIS);
		}

		/* Handle SOF Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_SOF)) {
			USB_PCD_HandleSofIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_SOF);
		}

		/* Handle Connection event Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_SRQINT)) {
			USB_PCD_HandleSrqIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_SRQINT);
		}

		/* Handle Disconnection event Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_OTGINT)) {
			USB_PCD_HandleOtgIntr(hpcd);
		}
	}
}

