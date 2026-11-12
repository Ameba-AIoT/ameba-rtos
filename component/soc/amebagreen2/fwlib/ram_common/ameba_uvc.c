/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UVC
* @brief UVC driver modules
* @{
*/

/* UVC_INTR: [31:16] R/W interrupt masks, [15:0] RW1C interrupt status */
#define UVC_INTR_MASK_BITS		0xFFFF0000U
#define UVC_INTR_STATUS_BITS	0x0000FFFFU

/* Bitmap of allocated UVC channels */
static u32 uvc_chn_map = 0;

/**
  * @brief  Use EOF or FID as frame done flag.
  * @param  uvc_chn: Channel number.
  *         eof: 1: frame done when xfercomp at EOF flag; 0: frame done when frame id changes.
  * @retval None.
  */
static void UVC_SetEOF(u32 uvc_chn, u32 eof)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);

	if (uvc_chn == 0) {
		UVC->UVC_CTRL &= ~UVC_BIT_CH0_FRM_DONE_OPT;
		UVC->UVC_CTRL |= UVC_CH0_FRM_DONE_OPT(eof);
	} else {
		UVC->UVC_CTRL &= ~UVC_BIT_CH1_FRM_DONE_OPT;
		UVC->UVC_CTRL |= UVC_CH1_FRM_DONE_OPT(eof);
	}
}

/**
  * @brief  Set HCDMAi for specific channel.
  * @param  uvc_chn: Channel number.
  *         value: HCDMAi value.
  * @retval None.
  */
static void UVC_SetHCDMAi(u32 uvc_chn, u32 value)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);

	if (uvc_chn == 0) {
		UVC->UVC_CHNL0_CTRL = value;
	} else {
		UVC->UVC_CHNL1_CTRL = value;
	}
}

/* Exported functions --------------------------------------------------------*/
/** @defgroup UVC_Exported_Functions UVC Exported Functions
  * @{
  */


/**
  * @brief  Alloc UVC Channel.
  * @param  None.
  * @retval UVC Channel number, 0xFF if no channel is available.
  */
u32 UVC_AllocChannel(void)
{
	u32 i;
	u32 uvc_chn = 0xFF;

	for (i = 0; i < UVC_TOTAL_CHANNEL_NUM; i++) {
		if ((uvc_chn_map & BIT(i)) == 0) {
			uvc_chn_map |= BIT(i);
			uvc_chn = i;
			break;
		}
	}

	return uvc_chn;
}

/**
  * @brief  Free UVC Channel.
  * @param  uvc_chn: UVC Channel number, invalid channel number is ignored.
  * @retval None.
  */
void UVC_FreeChannel(u32 uvc_chn)
{
	if (uvc_chn >= UVC_TOTAL_CHANNEL_NUM) {
		return;
	}

	uvc_chn_map &= (~BIT(uvc_chn));
}


/**
  * @brief  Enable/disable specific interrupt.
  * @param  uvc_intr: specifies the UVC interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg UVC_BIT_CHx_BUFy_FRM_DONE_MASK: UVC channel x buffer y frame done interrupt mask.
  *            @arg UVC_BIT_CHx_BUFy_FRM_OVERSIZE_MASK: UVC channel x buffer y frame oversize interrupt mask.
  *            @arg UVC_BIT_CHx_BUFy_HEADER_ERR_MASK: UVC channel x buffer y header error interrupt mask.
  *          where x can be 0 or 1, y can be 0 or 1.
  *         new_state: the new state.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void UVC_INTConfig(u32 uvc_intr, u32 new_state)
{
	/* Write 0 to the RW1C status bits, so that pending interrupts are not cleared */
	u32 reg = UVC->UVC_INTR & UVC_INTR_MASK_BITS;

	if (new_state == ENABLE) {
		reg |= (uvc_intr & UVC_INTR_MASK_BITS);
	} else {
		reg &= ~uvc_intr;
	}
	UVC->UVC_INTR = reg;
}

/**
  * @brief  Clear specific interrupt.
  * @param  uvc_intr: specifies the UVC interrupt type.
  *          This parameter can be one or combinations of the following values:
  *            @arg UVC_BIT_CHx_BUFy_FRM_DONE: UVC channel x buffer y frame done interrupt.
  *            @arg UVC_BIT_CHx_BUFy_FRM_OVERSIZE: UVC channel x buffer y frame oversize interrupt.
  *            @arg UVC_BIT_CHx_BUFy_HEADER_ERR: UVC channel x buffer y header error interrupt.
  *          where x can be 0 or 1, y can be 0 or 1.
  * @retval None.
  */
void UVC_INTClear(u32 uvc_intr)
{
	/* Keep the masks, write 1 only to the RW1C status bits to be cleared */
	UVC->UVC_INTR = (UVC->UVC_INTR & UVC_INTR_MASK_BITS) | (uvc_intr & UVC_INTR_STATUS_BITS);
}

/**
  * @brief  Get interrupt status.
  * @param  None
  * @retval Interrupt status.
  */
u32 UVC_GetIntStatus(void)
{
	return UVC->UVC_INTR & UVC_INTR_STATUS_BITS;
}


/**
  * @brief  Set EP number for specific channel.
  * @param  uvc_chn: Channel number.
  *         ep_num: EP number
  * @retval None.
  */
void UVC_SetEPNum(u32 uvc_chn, u32 ep_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);

	if (uvc_chn == 0) {
		UVC->UVC_CTRL &= ~UVC_MASK_CH0_EP_NUM;
		UVC->UVC_CTRL |= UVC_CH0_EP_NUM(ep_num);
	} else {
		UVC->UVC_CTRL &= ~UVC_MASK_CH1_EP_NUM;
		UVC->UVC_CTRL |= UVC_CH1_EP_NUM(ep_num);
	}
}

/**
  * @brief  Set buffer start and end address for specific channel and buffer number.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  *         start_addr: buffer start address.
  *         end_addr: buffer end address.
  * @retval None.
  */
void UVC_SetBufferAddr(u32 uvc_chn, u32 buf_num, u32 start_addr, u32 end_addr)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_START_ADDR = start_addr;
		UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_END_ADDR = end_addr;
	} else {
		UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_START_ADDR = start_addr;
		UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_END_ADDR = end_addr;
	}
}


/**
  * @brief  Get buffer start address for specific channel and buffer number.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  * @retval Buffer start address.
  */
u32 UVC_GetBufferAddr(u32 uvc_chn, u32 buf_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_START_ADDR;
	} else {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_START_ADDR;
	}
}


/**
  * @brief  Get received frame size for specific channel and buffer number.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  * @retval Received frame size .
  */
u32 UVC_GetFrameSize(u32 uvc_chn, u32 buf_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_FRM_SIZE;
	} else {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_FRM_SIZE;
	}
}

/**
  * @brief  Set UVC command register.
  * @param  uvc_chn: Channel number.
  *         uvc_cmd_num: command register number.
  *         cmd: pointer to command structure.
  * @retval None.
  */
void UVC_SetCMD(u32 uvc_chn, u32 uvc_cmd_num, UVC_CmdReg *cmd)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(uvc_cmd_num < UVC_MAX_CMD_NUM);

	if (uvc_chn == 0) {
		UVC->CH0_CMD[uvc_cmd_num].UVC_CH0_CMDx_CTRL = cmd->cmd_reg0.ctrl;
		UVC->CH0_CMD[uvc_cmd_num].UVC_CH0_CMDx_DATA = cmd->data;
	} else {
		UVC->CH1_CMD[uvc_cmd_num].UVC_CH1_CMDx_CTRL = cmd->cmd_reg0.ctrl;
		UVC->CH1_CMD[uvc_cmd_num].UVC_CH1_CMDx_DATA = cmd->data;
	}
}

/**
  * @brief  Set bInterval value for specific channel.
  * @param  uvc_chn: Channel number.
  *         bInterval: bInterval value.
  * @retval None.
  */
void UVC_SetInterval(u32 uvc_chn, u8 bInterval)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(bInterval > 0);

	if (uvc_chn == 0) {
		UVC->UVC_CH0_EP_CTRL &= (~UVC_MASK_CH0_EP_BINTERVAL);
		UVC->UVC_CH0_EP_CTRL |= UVC_CH0_EP_BINTERVAL(bInterval);
	} else {
		UVC->UVC_CH1_EP_CTRL &= (~UVC_MASK_CH1_EP_BINTERVAL);
		UVC->UVC_CH1_EP_CTRL |= UVC_CH1_EP_BINTERVAL(bInterval);
	}
}


/**
  * @brief  Init UVC_InitStruct structure.
  * @param  uvc_initstruct: pointer to UVC_InitStruct structure.
  * @retval None.
  */
void UVC_StructInit(UVC_InitStruct *uvc_initstruct)
{
	uvc_initstruct->chn_num = 0;
	uvc_initstruct->ep_num = 0;
	uvc_initstruct->frame_buf0_start_addr = 0;
	uvc_initstruct->frame_buf0_end_addr = 0;
	uvc_initstruct->frame_buf1_start_addr = 0;
	uvc_initstruct->frame_buf1_end_addr = 0;
	uvc_initstruct->hcdmai = 0;
	uvc_initstruct->eof = 0;
	uvc_initstruct->binterval = 1;

	_memset(&uvc_initstruct->uvc_cmd[0], 0, UVC_MAX_CMD_NUM * sizeof(UVC_CmdReg));
}


/**
  * @brief  Init specific UVC channel using UVC_InitStruct.
  * @note   EP auto is not enabled here, call UVC_ConcatEn() and then UVC_EPAutoEn() to start the channel.
  * @param  uvc_initstruct: pointer to UVC_InitStruct structure.
  * @retval None.
  */
void UVC_ChnlInit(UVC_InitStruct *uvc_initstruct)
{
	u32 i;

	assert_param(uvc_initstruct->chn_num < UVC_TOTAL_CHANNEL_NUM);

	UVC_SetEPNum(uvc_initstruct->chn_num, uvc_initstruct->ep_num);
	UVC_SetHCDMAi(uvc_initstruct->chn_num, uvc_initstruct->hcdmai);

	/* Set frame buffer */
	UVC_SetBufferAddr(uvc_initstruct->chn_num, 0, uvc_initstruct->frame_buf0_start_addr, uvc_initstruct->frame_buf0_end_addr);
	UVC_SetBufferAddr(uvc_initstruct->chn_num, 1, uvc_initstruct->frame_buf1_start_addr, uvc_initstruct->frame_buf1_end_addr);

	/* Set interval */
	UVC_SetInterval(uvc_initstruct->chn_num, uvc_initstruct->binterval);

	UVC_SetEOF(uvc_initstruct->chn_num, uvc_initstruct->eof);

	/* Set cmd reg*/
	for (i = 0; i < UVC_MAX_CMD_NUM; i ++) {
		UVC_SetCMD(uvc_initstruct->chn_num, i, &uvc_initstruct->uvc_cmd[i]);
	}
}


/**
  * @brief  Set Concat state for specific channel.
  * @param  uvc_chn: Channel number.
  *         state: Concat state, can be ENABLE or DISABLE.
  * @retval None.
  */
void UVC_ConcatEn(u32 uvc_chn, u32 state)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);

	if (state == ENABLE) {
		if (uvc_chn == 0) {
			UVC->UVC_CTRL |= UVC_BIT_CH0_CONCAT_EN;
		} else {
			UVC->UVC_CTRL |= UVC_BIT_CH1_CONCAT_EN;
		}
	} else {
		if (uvc_chn == 0) {
			UVC->UVC_CTRL &= ~UVC_BIT_CH0_CONCAT_EN;
		} else {
			UVC->UVC_CTRL &= ~UVC_BIT_CH1_CONCAT_EN;
		}
	}
}


/**
  * @brief  Set EP auto enable function.
  * @param  uvc_chn: Channel number.
  *         state: EP auto enable state, can be ENABLE or DISABLE.
  * @retval None.
  */
void UVC_EPAutoEn(u32 uvc_chn, u32 state)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);

	if (uvc_chn == 0) {
		if (state == ENABLE) {
			UVC->UVC_CH0_EP_CTRL |= UVC_BIT_CH0_EP_AUTO_EN;
		} else {
			UVC->UVC_CH0_EP_CTRL &= (~UVC_BIT_CH0_EP_AUTO_EN);
		}
	} else {
		if (state == ENABLE) {
			UVC->UVC_CH1_EP_CTRL |= UVC_BIT_CH1_EP_AUTO_EN;
		} else {
			UVC->UVC_CH1_EP_CTRL &= (~UVC_BIT_CH1_EP_AUTO_EN);
		}
	}

}


/**
  * @brief  Get PTS value of first isoc packet in a frame.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  * @retval PTS value.
  */
u32 UVC_GetPTS(u32 uvc_chn, u32 buf_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_PTS;
	} else {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_PTS;
	}
}


/**
  * @brief  Get SCR value of first isoc packet in a frame.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  * @retval SCR first value.
  */
u32 UVC_GetSCRFirst(u32 uvc_chn, u32 buf_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_SCR_FIRST;
	} else {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_SCR_FIRST;
	}
}


/**
  * @brief  Get SCR value of last isoc packet in a frame.
  * @param  uvc_chn: Channel number.
  *         buf_num: buffer number.
  * @retval SCR Last value.
  */
u32 UVC_GetSCRLast(u32 uvc_chn, u32 buf_num)
{
	assert_param(uvc_chn < UVC_TOTAL_CHANNEL_NUM);
	assert_param(buf_num < UVC_MAX_BUF_NUM);

	if (buf_num == 0) {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF0_SCR_LAST;
	} else {
		return UVC->CH_BUF[uvc_chn].UVC_CHx_BUF1_SCR_LAST;
	}
}

/** @} */

/** @} */

/**
  * @}
  */
