/**
  ******************************************************************************
  * @file    usbd_hal_rom.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides the functionalities of USBD HAL layer
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbd_hal_rom.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Reset the USB Core (needed after USB clock settings change)
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_CoreReset(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	u32 count = 0U;

	/* Wait for AHB master IDLE state. */
	do {
		USB_DELAY_US(10);
		if (++count > 100000U) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB_CoreReset: TO1\n");
			return HAL_TIMEOUT;
		}
	} while ((USB_GLOBAL->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0U);

	/* Core Soft Reset */
	count = 0U;
	USB_GLOBAL->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;

	do {
		if (++count > 10000U) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB_CoreReset: TO2\n");
			return HAL_TIMEOUT;
		}
		USB_DELAY_US(1);
	} while ((USB_GLOBAL->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);

	USB_DELAY_MS(100);

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initializes the USB Core
  * @param  PCD Instance
  * @param  cfg pointer to a USB_CfgTypeDef structure that contains
  *         the configuration information for the specified USB_GLOBAL peripheral.
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_HalInit(USB_PCD_HandleTypeDef *hpcd)
{
	HAL_Status ret = HAL_OK;
	USB_CfgTypeDef *cfg = &hpcd->Init;

	if (hpcd->PHY_Initialized == 0) {
		hpcd->PHY_Initialized = 1;
		USB_GLOBAL->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPIUTMISEL;
		USB_GLOBAL->GUSBCFG |= USB_OTG_GUSBCFG_PHYIF;
		ret = USB_CoreReset(hpcd);
	}

	if (cfg->dma_enable == 1U) {
		USB_GLOBAL->GAHBCFG |= (USB_OTG_GAHBCFG_HBSTLEN_2 | USB_OTG_GAHBCFG_DMAEN);
	}

	USB_GLOBAL->GOTGCTL &= ~USB_OTG_GOTGCTL_OTGVER;

	return ret;
}

/**
  * @brief  Set the USB turnaround time
  * @param  PCD Instance
  * @param  hclk: AHB clock frequency
  * @retval USB turnaround time In PHY Clocks number
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_SetTurnaroundTime(USB_PCD_HandleTypeDef *hpcd)
{
	u32 UsbTrd;

	/* The USBTRD is configured according to the tables below, depending on AHB frequency
	used by application. In the low AHB frequency range it is used to stretch enough the USB response
	time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access
	latency to the Data FIFO */
	if (hpcd->Init.speed == USBD_SPEED_HIGH) {
		UsbTrd = USBD_HS_TRDT_VALUE;
	} else {
		UsbTrd = USBD_FS_TRDT_VALUE;
	}

	USB_GLOBAL->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
	USB_GLOBAL->GUSBCFG |= (u32)((UsbTrd << 10) & USB_OTG_GUSBCFG_TRDT);

	return HAL_OK;
}

/**
  * @brief  Enables the controller's Global Int in the AHB Config reg
  * @param  void
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EnableGlobalInt(void)
{
	USB_GLOBAL->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
	return HAL_OK;
}

/**
  * @brief  USB_DisableGlobalInt
  *         Disable the controller's Global Int in the AHB Config reg
  * @param  void
  * @retval HAL status
*/
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_DisableGlobalInt(void)
{
	USB_GLOBAL->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
	return HAL_OK;
}

/**
  * @brief  Set functional mode
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_SetDeviceMode(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);

	USB_GLOBAL->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
	USB_GLOBAL->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;

	USB_DELAY_MS(100U);

	return HAL_OK;
}

/**
  * @brief  Predict next ep for DMA mode
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EPPredictNextEp(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	UNUSED(hpcd);
	UNUSED(ep);

	return HAL_OK;
}

/**
  * @brief  Reset IN Token Learn Queue
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_ResetLearnQueue(USB_PCD_HandleTypeDef *hpcd)
{
	u32 i;

	hpcd->in_ep_sequece[0] = 0;
	for (i = 1U; i < hpcd->Init.dev_endpoints; i++) {
		hpcd->in_ep_sequece[i] = 0xFF;
	}

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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_DevInit(USB_PCD_HandleTypeDef *hpcd)
{
	HAL_Status ret = HAL_OK;
	USB_CfgTypeDef *cfg = &hpcd->Init;
	u32 i;
	u32 reg;

	for (i = 0U; i < 15U; i++) {
		USB_GLOBAL->DIEPTXF[i] = 0U;
	}

	/* Restart the Phy Clock */
	USB_PCGCCTL &= ~USB_OTG_PCGCR_STPPCLK;

	USB_DELAY_US(10);

	/* Device mode configuration */
	reg = USB_DEVICE->DCFG;
	reg &= ~(USB_OTG_DCFG_DESCDMA | USB_OTG_DCFG_PFIVL_0 | USB_OTG_DCFG_PFIVL_1 | USB_OTG_DCFG_ENDEVOUTNAK);
	USB_DEVICE->DCFG = reg;

	if (cfg->speed == USBD_SPEED_HIGH) {
		/* Set High speed phy */
		(void)USB_SetDevSpeed(hpcd, USB_OTG_SPEED_HIGH);
	} else {
		/* set High speed phy in Full speed mode */
		(void)USB_SetDevSpeed(hpcd, USB_OTG_SPEED_HIGH_IN_FULL);
	}

	/* Flush the FIFOs */
	if (USB_FlushTxFifo(hpcd, 0x10U) != HAL_OK) { /* all Tx FIFOs */
		ret = HAL_ERR_PARA;
	}

	if (USB_FlushRxFifo(hpcd) != HAL_OK) {
		ret = HAL_ERR_PARA;
	}

	USB_ResetLearnQueue(hpcd);

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
		USB_INEP(i)->DIEPINT  = 0xFFFFU; //0xFB7FU;
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
		USB_OUTEP(i)->DOEPINT  = 0xFFFFU; //0xFB7FU;
		USB_OUTEP(i)->DOEPDMA  = 0U;
	}

	USB_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);

	/* Disable all interrupts. */
	USB_GLOBAL->GINTMSK = 0U;

	/* Clear any pending interrupts */
	USB_GLOBAL->GINTSTS = 0xFFFFFFFF; // 0xBFFFFFFFU;

	/* Enable the common interrupts */
	if (cfg->dma_enable == 0U) {
		USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
	}

	// FIXME: from AmebaD
	//USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_MMISM | USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_CIDSCHGM | USB_OTG_GINTMSK_SRQIM;
	USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_WUIM;

	/* Enable interrupts matching to the Device mode ONLY */
	USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |
						   USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
						   USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_EPMISM;

	return ret;
}

/**
  * @brief  Flush a Tx FIFO
  * @param  PCD Instance
  * @param  num  FIFO number
  *         This parameter can be a value from 1 to 15
            15 means Flush all Tx FIFOs
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_FlushTxFifo(USB_PCD_HandleTypeDef *hpcd, u32 num)
{
	UNUSED(hpcd);
	u32 count = 0U;

	USB_GLOBAL->GRSTCTL = (USB_OTG_GRSTCTL_TXFFLSH | (num << 6));

	do {
		if (++count > 200000U) {
			return HAL_TIMEOUT;
		}
		USB_DELAY_US(1);
	} while ((USB_GLOBAL->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
	USB_DELAY_US(1);

	return HAL_OK;
}

/**
  * @brief  Flush Rx FIFO
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_FlushRxFifo(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	u32 count = 0;

	USB_GLOBAL->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;

	do {
		if (++count > 200000U) {
			return HAL_TIMEOUT;
		}
		USB_DELAY_US(1);
	} while ((USB_GLOBAL->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
	USB_DELAY_US(1);

	return HAL_OK;
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_SetDevSpeed(USB_PCD_HandleTypeDef *hpcd, u8 speed)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
u8 USB_GetDevSpeed(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	u8 speed;
	u32 DevEnumSpeed = USB_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD;

	if (DevEnumSpeed == DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ) {
		speed = USB_OTG_SPEED_HIGH;
	} else if ((DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ) ||
			   (DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_48MHZ)) {
		speed = USB_OTG_SPEED_FULL;
	} else {
		speed = 0U;
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_GetTxFifoNum(USB_EPTypeDef *ep)
{
	u32 tx_fifo_num = 0;
	if (ep->type == EP_TYPE_INTR) {
		tx_fifo_num = 1;
	}
	return tx_fifo_num;
}

/**
  * @brief  Activate and configure an endpoint
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_ActivateEndpoint(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 tx_fifo_num = 0;
	u32 reg;

	UNUSED(hpcd);

	if (ep->is_in == 1U) {
		USB_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & (u32)(1UL << (ep->num & EP_ADDR_MSK));

		if ((USB_INEP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_USBAEP) == 0U) {
			tx_fifo_num = USB_GetTxFifoNum(ep);
			reg = (ep->maxpacket & USB_OTG_DIEPCTL_MPSIZ) |
				  ((u32)ep->type << USB_OTG_DIEPCTL_EPTYP_Pos) | (tx_fifo_num << USB_OTG_DIEPCTL_TXFNUM_Pos) |
				  USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				  USB_OTG_DIEPCTL_USBAEP;
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_DeactivateEndpoint(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 reg;

	UNUSED(hpcd);

	/* Read DEPCTLn register */
	if (ep->is_in == 1U) {
		USB_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & (u32)(1UL << (ep->num & EP_ADDR_MSK)));
		reg = USB_INEP(epnum)->DIEPCTL;
		reg &= ~(USB_OTG_DIEPCTL_USBAEP |
				 USB_OTG_DIEPCTL_MPSIZ |
				 USB_OTG_DIEPCTL_TXFNUM |
				 USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				 USB_OTG_DIEPCTL_EPTYP);
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EPStartXfer(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 dma = hpcd->Init.dma_enable;
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

			if (ep->type == EP_TYPE_ISOC) {
				USB_INEP(epnum)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT);
				USB_INEP(epnum)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1U << 29));
			}
		}

		if (dma == 1U) {
			if ((u32)ep->dma_addr != 0U) {
				USB_INEP(epnum)->DIEPDMA = (u32)(ep->dma_addr);
			}
			USB_EPPredictNextEp(hpcd, ep);
		} else {
			if (ep->type != EP_TYPE_ISOC) {
				/* Enable the Tx FIFO Empty Interrupt for this EP */
				if (ep->xfer_len > 0U) {
					USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
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

		if (ep->type == EP_TYPE_ISOC) {
			(void)USB_WritePacket(hpcd, ep->xfer_buff, ep->num, (u16)ep->xfer_len);
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
			if ((u32)ep->xfer_buff != 0U) {
				USB_OUTEP(epnum)->DOEPDMA = (u32)(ep->xfer_buff);
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EP0StartXfer(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	u32 epnum = (u32)ep->num;
	u32 dma = hpcd->Init.dma_enable;

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
			USB_EPPredictNextEp(hpcd, ep);
		} else {
			/* Enable the Tx FIFO Empty Interrupt for this EP */
			if (ep->xfer_len > 0U) {
				USB_GLOBAL->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
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
			if ((u32)ep->xfer_buff != 0U) {
				USB_OUTEP(epnum)->DOEPDMA = (u32)(ep->xfer_buff);
			}
		}

		/* EP enable */
		USB_OUTEP(epnum)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}

	return HAL_OK;
}

/**
  * @brief  Writes a packet into the Tx FIFO associated with the EP/channel
  * @param  PCD Instance
  * @param  src   pointer to source buffer
  * @param  ch_ep_num  endpoint or host channel number
  * @param  len  Number of bytes to write
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_WritePacket(USB_PCD_HandleTypeDef *hpcd, u8 *src, u8 ch_ep_num, u16 len)
{
	//u32 *pSrc = (u32 *)src;
	USB_Unaligned32TypeDef *pSrc = (USB_Unaligned32TypeDef *)src;
	u32 count32b, i;
	u32 dma = hpcd->Init.dma_enable;

	//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "USB_WritePacket EP%d len=%d\n", ch_ep_num, len);

	if (dma == 0U) {
		count32b = ((u32)len + 3U) / 4U;
		for (i = 0U; i < count32b; i++) {
			//USB_DFIFO((u32)ch_ep_num) = *((__packed u32 *)pSrc);
			USB_DFIFO((u32)ch_ep_num) = pSrc->data;
			pSrc++;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Read a packet from the Tx FIFO associated with the EP/channel
  * @param  PCD Instance
  * @param  dest  source pointer
  * @param  len  Number of bytes to read
  * @retval pointer to destination buffer
  */
USBD_WEEK USBD_TEXT_SECTION
void *USB_ReadPacket(USB_PCD_HandleTypeDef *hpcd, u8 *dest, u16 len)
{
	UNUSED(hpcd);
	//u32 *pDest = (u32 *)dest;
	USB_Unaligned32TypeDef *pDest = (USB_Unaligned32TypeDef *)dest;
	u32 i;
	u32 count32b = ((u32)len + 3U) / 4U;

	for (i = 0U; i < count32b; i++) {
		//*(__packed u32 *)pDest = USB_DFIFO(0U);
		pDest->data = USB_DFIFO(0U);
		pDest++;
	}

	return ((void *)pDest);
}

/**
  * @brief  Set a stall condition over an EP
  * @param  PCD Instance
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EPSetStall(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EPClearStall(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_StopDevice(USB_PCD_HandleTypeDef *hpcd)
{
	HAL_Status ret;
	u32 i;

	/* Clear Pending interrupt */
	for (i = 0U; i < 15U; i++) {
		USB_INEP(i)->DIEPINT = 0xFB7FU;
		USB_OUTEP(i)->DOEPINT = 0xFB7FU;
	}

	/* Clear interrupt masks */
	USB_GLOBAL->GINTMSK  = 0U;
	USB_DEVICE->DIEPMSK  = 0U;
	USB_DEVICE->DOEPMSK  = 0U;
	USB_DEVICE->DAINTMSK = 0U;

	/* Flush the FIFO */
	ret = USB_FlushRxFifo(hpcd);
	if (ret != HAL_OK) {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB_StopDevice: USB_FlushRxFifo fail\n");
		return ret;
	}

	ret = USB_FlushTxFifo(hpcd,  0x10U);
	if (ret != HAL_OK) {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB_StopDevice: USB_FlushTxFifo fail\n");
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_SetDevAddress(USB_PCD_HandleTypeDef *hpcd, u8 address)
{
	UNUSED(hpcd);
	USB_DEVICE->DCFG &= ~(USB_OTG_DCFG_DAD);
	USB_DEVICE->DCFG |= ((u32)address << 4) & USB_OTG_DCFG_DAD;

	return HAL_OK;
}

/**
  * @brief  Connect the USB device by enabling the pull-up/pull-down
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_DevConnect(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	USB_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
	USB_DELAY_MS(100U);

	return HAL_OK;
}

/**
  * @brief  Disconnect the USB device by disabling the pull-up/pull-down
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_DevDisconnect(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	USB_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
	USB_DELAY_MS(3U);

	return HAL_OK;
}

/**
  * @brief  Return the global USB interrupt status
  * @param  PCD Instance
  * @retval Global USB interrupt status
  */
USBD_WEEK USBD_TEXT_SECTION
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_ReadDevAllOutEpInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_ReadDevAllInEpInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_ReadDevOutEPInterrupt(USB_PCD_HandleTypeDef *hpcd, u8 epnum)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_ReadDevInEPInterrupt(USB_PCD_HandleTypeDef *hpcd, u8 epnum)
{
	UNUSED(hpcd);
	u32 tmpreg, msk;

	msk = USB_DEVICE->DIEPMSK;
	tmpreg = USB_INEP((u32)epnum)->DIEPINT & msk;

	return tmpreg;
}

/**
  * @brief  Clear a USB interrupt
  * @param  PCD Instance
  * @param  interrupt  interrupt flag
  * @retval None
  */
USBD_WEEK USBD_TEXT_SECTION
void  USB_ClearInterrupts(USB_PCD_HandleTypeDef *hpcd, u32 interrupt)
{
	UNUSED(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
u32 USB_GetMode(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);
	return ((USB_GLOBAL->GINTSTS) & 0x1U);
}

/**
  * @brief  Activate EP0 for Setup transactions
  * @param  PCD Instance
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status  USB_ActivateSetup(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);

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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_EP0_OutStart(USB_PCD_HandleTypeDef *hpcd)
{
	u32 gSNPSiD = USB_GLOBAL->GSNPSID;
	u32 dma = hpcd->Init.dma_enable;
	u8 *psetup = (u8 *)hpcd->Setup;

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

