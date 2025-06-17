/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"
#include "usbh_hal.h"

#if (defined(UVC_USE_HW) && (UVC_USE_HW == 1))

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UVC";

static usbh_uvc_dec uvc_dec_struct[UVC_TOTAL_CHANNEL_NUM];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get buffer number through address
  * @param  dec: pointer to usbh_uvc_dec structure
  * @param  addr: buffer address.
  * @retval None
  */
static u32 usbh_uvc_get_number_from_addr(usbh_uvc_dec *dec, u32 addr)
{
	u32 i;
	for (i = 0; i < USBH_MAX_BUF_NUM; i++) {
		if (addr == dec->uvc_dec_buf[i].buf_start_addr) {
			return i;
		}
	}

	return 0;
}

/**
  * @brief  UVC decoder interrupt handler
  * @param  Data: delivered data to handler.
  * @retval None
  */
static u32 usbh_uvc_dec_irq(u32 *Data)
{
	UNUSED(Data);
	u32 irq_status = UVC_GetIntStatus();
	u32 start_addr, end_addr;

	//RTK_LOGI(TAG, "uvc irq:%x\n", irq_status);

	if (irq_status & UVC_BIT_CH0_BUF0_FRM_DONE) {
		UVC_INTClear(UVC_BIT_CH0_BUF0_FRM_DONE);
		uvc_dec_struct[0].frame_done_num = usbh_uvc_get_number_from_addr(&uvc_dec_struct[0], UVC_GetBufferAddr(0, 0));
		uvc_dec_struct[0].frame_done_size = UVC_GetFrameSize(0, 0);
		start_addr = uvc_dec_struct[0].uvc_dec_buf[uvc_dec_struct[0].free_buf_num].buf_start_addr;
		end_addr = start_addr + uvc_dec_struct[0].uvc_dec_buf[uvc_dec_struct[0].free_buf_num].buf_size - 1;
		UVC_SetBufferAddr(0, 0, start_addr, end_addr);
		if (++uvc_dec_struct[0].free_buf_num > 2) {
			uvc_dec_struct[0].free_buf_num = 0;
		}
		rtos_sema_give(uvc_dec_struct[0].dec_sema);
	}

	if (irq_status & UVC_BIT_CH0_BUF1_FRM_DONE) {
		UVC_INTClear(UVC_BIT_CH0_BUF1_FRM_DONE);
		uvc_dec_struct[0].frame_done_num = usbh_uvc_get_number_from_addr(&uvc_dec_struct[0], UVC_GetBufferAddr(0, 1));
		uvc_dec_struct[0].frame_done_size = UVC_GetFrameSize(0, 1);
		start_addr = uvc_dec_struct[0].uvc_dec_buf[uvc_dec_struct[0].free_buf_num].buf_start_addr;
		end_addr = start_addr + uvc_dec_struct[0].uvc_dec_buf[uvc_dec_struct[0].free_buf_num].buf_size - 1;
		UVC_SetBufferAddr(0, 1, start_addr, end_addr);
		if (++uvc_dec_struct[0].free_buf_num > 2) {
			uvc_dec_struct[0].free_buf_num = 0;
		}
		rtos_sema_give(uvc_dec_struct[0].dec_sema);
	}

	if (irq_status & UVC_BIT_CH1_BUF0_FRM_DONE) {
		UVC_INTClear(UVC_BIT_CH1_BUF0_FRM_DONE);
		uvc_dec_struct[1].frame_done_num = usbh_uvc_get_number_from_addr(&uvc_dec_struct[1], UVC_GetBufferAddr(1, 0));
		uvc_dec_struct[1].frame_done_size = UVC_GetFrameSize(1, 0);
		start_addr = uvc_dec_struct[1].uvc_dec_buf[uvc_dec_struct[1].free_buf_num].buf_start_addr;
		end_addr = start_addr + uvc_dec_struct[1].uvc_dec_buf[uvc_dec_struct[1].free_buf_num].buf_size - 1;
		UVC_SetBufferAddr(1, 0, start_addr, end_addr);
		if (++uvc_dec_struct[1].free_buf_num > 2) {
			uvc_dec_struct[1].free_buf_num = 0;
		}
		rtos_sema_give(uvc_dec_struct[1].dec_sema);
	}

	if (irq_status & UVC_BIT_CH1_BUF1_FRM_DONE) {
		UVC_INTClear(UVC_BIT_CH1_BUF1_FRM_DONE);
		uvc_dec_struct[1].frame_done_num = usbh_uvc_get_number_from_addr(&uvc_dec_struct[1], UVC_GetBufferAddr(1, 1));
		uvc_dec_struct[1].frame_done_size = UVC_GetFrameSize(1, 1);
		start_addr = uvc_dec_struct[1].uvc_dec_buf[uvc_dec_struct[1].free_buf_num].buf_start_addr;
		end_addr = start_addr + uvc_dec_struct[1].uvc_dec_buf[uvc_dec_struct[1].free_buf_num].buf_size - 1;
		UVC_SetBufferAddr(1, 1, start_addr, end_addr);
		if (++uvc_dec_struct[1].free_buf_num > 2) {
			uvc_dec_struct[1].free_buf_num = 0;
		}
		rtos_sema_give(uvc_dec_struct[1].dec_sema);
	}

	if (irq_status & UVC_BIT_CH0_BUF0_FRM_OVERSIZE) {
		UVC_INTClear(UVC_BIT_CH0_BUF0_FRM_OVERSIZE);
		RTK_LOGE(TAG, "CH0 BUF0 frame oversize error\n");
	}

	if (irq_status & UVC_BIT_CH0_BUF1_FRM_OVERSIZE) {
		UVC_INTClear(UVC_BIT_CH0_BUF1_FRM_OVERSIZE);
		RTK_LOGE(TAG, "CH0 BUF1 frame oversize error\n");
	}

	if (irq_status & UVC_BIT_CH0_BUF0_HEADER_ERR) {
		UVC_INTClear(UVC_BIT_CH0_BUF0_HEADER_ERR);
		RTK_LOGE(TAG, "CH0 BUF0 payload header error\n");
	}

	if (irq_status & UVC_BIT_CH0_BUF1_HEADER_ERR) {
		UVC_INTClear(UVC_BIT_CH0_BUF1_HEADER_ERR);
		RTK_LOGE(TAG, "CH0 BUF1 payload header error\n");
	}

	if (irq_status & UVC_BIT_CH1_BUF0_FRM_OVERSIZE) {
		UVC_INTClear(UVC_BIT_CH1_BUF0_FRM_OVERSIZE);
		RTK_LOGE(TAG, "CH1 BU1 frame oversize error\n");
	}

	if (irq_status & UVC_BIT_CH1_BUF1_FRM_OVERSIZE) {
		UVC_INTClear(UVC_BIT_CH1_BUF1_FRM_OVERSIZE);
		RTK_LOGE(TAG, "CH1 BUF1 frame oversize error\n");
	}

	if (irq_status & UVC_BIT_CH1_BUF0_HEADER_ERR) {
		UVC_INTClear(UVC_BIT_CH1_BUF0_HEADER_ERR);
		RTK_LOGE(TAG, "CH1 BUF0 payload header error\n");
	}

	if (irq_status & UVC_BIT_CH1_BUF1_HEADER_ERR) {
		UVC_INTClear(UVC_BIT_CH1_BUF1_HEADER_ERR);
		RTK_LOGE(TAG, "CH1 BUF1 payload header error\n");
	}

	return 0;
}


/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Alloc UVC channel structure
  * @param  None.
  * @retval Pointer to usbh_uvc_dec structure
  */
usbh_uvc_dec *usbh_uvc_dec_alloc_channel(void)
{
	u32 ch = UVC_AllocChannel();
	if (ch == 0xFF) {
		RTK_LOGE(TAG, "No Free UVC decoder channel\n");
		return NULL;
	}

	uvc_dec_struct[ch].ch = ch;
	return &uvc_dec_struct[ch];
}

/**
  * @brief  Free UVC channel
  * @param  uvc_dec: pointer to usbh_uvc_dec structure
  * @retval None
  */
void usbh_uvc_dec_free_channel(usbh_uvc_dec *uvc_dec)
{
	UVC_FreeChannel(uvc_dec->ch);
}

/**
  * @brief  Init UVC decoder
  * @param  uvc_dec: pointer to usbh_uvc_dec structure
  * @retval None
  */
void usbh_uvc_dec_init(usbh_uvc_dec *uvc_dec)
{
	UVC_InitStruct uvc_initstruct;
	u32 mc;
	u32 num_packets;
	u32 hcsize = 0;
	u32 hcchar = 0;
	u32 data_pid;

	if ((uvc_dec->uvc_dec_buf[0].buf_size <= 1024) || (uvc_dec->uvc_dec_buf[1].buf_size <= 1024)) {
		RTK_LOGE(TAG, "Buffer size should > 1024 bytes\n");
		return;
	}

	uvc_dec->free_buf_num = 2;

	UVC_StructInit(&uvc_initstruct);
	uvc_initstruct.chn_num = uvc_dec->ch;
	uvc_initstruct.frame_buf0_start_addr = uvc_dec->uvc_dec_buf[0].buf_start_addr;
	uvc_initstruct.frame_buf0_end_addr = uvc_initstruct.frame_buf0_start_addr + uvc_dec->uvc_dec_buf[0].buf_size - 1 - 1024;
	uvc_initstruct.frame_buf1_start_addr = uvc_dec->uvc_dec_buf[1].buf_start_addr;
	uvc_initstruct.frame_buf1_end_addr = uvc_initstruct.frame_buf1_start_addr + uvc_dec->uvc_dec_buf[1].buf_size - 1 - 1024;
	uvc_initstruct.ep_num = uvc_dec->pipe_num;
	uvc_initstruct.binterval = uvc_dec->binterval;

	/* Do not use eof as frame done flag default */
	uvc_initstruct.eof = 0;

	/* uvc cmd 0 */
	uvc_initstruct.uvc_cmd[0].cmd_reg0.enable = 1;
	uvc_initstruct.uvc_cmd[0].cmd_reg0.type = 0;
	uvc_initstruct.uvc_cmd[0].cmd_reg0.rsvd =  0;
	uvc_initstruct.uvc_cmd[0].cmd_reg0.offset = 0x514 + uvc_initstruct.ep_num * 0x20; 	/* HCDMA offset */
	uvc_initstruct.uvc_cmd[0].data = 0x20060000;// uvc_initstruct.frame_buf0_start_addr; 	/* HCDMA value */
	uvc_initstruct.hcdmai = uvc_initstruct.uvc_cmd[0].data;

	/* uvc cmd 1 */
	num_packets = uvc_dec->ep_size / uvc_dec->mps;
	if (num_packets == 1) {
		data_pid = USBH_PID_DATA0;
	} else if (num_packets == 2) {
		data_pid = USBH_PID_DATA1;
	} else {
		data_pid = USBH_PID_DATA2;
	}
	hcsize = (uvc_dec->ep_size & USB_OTG_HCTSIZ_XFRSIZ) |
			 (((u32)num_packets << USB_OTG_HCTSIZ_PKTCNT_Pos) & USB_OTG_HCTSIZ_PKTCNT) |
			 (((u32)data_pid << USB_OTG_HCTSIZ_DPID_Pos) & USB_OTG_HCTSIZ_DPID);
	uvc_initstruct.uvc_cmd[1].cmd_reg0.enable = 1;
	uvc_initstruct.uvc_cmd[1].cmd_reg0.type = 0;
	uvc_initstruct.uvc_cmd[1].cmd_reg0.rsvd =  0;
	uvc_initstruct.uvc_cmd[1].cmd_reg0.offset = 0x510 + uvc_initstruct.ep_num * 0x20; 	/* HCSIZ offset */
	uvc_initstruct.uvc_cmd[1].data = hcsize; 	/* HCSIZ value */

	/* uvc cmd 2 */
	mc = uvc_dec->ep_size / uvc_dec->mps;
	hcchar &= ~USB_OTG_HCCHAR_MC;
	hcchar |= (u32)mc << USB_OTG_HCCHAR_MC_Pos;
	hcchar &= ~USB_OTG_HCCHAR_CHDIS;
	hcchar |= USB_OTG_HCCHAR_EPDIR;
	hcchar &= ~USB_OTG_HCCHAR_MPSIZ_Msk;
	hcchar |=  uvc_dec->mps << USB_OTG_HCCHAR_MPSIZ_Pos;
	hcchar &= ~USB_OTG_HCCHAR_EPNUM_Msk;
	hcchar |= uvc_dec->ep_num << USB_OTG_HCCHAR_EPNUM_Pos;
	hcchar &= ~USB_OTG_HCCHAR_EPTYP_Msk;
	hcchar |= 0x1 << USB_OTG_HCCHAR_EPTYP_Pos;
	hcchar |= USB_OTG_HCCHAR_CHENA;
	hcchar &= ~USB_OTG_HCCHAR_DAD_Msk;
	hcchar |= (u32)uvc_dec->dev_addr << USB_OTG_HCCHAR_DAD_Pos;
	uvc_initstruct.uvc_cmd[2].cmd_reg0.enable = 1;
	uvc_initstruct.uvc_cmd[2].cmd_reg0.type = 2;
	uvc_initstruct.uvc_cmd[2].cmd_reg0.rsvd =  0;
	uvc_initstruct.uvc_cmd[2].cmd_reg0.offset =  0x500 + uvc_initstruct.ep_num * 0x20; 	/* HCCHAR offset */
	uvc_initstruct.uvc_cmd[2].data = hcchar; 	/* HCCHAR value */

	/* Disable hc interrupt */
	USB_HOST->HAINTMSK &= ~(1UL << (uvc_dec->pipe_num & 0xFU));
	USB_HC(uvc_dec->pipe_num)->HCINTMSK &= ~USB_OTG_HCINTMSK_XFRCM;

	/* csr_sysdone_lasttrans, csr_sys_dmadone_mode */
	USB_GLOBAL->GAHBCFG |=  BIT(21) | BIT(22);

	/* Init channel */
	UVC_ChnlInit(&uvc_initstruct);

	/* register interrupt */
	InterruptRegister((IRQ_FUN) usbh_uvc_dec_irq, UVC_DEC_IRQ, (u32)NULL, UVC_IRQ_PRIORITY);
	InterruptEn(UVC_DEC_IRQ, UVC_IRQ_PRIORITY);

	if (uvc_dec->ch == 0) {
		UVC_INTClear(UVC_BIT_CH0_BUF0_FRM_DONE | UVC_BIT_CH0_BUF1_FRM_DONE |
					 UVC_BIT_CH0_BUF0_FRM_OVERSIZE | UVC_BIT_CH0_BUF1_FRM_OVERSIZE |
					 UVC_BIT_CH0_BUF0_HEADER_ERR | UVC_BIT_CH0_BUF1_HEADER_ERR);
		UVC_INTConfig(UVC_BIT_CH0_BUF0_FRM_DONE_MASK | UVC_BIT_CH0_BUF1_FRM_DONE_MASK |
					  UVC_BIT_CH0_BUF0_FRM_OVERSIZE_MASK | UVC_BIT_CH0_BUF1_FRM_OVERSIZE_MASK |
					  UVC_BIT_CH0_BUF0_HEADER_ERR_MASK | UVC_BIT_CH0_BUF1_HEADER_ERR_MASK, ENABLE);
	} else {
		UVC_INTClear(UVC_BIT_CH1_BUF0_FRM_DONE | UVC_BIT_CH1_BUF1_FRM_DONE |
					 UVC_BIT_CH1_BUF0_FRM_OVERSIZE | UVC_BIT_CH1_BUF1_FRM_OVERSIZE |
					 UVC_BIT_CH1_BUF0_HEADER_ERR | UVC_BIT_CH1_BUF1_HEADER_ERR);
		UVC_INTConfig(UVC_BIT_CH1_BUF0_FRM_DONE_MASK | UVC_BIT_CH1_BUF1_FRM_DONE_MASK |
					  UVC_BIT_CH1_BUF0_FRM_OVERSIZE_MASK | UVC_BIT_CH1_BUF1_FRM_OVERSIZE_MASK |
					  UVC_BIT_CH1_BUF0_HEADER_ERR_MASK | UVC_BIT_CH1_BUF1_HEADER_ERR_MASK, ENABLE);
	}
}

/**
  * @brief  Deinit UVC decoder
  * @param  uvc_dec: pointer to usbh_uvc_dec structure
  * @retval None
  */
void usbh_uvc_dec_deinit(usbh_uvc_dec *uvc_dec)
{
	if (uvc_dec->ch == 0) {
		UVC_INTConfig(UVC_BIT_CH0_BUF0_FRM_DONE_MASK | UVC_BIT_CH0_BUF1_FRM_DONE_MASK | \
					  UVC_BIT_CH0_BUF0_FRM_OVERSIZE_MASK | UVC_BIT_CH0_BUF1_FRM_OVERSIZE_MASK | \
					  UVC_BIT_CH0_BUF0_HEADER_ERR_MASK | UVC_BIT_CH0_BUF1_HEADER_ERR_MASK, DISABLE);
	} else {
		UVC_INTConfig(UVC_BIT_CH1_BUF0_FRM_DONE_MASK | UVC_BIT_CH1_BUF1_FRM_DONE_MASK | \
					  UVC_BIT_CH1_BUF0_FRM_OVERSIZE_MASK | UVC_BIT_CH1_BUF1_FRM_OVERSIZE_MASK | \
					  UVC_BIT_CH1_BUF0_HEADER_ERR_MASK | UVC_BIT_CH1_BUF1_HEADER_ERR_MASK, DISABLE);
	}
}

/**
  * @brief  Start UVC decoder
  * @param  uvc_dec: pointer to usbh_uvc_dec structure
  * @retval None
  */
void usbh_uvc_dec_start(usbh_uvc_dec *uvc_dec)
{
	UVC_ConcatEn(uvc_dec->ch, ENABLE);
	UVC_EPAutoEn(uvc_dec->ch, ENABLE);
}

/**
  * @brief  Stop UVC decoder
  * @param  uvc_dec: pointer to usbh_uvc_dec structure
  * @retval None
  */
void usbh_uvc_dec_stop(usbh_uvc_dec *uvc_dec)
{
	UVC_EPAutoEn(uvc_dec->ch, DISABLE);
	/* After disable epautoen， maybe last frame is not processed, so need a delay to disable concat */
	DelayMs(1);
	UVC_ConcatEn(uvc_dec->ch, DISABLE);
}

#endif
