/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Set the USB turnaround time
  * @param  PCD Instance
  * @param  hclk: AHB clock frequency
  * @retval USB turnaround time In PHY Clocks number
  */
HAL_Status USB_SetTurnaroundTime(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);

	/* The USBTRD is configured according to the tables below, depending on AHB frequency
	used by application. In the low AHB frequency range it is used to stretch enough the USB response
	time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access
	latency to the Data FIFO */

	USB_GLOBAL->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
	USB_GLOBAL->GUSBCFG |= (u32)((USBD_UTMI_16_BIT_TRDT_VALUE << 10) & USB_OTG_GUSBCFG_TRDT);

	return HAL_OK;
}

/**
  * @brief  Predict next ep for DMA mode
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_EPPredictNextEp(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
#if USBD_DMA_ENABLE
	u32 reg;
	u32 epnum = (u32)ep->num;

	reg = USB_INEP(epnum)->DIEPCTL;
	reg &= ~USB_OTG_DIEPCTL_NEXTEP_Msk;
	reg |= pcd->nextep_seq[epnum] << USB_OTG_DIEPCTL_NEXTEP_Pos;
	USB_INEP(epnum)->DIEPCTL = reg;
#else
	UNUSED(pcd);
	UNUSED(ep);
#endif

	return HAL_OK;
}

/**
  * @brief  Reset IN Token Learn Queue
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status USB_ResetLearnQueue(USB_PCD_HandleTypeDef *pcd)
{
	u32 i;
#if USBD_DMA_ENABLE
	u32 reg;
#endif

	pcd->in_ep_sequece[0] = 0;
	for (i = 1U; i < pcd->Init.dev_endpoints; i++) {
		pcd->in_ep_sequece[i] = 0xFF;
	}

#if USBD_DMA_ENABLE

	if (pcd->Init.dma_enable == 1U) {
		pcd->start_predict = 0;
		pcd->first_in_nextep_seq = 0;
		pcd->nextep_seq[0] = 0;
		for (i = 1U; i < pcd->Init.dev_endpoints; i++) {
			pcd->nextep_seq[i] = 0xFF;
		}
	}

	USB_GLOBAL->GRSTCTL |= USB_OTG_GRSTCTL_INTKNQFLSH;

	// FIXME: init nextep data
	reg = USB_INEP(0)->DIEPCTL;
	reg &= ~USB_OTG_DIEPCTL_NEXTEP_Msk;
	reg |= 0 << USB_OTG_DIEPCTL_NEXTEP_Pos;
	USB_INEP(0)->DIEPCTL = reg;

	reg = USB_DEVICE->DCFG;
	reg &= ~USB_OTG_DCFG_EPMISCNT_Msk;
	reg |= 8 << USB_OTG_DCFG_EPMISCNT_Pos;  // two IN EP, CTRL IN + BULK IN + 1
	USB_DEVICE->DCFG = reg;

#endif

	return HAL_OK;
}

/**
  * @brief  Initializes the USB_OTG controller registers
  *         for device mode
  * @param  PCD Instance
  * @param  cfg   pointer to a USB_CfgTypeDef structure that contains
  *         the configuration information for the specified USB_GLOBAL peripheral.
  * @retval HAL status
  */
HAL_Status USB_DevInit(USB_PCD_HandleTypeDef *pcd)
{
	HAL_Status ret = HAL_OK;
	USB_CfgTypeDef *cfg = &pcd->Init;
	u32 i;
	u32 reg;

	for (i = 0U; i < 15U; i++) {
		USB_GLOBAL->DPTXFSIZ_DIEPTXF[i] = 0U;
	}
	pcd->ded_tx_fifo_en = ((USB_GLOBAL->GHWCFG4 & USB_OTG_GHWCFG4_DEDFIFO) == 0) ? 0 : 1;

	/* Restart the Phy Clock */
	USB_PCGCCTL &= ~USB_OTG_PCGCCTL_STOPCLK;

	usb_os_delay_us(10);

	reg = USB_DEVICE->DCFG;
	reg &= ~(USB_OTG_DCFG_DESCDMA | USB_OTG_DCFG_PFIVL_0 | USB_OTG_DCFG_PFIVL_1 | USB_OTG_DCFG_ENDEVOUTNAK);
	USB_DEVICE->DCFG = reg;

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
	/* Full speed phy */
	USB_SetDevSpeed(pcd, USB_SPEED_FULL);
#else
	if (cfg->speed == USB_SPEED_HIGH) {
		/* High speed phy */
		USB_SetDevSpeed(pcd, USB_SPEED_HIGH);
	} else {
		/* High speed phy in Full speed mode */
		USB_SetDevSpeed(pcd, USB_SPEED_HIGH_IN_FULL);
	}
#endif

	/* Flush the FIFOs */
	if (usb_hal_flush_tx_fifo(0x10U) != HAL_OK) { /* all Tx FIFOs */
		ret = HAL_ERR_PARA;
	}

	if (usb_hal_flush_rx_fifo() != HAL_OK) {
		ret = HAL_ERR_PARA;
	}
	if (pcd->ded_tx_fifo_en == 0) {
		USB_ResetLearnQueue(pcd);
	}

	/* Clear all pending Device Interrupts */
	USB_DEVICE->DIEPMSK = 0U;
	USB_DEVICE->DOEPMSK = 0U;
	USB_DEVICE->DAINT = 0xFFFFFFFFU;
	USB_DEVICE->DAINTMSK = 0U;

	for (i = 0U; i < cfg->dev_endpoints; i++) {
		if ((USB_INEP(i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA) {
			if (i == 0U) {
				USB_INEP(i)->DIEPCTL = USB_OTG_DIEPCTL_SNAK;
			} else {
				USB_INEP(i)->DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK;
			}
		} else {
			USB_INEP(i)->DIEPCTL = 0U;
		}

		USB_INEP(i)->DIEPTSIZ = 0U;
		USB_INEP(i)->DIEPINT  = 0xFFFFU;
		USB_INEP(i)->DIEPDMA  = 0U;
	}

	for (i = 0U; i < cfg->dev_endpoints; i++) {
		// FIXME: sgoutnak & epdis+snak & cgoutnak
		if ((USB_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA) {
			if (i == 0U) {
				USB_OUTEP(i)->DOEPCTL = USB_OTG_DOEPCTL_SNAK;
			} else {
				USB_OUTEP(i)->DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK;
			}
		} else {
			USB_OUTEP(i)->DOEPCTL = 0U;
		}

		USB_OUTEP(i)->DOEPTSIZ = 0U;
		USB_OUTEP(i)->DOEPINT  = 0xFFFFU;
		USB_OUTEP(i)->DOEPDMA  = 0U;
	}

	USB_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);

	/* Disable all interrupts. */
	USB_GLOBAL->GINTMSK = 0U;

	/* Clear any pending interrupts */
	USB_GLOBAL->GINTSTS = 0xFFFFFFFF;

	/* Enable the common interrupts */
	if (cfg->dma_enable == 0U) {
		USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
	}
	if (pcd->ded_tx_fifo_en == 0) {
		USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_EPMISM;
	}
	/* Enable interrupts matching to the Device mode ONLY */
	USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |
						   USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
						   USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_WUIM;

	return ret;
}

/**
  * @brief  Initializes the DevSpd field of DCFG register
  *         depending the PHY type and the enumeration speed of the device.
  * @param  PCD Instance
  * @param  speed  device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_HIGH_IN_FULL: High speed core in Full Speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  * @retval  Hal status
  */
HAL_Status USB_SetDevSpeed(USB_PCD_HandleTypeDef *pcd, u8 speed)
{
	UNUSED(pcd);
	USB_DEVICE->DCFG |= speed;
	return HAL_OK;
}

/**
  * @brief  Return the Dev Speed
  * @param  PCD Instance
  * @retval speed  device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  */
u8 USB_GetDevSpeed(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	u8 speed;
	u32 DevEnumSpeed = USB_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD;

	if (DevEnumSpeed == DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ) {
		speed = USB_SPEED_HIGH;
	} else if (DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ) {
		speed = USB_SPEED_HIGH_IN_FULL;
	} else if (DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_48MHZ) {
		speed = USB_SPEED_FULL;
	} else {
		speed = USB_SPEED_HIGH;
	}

	return speed;
}

/**
  * @brief  Get TxFIFO number
  * @param  ep pointer to endpoint structure
  * @retval TxFIFO number
  *          1 for INTR EP, using Periodical TxFIFO
  *          0 for other EP, using Non-Periodical TxFIFO
  */
u32 USB_GetTxFifoNum(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	u32 tx_fifo_num = 0;
	if (pcd->ded_tx_fifo_en) {
#ifdef CONFIG_AMEBAGREEN2
		if (ep->num == 6U) {
			tx_fifo_num = 5;
		} else {
			tx_fifo_num = ep->num;
		}
#else
		tx_fifo_num = ep->num;
#endif
	} else {
		if (ep->is_periodic != 0U) {
			tx_fifo_num = 1;
		}
	}
	return tx_fifo_num;
}

/**
  * @brief  Activate and configure an endpoint
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_ActivateEndpoint(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	u32 epnum = ep->num;
	u32 reg;
#if USBD_DMA_ENABLE
	u32 i;
	u32 dcfg;
	u32 epmscnt;
#else
	UNUSED(pcd);
#endif

	if (ep->is_in == 1U) {
		USB_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & (u32)(1UL << (ep->num & EP_ADDR_MSK));

		if ((USB_INEP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_USBAEP) == 0U) {
			reg = (ep->maxpacket & USB_OTG_DIEPCTL_MPSIZ) |
				  ((u32)ep->type << USB_OTG_DIEPCTL_EPTYP_Pos) | (ep->tx_fifo_num << USB_OTG_DIEPCTL_TXFNUM_Pos) |
				  USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				  USB_OTG_DIEPCTL_USBAEP;

#if USBD_DMA_ENABLE
			if ((ep->type != EP_TYPE_INTR) && (pcd->Init.dma_enable == 1U)) {
				for (i = 0; i < pcd->Init.dev_endpoints; i++) {
					if (pcd->nextep_seq[i] == pcd->first_in_nextep_seq) {
						break;
					}
				}
				pcd->nextep_seq[i] = ep->num;
				pcd->nextep_seq[ep->num] = pcd->first_in_nextep_seq;
				reg &= ~USB_OTG_DIEPCTL_NEXTEP_Msk;
				reg |= pcd->nextep_seq[ep->num];
				dcfg = USB_DEVICE->DCFG;
				epmscnt = ((dcfg & USB_OTG_DCFG_EPMISCNT) >> USB_OTG_DCFG_EPMISCNT_Pos) + 1;
				dcfg &= ~USB_OTG_DCFG_EPMISCNT;
				dcfg |= epmscnt << USB_OTG_DCFG_EPMISCNT_Pos;
				USB_DEVICE->DCFG = dcfg;
			}
#endif

			USB_INEP(epnum)->DIEPCTL |= reg;
		}
	} else {
		USB_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & ((u32)(1UL << (ep->num & EP_ADDR_MSK)) << 16);

		if (((USB_OUTEP(epnum)->DOEPCTL) & USB_OTG_DOEPCTL_USBAEP) == 0U) {
			USB_OUTEP(epnum)->DOEPCTL |= (ep->maxpacket & USB_OTG_DOEPCTL_MPSIZ) |
										 ((u32)ep->type << USB_OTG_DIEPCTL_EPTYP_Pos) |
										 USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
										 USB_OTG_DOEPCTL_USBAEP;
		}
	}
	return HAL_OK;
}

/**
  * @brief  De-activate and de-initialize an endpoint
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_DeactivateEndpoint(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 reg;
#if USBD_DMA_ENABLE
	u32 i;
	u32 dcfg;
	u32 epmscnt;
#else
	UNUSED(pcd);
#endif

	/* Read DEPCTLn register */
	if (ep->is_in == 1U) {
		USB_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & (u32)(1UL << (ep->num & EP_ADDR_MSK)));
		reg = USB_INEP(epnum)->DIEPCTL;
		reg &= ~(USB_OTG_DIEPCTL_USBAEP |
				 USB_OTG_DIEPCTL_MPSIZ |
				 USB_OTG_DIEPCTL_TXFNUM |
				 USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				 USB_OTG_DIEPCTL_EPTYP);

#if USBD_DMA_ENABLE
		if ((ep->type != EP_TYPE_INTR) && (pcd->Init.dma_enable == 1U)) {
			for (i = 0; i < pcd->Init.dev_endpoints; i++) {
				if (pcd->nextep_seq[i] == ep->num) {
					break;
				}
			}
			pcd->nextep_seq[i] = pcd->nextep_seq[ep->num];
			if (pcd->first_in_nextep_seq == ep->num) {
				pcd->first_in_nextep_seq = i;
			}
			pcd->nextep_seq[ep->num] = 0xFF;
			reg &= ~USB_OTG_DIEPCTL_NEXTEP_Msk;
			dcfg = USB_DEVICE->DCFG;
			epmscnt = (dcfg & USB_OTG_DCFG_EPMISCNT) >> USB_OTG_DCFG_EPMISCNT_Pos;
			if (epmscnt >= 1) {
				epmscnt--;
			}
			dcfg &= ~USB_OTG_DCFG_EPMISCNT;
			dcfg |= epmscnt << USB_OTG_DCFG_EPMISCNT_Pos;
			USB_DEVICE->DCFG = dcfg;
		}
#endif

		USB_INEP(epnum)->DIEPCTL |= reg;
	} else {
		USB_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((u32)(1UL << (ep->num & EP_ADDR_MSK)) << 16));
		USB_OUTEP(epnum)->DOEPCTL &= ~(USB_OTG_DOEPCTL_USBAEP |
									   USB_OTG_DOEPCTL_MPSIZ |
									   USB_OTG_DOEPCTL_SD0PID_SEVNFRM |
									   USB_OTG_DOEPCTL_EPTYP);
	}

	return HAL_OK;
}

/**
  * @brief  Setup and starts a transfer over an EP
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_EPStartXfer(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 dma = pcd->Init.dma_enable;
	u16 pktcnt;

	/* IN endpoint */
	if (ep->is_in == 1U) {
		/* Zero Length Packet? */
		if (ep->xfer_len == 0U) {
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1U << 19));
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
		} else {
			/* Program the transfer size and packet count
			* as follows: xfersize = N * maxpacket +
			* short_packet pktcnt = N + (short_packet
			* exist ? 1 : 0)
			*/
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((ep->xfer_len + ep->maxpacket - 1U) / ep->maxpacket) << 19));
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len);

			if (ep->is_periodic != 0U) {
				USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT);
				USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1U << 29));
			}
		}

		if (dma == 1U) {
			if ((u32)ep->dma_addr != 0U) {
				USB_INEP(epnum)->DIEPDMA = (u32)(ep->dma_addr);
			}
			if (pcd->ded_tx_fifo_en == 0) {
				USB_EPPredictNextEp(pcd, ep);
			}
		} else {
			if (ep->is_periodic == 0U) {
				/* Enable the Tx FIFO Empty Interrupt for this EP */
				if (ep->xfer_len > 0U) {
					if (pcd->ded_tx_fifo_en == 0U) {
						USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
					} else {
						USB_DEVICE->DIEPEMPMSK |= 1UL << (epnum & EP_ADDR_MSK);
					}
				}
			}
		}

		if (ep->type == EP_TYPE_ISOC) {
			if ((USB_DEVICE->DSTS & (1U << 8)) == 0U) {
				USB_INEP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
			} else {
				USB_INEP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
			}
		}

		/* EP enable, IN data in FIFO */
		USB_INEP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

		if ((ep->is_periodic != 0U) && (ep->xfer_len > 0U)) {
			usb_hal_write_packet(ep->xfer_buff, ep->num, (u16)ep->xfer_len);
		}
	} else { /* OUT endpoint */
		/* Program the transfer size and packet count as follows:
		* pktcnt = N
		* xfersize = N * maxpacket
		*/
		USB_OUTEP(epnum)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
		USB_OUTEP(epnum)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);

		if (ep->xfer_len == 0U) {
			USB_OUTEP(epnum)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & ep->maxpacket);
			USB_OUTEP(epnum)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1U << 19));
		} else {
			pktcnt = (u16)((ep->xfer_len + ep->maxpacket - 1U) / ep->maxpacket);
			USB_OUTEP(epnum)->DOEPTSIZ |= USB_OTG_DOEPTSIZ_PKTCNT & ((u32)pktcnt << 19);
			USB_OUTEP(epnum)->DOEPTSIZ |= USB_OTG_DOEPTSIZ_XFRSIZ & (ep->maxpacket * pktcnt);
		}

		if (dma == 1U) {
			if ((u32)ep->dma_addr != 0U) {
				USB_OUTEP(epnum)->DOEPDMA = (u32)(ep->dma_addr);
			}
		}

		if (ep->type == EP_TYPE_ISOC) {
			if ((USB_DEVICE->DSTS & (1U << 8)) == 0U) {
				USB_OUTEP(epnum)->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
			} else {
				USB_OUTEP(epnum)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
			}
		}
		/* EP enable */
		USB_OUTEP(epnum)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}

	return HAL_OK;
}

/**
  * @brief  Setup and starts a transfer over the EP  0
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_EP0StartXfer(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 dma = pcd->Init.dma_enable;

	/* IN endpoint */
	if (ep->is_in == 1U) {
		/* Zero Length Packet? */
		if (ep->xfer_len == 0U) {
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1U << USB_OTG_DIEPTSIZ_PKTCNT_Pos));
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
		} else {
			/* Program the transfer size and packet count
			* as follows: xfersize = N * maxpacket +
			* short_packet pktcnt = N + (short_packet
			* exist ? 1 : 0)
			*/
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
			USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);

			if (ep->xfer_len > ep->maxpacket) {
				ep->xfer_len = ep->maxpacket;
			}
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1U << USB_OTG_DIEPTSIZ_PKTCNT_Pos));
			USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len);
		}

		if (dma == 1U) {
			if ((u32)ep->dma_addr != 0U) {
				USB_INEP(epnum)->DIEPDMA = (u32)(ep->dma_addr);
			}
			if (pcd->ded_tx_fifo_en == 0) {
				USB_EPPredictNextEp(pcd, ep);
			}
		} else {
			/* Enable the Tx FIFO Empty Interrupt for this EP */
			if (ep->xfer_len > 0U) {
				if (pcd->ded_tx_fifo_en == 0U) {
					USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
				} else {
					USB_DEVICE->DIEPEMPMSK |= 1UL << (epnum & EP_ADDR_MSK);
				}
			}
		}

		/* EP enable, IN data in FIFO */
		USB_INEP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
	} else { /* OUT endpoint */
		/* Program the transfer size and packet count as follows:
		* pktcnt = N
		* xfersize = N * maxpacket
		*/
		USB_OUTEP(epnum)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
		USB_OUTEP(epnum)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);

		if (ep->xfer_len > 0U) {
			ep->xfer_len = ep->maxpacket;
		}

		USB_OUTEP(epnum)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1U << 19));
		USB_OUTEP(epnum)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & (ep->maxpacket));

		if (dma == 1U) {
			if ((u32)ep->dma_addr != 0U) {
				USB_OUTEP(epnum)->DOEPDMA = (u32)(ep->dma_addr);
			}
		}

		/* EP enable */
		USB_OUTEP(epnum)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}

	return HAL_OK;
}

/**
  * @brief  Set a stall condition over an EP
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_EPSetStall(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	UNUSED(pcd);
	u32 epnum = (u32)ep->num;

	if (ep->is_in == 1U) {
		if (((USB_INEP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == 0U) && (epnum != 0U)) {
			USB_INEP(epnum)->DIEPCTL &= ~(USB_OTG_DIEPCTL_EPDIS);
		}
		USB_INEP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
	} else {
		if (((USB_OUTEP(epnum)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == 0U) && (epnum != 0U)) {
			USB_OUTEP(epnum)->DOEPCTL &= ~(USB_OTG_DOEPCTL_EPDIS);
		}
		USB_OUTEP(epnum)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
	}

	return HAL_OK;
}

/**
  * @brief  Clear a stall condition over an EP
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_Status USB_EPClearStall(USB_PCD_HandleTypeDef *pcd, USB_EPTypeDef *ep)
{
	UNUSED(pcd);
	u32 epnum = (u32)ep->num;

	if (ep->is_in == 1U) {
		USB_INEP(epnum)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
		if ((ep->type == EP_TYPE_INTR) || (ep->type == EP_TYPE_BULK)) {
			USB_INEP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM; /* DATA0 */
		}
	} else {
		USB_OUTEP(epnum)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
		if ((ep->type == EP_TYPE_INTR) || (ep->type == EP_TYPE_BULK)) {
			USB_OUTEP(epnum)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM; /* DATA0 */
		}
	}
	return HAL_OK;
}

/**
  * @brief  Stop the usb device mode
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status USB_StopDevice(USB_PCD_HandleTypeDef *pcd)
{
	HAL_Status ret;
	u32 i;

	UNUSED(pcd);

	/* Clear Pending interrupt */
	for (i = 0U; i < 15U; i++) {
		USB_INEP(i)->DIEPINT = 0xFB7FU;
		USB_OUTEP(i)->DOEPINT = 0xFB7FU;
	}

	/* Clear interrupt masks */
	USB_GLOBAL->GINTMSK = 0U;
	USB_DEVICE->DIEPMSK  = 0U;
	USB_DEVICE->DOEPMSK  = 0U;
	USB_DEVICE->DAINTMSK = 0U;

	/* Flush the FIFO */
	ret = usb_hal_flush_rx_fifo();
	if (ret != HAL_OK) {
		//RTK_LOGE(TAG, "USB_StopDevice: usb_hal_flush_rx_fifo fail\n");
		return ret;
	}

	ret = usb_hal_flush_tx_fifo(0x10U);
	if (ret != HAL_OK) {
		//RTK_LOGE(TAG, "USB_StopDevice: usb_hal_flush_tx_fifo fail\n");
		return ret;
	}

	return ret;
}

/**
  * @brief  Set an address to the usb device
  * @param  PCD Instance
  * @param  address  new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
HAL_Status USB_SetDevAddress(USB_PCD_HandleTypeDef *pcd, u8 address)
{
	UNUSED(pcd);
	USB_DEVICE->DCFG &= ~(USB_OTG_DCFG_DAD);
	USB_DEVICE->DCFG |= ((u32)address << 4) & USB_OTG_DCFG_DAD;

	return HAL_OK;
}

/**
  * @brief  Connect the USB device by enabling the pull-up/pull-down
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status USB_DevConnect(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	USB_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;

	return HAL_OK;
}

/**
  * @brief  Disconnect the USB device by disabling the pull-up/pull-down
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status USB_DevDisconnect(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	USB_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;

	return HAL_OK;
}

/**
  * @brief  Return the global USB interrupt status
  * @param  PCD Instance
  * @retval Global USB interrupt status
  */
u32 USB_ReadInterrupts(void)
{
	u32 tmpreg;

	tmpreg = USB_GLOBAL->GINTSTS;
	tmpreg &= USB_GLOBAL->GINTMSK;

	return tmpreg;
}

/**
  * @brief  Return the USB device OUT endpoints interrupt status
  * @param  PCD Instance
  * @retval OUT endpoints interrupt status
  */
u32 USB_ReadDevAllOutEpInterrupt(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	u32 tmpreg;

	tmpreg  = USB_DEVICE->DAINT;
	tmpreg &= USB_DEVICE->DAINTMSK;

	return ((tmpreg & 0xffff0000U) >> 16);
}

/**
  * @brief  Return the USB device IN endpoints interrupt status
  * @param  PCD Instance
  * @retval IN endpoints interrupt status
  */
u32 USB_ReadDevAllInEpInterrupt(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	u32 tmpreg;

	tmpreg  = USB_DEVICE->DAINT;
	tmpreg &= USB_DEVICE->DAINTMSK;

	return ((tmpreg & 0xFFFFU));
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  PCD Instance
  * @param  epnum  endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device OUT EP Interrupt register
  */
u32 USB_ReadDevOutEPInterrupt(USB_PCD_HandleTypeDef *pcd, u8 epnum)
{
	UNUSED(pcd);
	u32 tmpreg;

	tmpreg  = USB_OUTEP((u32)epnum)->DOEPINT;
	tmpreg &= USB_DEVICE->DOEPMSK;

	return tmpreg;
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  PCD Instance
  * @param  epnum  endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device IN EP Interrupt register
  */
u32 USB_ReadDevInEPInterrupt(USB_PCD_HandleTypeDef *pcd, u8 epnum)
{
	UNUSED(pcd);
	u32 tmpreg, msk;
	u32 emp_msk;

	msk = USB_DEVICE->DIEPMSK;
	if (pcd->ded_tx_fifo_en) {
		emp_msk = USB_DEVICE->DIEPEMPMSK;
		msk |= ((emp_msk >> (epnum & EP_ADDR_MSK)) & 0x1U) << USB_OTG_DIEPINT_TXFE_Pos;
	}
	tmpreg = USB_INEP((u32)epnum)->DIEPINT & msk;

	return tmpreg;
}

/**
  * @brief  Clear a USB interrupt
  * @param  PCD Instance
  * @param  interrupt  interrupt flag
  * @retval None
  */
void  USB_ClearInterrupts(USB_PCD_HandleTypeDef *pcd, u32 interrupt)
{
	UNUSED(pcd);
	USB_GLOBAL->GINTSTS |= interrupt;
}

/**
  * @brief  Returns USB core mode
  * @param  PCD Instance
  * @retval return core mode : Host or Device
  *          This parameter can be one of these values:
  *           0 : Host
  *           1 : Device
  */
u32 USB_GetMode(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);
	return ((USB_GLOBAL->GINTSTS) & 0x1U);
}

/**
  * @brief  Activate EP0 for Setup transactions
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status  USB_ActivateSetup(USB_PCD_HandleTypeDef *pcd)
{
	UNUSED(pcd);

	/* Set the MPS of the IN EP based on the enumeration speed */
	USB_INEP(0U)->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;

	if ((USB_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ) {
		USB_INEP(0U)->DIEPCTL |= 3U;
	}
	USB_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;

	return HAL_OK;
}

/**
  * @brief  Prepare the EP0 to start the first control setup
  * @param  PCD Instance
  * @retval HAL status
  */
HAL_Status USB_EP0_OutStart(USB_PCD_HandleTypeDef *pcd)
{
	u32 gSNPSiD = USB_GLOBAL->GSNPSID;
	u32 dma = pcd->Init.dma_enable;
	u8 *psetup = (u8 *)pcd->Setup;

	if (gSNPSiD > USB_OTG_CORE_ID_300A) {
		if ((USB_OUTEP(0U)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA) {
			return HAL_OK;
		}
	}

	USB_OUTEP(0U)->DOEPTSIZ = 0U;
	USB_OUTEP(0U)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1U << 19));
	USB_OUTEP(0U)->DOEPTSIZ |= (3U * 8U);
	USB_OUTEP(0U)->DOEPTSIZ |=  USB_OTG_DOEPTSIZ_STUPCNT;

	if (dma == 1U) {
		USB_OUTEP(0U)->DOEPDMA = (u32)psetup;
		/* EP enable */
		USB_OUTEP(0U)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_USBAEP;
	}

	return HAL_OK;
}

