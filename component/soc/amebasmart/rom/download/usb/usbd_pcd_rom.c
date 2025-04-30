/**
  ******************************************************************************
  * @file    usbd_pcd_rom.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides the functionalities of USB PCD layer
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

#include "usbd_core_rom.h"
#include "usbd_pcd_rom.h"

/* Private defines -----------------------------------------------------------*/

/* Do USB power on flow after USB download done/aborted */
#define USB_PWR_OFF_EN         0

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Load PHY vendor control registers
  * @param  addr: PHY register address
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_LoadPhyVendorControlRegister(u8 addr)
{
	HAL_Status ret = HAL_OK;
	u32 count = 0U;
	u32 reg = 0x0A300000U;

	reg |= (((u32)(USB_OTG_PHY_LOW_ADDR(addr))) << USB_OTG_GPVNDCTL_VCTRL_Pos);
	USB_GLOBAL->GPVNDCTL = reg;

	do {
		/* 1us timeout expected, 1ms for safe */
		USB_DELAY_US(1);
		if (++count > 1000U) {
			ret = HAL_TIMEOUT;
			break;
		}
	} while ((USB_GLOBAL->GPVNDCTL & USB_OTG_GPVNDCTL_VSTSDONE) == 0U);

	return ret;
}

/**
  * @brief  Write USB PHY register
  * @param  addr: USB PHY register address
  * @param  data: USB PHY register value
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_WritePhyRegister(u8 addr, u8 value)
{
	HAL_Status ret;
	u32 reg;

	reg = HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_VND_STS_OUT);
	reg &= (~USB_OTG_PHY_DATA_MASK);
	reg |= value;
	HAL_WRITE32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_VND_STS_OUT, reg);

	/* Load low addr */
	ret = USB_PCD_LoadPhyVendorControlRegister(USB_OTG_PHY_LOW_ADDR(addr));
	if (ret == HAL_OK) {
		/* Load high addr */
		ret = USB_PCD_LoadPhyVendorControlRegister(USB_OTG_PHY_HIGH_ADDR(addr));
	}

	return ret;
}

/**
  * @brief  Read USB PHY register
  * @param  addr: USB PHY register address
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_ReadPhyRegister(u8 addr, u8 *value)
{
	HAL_Status ret;
	u32 reg;
	u8 addr_read;

	if (addr >= 0xE0) {
		addr_read = addr - 0x20;
	} else {
		addr_read = addr;
	}

	/* Load low addr */
	ret = USB_PCD_LoadPhyVendorControlRegister(USB_OTG_PHY_LOW_ADDR(addr_read));
	if (ret == HAL_OK) {
		/* Load high addr */
		ret = USB_PCD_LoadPhyVendorControlRegister(USB_OTG_PHY_HIGH_ADDR(addr_read));
		if (ret == HAL_OK) {
			reg = USB_GLOBAL->GPVNDCTL;
			*value = ((reg & USB_OTG_GPVNDCTL_REGDATA_Msk) >> USB_OTG_GPVNDCTL_REGDATA_Pos) & 0xFF;
		}
	}

	return ret;
}

/**
  * @brief  Select USB PHY page
  * @param  None
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_PhySelectPage(u8 page)
{
	HAL_Status ret;
	u8 reg;

	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_F4, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_F4: %d\n", ret);
		return ret;
	}

	reg &= (~USB_OTG_PHY_REG_F4_BIT_PAGE_SEL_MASK);
	reg |= (page << USB_OTG_PHY_REG_F4_BIT_PAGE_SEL_POS) & USB_OTG_PHY_REG_F4_BIT_PAGE_SEL_MASK;
	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_F4, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_F4: %d\n", ret);
	}

	return ret;
}

/**
  * @brief  USB PHY calibration
  *			Shall be called after soft disconnect
  * @param  None
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_PhyCalibration(void)
{
	HAL_Status ret;
	u8 reg;
	u32 cal;
	u32 cal2;

	cal = SYSCFG_OTP_GetUsbPhyCalData();
	if (0 == (cal & SEC_BIT_USB_PHY_CAL_EN)) {
		return HAL_OK;
	}

	/* 3ms + 2.5us from DD, 3ms already delayed after soft disconnect */
	USB_DELAY_US(3);

	/* Calibrate page 0 registers */
	ret = USB_PCD_PhySelectPage(USB_OTG_PHY_REG_F4_BIT_PAGE0);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to select page 0: %d\n", ret);
		return ret;
	}

	/* E0 / Page 0 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E0, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P0_E0: %d\n", ret);
		return ret;
	}

	if (cal & SEC_BIT_USB_PHY_Z0_AUTO_K) {
		reg |= USB_OTG_PHY_REG_E0_PAGE0_BIT_Z0_AUTO_K;
	} else {
		reg &= (~USB_OTG_PHY_REG_E0_PAGE0_BIT_Z0_AUTO_K);
	}

	reg &= (~USB_OTG_PHY_REG_E0_PAGE0_BIT_Z0_ADJR_MASK);
	reg |= (SEC_GET_USB_PHY_Z0_ADJR(cal) << USB_OTG_PHY_REG_E0_PAGE0_BIT_Z0_ADJR_POS) & USB_OTG_PHY_REG_E0_PAGE0_BIT_Z0_ADJR_MASK;

	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E0, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P0_E0: %d\n", ret);
		return ret;
	}

	/* E4 / Page 0 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E4, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P0_E4: %d\n", ret);
		return ret;
	}

	reg &= (~USB_OTG_PHY_REG_E4_PAGE0_BIT_REG_SITX_MASK);
	reg |= (SEC_GET_USB_PHY_REG_SITX(cal) << USB_OTG_PHY_REG_E4_PAGE0_BIT_REG_SITX_POS) & USB_OTG_PHY_REG_E4_PAGE0_BIT_REG_SITX_MASK;
	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E4, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P0_E4: %d\n", ret);
		return ret;
	}

	/* E6 / Page 0 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E6, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P0_E6: %d\n", ret);
		return ret;
	}

	reg &= (~USB_OTG_PHY_REG_E6_PAGE0_BIT_RX_BOOST_MASK);
	reg |= (SEC_GET_USB_PHY_RX_BOOST(cal) << USB_OTG_PHY_REG_E6_PAGE0_BIT_RX_BOOST_POS) & USB_OTG_PHY_REG_E6_PAGE0_BIT_RX_BOOST_MASK;
	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E6, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P0_E6: %d\n", ret);
		return ret;
	}

	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_F1, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P0_F1: %d\n", ret);
		return ret;
	}

	/* F1 / Page 0 */
	if (cal & SEC_BIT_USB_PHY_UTMI_POS_OUT) {
		reg |= USB_OTG_PHY_REG_F1_PAGE0_BIT_UTMI_POS_OUT;
	} else {
		reg &= (~USB_OTG_PHY_REG_F1_PAGE0_BIT_UTMI_POS_OUT);
	}

	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_F1, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P0_F1: %d\n", ret);
		return ret;
	}

	/* Calibrate page 1 registers */
	ret = USB_PCD_PhySelectPage(USB_OTG_PHY_REG_F4_BIT_PAGE1);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to select page 1: %d\n", ret);
		return ret;
	}

	/* E1 / Page 1 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E1, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P1_E1: %d\n", ret);
		return ret;
	}

	reg &= (~USB_OTG_PHY_REG_E1_PAGE1_BIT_REG_SRC_MASK);
	reg |= (SEC_GET_USB_PHY_REG_SRC(cal) << USB_OTG_PHY_REG_E1_PAGE1_BIT_REG_SRC_POS) & USB_OTG_PHY_REG_E1_PAGE1_BIT_REG_SRC_MASK;

	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E1, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P1_E1: %d\n", ret);
		return ret;
	}

	/* E2 / Page 1 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E2, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P1_E2: %d\n", ret);
		return ret;
	}

	if (cal & SEC_BIT_USB_PHY_SREN) {
		reg |= USB_OTG_PHY_REG_E2_PAGE1_BIT_SREN;
	} else {
		reg &= (~USB_OTG_PHY_REG_E2_PAGE1_BIT_SREN);
	}

	reg &= (~USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRVH_MASK);
	reg |= (SEC_GET_USB_PHY_REG_SRVH(cal) << USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRVH_POS) & USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRVH_MASK;
	reg &= (~USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRM_MASK);
	reg |= (SEC_GET_USB_PHY_REG_SRM(cal) << USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRM_POS) & USB_OTG_PHY_REG_E2_PAGE1_BIT_REG_SRM_MASK;

	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E2, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P1_E2: %d\n", ret);
		return ret;
	}

	/* F6 / Page 1 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_F6, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P1_F6: %d\n", ret);
		return ret;
	}

	cal2 = SYSCFG_OTP_GetUsbPhyCalData2();

	reg &= (~USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_LATE_DLLEN_MASK);
	reg |= (SEC_GET_USB_PHY_REG_LATE_DLLEN(cal2) << USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_LATE_DLLEN_POS) & USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_LATE_DLLEN_MASK;

	if (cal2 & SEC_BIT_USB_PHY_REG_NSQ_VLD_SEL) {
		reg |= USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_NSQ_VLD_SEL;
	} else {
		reg &= (~USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_NSQ_VLD_SEL);
	}

	if (cal2 & SEC_BIT_USB_PHY_REG_DISABLE_EB_WAIT4IDLE) {
		reg |= USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_DISABLE_EB_WAIT4IDLE;
	} else {
		reg &= (~USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_DISABLE_EB_WAIT4IDLE);
	}

	if (cal2 & SEC_BIT_USB_PHY_REG_WATER_LEVEL_CLEAN_SEL) {
		reg |= USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_WATER_LEVEL_CLEAN_SEL;
	} else {
		reg &= (~USB_OTG_PHY_REG_F6_PAGE1_BIT_REG_WATER_LEVEL_CLEAN_SEL);
	}

	reg &= (~USB_OTG_PHY_REG_F6_PAGE1_BIT_NSQ_DEL_SEL_MASK);
	reg |= (SEC_GET_USB_PHY_NSQ_DEL_SEL(cal2) << USB_OTG_PHY_REG_F6_PAGE1_BIT_NSQ_DEL_SEL_POS) & USB_OTG_PHY_REG_F6_PAGE1_BIT_NSQ_DEL_SEL_MASK;

	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_F6, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P1_F6: %d\n", ret);
		return ret;
	}

	/* Calibrate page 2 registers */
	ret = USB_PCD_PhySelectPage(USB_OTG_PHY_REG_F4_BIT_PAGE2);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to select page 2: %d\n", ret);
		return ret;
	}

	/* E7 / Page 2 */
	ret = USB_PCD_ReadPhyRegister(USB_OTG_PHY_REG_E7, &reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to read USB_OTG_PHY_REG_P2_E7: %d\n", ret);
		return ret;
	}

	reg &= (~USB_OTG_PHY_REG_E7_PAGE2_BIT_SEND_OBJ_MASK);
	reg |= (SEC_GET_USB_PHY_SEND_OBJ(cal) << USB_OTG_PHY_REG_E7_PAGE2_BIT_SEND_OBJ_POS) & USB_OTG_PHY_REG_E7_PAGE2_BIT_SEND_OBJ_MASK;
	reg &= (~USB_OTG_PHY_REG_E7_PAGE2_BIT_SENH_OBJ_MASK);
	reg |= (SEC_GET_USB_PHY_SENH_OBJ(cal) << USB_OTG_PHY_REG_E7_PAGE2_BIT_SENH_OBJ_POS) & USB_OTG_PHY_REG_E7_PAGE2_BIT_SENH_OBJ_MASK;
	ret = USB_PCD_WritePhyRegister(USB_OTG_PHY_REG_E7, reg);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Fail to write USB_OTG_PHY_REG_P2_E7: %d\n", ret);
		return ret;
	}

	return ret;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_MspInit(USB_PCD_HandleTypeDef *hpcd)
{
	u32 reg = 0;
	u32 count = 0;

	UNUSED(hpcd);

	/* Enable BG */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1);
	reg |= (LSYS_BIT_BG_PWR | LSYS_BIT_BG_ON_USB2);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, reg);

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, ENABLE);

	/* USB Power Sequence */
	/* USB digital pad en,dp/dm sharing GPIO PAD */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~(HSYS_BIT_USB2_DIGOTGPADEN | HSYS_BIT_USB_OTGMODE | HSYS_BIT_USB2_DIGPADEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB PWC_UALV_EN,  PWC_UAHV_EN */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= (HSYS_BIT_PWC_UALV_EN | HSYS_BIT_PWC_UAHV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	USB_DELAY_US(2);

	/* USB PWC_UABG_EN */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= HSYS_BIT_PWC_UABG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	USB_DELAY_US(10);

	/* USB ISO_USBD_EN = 0 => disable isolation output signal from PD_USBD*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~HSYS_BIT_ISO_USBA_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	USB_DELAY_US(10);

	/* USBPHY_EN = 1 */
	reg = HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL);
	reg |= (USB_OTG_ADDON_REG_CTRL_BIT_USBPHY_EN | USB_OTG_ADDON_REG_CTRL_BIT_PORETB_TOP);
	HAL_WRITE32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL, reg);
	USB_DELAY_US(34);

	/* Wait UPLL_CKRDY */
	do {
		/* 1ms timeout expected, 10ms for safe */
		USB_DELAY_US(10);
		if (++count > 1000U) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB_PCD_MspInit timeout\n");
			return HAL_TIMEOUT;
		}
	} while (!(HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL) & USB_OTG_ADDON_REG_CTRL_BIT_UPLL_CKRDY));

	/* USBOTG_EN = 1 => enable USBOTG */
	reg = HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL);
	reg |= USB_OTG_ADDON_REG_CTRL_BIT_USBOTG_EN;
	HAL_WRITE32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL, reg);

	return HAL_OK;
}

#if USB_PWR_OFF_EN

/**
  * @brief  DeInitializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_MspDeInit(USB_PCD_HandleTypeDef *hpcd)
{
	UNUSED(hpcd);

	u32 reg = 0;

	/* USBOTG_EN = 0 => disable USBOTG */
	reg = HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL);
	reg &= (~USB_OTG_ADDON_REG_CTRL_BIT_USBOTG_EN);
	HAL_WRITE32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL, reg);

	/* USBPHY_EN = 0 */
	reg = HAL_READ32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL);
	reg &= (~(USB_OTG_ADDON_REG_CTRL_BIT_USBPHY_EN | USB_OTG_ADDON_REG_CTRL_BIT_PORETB_TOP));
	HAL_WRITE32(USB_OTG_REG_BASE, USB_OTG_ADDON_REG_CTRL, reg);

	/* USB ISO_USBD_EN = 1 => enable isolation output signal from PD_USBD*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= HSYS_BIT_ISO_USBA_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB PWC_UABG_EN = 0 */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~HSYS_BIT_PWC_UABG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* PWC_UPHV_EN  = 0 => disable USBPHY analog 3.3V power */
	/* PWC_UPLV_EN = 0 => disable USBPHY analog 1.2V power */
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg &= ~(HSYS_BIT_PWC_UALV_EN | HSYS_BIT_PWC_UAHV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);

	/* USB digital pad disable*/
	reg = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL);
	reg |= (HSYS_BIT_USB2_DIGOTGPADEN | HSYS_BIT_USB2_DIGPADEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_USB_CTRL, reg);
	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, DISABLE);

	return HAL_OK;
}

#endif // USB_PWR_OFF_EN

/**
  * @brief  Initializes the PCD interrupt.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_InitInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	if (hpcd->ISR_Installed) {
		InterruptDis(USB_OTG_IRQ);
		InterruptUnRegister(USB_OTG_IRQ);
	}
	InterruptRegister((IRQ_FUN)USBD_IRQHandler, USB_OTG_IRQ, NULL, 5);
	InterruptEn(USB_OTG_IRQ, 5);
	hpcd->ISR_Installed = 1;
	return HAL_OK;
}

/**
  * @brief  Deinitializes the PCD interrupt.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_DeInitInterrupt(USB_PCD_HandleTypeDef *hpcd)
{
	if (hpcd->ISR_Installed) {
		InterruptDis(USB_OTG_IRQ);
		InterruptUnRegister(USB_OTG_IRQ);
		hpcd->ISR_Installed = 0;
	}
	return HAL_OK;
}

/**
  * @brief  Handle PCD reset interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleResetIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 i;

	USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

	(void)USB_FlushTxFifo(hpcd, 0x10U);

	for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
		USB_INEP(i)->DIEPINT = 0xFFFFU; // 0xFB7FU;
		USB_INEP(i)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
		USB_OUTEP(i)->DOEPINT = 0xFFFFU; // 0xFB7FU;
		USB_OUTEP(i)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
	}

	USB_ResetLearnQueue(hpcd);

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
	(void)USB_EP0_OutStart(hpcd);

}

/**
  * @brief  Handle PCD enum done interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleEnumDoneIntr(USB_PCD_HandleTypeDef *hpcd)
{
	(void)USB_ActivateSetup(hpcd);

	if (USB_GetDevSpeed(hpcd) == USB_OTG_SPEED_HIGH) {
		hpcd->Init.speed = USBD_SPEED_HIGH;
	} else {
		hpcd->Init.speed = USBD_SPEED_FULL;
	}

	/* Set USB Turnaround time */
	(void)USB_SetTurnaroundTime(hpcd);

	USBD_SetSpeed(hpcd->pData, hpcd->Init.speed);

	/* Reset Device */
	USBD_Reset(hpcd->pData);
}

/**
  * @brief  Handle PCD suspend interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
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
USBD_TEXT_SECTION
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
		(void)USB_EP0_OutStart(hpcd);
	}

	return HAL_OK;
}

/**
  * @brief  process EP OUT transfer complete interrupt.
  * @param  hpcd : PCD handle
  * @param  epnum : endpoint number
  * @retval HAL status
  */
USBD_TEXT_SECTION
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

			(void)USB_EP0_OutStart(hpcd);
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
					(void)USB_EP0_OutStart(hpcd);
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
USBD_TEXT_SECTION
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

			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d 0x%08X\n", epnum, epint);

			if ((epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d XFRC\n", epnum);
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);
				(void)USB_PCD_HandleEPOutXfrCompleteIntr(hpcd, epnum);
			}

			if ((epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d STUP\n", epnum);
				/* Class B setup phase done for previous decoded setup */
				(void)USB_PCD_HandleEPOutSetupPacketIntr(hpcd, epnum);
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
			}

			if ((epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d DIS\n", epnum);
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);
			}

			/* Clear Status Phase Received interrupt */
			if ((epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d SPR\n", epnum);
				if (hpcd->Init.dma_enable == 1U) {
					(void)USB_EP0_OutStart(hpcd);
				}
				USB_PCD_CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
			}

			/* Clear OUT NAK interrupt */
			if ((epint & USB_OTG_DOEPINT_NAK) == USB_OTG_DOEPINT_NAK) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OUT EP%d NAK\n", epnum);
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
USBD_TEXT_SECTION
static void USB_PCD_HandleInEpDisableIntr(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
	UNUSED(hpcd);
	UNUSED(epnum);
}

/**
  * @brief  Write TxFIFO when EP TxFIFO emptry interrupt occurs
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval HAL status
  */
USBD_TEXT_SECTION
static HAL_Status USB_PCD_WriteEmptyTxFifo(USB_PCD_HandleTypeDef *hpcd, u32 epnum)
{
	USB_EPTypeDef *ep;
	u32 len;
	u32 len32b;

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

		(void)USB_WritePacket(hpcd, ep->xfer_buff, (u8)epnum, (u16)len);

		ep->xfer_buff  += len;
		ep->xfer_count += len;
	}

	if (ep->xfer_len <= ep->xfer_count) {
		// FIX
		//fifoemptymsk = (u32)(0x1UL << (epnum & EP_ADDR_MSK));
		//USB_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
		USB_GLOBAL->GINTMSK &= ~USB_OTG_GINTMSK_NPTXFEM;
	}

	return HAL_OK;
}

/**
  * @brief  Handle PCD IN EP interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleInEpIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 ep_intr;
	u32 epint;
	u32 epnum = 0U;

	/* Read in the device interrupt bits */
	ep_intr = USB_ReadDevAllInEpInterrupt(hpcd);
	while ((ep_intr != 0U) && (epnum < hpcd->Init.dev_endpoints)) {
		if ((ep_intr & 0x1U) != 0U) { /* In ITR */
			epint = USB_ReadDevInEPInterrupt(hpcd, (u8)epnum);
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d 0x%08X\n", epnum, epint);

			if ((epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d XFRC\n", epnum);

				USB_GLOBAL->GINTMSK &= ~USB_OTG_GINTMSK_NPTXFEM;

				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);

				if (hpcd->Init.dma_enable == 1U) {
					hpcd->IN_ep[epnum].xfer_buff += hpcd->IN_ep[epnum].maxpacket;
				}

				USBD_DataInStage(hpcd->pData, (u8)epnum, hpcd->IN_ep[epnum].xfer_buff);

				if (hpcd->Init.dma_enable == 1U) {
					/* this is ZLP, so prepare EP0 for next setup */
					if ((epnum == 0U) && (hpcd->IN_ep[epnum].xfer_len == 0U)) {
						/* prepare to rx more setup packets */
						(void)USB_EP0_OutStart(hpcd);
					}
				}
			}
			if ((epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d TOC\n", epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_TOC);
			}
			if ((epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d: ITTXFE\n", epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_ITTXFE);
			}
			if ((epint & USB_OTG_DIEPINT_INEPNM) == USB_OTG_DIEPINT_INEPNM) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d INEPNM\n", epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNM);
			}
			if ((epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d INEPNE\n", epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNE);
			}
			if ((epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d EPDISD\n", epnum);
				USB_PCD_HandleInEpDisableIntr(hpcd, epnum);
				USB_PCD_CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_EPDISD);
			}
			if ((epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE) {
				//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "IN EP%d TXFE\n", epnum);
				(void)USB_PCD_WriteEmptyTxFifo(hpcd, epnum);
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
USBD_TEXT_SECTION
static void USB_PCD_HandleRxFifoNonEmptyIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USB_EPTypeDef *ep;
	u32 reg;
	u32 xfer_cnt;

	USB_PCD_MASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);

	reg = USB_GLOBAL->GRXSTSP;
	xfer_cnt = (reg & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;

	ep = &hpcd->OUT_ep[reg & USB_OTG_GRXSTSP_EPNUM];

	if (((reg & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_DATA_UPDT) {
		if (xfer_cnt != 0U) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "RXFLVL EP%d STS_DATA_UPDT %d\n", ep->num, xfer_cnt);
			(void)USB_ReadPacket(hpcd, ep->xfer_buff, (u16)(xfer_cnt & 0xFFFF));

			ep->xfer_buff += xfer_cnt;
			ep->xfer_count += xfer_cnt;
		} else {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "RXFLVL EP%d STS_DATA_UPDT ZLP\n", ep->num);
		}
	} else if (((reg & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_SETUP_UPDT) {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "RXFLVL EP%d STS_SETUP_UPDT %d\n", ep->num, xfer_cnt);
		(void)USB_ReadPacket(hpcd, (u8 *)hpcd->Setup, 8U);
		ep->xfer_count += xfer_cnt;
	} else {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "RXFLVL EP%d %d\n", ep->num, xfer_cnt);
	}
	USB_PCD_UNMASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);
}

/**
  * @brief  Get IN EP number sequence as per IN Token learn queue
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
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
	u8 intkn_seq[USBD_IN_TOKEN_QUEUE_DEPTH];
	u8 seq[USBD_MAX_ENDPOINTS];
	u8 temp;

	dtknqr1 = USB_DEVICE->DTKNQR1;
	dtknqr2 = USB_DEVICE->DTKNQR2;

	USB_GLOBAL->GRSTCTL |= USB_OTG_GRSTCTL_INTKNQFLSH;

	if ((dtknqr1 & USB_OTG_DTKNQR1_WRAPBIT) != 0) {
		start = (int)(dtknqr1 & USB_OTG_DTKNQR1_INTKNWPTR_Msk);
		end = start - 1;
		if (end < 0) {
			end = USBD_IN_TOKEN_QUEUE_DEPTH - 1;
		}
		cnt = USBD_IN_TOKEN_QUEUE_DEPTH;
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

	for (i = 6; i < USBD_IN_TOKEN_QUEUE_DEPTH; i++) {
		intkn_seq[i] = (dtknqr2 >> ((i - 6) * 4)) & 0xF;
	}

	i = 0;
	idx = start;
	do {
		seq[intkn_seq[idx]] = i;
		idx++;
		i++;
		if (idx == USBD_IN_TOKEN_QUEUE_DEPTH) {
			idx = 0;
		}
	} while (i < cnt);

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

	for (i = 0; i <= hpcd->Init.dev_endpoints; i++) {
		idx = start + i;
		if (seq[i] < 31) {
			idx = start + seq[i];
			if (idx >= USBD_IN_TOKEN_QUEUE_DEPTH) {
				idx = idx % USBD_IN_TOKEN_QUEUE_DEPTH;
			}
			hpcd->in_ep_sequece[i] = intkn_seq[idx];
		} else {
			hpcd->in_ep_sequece[i] = 0xFF;
		}
	}
}

/**
  * @brief  Handle PCD EP non-periodical TxFIFO empty interrupt.
  * @param  hpcd: PCD handle
  * @param  epnum endpoint number
  * @retval void
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleEpNPTxFifoEmptyIntr(USB_PCD_HandleTypeDef *hpcd, u8 epnum)
{
	USB_EPTypeDef *ep;
	u32 len;
	u32 reg;

	ep = &hpcd->IN_ep[epnum];

	if ((ep->xfer_len == 0) && (ep->is_zlp == 0)) {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "NPTxFEmp: token received for EP%d when TxFIFO is not ready\n", epnum);
		return;
	}

	if ((ep->xfer_count > ep->xfer_len) || ((ep->xfer_len > 0) && (ep->xfer_buff == NULL))) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "NPTxFEmp: invalid parameter\n");
		return;
	}

	len = ep->xfer_len - ep->xfer_count;
	if (len > ep->maxpacket) {
		len = ep->maxpacket;
	}

	//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "NPTxFEmp EP%d xfer_len=%d xfer_count=%d len=%d\n", epnum, ep->xfer_len, ep->xfer_count, len);

	/* While there is space in the queue and space in the FIFO and
	 * More data to tranfer, Write packets to the Tx FIFO */
	reg = USB_GLOBAL->GNPTXSTS;

	while (((reg & USB_OTG_GNPTXSTS_NPTQXSAV_Msk) > 0)
		   && ((reg & USB_OTG_GNPTXSTS_NPTXFSAV_Msk) > ((len + 3U) / 4U))
		   && (ep->xfer_count < ep->xfer_len)) {
		USB_WritePacket(hpcd, ep->xfer_buff, ep->num, (u16)len);
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
USBD_TEXT_SECTION
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
			} else {
				DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Invalid EP num %d in IN token learn queue\n", epnum);
			}
		}
	}
}

/**
  * @brief  Handle PCD EP mismatch interrupt.
  * @param  hpcd: PCD handle
  * @retval void
  */
USBD_TEXT_SECTION
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
USBD_TEXT_SECTION
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
USBD_TEXT_SECTION
static void USB_PCD_HandleSofIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USBD_SOF(hpcd->pData);
}

/**
  * @brief  Handle SRQ interrupt
  * @param  hpcd PCD handle
  * @retval None
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleSrqIntr(USB_PCD_HandleTypeDef *hpcd)
{
	USBD_DevConnected(hpcd->pData);
}

/**
  * @brief  Handle OTG interrupt
  * @param  hpcd PCD handle
  * @retval None
  */
USBD_TEXT_SECTION
static void USB_PCD_HandleOtgIntr(USB_PCD_HandleTypeDef *hpcd)
{
	u32 reg = USB_GLOBAL->GOTGINT;

	if ((reg & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET) {
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "GOTGINT_SEDET\n");
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_Init(USB_PCD_HandleTypeDef *hpcd)
{
	u8 i;

	/* Check the PCD handle allocation */
	if (hpcd == NULL) {
		return HAL_ERR_PARA;
	}

	if (hpcd->State == HAL_PCD_STATE_RESET) {
		/* Allocate lock resource and initialize it */
		hpcd->Lock = USB_HAL_UNLOCKED;

		/* Init the low level hardware : GPIO, CLOCK, NVIC... */
		if (USB_PCD_MspInit(hpcd) != HAL_OK) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "USB power on fail\n");
			hpcd->State = HAL_PCD_STATE_ERROR;
			return HAL_ERR_HW;
		}
	}

	hpcd->State = HAL_PCD_STATE_BUSY;
	hpcd->PHY_Initialized = 0;
	hpcd->ISR_Installed = 0;

	/* Disable the Interrupts */

	USB_DisableGlobalInt();

	/*Init the Core (common init.) */
	if (USB_HalInit(hpcd) != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Core init fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}

	/* Force Device Mode*/
	(void)USB_SetDeviceMode(hpcd);

	/* Init endpoints structures */
	for (i = 0U; i < hpcd->Init.dev_endpoints; i++) {
		/* Init ep structure */
		hpcd->IN_ep[i].is_in = 1U;
		hpcd->IN_ep[i].num = i;
		hpcd->IN_ep[i].tx_fifo_num = i;
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
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Device init fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}

	hpcd->USB_Address = 0U;
	hpcd->State = HAL_PCD_STATE_READY;
	(void)USB_DevDisconnect(hpcd);

	if (USB_PCD_PhyCalibration() != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "PHY calibration fail\n");
		hpcd->State = HAL_PCD_STATE_ERROR;
		return HAL_ERR_HW;
	}

	USB_PCD_InitInterrupt(hpcd);

	return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
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
	USB_PCD_MspDeInit(hpcd);
#endif

	hpcd->State = HAL_PCD_STATE_RESET;

	return result;
}

/**
  * @brief  Start the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_Start(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	(void)USB_DevConnect(hpcd);
	USB_EnableGlobalInt();
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Stop the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_Stop(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);

	USB_DisableGlobalInt();

	if (USB_StopDevice(hpcd) != HAL_OK) {
		USB_HAL_UNLOCK(hpcd);
		return HAL_ERR_HW;
	}

	(void)USB_DevDisconnect(hpcd);
	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Connect the USB device
  * @param  hpcd PCD handle
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_DevConnect(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	(void)USB_DevConnect(hpcd);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Disconnect the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_DevDisconnect(USB_PCD_HandleTypeDef *hpcd)
{
	USB_HAL_LOCK(hpcd);
	(void)USB_DevDisconnect(hpcd);
	USB_HAL_UNLOCK(hpcd);
	return HAL_OK;
}

/**
  * @brief  Set the USB Device address.
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_SetAddress(USB_PCD_HandleTypeDef *hpcd, u8 address)
{
	USB_HAL_LOCK(hpcd);
	hpcd->USB_Address = address;
	(void)USB_SetDevAddress(hpcd, address);
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
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_EP_Open(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u16 ep_mps, u8 ep_type)
{
	HAL_Status  ret = HAL_OK;
	USB_EPTypeDef *ep;

	if ((ep_addr & 0x80U) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
	} else {
		ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 0U;
	}

	ep->num = ep_addr & EP_ADDR_MSK;
	ep->maxpacket = ep_mps;
	ep->type = ep_type;

	if (ep->is_in != 0U) {
		/* Assign a Tx FIFO */
		// FIXME
		//ep->tx_fifo_num = ep->num;
		if (ep_type == EP_TYPE_INTR) {
			ep->tx_fifo_num = 1;
		} else {
			ep->tx_fifo_num = 0;
		}
	}
	/* Set initial data PID. */
	if (ep_type == EP_TYPE_BULK) {
		ep->data_pid_start = 0U;
	}

	USB_HAL_LOCK(hpcd);
	(void)USB_ActivateEndpoint(hpcd, ep);
	USB_HAL_UNLOCK(hpcd);

	return ret;
}

/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_EP_Close(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_EPTypeDef *ep;

	if ((ep_addr & 0x80U) == 0x80U) {
		ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 1U;
	} else {
		ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
		ep->is_in = 0U;
	}
	ep->num   = ep_addr & EP_ADDR_MSK;

	USB_HAL_LOCK(hpcd);
	(void)USB_DeactivateEndpoint(hpcd, ep);
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
USBD_WEEK USBD_TEXT_SECTION
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
		(void)USB_EP0StartXfer(hpcd, ep);
	} else {
		(void)USB_EPStartXfer(hpcd, ep);
	}

	return HAL_OK;
}

/**
  * @brief  Get Received Data Size
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval Data Size
  */
USBD_WEEK USBD_TEXT_SECTION
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
USBD_WEEK USBD_TEXT_SECTION
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
		(void)USB_EP0StartXfer(hpcd, ep);
	} else {
		(void)USB_EPStartXfer(hpcd, ep);
	}

	return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
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

	(void)USB_EPSetStall(hpcd, ep);
	if ((ep_addr & EP_ADDR_MSK) == 0U) {
		(void)USB_EP0_OutStart(hpcd);
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
USBD_WEEK USBD_TEXT_SECTION
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
	(void)USB_EPClearStall(hpcd, ep);
	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
HAL_Status USB_PCD_EP_Flush(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr)
{
	USB_HAL_LOCK(hpcd);

	if ((ep_addr & 0x80U) == 0x80U) {
		(void)USB_FlushTxFifo(hpcd, (u32)ep_addr & EP_ADDR_MSK);
	} else {
		(void)USB_FlushRxFifo(hpcd);
	}

	USB_HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Return the PCD handle state.
  * @param  hpcd PCD handle
  * @retval HAL state
  */
USBD_WEEK USBD_TEXT_SECTION
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
USBD_WEEK USBD_TEXT_SECTION
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
		USB_GLOBAL->DIEPTXF0_HNPTXFSIZ = ((u32)size << 16) | Tx_Offset;
	} else {
		Tx_Offset += (USB_GLOBAL->DIEPTXF0_HNPTXFSIZ) >> 16;
		for (i = 0U; i < (fifo - 1U); i++) {
			Tx_Offset += (USB_GLOBAL->DIEPTXF[i] >> 16);
		}

		/* Multiply Tx_Size by 2 to get higher performance */
		USB_GLOBAL->DIEPTXF[fifo - 1U] = ((u32)size << 16) | Tx_Offset;
	}

	return HAL_OK;
}

/**
  * @brief  Set Rx FIFO
  * @param  hpcd PCD handle
  * @param  size Size of Rx fifo
  * @retval HAL status
  */
USBD_WEEK USBD_TEXT_SECTION
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
USBD_WEEK USBD_TEXT_SECTION
void USB_PCD_IRQHandler(USB_PCD_HandleTypeDef *hpcd)
{
	/* ensure that we are in device mode */
	if (USB_GetMode(hpcd) == USB_OTG_MODE_DEVICE) {
		/* avoid spurious interrupt */
		if (USB_ReadInterrupts() == 0U) {
			return;
		}

		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "====== IRQ 0x%08X =======\n", USB_ReadInterrupts());

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_MMIS)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "MMIS\n");
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
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "WKUINT\n");
			USB_PCD_HandleWakeupIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_WKUINT);
		}

		/* Handle Suspend Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_USBSUSP)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "USBSUSP\n");
			USB_PCD_HandleSuspendIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_USBSUSP);
		}
		/* Handle Reset Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_USBRST)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "USBRST\n");
			USB_PCD_HandleResetIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_USBRST);
		}

		/* Handle Enumeration done Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_ENUMDNE)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "ENUMDNE\n");
			USB_PCD_HandleEnumDoneIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_ENUMDNE);
		}

		/* Handle RxQLevel Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_RXFLVL)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "RXFLVL\n");
			USB_PCD_HandleRxFifoNonEmptyIntr(hpcd);
		}

		/* Handle NP TxFIFO Empty Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_NPTXFE)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "NPTXFE\n");
			USB_PCD_HandleNPTxFifoEmptyIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_NPTXFE);
		}

		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_EPMIS)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "EPMIS\n");
			USB_PCD_HandleEpMismatchIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_EPMIS);
		}

		/* Handle SOF Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_SOF)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "SOF\n");
			USB_PCD_HandleSofIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_SOF);
		}

		/* Handle Connection event Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_SRQINT)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "SRQINT\n");
			USB_PCD_HandleSrqIntr(hpcd);
			USB_PCD_CLEAR_FLAG(USB_OTG_GINTSTS_SRQINT);
		}

		/* Handle Disconnection event Interrupt */
		if (USB_PCD_GET_FLAG(USB_OTG_GINTSTS_OTGINT)) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "OTGINT\n");
			USB_PCD_HandleOtgIntr(hpcd);
		}
	}
}

