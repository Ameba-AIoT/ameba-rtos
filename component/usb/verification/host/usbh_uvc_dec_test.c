/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_uvc_intf.h"
#include "usbh.h"
//#include "ringbuffer.h"
#include <stdlib.h>
/*
//For USE_MJPEG, comment because of compile err,remove comment when fix err
#include "basetype.h"
#include "jpegdecapi.h"
#include "ppapi.h"
*/

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "USBH";
/*Just capture and abandon frame*/
#define USBH_UVC_APP_SIMPLE	1

/*Capture frame and write it to SD card through vfs*/
#define USBH_UVC_APP_VFS	2

/*Choose which application example*/
#define CONFIG_USBH_UVC_APP  USBH_UVC_APP_SIMPLE

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
/*Just capture frame and save length*/
#define USBH_UVC_CAL_TP 	1
#endif

#define USE_MJPEG			0
#define USE_LCDC			0

/* Private includes -------------------------------------------------------------*/

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#include "vfs.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#endif


/* Private macros ------------------------------------------------------------*/

#define USBH_UVC_BUF_SIZE       UVC_VIDEO_FRAME_SIZE   // Frame buffer size, resident in PSRAM, depends on format type


#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#define USBH_UVC_VFS_WRITE_SIZE          (16 * 1024)
#define USBH_UVC_VFS_VIDEO_SIZE          (2 * 1024 * 1024)
#endif


/* Private function prototypes -----------------------------------------------*/

static int uvc_cb_init(void);
static int uvc_cb_deinit(void);
static int uvc_cb_attach(void);
static int uvc_cb_detach(void);

/* Private variables ---------------------------------------------------------*/

static u32 uvc_if0;
static u32 uvc_formart0;
static u32 uvc_width0;
static u32 uvc_height0;
static u32 uvc_fps0;
static u32 uvc_frame_num0;

static u32 uvc_if1;
static u32 uvc_formart1;
static u32 uvc_width1;
static u32 uvc_height1;
static u32 uvc_fps1;
static u32 uvc_frame_num1;

static rtos_sema_t uvc_conn_sema;
static rtos_sema_t uvc_disconn_sema;
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
static rtos_mutex_t uvc_buf_mutex_mjpeg = NULL;
static rtos_mutex_t uvc_buf_mutex_h264 = NULL;
#endif
static u32 uvc_stop_capture = 0;
static u32 uvc_restart = 0;
#if USE_MJPEG
static u32 switch_pic = 0;
#endif

static u8 *psram_copy_addr = (u8 *) 0x60200000;
static u32 rx_total_H;
static u32 rx_total_L;
static u32 rx_start;

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
static rtos_sema_t uvc_vfs_save_img_sema_mjpeg = NULL;
static int uvc_vfs_is_init;
static int uvc_vfs_is_init_mjpeg = 0;
static int uvc_vfs_is_init_h264 = 0;
static int uvc_vfs_img_file_no_mjpeg = 0;
static int uvc_buf_size_mjpeg = 0;
static RingBuffer *uvc_rb;
static u8 uvc_buf_h264[USBH_UVC_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 uvc_buf_mjpeg[USBH_UVC_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
#endif

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
#if UVC_USE_SOF
	.ext_intr_enable = USBH_SOF_INTR,
	.sof_tick_enable = 1,
#endif
	.alt_max_cnt = 25,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = 3U,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*FIFO total depth is 1280 DWORD, reserve 14 DWORD for DMA addr*/
	.rx_fifo_depth = 754,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_uvc_cb_t uvc_cb = {
	.init = uvc_cb_init,
	.deinit = uvc_cb_deinit,
	.attach = uvc_cb_attach,
	.detach = uvc_cb_detach,
};

/* Private functions ---------------------------------------------------------*/

static int uvc_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int uvc_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int uvc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(uvc_conn_sema);
	return HAL_OK;
}

static int uvc_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	rtos_sema_give(uvc_disconn_sema);
	return HAL_OK;
}

#if USE_MJPEG

#if USE_LCDC

#define RGB_HV_IMG_WIDTH        (800)
#define RGB_HV_IMG_HEIGHT       (480)


#define MEM_SIZE	(RGB_HV_IMG_WIDTH * RGB_HV_IMG_HEIGHT * 2)		//rgb565

#define PSRAM_RGBIF_FRAME_BUF_BADDR	0x60000100

#define LCD_BLEN_SRGB	_PA_31
#define LCD_BLEN_RGB	_PA_31
#define LCD_BLEN_MCU	_PB_2

void PINMUX_LCDCRgb(u32 group)
{
	RTK_LOGI(NOTAG, "PPINMUX_LCDCRgb%u \r\n", group);

	if (group == PINMUX_S0) {//0 rgb,  1 mcu

		/* LCD Display Pin for ST7262.
		high: normal display; low： standby */
		GPIO_InitTypeDef GPIO_InitStruct_Display;

		GPIO_InitStruct_Display.GPIO_Pin = _PB_30;
		GPIO_InitStruct_Display.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(&GPIO_InitStruct_Display);
		GPIO_WriteBit(_PB_30, 1);

		/* LCD BLEN Pin for ST7262.
		high: BL enable; low: BL disable */
		GPIO_InitTypeDef GPIO_InitStruct_BLEN;
		GPIO_InitStruct_BLEN.GPIO_Pin = LCD_BLEN_RGB;
		GPIO_InitStruct_BLEN.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(&GPIO_InitStruct_BLEN);

		GPIO_WriteBit(LCD_BLEN_RGB, 1);

		/* LCD Signal for RGB interface in HV mode
		DE signal is required for LCD SYNC-DE mode */
		Pinmux_Config(_PA_23, PINMUX_FUNCTION_LCD_D0);	/*D0 - B0*/
		Pinmux_Config(_PB_31, PINMUX_FUNCTION_LCD_D1);	/*D1*/
		Pinmux_Config(_PB_27, PINMUX_FUNCTION_LCD_D2);	/*D2*/
		Pinmux_Config(_PB_22, PINMUX_FUNCTION_LCD_D3);	/*D3*/
		Pinmux_Config(_PC_1, PINMUX_FUNCTION_LCD_D4);	/*D4*/
		Pinmux_Config(_PA_21, PINMUX_FUNCTION_LCD_D5);	/*D5*/
		Pinmux_Config(_PA_22, PINMUX_FUNCTION_LCD_D6);	/*D6*/
		Pinmux_Config(_PB_23, PINMUX_FUNCTION_LCD_D7);	/*D7 - B7 */

		Pinmux_Config(_PA_12, PINMUX_FUNCTION_LCD_D8);	/*D8 - G0*/
		Pinmux_Config(_PA_20, PINMUX_FUNCTION_LCD_D9);	/*D9*/
		Pinmux_Config(_PB_26, PINMUX_FUNCTION_LCD_D10);	/*D10*/
		Pinmux_Config(_PA_17, PINMUX_FUNCTION_LCD_D11);	/*D11*/
		Pinmux_Config(_PA_25, PINMUX_FUNCTION_LCD_D12);	/*D12*/
		Pinmux_Config(_PB_25, PINMUX_FUNCTION_LCD_D13);	/*D13*/
		Pinmux_Config(_PB_0, PINMUX_FUNCTION_LCD_D14);	/*D14*/
		Pinmux_Config(_PA_24, PINMUX_FUNCTION_LCD_D15);	/*D15 - G7*/

		Pinmux_Config(_PB_21, PINMUX_FUNCTION_LCD_D16);	/*D16 - R0*/
		Pinmux_Config(_PB_2, PINMUX_FUNCTION_LCD_D17);	/*D17 */
		Pinmux_Config(_PB_28, PINMUX_FUNCTION_LCD_D18);	/*D18*/
		Pinmux_Config(_PC_0, PINMUX_FUNCTION_LCD_D19);	/*D19*/
		Pinmux_Config(_PA_13, PINMUX_FUNCTION_LCD_D20);	/*D20*/
		Pinmux_Config(_PA_28, PINMUX_FUNCTION_LCD_D21);	/*D21*/
		Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_D22);	/*D22*/
		Pinmux_Config(_PB_29, PINMUX_FUNCTION_LCD_D23);	/*D23 - R7*/

		// Pinmux_Config(_PA_26, PINMUX_FUNCTION_MCU_RD);	/*RD, HSYNC*/
		Pinmux_Config(_PA_26, PINMUX_FUNCTION_LCD_RGB_HSYNC);	/*RD, HSYNC*/
		Pinmux_Config(_PA_16, PINMUX_FUNCTION_LCD_RGB_VSYNC);	/*VSYNC-TE, VSYNC*/
		Pinmux_Config(_PA_27, PINMUX_FUNCTION_LCD_RGB_DCLK);	/*WR, DCLK*/

#if 0
		GPIO_InitTypeDef GPIO_InitStruct_DE;
		// RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
		GPIO_InitStruct_DE.GPIO_Pin = _PB_1;
		GPIO_InitStruct_DE.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(&GPIO_InitStruct_DE);
		GPIO_WriteBit(_PB_1, 0);/* sync mode： de 接地 */
#else
		Pinmux_Config(_PB_1, PINMUX_FUNCTION_LCD_RGB_DE);	/*SYNC-DE*/

#endif

	} else if (group == 1) {
		// Pinmux_Config(_PA_29, PINMUX_FUNCTION_LCD_MCU_DCX);
		// Pinmux_Config(_PB_29, PINMUX_FUNCTION_LCD_MCU_WR);
		// Pinmux_Config(_PA_12, PINMUX_FUNCTION_LCD_MCU_RD);
		// Pinmux_Config(_PA_20, PINMUX_FUNCTION_LCD_MCU_CSX);

		// //for pinmux test, not lt J2
		// Pinmux_Config(_PA_7, PINMUX_FUNCTION_LCD_MCU_TE);
		// Pinmux_Config(_PA_8, PINMUX_FUNCTION_LCD_MCU_VSYNC);
	}

}

void RCC_LCDCFenCke(void)
{
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		RTK_LOGI(NOTAG, "Platform: [FPGA] . \r\n");

		LCDC_SYS_CLK = 40000000;

		RCC_PeriphClockDividerFENSet(USB_PLL_VO, ENABLE);//lcdc clk  LSYS_BIT_FEN_USB_PLL_SHPERI: 4080A244[6]
		RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, ENABLE);//lcdc aclk  LSYS_BIT_FEN_USB_PLL_SHPERI 4080A244[4]

		// RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_SHPERI_CLOCK, ENABLE);
		// RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDC_CLOCK, ENABLE);

		/* select lcdc from debug port: SDM 0x20[19:16]: 1, lcdc */
		u32 regval = HAL_READ32(0x4080A2FC, 0);//240924 bitfile
		//regval &= (~(0x7 << 8));
		//regval |= ((0x1 & 0x7) << 8);
		regval &= (~(0xf));
		regval |= (0x1);
		HAL_WRITE32(0x4080A2FC, 0x0, regval);

		/* adjust voclk to 80M in FPGA */
//#if !(LCDC_FPGA_RTLSIM)
		RTK_LOGI(NOTAG, "Adjust voclk to 80MHz \r\n");

		regval = HAL_READ32(0x4080A2FC, 0);
		regval &= (~(0x7 << 5));
		regval |= ((0x5 & 0x7) << 5);

		HAL_WRITE32(0x4080A2FC, 0x0, regval);
//#endif

	} else {
		RTK_LOGI(NOTAG, "Platform: [ASIC or RTLSIM] . \r\n");

#if 1//LCDC_VO_SHPERI_USE_USB_PLL
		RTK_LOGI(NOTAG, " USB_PLL \r\n");
		RTK_LOGI(NOTAG, ">> 0x%08X cksl_usb_pll_vo[15:14] cksl_usb_pll_shperi[11:10] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0));
		RCC_PeriphClockSourceSet(VO, USB_PLL);//cksl_vo: 4080A220[15:14]: 00:xtal, 01:usb pll, 11: sys pll
		RCC_PeriphClockSourceSet(SHPERI, USB_PLL);//cksl_shperi: 4080A220[11:10]
		RTK_LOGI(NOTAG, "<< 0x%08X cksl_usb_pll_vo[15:14] cksl_usb_pll_shperi[11:10] \r\n\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0));

		RTK_LOGI(NOTAG, " ======= Set ckd_usb_pll_vo & shperi in Bootloader ======= \r\n");
		// RTK_LOGI(NOTAG, ">> 0x%08X ckd_usb_pll_vo[5:0],  0x%08X ckd_usb_pll_shperi[19:16] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_USB_PLL_GRP1), HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_USB_PLL_GRP0));
		// //REG_LSYS_CKD_USB_PLL_GRP1
		// RCC_PeriphClockDividerSet(USB_PLL_VO, 6);//1--64: 960M/6=160M
		// RCC_PeriphClockDividerSet(USB_PLL_SHPERI, 8);//1--16: 960M/8=120M
		// RTK_LOGI(NOTAG, "<< 0x%08X ckd_usb_pll_vo[5:0],  0x%08X ckd_usb_pll_shperi[19:16] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_USB_PLL_GRP1), HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_USB_PLL_GRP0));

		RCC_PeriphClockDividerFENSet(USB_PLL_VO, ENABLE);//lcdc clk  LSYS_BIT_FEN_USB_PLL_SHPERI: 4080A244[6]
		RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, ENABLE);//lcdc aclk  LSYS_BIT_FEN_USB_PLL_SHPERI 4080A244[4]

#else
		RTK_LOGI(NOTAG, " SYS_PLL \r\n");
		//sys pll

PLL_SSC_CTRL4 = 0x14:
						PLL_BIT_PWC_UALV_PLL
						u32 regtmp = HAL_READ32(PLL_REG_BASE, 0x14);//PLL_SSC_CTRL4
		RTK_LOGI(NOTAG, "PLL_BIT_PWC_UALV_PLL[Bit30] 0x%08x >> ", regtmp);

		regtmp |= PLL_BIT_PWC_UALV_PLL;
		HAL_WRITE32(PLL_REG_BASE, 0x14, regtmp);

		RTK_LOGI(NOTAG, " 0x%08x\r\n", HAL_READ32(PLL_REG_BASE, 0x14));


		RTK_LOGI(NOTAG, ">> 0x%08X cksl_vo[15:14] cksl_shpei[11:10] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0));
		RCC_PeriphClockSourceSet(VO, SYS_PLL);
		RCC_PeriphClockSourceSet(SHPERI, SYS_PLL);
		RTK_LOGI(NOTAG, "<< 0x%08X cksl_vo[15:14] cksl_shpei[11:10] \r\n\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0));

		RTK_LOGI(NOTAG, ">> 0x%08X ckd_vo[5:0],  0x%08X ckd_shpei[19:16] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_SYS_PLL_GRP1), HAL_READ32(SYSTEM_CTRL_BASE,
				 REG_LSYS_CKD_SYS_PLL_GRP0));
		RCC_PeriphClockDividerSet(SYS_PLL_VO, 2);
		RCC_PeriphClockDividerSet(SYS_PLL_SHPERI, 4);
		RTK_LOGI(NOTAG, "<< 0x%08X ckd_vo[5:0],  0x%08X ckd_shpei[19:16] \r\n\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_SYS_PLL_GRP1), HAL_READ32(SYSTEM_CTRL_BASE,
				 REG_LSYS_CKD_SYS_PLL_GRP0));

		RTK_LOGI(NOTAG, ">> 0x%08X fen_sys_pll_vo[6], fen_sys_pll_shpei[4] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_SYS_PLL));
		RCC_PeriphClockDividerFENSet(SYS_PLL_VO, ENABLE);//REG_LSYS_CKD_FEN_SYS_PLL BIT6
		RCC_PeriphClockDividerFENSet(SYS_PLL_SHPERI, ENABLE);
		RTK_LOGI(NOTAG, "<< 0x%08X fen_sys_pll_vo[6], fen_sys_pll_shpei[4] \r\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_SYS_PLL));
#endif
	}

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_SHPERI_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDC_CLOCK, ENABLE);
}

static void lcdc_rgb_init(void)
{
	LCDC_RGBInitTypeDef LCDC_RGBInitStruct;

	DCache_Clean(0x60200000, RGB_HV_IMG_WIDTH * RGB_HV_IMG_HEIGHT * 3);

	RCC_LCDCFenCke();

	LCDC_Cmd(LCDC, DISABLE);

	LCDC_RGBStructInit(&LCDC_RGBInitStruct);

	/* set HV para according to lcd spec */
	LCDC_RGBInitStruct.LCDC_RGBVSW = 1;//4;
	LCDC_RGBInitStruct.LCDC_RGBVBP = 4;//8;
	LCDC_RGBInitStruct.LCDC_RGBVFP = 6;//8;

	LCDC_RGBInitStruct.LCDC_RGBHSW = 4;
	LCDC_RGBInitStruct.LCDC_RGBHBP = 40;//39;
	LCDC_RGBInitStruct.LCDC_RGBHFP = 40;

	LCDC_RGBInitStruct.LCDC_RGBImgWidth = RGB_HV_IMG_WIDTH;
	LCDC_RGBInitStruct.LCDC_RGBImgHeight = RGB_HV_IMG_HEIGHT;

	LCDC_RGBInitStruct.LCDC_RGBSyncMode = LCDC_RGB_HV_MODE;
	LCDC_RGBInitStruct.LCDC_RGBEnPulPlrty = LCDC_RGB_EN_PUL_HIGH_LEV_ACTIVE;
	LCDC_RGBInitStruct.LCDC_RGBDclkActvEdge = LCDC_RGB_DCLK_FALLING_EDGE_FETCH;
	LCDC_RGBInitStruct.LCDC_RGBHsPulPlrty = LCDC_RGB_HS_PUL_LOW_LEV_SYNC;
	LCDC_RGBInitStruct.LCDC_RGBVsPulPlrty = LCDC_RGB_VS_PUL_LOW_LEV_SYNC;

	LCDC_RGBInitStruct.LCDC_RGBIfMode = LCDC_RGB_IF_24_BIT;
	LCDC_RGBInitStruct.LCDC_RGBInputFormat = LCDC_INPUT_FORMAT_RGB565;
	LCDC_RGBInitStruct.LCDC_RGBOutputFormat = LCDC_OUTPUT_FORMAT_RGB888;
	LCDC_RGBInitStruct.LCDC_RGBRefreshFreq = 30;//50;

	LCDC_RGBInit(LCDC, &LCDC_RGBInitStruct);

	// /* for fpga voclk swtich to 80M selected by dummy reg */
	// if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
	// 	u32 RegVal = HAL_READ32(0x4080A2FC, 0);

	// 	if ((RegVal >> 5 & 0x7) == 0x5) {
	// 		/* force to 80M/4 = 20M */
	// 		RTK_LOGI(NOTAG, "force to 80M/4 = 20M \n");
	LCDC->LCDC_RGB_CLK_DIV &= (~LCDC_MASK_CLKDIV);
	LCDC->LCDC_RGB_CLK_DIV |= LCDC_CLKDIV((4 / 2) - 1);
	// 	}
	// }

	/* configure DMA burst size */
	//LCDC_DMAModeConfig(LCDC, 4);

	/*configure base address*/
	LCDC_DMAImageBaseAddrConfig(LCDC, (u32)0x60200000);

	/*enable the LCDC*/
	LCDC_Cmd(LCDC, ENABLE);

}

void LCDC_DumpRegister(void)
{
	LCDC_TypeDef *pLCDCx = LCDC;

	RTK_LOGI(NOTAG, "LCDC Register Base: 0x%08x\n", (u32)pLCDCx);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_CTRL = 0x%08x\n", (void *)(&pLCDCx->LCDC_CTRL), pLCDCx->LCDC_CTRL);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_PLANE_SIZE = 0x%08x\n", (void *)(&pLCDCx->LCDC_PLANE_SIZE), pLCDCx->LCDC_PLANE_SIZE);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_UNDFLW_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_UNDFLW_CFG), pLCDCx->LCDC_UNDFLW_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_DMA_MODE_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_DMA_MODE_CFG), pLCDCx->LCDC_DMA_MODE_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_SHW_RLD_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_SHW_RLD_CFG), pLCDCx->LCDC_SHW_RLD_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_COLOR_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_COLOR_CFG), pLCDCx->LCDC_COLOR_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_SEC_DEST_ADDR = 0x%08x\n", (void *)(&pLCDCx->LCDC_SEC_DEST_ADDR), pLCDCx->LCDC_SEC_DEST_ADDR);

	/* interrupt related register */
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IRQ_EN = 0x%08x\n", (void *)(&pLCDCx->LCDC_IRQ_EN), pLCDCx->LCDC_IRQ_EN);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IRQ_STATUS = 0x%08x\n", (void *)(&pLCDCx->LCDC_IRQ_STATUS), pLCDCx->LCDC_IRQ_STATUS);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IRQ_RAW = 0x%08x\n", (void *)(&pLCDCx->LCDC_IRQ_RAW), pLCDCx->LCDC_IRQ_RAW);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_LINE_INT_POS = 0x%08x\n", (void *)(&pLCDCx->LCDC_LINE_INT_POS), pLCDCx->LCDC_LINE_INT_POS);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_CUR_POS_STATUS = 0x%08x\n", (void *)(&pLCDCx->LCDC_CUR_POS_STATUS), pLCDCx->LCDC_CUR_POS_STATUS);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_STATUS = 0x%08x\n", (void *)(&pLCDCx->LCDC_STATUS), pLCDCx->LCDC_STATUS);

	RTK_LOGI(NOTAG, "[0x%08x] LCDC_DEBUG_STATUS = 0x%08x\n", (void *)(&pLCDCx->LCDC_DEBUG_STATUS), pLCDCx->LCDC_DEBUG_STATUS);

	/* RGB I/F related register */
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_RGB_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_RGB_CFG), pLCDCx->LCDC_RGB_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_RGB_VSYNC_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_RGB_VSYNC_CFG), pLCDCx->LCDC_RGB_VSYNC_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_RGB_HSYNC_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_RGB_HSYNC_CFG), pLCDCx->LCDC_RGB_HSYNC_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_RGB_CLK_DIV = 0x%08x\n", (void *)(&pLCDCx->LCDC_RGB_CLK_DIV), pLCDCx->LCDC_RGB_CLK_DIV);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_RGB_SYNC_STATUS = 0x%08x\n", (void *)(&pLCDCx->LCDC_RGB_SYNC_STATUS), pLCDCx->LCDC_RGB_SYNC_STATUS);

	/* MCU I/F related register */
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CFG), pLCDCx->LCDC_MCU_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_VSYNC_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_VSYNC_CFG), pLCDCx->LCDC_MCU_VSYNC_CFG);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_TIMING_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_TIMING_CFG), pLCDCx->LCDC_MCU_TIMING_CFG);
	//RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_IO_DATA = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_IO_DATA), pLCDCx->LCDC_MCU_IO_DATA); /*when not enable LCDC IP, reading this register will  hang here for CPU*/
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_IO_TO_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_IO_TO_CFG), pLCDCx->LCDC_MCU_IO_TO_CFG);

	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IMG_CFG = 0x%08x\n", (void *)(&pLCDCx->LCDC_IMG_CFG), pLCDCx->LCDC_IMG_CFG);

	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IMG_A_BASE_ADDR = 0x%08x\n", (void *)(&pLCDCx->LCDC_IMG_A_BASE_ADDR), pLCDCx->LCDC_IMG_A_BASE_ADDR);//following hardfault
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IMG_B_BASE_ADDR = 0x%08x\n", (void *)(&pLCDCx->LCDC_IMG_B_BASE_ADDR), pLCDCx->LCDC_IMG_B_BASE_ADDR);//following hardfault
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_IMG_SLIDE = 0x%08x\n", (void *)(&pLCDCx->LCDC_IMG_SLIDE), pLCDCx->LCDC_IMG_SLIDE);//following hardfault

#ifdef AMEBAGREEN2_TODO
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CMD_DWORDx[0] = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CMD_DWORDx), pLCDCx->LCDC_MCU_CMD_DWORDx[0]);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CMD_DWORDx[1] = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CMD_DWORDx), pLCDCx->LCDC_MCU_CMD_DWORDx[1]);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CMD_DWORDx[2] = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CMD_DWORDx), pLCDCx->LCDC_MCU_CMD_DWORDx[2]);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CMD_DWORDx[3] = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CMD_DWORDx), pLCDCx->LCDC_MCU_CMD_DWORDx[3]);
	RTK_LOGI(NOTAG, "[0x%08x] LCDC_MCU_CMD_WRNUM = 0x%08x\n", (void *)(&pLCDCx->LCDC_MCU_CMD_WRNUM), pLCDCx->LCDC_MCU_CMD_WRNUM);
#endif
}

void uvc_rgb_init(void)
{
	RTK_LOGI(TAG, "%s \r\n", __func__);

	PINMUX_LCDCRgb(PINMUX_S0);
	/* init controller */
	lcdc_rgb_init();

	//LCDC_DumpRegister();
}

#endif

#if 0
static void print_image_info(JpegDecImageInfo *info)
{
	RTK_LOGI(TAG, "===========JpegDecImageInfo start============\n");
	RTK_LOGI(TAG, "    JpegDecImageInfo:\n");
	RTK_LOGI(TAG, "    displayWidth: %d\n", info->displayWidth);
	RTK_LOGI(TAG, "    displayHeight: %d\n", info->displayHeight);
	RTK_LOGI(TAG, "    outputWidth: %d\n", info->outputWidth);
	RTK_LOGI(TAG, "    outputHeight: %d\n", info->outputHeight);
	RTK_LOGI(TAG, "    version: %d\n", info->version);
	RTK_LOGI(TAG, "    units: %d\n", info->units);
	RTK_LOGI(TAG, "    xDensity: %d\n", info->xDensity);
	RTK_LOGI(TAG, "    outputFormat: %x\n", info->outputFormat);
	RTK_LOGI(TAG, "    codingMode: %d\n", info->codingMode);
	RTK_LOGI(TAG, "    thumbnailType: %d\n", info->thumbnailType);
	RTK_LOGI(TAG, "    displayWidthThumb: %d\n", info->displayWidthThumb);
	RTK_LOGI(TAG, "    displayHeightThumb: %d\n", info->displayHeightThumb);
	RTK_LOGI(TAG, "    outputWidthThumb: %d\n", info->outputWidthThumb);
	RTK_LOGI(TAG, "    outputHeightThumb: %d\n", info->outputHeightThumb);
	RTK_LOGI(TAG, "    outputFormatThumb: %d\n", info->outputFormatThumb);
	RTK_LOGI(TAG, "    codingModeThumb: %d\n", info->codingModeThumb);
	RTK_LOGI(TAG, "===========JpegDecImageInfo end============\n");
}



static void print_pp_config(PPConfig *config)
{
	RTK_LOGI(TAG, "===========PPConfig start============\n");

	RTK_LOGI(TAG, "PPConfig->ppInImg:\n");
	RTK_LOGI(TAG, "    bufferBusAddr: 0x%x\n", config->ppInImg.bufferBusAddr);
	RTK_LOGI(TAG, "    bufferBusAddrBot: 0x%x\n", config->ppInImg.bufferBusAddrBot);
	RTK_LOGI(TAG, "    bufferBusAddrChBot: 0x%x\n", config->ppInImg.bufferBusAddrChBot);
	RTK_LOGI(TAG, "    bufferCbBusAddr: 0x%x\n", config->ppInImg.bufferCbBusAddr);
	RTK_LOGI(TAG, "    bufferCrBusAddr: 0x%x\n", config->ppInImg.bufferCrBusAddr);
	RTK_LOGI(TAG, "    height: %d\n", config->ppInImg.height);
	RTK_LOGI(TAG, "    width: %d\n", config->ppInImg.width);
	RTK_LOGI(TAG, "    picStruct: %d\n", config->ppInImg.picStruct);
	RTK_LOGI(TAG, "    pixFormat: %x\n", config->ppInImg.pixFormat);
	RTK_LOGI(TAG, "    videoRange: %d\n", config->ppInImg.videoRange);

	RTK_LOGI(TAG, "PPConfig->ppInCrop:\n");
	RTK_LOGI(TAG, "    enable: %d\n", config->ppInCrop.enable);
	RTK_LOGI(TAG, "    height: %d\n", config->ppInCrop.height);
	RTK_LOGI(TAG, "    width: %d\n", config->ppInCrop.width);
	RTK_LOGI(TAG, "    originX: %d\n", config->ppInCrop.originX);
	RTK_LOGI(TAG, "    originY: %d\n", config->ppInCrop.originY);

	RTK_LOGI(TAG, "PPConfig->ppInRotation:\n");
	RTK_LOGI(TAG, "    rotation: %d\n", config->ppInRotation.rotation);

	RTK_LOGI(TAG, "PPConfig->ppOutImg:\n");
	RTK_LOGI(TAG, "    bufferBusAddr:  0x%x\n", config->ppOutImg.bufferBusAddr);
	RTK_LOGI(TAG, "    bufferChromaBusAddr:  0x%x\n", config->ppOutImg.bufferChromaBusAddr);
	RTK_LOGI(TAG, "    height: %d\n", config->ppOutImg.height);
	RTK_LOGI(TAG, "    width: %d\n", config->ppOutImg.width);
	RTK_LOGI(TAG, "    multiBuffer: %d\n", config->ppOutImg.multiBuffer);
	RTK_LOGI(TAG, "    pixFormat: %x\n", config->ppOutImg.pixFormat);

	RTK_LOGI(TAG, "PPConfig->ppOutRgb:\n");
	RTK_LOGI(TAG, "    alpha: %d\n", config->ppOutRgb.alpha);
	RTK_LOGI(TAG, "    brightness: %d\n", config->ppOutRgb.brightness);
	RTK_LOGI(TAG, "    contrast: %d\n", config->ppOutRgb.contrast);
	RTK_LOGI(TAG, "    saturation: %d\n", config->ppOutRgb.saturation);
	RTK_LOGI(TAG, "    transparency: %d\n", config->ppOutRgb.transparency);
	RTK_LOGI(TAG, "    ditheringEnable: %d\n", config->ppOutRgb.ditheringEnable);
	RTK_LOGI(TAG, "    rgbBitmask.maskAlpha: %d\n", config->ppOutRgb.rgbBitmask.maskAlpha);
	RTK_LOGI(TAG, "    rgbBitmask.maskR: %d\n", config->ppOutRgb.rgbBitmask.maskR);
	RTK_LOGI(TAG, "    rgbBitmask.maskG: %d\n", config->ppOutRgb.rgbBitmask.maskG);
	RTK_LOGI(TAG, "    rgbBitmask.maskB: %d\n", config->ppOutRgb.rgbBitmask.maskB);
	RTK_LOGI(TAG, "    rgbTransform: %d\n", config->ppOutRgb.rgbTransform);
	RTK_LOGI(TAG, "    rgbTransformCoeffs.a: %d\n", config->ppOutRgb.rgbTransformCoeffs.a);
	RTK_LOGI(TAG, "    rgbTransformCoeffs.b: %d\n", config->ppOutRgb.rgbTransformCoeffs.b);
	RTK_LOGI(TAG, "    rgbTransformCoeffs.c: %d\n", config->ppOutRgb.rgbTransformCoeffs.c);
	RTK_LOGI(TAG, "    rgbTransformCoeffs.d: %d\n", config->ppOutRgb.rgbTransformCoeffs.d);
	RTK_LOGI(TAG, "    rgbTransformCoeffs.e: %d\n", config->ppOutRgb.rgbTransformCoeffs.e);

	RTK_LOGI(TAG, "PPConfig->ppOutMask1:\n");
	RTK_LOGI(TAG, "    alphaBlendEna:  %d\n", config->ppOutMask1.alphaBlendEna);
	RTK_LOGI(TAG, "    blendComponentBase:  0x%x\n", config->ppOutMask1.blendComponentBase);
	RTK_LOGI(TAG, "    blendHeight:  %d\n", config->ppOutMask1.blendHeight);
	RTK_LOGI(TAG, "    blendWidth:  %d\n", config->ppOutMask1.blendWidth);
	RTK_LOGI(TAG, "    blendOriginX:  %d\n", config->ppOutMask1.blendOriginX);
	RTK_LOGI(TAG, "    blendOriginY:  %d\n", config->ppOutMask1.blendOriginY);
	RTK_LOGI(TAG, "    width:  %d\n", config->ppOutMask1.width);
	RTK_LOGI(TAG, "    height:  %d\n", config->ppOutMask1.height);
	RTK_LOGI(TAG, "    originX:  %d\n", config->ppOutMask1.originX);
	RTK_LOGI(TAG, "    originY:  %d\n", config->ppOutMask1.originY);
	RTK_LOGI(TAG, "    enable:  %d\n", config->ppOutMask1.enable);

	RTK_LOGI(TAG, "PPConfig->ppOutMask2:\n");
	RTK_LOGI(TAG, "    alphaBlendEna:  %d\n", config->ppOutMask2.alphaBlendEna);
	RTK_LOGI(TAG, "    blendComponentBase:  0x%x\n", config->ppOutMask2.blendComponentBase);
	RTK_LOGI(TAG, "    blendHeight:  %d\n", config->ppOutMask2.blendHeight);
	RTK_LOGI(TAG, "    blendWidth:  %d\n", config->ppOutMask2.blendWidth);
	RTK_LOGI(TAG, "    blendOriginX:  %d\n", config->ppOutMask2.blendOriginX);
	RTK_LOGI(TAG, "    blendOriginY:  %d\n", config->ppOutMask2.blendOriginY);
	RTK_LOGI(TAG, "    width:  %d\n", config->ppOutMask2.width);
	RTK_LOGI(TAG, "    height:  %d\n", config->ppOutMask2.height);
	RTK_LOGI(TAG, "    originX:  %d\n", config->ppOutMask2.originX);
	RTK_LOGI(TAG, "    originY:  %d\n", config->ppOutMask2.originY);
	RTK_LOGI(TAG, "    enable:  %d\n", config->ppOutMask2.enable);

	RTK_LOGI(TAG, "PPConfig->PPOutFrameBuffer:\n");
	RTK_LOGI(TAG, "    enable:  %d\n", config->ppOutFrmBuffer.enable);
	RTK_LOGI(TAG, "    frameBufferHeight:  %d\n", config->ppOutFrmBuffer.frameBufferHeight);
	RTK_LOGI(TAG, "    frameBufferWidth:  %d\n", config->ppOutFrmBuffer.frameBufferWidth);
	RTK_LOGI(TAG, "    writeOriginX:  %d\n", config->ppOutFrmBuffer.writeOriginX);
	RTK_LOGI(TAG, "    writeOriginY:  %d\n", config->ppOutFrmBuffer.writeOriginY);

	RTK_LOGI(TAG, "PPConfig->ppMaxSupportedOutRes:\n");
	RTK_LOGI(TAG, "    maxOutHeight:  %d\n", config->ppMaxSupportedOutRes.maxOutHeight);
	RTK_LOGI(TAG, "    maxOutWidth:  %d\n", config->ppMaxSupportedOutRes.maxOutWidth);

	RTK_LOGI(TAG, "===========PPConfig end============\n");
}
#endif
#if USE_MJPEG
void PipeLine(u8 *addr, u32 size)
{
	JpegDecInst jpegInst;
	JpegDecRet jpegRet;
	JpegDecImageInfo imageInfo;
	JpegDecInput jpegIn;
	JpegDecOutput jpegOut;
	JpegDecLinearMem input;
	u8 *input_bus_address = 0;
	u8 *pp_out_bus_address;
	PPInst pp = NULL;
	PPResult ppRet;
	PPConfig pPpConf;
	u32 streamLen = size;

	_memset(&jpegIn, 0, sizeof(JpegDecInput));
	_memset(&jpegOut, 0, sizeof(JpegDecOutput));
	_memset(&imageInfo, 0, sizeof(JpegDecImageInfo));
	_memset(&pPpConf, 0, sizeof(PPConfig));

	switch_pic++;

	input_bus_address = addr;
	if ((switch_pic) % 2 == 0) {
		pp_out_bus_address = (u8 *)0x60200000;
	} else {
		pp_out_bus_address = (u8 *)0x60300000;
	}

	jpegRet = JpegDecInit(&jpegInst);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGE(TAG, "JpegDecInit err: %d\n", jpegRet);
		goto end;
	} else {
		//RTK_LOGI(TAG, "JpegDecInit OK\n");
	}

	input.pVirtualAddress = (u32 *)input_bus_address;
	input.busAddress = (u32) input_bus_address;
	/* Pointer to the input JPEG */
	jpegIn.streamBuffer.pVirtualAddress = (u32 *) input.pVirtualAddress;
	jpegIn.streamBuffer.busAddress = input.busAddress;
	jpegIn.streamLength = streamLen; /* JPEG length in bytes */
	jpegIn.bufferSize = 0; /* input buffering (0 == not used) */

	jpegRet = JpegDecGetImageInfo(jpegInst, &jpegIn, &imageInfo);
	if (jpegRet != JPEGDEC_OK) {
		RTK_LOGE(TAG, "JpegDecGetImageInfo err: %d\n", jpegRet);
		goto end;
	}

	//print_image_info(&imageInfo);

	ppRet = PPInit(&pp);
	if (ppRet != PP_OK) {
		RTK_LOGE(TAG, "PPInit error: %d\n", ppRet);
		goto end;
	}

	ppRet = PPDecCombinedModeEnable(pp, jpegInst, PP_PIPELINED_DEC_TYPE_JPEG);
	if (ppRet != PP_OK) {
		RTK_LOGE(TAG, "PPDecCombinedModeEnable err: %d\n", ppRet);
		goto end;
	}

	ppRet = PPGetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGE(TAG, "PPGetConfig err: %d\n", ppRet);
		goto end;
	}

	pPpConf.ppInImg.width = imageInfo.outputWidth;
	pPpConf.ppInImg.height = imageInfo.outputHeight;
	pPpConf.ppInImg.videoRange = 1;
	pPpConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
	pPpConf.ppOutImg.width = 800;
	pPpConf.ppOutImg.height = 480;
	pPpConf.ppOutImg.bufferBusAddr = (u32)pp_out_bus_address;
	pPpConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB16_5_6_5;
	pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_709;

#if 0// USE_LCDC
	pPpConf.ppOutFrmBuffer.enable = 1;
	pPpConf.ppOutFrmBuffer.frameBufferWidth = RGB_HV_IMG_WIDTH;
	pPpConf.ppOutFrmBuffer.frameBufferHeight = RGB_HV_IMG_HEIGHT;
	pPpConf.ppOutFrmBuffer.writeOriginX = 0;
	pPpConf.ppOutFrmBuffer.writeOriginY = 0;
#endif

	ppRet = PPSetConfig(pp, &pPpConf);
	if (ppRet != PP_OK) {
		RTK_LOGE(TAG, "PPSetConfig err: %d\n", ppRet);
		goto end;
	}

	//print_pp_config(&pPpConf);

	jpegRet = JpegDecDecode(jpegInst, &jpegIn, &jpegOut);
	if (jpegRet != JPEGDEC_FRAME_READY) {
		RTK_LOGE(TAG, "JpegDecDecode err: %d\n", jpegRet);
		goto end;
	} else {
		RTK_LOGI(TAG, "JpegDecDecode OK:%x\n", (u32)pp_out_bus_address);
		//DCache_Invalidate((u32)pp_out_bus_address, imageInfo.outputWidth*imageInfo.outputHeight*2);
#if USE_LCDC
		LCDC_DMAImageBaseAddrConfig(LCDC, (u32) pp_out_bus_address);
		LCDC_ShadowReloadConfig(LCDC);
#endif
	}

	ppRet = PPDecCombinedModeDisable(pp, jpegInst);
	if (ppRet != PP_OK) {
		RTK_LOGE(TAG, "PPDecCombinedModeDisable err: %d\n", ppRet);
		goto end;
	}

end:
	PPRelease(pp);
	JpegDecRelease(jpegInst);
}
#endif
#endif

static void uvc_img_prepare_h264(uvc_frame_t *frame)
{
	u32 len = 0;

	if (frame == NULL) {
		return;
	}

	len = frame->byteused;
	DCache_Invalidate((u32)frame->buf, len);
	RTK_LOGS(TAG, RTK_LOG_INFO, "[h264] addr %x len=%d\n", frame->buf, len);
	if (len > USBH_UVC_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[h264] Image len overflow!\n");
		return;
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
#if USBH_UVC_CAL_TP
	/* Just add length without any further processing */
	if (rx_total_L > (rx_total_L + len)) {
		rx_total_H ++;//u32-u32: rx_total_H - rx_total_L
	}
	rx_total_L += len;
#else
	/* just copy data without any further processing */
	memcpy(psram_copy_addr, (void *)(frame->buf), len);
	rx_total_L += len;
	psram_copy_addr += len;

	if ((u32)psram_copy_addr > 0x60300000) {
		//uvc_stop_capture = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "[UVC-h264] total Capture len=%x now_len=%x\n", rx_total_L, (u32)psram_copy_addr - 0x60200000);
		psram_copy_addr = (u8 *) 0x60200000;
	}
#endif
#endif
#if ( CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	if (rtos_mutex_take(uvc_buf_mutex_h264, 1000 / 30 / 2) == RTK_SUCCESS) {
		if (RingBuffer_Space(uvc_rb) > frame->byteused) {
			RingBuffer_Write(uvc_rb, frame->buf, frame->byteused);
		}
		rtos_mutex_give(uvc_buf_mutex_h264);
	}
#endif
}
static void uvc_img_prepare_yuv(uvc_frame_t *frame)
{
	u32 len = 0;

	if (frame == NULL) {
		return;
	}

	len = frame->byteused;
	DCache_Invalidate((u32)frame->buf, len);
	RTK_LOGS(TAG, RTK_LOG_INFO, "[yuv] addr=%x len=%d\n", frame->buf, len);
	if (len > USBH_UVC_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[yuv] Image len overflow!\n");
		return;
	}
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
	/* Just add length without any further processing */
	if (rx_total_L > (rx_total_L + len)) {
		rx_total_H ++;
	}
	rx_total_L += len;
#endif
}

static void uvc_img_prepare_mjpeg(uvc_frame_t *frame)
{
	u32 len = 0;

	if (frame == NULL) {
		return;
	}

	len = frame->byteused;
	DCache_Invalidate((u32)frame->buf, len);
	RTK_LOGS(TAG, RTK_LOG_INFO, "[mjpeg] addr=%x len=%d\n", frame->buf, len);

	if (len > USBH_UVC_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] Image len overflow!\n");
		return;
	}

	if (len == 12 || len == 0) {
		RTK_LOGI(TAG, "[mjpeg] len= %d\n", len);
		return; //lenght of first frame could be 0, then uvc capture len=12, just skip it.
	}

	while (1) {    //some camera may pad 0 to the end of image
		if (frame->buf[len - 1] == 0) {
			len--;
		} else {
			break;
		}
	}

#if 0
	if (frame->buf[0] != 0xff || frame->buf[1] != 0xd8 || frame->buf[len - 2] != 0xff || frame->buf[len - 1] != 0xd9) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[0], frame->buf[1], frame->buf[2], frame->buf[3]);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[len - 4], frame->buf[len - 3], frame->buf[len - 2], frame->buf[len - 1]);
		uvc_stop_capture = 1;
		return;
	}
#endif
#if USE_MJPEG
	PipeLine(frame->buf, len);
#endif
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
	/* just copy data without any further processing */
	//memcpy(uvc_buf_mjpeg, (void *)(frame->buf), len);
	if (rx_total_L > (rx_total_L + len)) {
		rx_total_H ++;
	}
	rx_total_L += len;
#endif

#if ( CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	if (rtos_mutex_take(uvc_buf_mutex_mjpeg, 1000 / 30 / 2) == RTK_SUCCESS) {
		memcpy(uvc_buf_mjpeg, (void *)(frame->buf), len);
		uvc_buf_size = len;
		rtos_mutex_give(uvc_buf_mutex_mjpeg);
		rtos_sema_give(uvc_vfs_save_img_sema_mjpeg);
	}
#endif
}

static void uvc_calculate_tp(u32 loop)
{
	u32 rx_elapse;
	u32 rx_perf;
	u32 rx_fps;
	u32 rx_perf_total;

	rx_elapse = SYSTIMER_GetPassTime(rx_start);

	rx_fps = loop * 1000 / rx_elapse;
	rx_perf = rx_total_L / 1024 * 1000 / rx_elapse;//KB/S

	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %d KB/s @ %d ms, fps %d/s\n", rx_perf, rx_elapse, rx_fps);

	rx_perf = rx_perf * 10 / 1024;
	rx_perf_total = rx_perf + ((rx_total_H * 10000 << 12) / rx_elapse);
	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %d.%d MB/s-%d (%d_%d/%d)\n", rx_perf_total / 10, rx_perf_total % 10, rx_perf, rx_total_H, rx_total_L, loop);

	rx_total_L = 0;
	rx_total_H = 0;
}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)

static void uvc_vfs_thread_mjpeg(void *param)
{
	char path[128] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;
	char filename[64] = {0};
	char f_num[15];
	UNUSED(param);

	rtos_sema_create(&uvc_vfs_save_img_sema_mjpeg, 0U, 1U);
	uvc_vfs_is_init_mjpeg = 1;

	prefix = find_vfs1_tag();

	while (uvc_vfs_is_init_mjpeg) {
		if (rtos_sema_take(uvc_vfs_save_img_sema_mjpeg, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[UVC] Fail to take img_sema\n");
			continue;
		}
		memset(filename, 0, 64);
		sprintf(filename, "img");
		sprintf(f_num, "%d", uvc_vfs_img_file_no_mjpeg);
		strcat(filename, f_num);
		strcat(filename, ".jpeg");
		snprintf(path, sizeof(path), "%s:%s", prefix, filename);
		RTK_LOGS(TAG, RTK_LOG_INFO, "[UVC] Create image file: %s\n", path);

		finfo = (vfs_file *)fopen(path, "w");
		if (finfo == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[UVC]  fopen image fail\n");
			goto exit;
		}

		if (rtos_mutex_take(uvc_buf_mutex_mjpeg, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
			res = fwrite(uvc_buf_mjpeg, uvc_buf_size_mjpeg, 1, (FILE *)finfo);
			if (res != 1) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "buf fwrite fail: %d\n", res);
			} else {
				//RTK_LOGS(TAG, RTK_LOG_INFO, "buf fwrite succeed\n");
			}

			rtos_mutex_give(uvc_buf_mutex_mjpeg);
		}

		fclose((FILE *)finfo);

		uvc_vfs_img_file_no_mjpeg++;
	}
exit:
	uvc_vfs_is_init_mjpeg = 0;
	rtos_task_delete(NULL);
}

static int uvc_vfs_start_mjpeg(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "uvc_vfs_thread_mjpeg", uvc_vfs_thread_mjpeg, NULL, 1024U * 8, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create vfs thread fail\n");
		ret = 1;
	} else {
		ret = 0;
		tf
	}

	return ret;
}

static void uvc_vfs_thread_h264(void *param)
{
	char path[128] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;
	char filename[64] = {0};
	u32 total_len = 0;
	UNUSED(param);
	u8 *buffer_h264 = rtos_mem_malloc(USBH_UVC_VFS_WRITE_SIZE);
	uvc_vfs_is_init_h264 = 1;

	prefix = find_vfs1_tag();

	memset(filename, 0, 64);
	sprintf(filename, "stream");
	strcat(filename, ".h264");

	RTK_LOGI(TAG, path, sizeof(path), "%s:%s", prefix, filename);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Create image file: %s\n", path);

	finfo = (vfs_file *)fopen(path, "wb+");
	if (finfo == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "fopen image fail\n");
		goto exit;
	}

	while (total_len < USBH_UVC_VFS_VIDEO_SIZE) {
		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_VFS_WRITE_SIZE) {
			rtos_mutex_take(uvc_buf_mutex_h264, RTOS_MAX_TIMEOUT);
			RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_VFS_WRITE_SIZE);
			rtos_mutex_give(uvc_buf_mutex_h264);
			res = fwrite(buffer_h264, USBH_UVC_VFS_WRITE_SIZE, 1, (FILE *)finfo);
			if (res != 1) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "buf fwrite fail: %d\n", res);
				goto exit;
			}
			total_len += USBH_UVC_VFS_WRITE_SIZE;
		} else {
			rtos_time_delay_ms(2);
		}
	}

exit:
	uvc_vfs_is_init_h264 = 0;
	fclose((FILE *)finfo);
	rtos_mem_free(buffer_h264);
	RingBuffer_Destroy(uvc_rb);
	rtos_task_delete(NULL);
}

static int uvc_vfs_start_h264(void)
{
	int ret;
	rtos_task_t task;

	uvc_rb = RingBuffer_Create(uvc_buf_h264, USBH_UVC_BUF_SIZE, LOCAL_RINGBUFF, 0);

	ret = rtos_task_create(&task, "uvc_vfs_thread_h264", uvc_vfs_thread_h264, NULL, 1024U * 8, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create vfs thread fail\n");
		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

#endif

static void usbh_uvc_instance1_test(void *param)
{
	int ret = 0;
	uvc_frame_t *buf;
	u32 img_cnt = 0;
	UNUSED(param);
	uvc_config_t uvc_ctx;

	if (uvc_formart1 == 0) {
		uvc_ctx.fmt_type = UVC_FORMAT_MJPEG;
	} else if (uvc_formart1 == 1) {
		uvc_ctx.fmt_type = UVC_FORMAT_H264;
	} else {
		uvc_ctx.fmt_type = UVC_FORMAT_YUV;
	}

	uvc_ctx.width = uvc_width1;
	uvc_ctx.height = uvc_height1;
	uvc_ctx.frame_rate = uvc_fps1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Set parameters\n");
	ret = usbh_uvc_set_param(&uvc_ctx, uvc_if1);
	if (ret) {
		goto exit1;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Para: %d*%d@%dfps\n", uvc_ctx.width, uvc_ctx.height, uvc_ctx.frame_rate);

		if (uvc_ctx.fmt_type == UVC_FORMAT_MJPEG) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: MJPEG Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_H264) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: H264 Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_YUV) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: YUV Stream\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Unsupport Stream\n");
			goto exit1;
		}
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Start vfs service\n");
	if (uvc_formart1 == 0) {
		ret = uvc_vfs_start_mjpeg();
		if (ret != 0) {
			goto exit1;
		}
		rtos_mutex_create(&uvc_buf_mutex_mjpeg);
		while (uvc_vfs_is_init_mjpeg == 0) {
			rtos_time_delay_ms(500);
		}
	} else {
		ret = uvc_vfs_start_h264();
		if (ret != 0) {
			goto exit1;
		}
		rtos_mutex_create(&uvc_buf_mutex_h264);
		while (uvc_vfs_is_init_h264 == 0) {
			rtos_time_delay_ms(500);
		}
	}

#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Stream on\n");
	ret = usbh_uvc_stream_on(uvc_if1);
	if (ret) {
		goto exit2;
	}

	//rtos_time_delay_ms(3000);

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	while (uvc_vfs_is_init && img_cnt < uvc_frame_num1) {
#else
	while (img_cnt < uvc_frame_num1) {
#endif
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Get frame %d\n", img_cnt);
		buf = usbh_uvc_get_frame(uvc_if1);
		if (buf == NULL) {
			ret = 1;
		} else {
			ret = 0;
		}

		if (ret == 0) {
			if (uvc_formart1 == 0) {
				uvc_img_prepare_mjpeg(buf);
			} else if (uvc_formart1 == 1) {
				uvc_img_prepare_h264(buf);
			} else {
				uvc_img_prepare_yuv(buf);
			}
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Put frame\n");
		usbh_uvc_put_frame(buf, uvc_if1);

		img_cnt ++;

		if (uvc_stop_capture == 1) {
			if (uvc_formart1 != 0) {
				if ((u32)psram_copy_addr > 0x60280000) {
					RTK_LOGI(TAG, "[UVC-INS1] psram_copy_addr:%x\n", psram_copy_addr);
					break;
				}
			} else {
				break;
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: Stop capturing images\n");
	usbh_uvc_stream_off(uvc_if1);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap 0x%x bytes\n",  rtos_mem_get_free_heap_size());
exit2:
exit1:
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: USBH UVC demo stop\n");
	rtos_task_delete(NULL);
}

static void usbh_uvc_instance0_test(void *param)
{
	int ret = 0;
	uvc_frame_t *buf;
	u32 img_cnt = 0;
	UNUSED(param);
	uvc_config_t uvc_ctx;
	u32 temp1, temp2;

	ret = usbh_init(&usbh_cfg, NULL);
	if (ret != HAL_OK) {
		goto exit;
	}

	rtos_sema_create(&uvc_conn_sema, 0U, 1U);
	rtos_sema_create(&uvc_disconn_sema, 0U, 1U);

	ret = usbh_uvc_init(&uvc_cb);
	if (ret) {
		goto exit1;
	}

	if (uvc_formart0 == 0) {
		uvc_ctx.fmt_type = UVC_FORMAT_MJPEG;
	} else if (uvc_formart0 == 1) {
		uvc_ctx.fmt_type = UVC_FORMAT_H264;
	} else {
		uvc_ctx.fmt_type = UVC_FORMAT_YUV;
	}

	uvc_ctx.width = uvc_width0;
	uvc_ctx.height = uvc_height0;
	uvc_ctx.frame_rate = uvc_fps0;

	rtos_sema_take(uvc_conn_sema, RTOS_SEMA_MAX_COUNT);

	RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Set parameters\n");
	ret = usbh_uvc_set_param(&uvc_ctx, uvc_if0);
	if (ret) {
		goto exit1;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Para: %d*%d@%dfps\n", uvc_ctx.width, uvc_ctx.height, uvc_ctx.frame_rate);
		if (uvc_ctx.fmt_type == UVC_FORMAT_MJPEG) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: MJPEG Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_H264) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: H264 Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_YUV) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: YUV Stream\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Unsupport Stream\n");
			goto exit1;
		}
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Start vfs service\n");
	if (uvc_formart0 == 0) {
		ret = uvc_vfs_start_mjpeg();
		if (ret != 0) {
			goto exit1;
		}
		rtos_mutex_create(&uvc_buf_mutex_mjpeg);
		while (uvc_vfs_is_init_mjpeg == 0) {
			rtos_time_delay_ms(500);
		}
	} else {
		ret = uvc_vfs_start_h264();
		if (ret != 0) {
			goto exit1;
		}
		rtos_mutex_create(&uvc_buf_mutex_h264);
		while (uvc_vfs_is_init_h264 == 0) {
			rtos_time_delay_ms(500);
		}
	}
#endif

restart:
	img_cnt = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Stream on\n");
	ret = usbh_uvc_stream_on(uvc_if0);
	if (ret) {
		goto exit2;
	}
	//rtos_time_delay_ms(3000);
	rx_start = SYSTIMER_TickGet();
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	while (uvc_vfs_is_init && img_cnt < uvc_frame_num0) {
#else
	while (img_cnt < uvc_frame_num0) {
#endif
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Get frame %d\n", img_cnt);
		buf = usbh_uvc_get_frame(uvc_if0);
		if (buf == NULL) {
			ret = 1;
		} else {
			ret = 0;
		}

		if (ret == 0) {
			if (uvc_formart0 == 0) {
				uvc_img_prepare_mjpeg(buf);
			} else if (uvc_formart0 == 1) {
				uvc_img_prepare_h264(buf);
			} else {
				uvc_img_prepare_yuv(buf);
			}
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Put frame\n");
		usbh_uvc_put_frame(buf, uvc_if0);

		if (!usbh_get_status()) {
			rtos_sema_give(uvc_disconn_sema);
			break;
		}

		img_cnt ++;

		if (uvc_stop_capture == 1) {
			//goto exit;
			break;
		}
	}

	uvc_calculate_tp(img_cnt);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: Stop capturing images\n");
	usbh_uvc_stream_off(uvc_if0);

	RTK_LOGI(TAG, "[UVC-INS0] Free heap size: %d\n", rtos_mem_get_free_heap_size());

	if (uvc_restart == 1) {
		//RTK_LOGI(TAG,"[UVC-INS0]t1\n");
		temp1 = _rand() % 3000;
		temp2 = 10 + (_rand() % 100);

		RTK_LOGI(TAG, "[UVC-INS0]taskdelay %dms, new capture frame num = %d\n", temp1, temp2);
		rtos_time_delay_ms(temp1);
		usbh_uvc_stream_off(uvc_if0);
		uvc_frame_num0 = temp2;
		RTK_LOGI(TAG, "[UVC-INS0]new loop = %d\n", uvc_frame_num0);
		goto restart;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap 0x%x bytes\n",  rtos_mem_get_free_heap_size());

exit2:
	usbh_uvc_deinit();
exit1:
	//rtos_mutex_delete(uvc_buf_mutex);
	rtos_sema_delete(uvc_conn_sema);
	rtos_sema_delete(uvc_disconn_sema);

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: USBH UVC demo stop\n");
	usbh_deinit();
	rtos_task_delete(NULL);
}

u32 usbh_uvc_dec_test(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	u32 instance;
	instance = _strtoul((const char *)(argv[0]), (char **)NULL, 10);

#if USE_MJPEG
	RCC_PeriphClockCmd(APBPeriph_MJPEG, APBPeriph_MJPEG_CLOCK, ENABLE);
	hx170dec_init();
#if USE_LCDC
	uvc_rgb_init();
#endif
#endif

	if (argv[7]) {
		usbh_cfg.speed = _strtoul((const char *)(argv[7]), (char **)NULL, 10);
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Speed %d\n", usbh_cfg.speed);

	if (instance == 0) {
		uvc_if0 = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		uvc_formart0 = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		uvc_width0 = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
		uvc_height0 = _strtoul((const char *)(argv[4]), (char **)NULL, 10);
		uvc_fps0 = _strtoul((const char *)(argv[5]), (char **)NULL, 10);
		uvc_frame_num0 = _strtoul((const char *)(argv[6]), (char **)NULL, 10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS0: %d %d %d %d %d %d\n", uvc_if0, uvc_formart0, uvc_width0, uvc_height0, uvc_fps0, uvc_frame_num0);
		rtos_task_create(NULL, "example_usbh_uvc_thread0", usbh_uvc_instance0_test, NULL, 1024 * 16U, 1U);
	} else {
		uvc_if1 = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		uvc_formart1 = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		uvc_width1 = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
		uvc_height1 = _strtoul((const char *)(argv[4]), (char **)NULL, 10);
		uvc_fps1 = _strtoul((const char *)(argv[5]), (char **)NULL, 10);
		uvc_frame_num1 = _strtoul((const char *)(argv[6]), (char **)NULL, 10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "INS1: %d %d %d %d %d %d\n", uvc_if1, uvc_formart1, uvc_width1, uvc_height1, uvc_fps1, uvc_frame_num1);
		rtos_task_create(NULL, "example_usbh_uvc_thread1", usbh_uvc_instance1_test, NULL, 1024 * 16U, 1U);
	}
	return 0;
}

u32 usbh_uvc_stop_test(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	uvc_stop_capture = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
	return 0;
}

_OPTIMIZE_NONE_
void usbh_access_otg(void *para)
{
	UNUSED(para);
	u32 reg;
	u32 val;

	RTK_LOGS(TAG, RTK_LOG_INFO, "start access usb reg\n");

	while (1) {
		val = _rand() % 0xFFFFFFFF;
		HAL_WRITE32(USB_REG_BASE, 0x60 + 0x550, val);
		reg = HAL_READ32(USB_REG_BASE, 0x60 + 0x550);
		if (reg != val) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reg err:%x %x\n", HAL_READ32(USB_REG_BASE, 0x60 + 0x550), val);
			uvc_stop_capture = 1;
		}
		DelayUs(100);
	}
}

u32 usbh_uvc_restart_test(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	uvc_restart = _strtoul((const char *)(argv[0]), (char **)NULL, 10);

	if (uvc_restart == 1) {
		rtos_task_create(NULL, "usbh_access_otg", usbh_access_otg, NULL, 1024 * 2U, 1U);
	}
	return 0;
}

#if UVC_USE_HW
u32 usbh_uvc_dec_dump(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	RTK_LOGI(TAG, "[0x%08x]UVC_CTRL: 0x%08x\n", &UVC->UVC_CTRL, UVC->UVC_CTRL);
	RTK_LOGI(TAG, "[0x%08x]UVC_CHNL0_CTRL: 0x%08x\n", &UVC->UVC_CHNL0_CTRL, UVC->UVC_CHNL0_CTRL);
	RTK_LOGI(TAG, "[0x%08x]UVC_CHNL1_CTRL: 0x%08x\n", &UVC->UVC_CHNL1_CTRL, UVC->UVC_CHNL1_CTRL);
	RTK_LOGI(TAG, "[0x%08x]UVC_INTR: 0x%08x\n", &UVC->UVC_INTR, UVC->UVC_INTR);

	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_START_ADDR: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_START_ADDR, UVC->CH_BUF[0].UVC_CHx_BUF0_START_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_END_ADDR: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_END_ADDR, UVC->CH_BUF[0].UVC_CHx_BUF0_END_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_FRM_SIZE: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_FRM_SIZE, UVC->CH_BUF[0].UVC_CHx_BUF0_FRM_SIZE);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_PTS: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_PTS, UVC->CH_BUF[0].UVC_CHx_BUF0_PTS);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_SCR_FIRST: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_SCR_FIRST, UVC->CH_BUF[0].UVC_CHx_BUF0_SCR_FIRST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_SCR_LAST: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_SCR_LAST, UVC->CH_BUF[0].UVC_CHx_BUF0_SCR_LAST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF0_SOF_CNT: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF0_SOF_CNT, UVC->CH_BUF[0].UVC_CHx_BUF0_SOF_CNT);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_START_ADDR: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_START_ADDR, UVC->CH_BUF[0].UVC_CHx_BUF1_START_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_END_ADDR: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_END_ADDR, UVC->CH_BUF[0].UVC_CHx_BUF1_END_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_FRM_SIZE: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_FRM_SIZE, UVC->CH_BUF[0].UVC_CHx_BUF1_FRM_SIZE);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_PTS: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_PTS, UVC->CH_BUF[0].UVC_CHx_BUF1_PTS);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_SCR_FIRST: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_SCR_FIRST, UVC->CH_BUF[0].UVC_CHx_BUF1_SCR_FIRST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_SCR_LAST: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_SCR_LAST, UVC->CH_BUF[0].UVC_CHx_BUF1_SCR_LAST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[0].UVC_CHx_BUF1_SOF_CNT: 0x%08x\n", &UVC->CH_BUF[0].UVC_CHx_BUF1_SOF_CNT, UVC->CH_BUF[0].UVC_CHx_BUF1_SOF_CNT);

	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_START_ADDR: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_START_ADDR, UVC->CH_BUF[1].UVC_CHx_BUF0_START_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_END_ADDR: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_END_ADDR, UVC->CH_BUF[1].UVC_CHx_BUF0_END_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_FRM_SIZE: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_FRM_SIZE, UVC->CH_BUF[1].UVC_CHx_BUF0_FRM_SIZE);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_PTS: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_PTS, UVC->CH_BUF[1].UVC_CHx_BUF0_PTS);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_SCR_FIRST: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_SCR_FIRST, UVC->CH_BUF[1].UVC_CHx_BUF0_SCR_FIRST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_SCR_LAST: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_SCR_LAST, UVC->CH_BUF[1].UVC_CHx_BUF0_SCR_LAST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF0_SOF_CNT: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF0_SOF_CNT, UVC->CH_BUF[1].UVC_CHx_BUF0_SOF_CNT);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_START_ADDR: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_START_ADDR, UVC->CH_BUF[1].UVC_CHx_BUF1_START_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_END_ADDR: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_END_ADDR, UVC->CH_BUF[1].UVC_CHx_BUF1_END_ADDR);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_FRM_SIZE: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_FRM_SIZE, UVC->CH_BUF[1].UVC_CHx_BUF1_FRM_SIZE);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_PTS: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_PTS, UVC->CH_BUF[1].UVC_CHx_BUF1_PTS);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_SCR_FIRST: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_SCR_FIRST, UVC->CH_BUF[1].UVC_CHx_BUF1_SCR_FIRST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_SCR_LAST: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_SCR_LAST, UVC->CH_BUF[1].UVC_CHx_BUF1_SCR_LAST);
	RTK_LOGI(TAG, "[0x%08x]CH_BUF[1].UVC_CHx_BUF1_SOF_CNT: 0x%08x\n", &UVC->CH_BUF[1].UVC_CHx_BUF1_SOF_CNT, UVC->CH_BUF[1].UVC_CHx_BUF1_SOF_CNT);

	RTK_LOGI(TAG, "[0x%08x]UVC_CH0_EP_CTRL: 0x%08x\n", &UVC->UVC_CH0_EP_CTRL, UVC->UVC_CH0_EP_CTRL);
	RTK_LOGI(TAG, "[0x%08x]UVC_CH1_EP_CTRL: 0x%08x\n", &UVC->UVC_CH1_EP_CTRL, UVC->UVC_CH1_EP_CTRL);
	RTK_LOGI(TAG, "[0x%08x]UVC_HST_FRREM_CTRL: 0x%08x\n", &UVC->UVC_HST_FRREM_CTRL, UVC->UVC_HST_FRREM_CTRL);

	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[0].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[0].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[0].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[0].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[0].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[0].UVC_CH0_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[1].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[1].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[1].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[1].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[1].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[1].UVC_CH0_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[2].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[2].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[2].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[2].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[2].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[2].UVC_CH0_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[3].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[3].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[3].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[3].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[3].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[3].UVC_CH0_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[4].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[4].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[4].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[4].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[4].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[4].UVC_CH0_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[5].UVC_CH0_CMDx_CTRL: 0x%08x\n", &UVC->CH0_CMD[5].UVC_CH0_CMDx_CTRL, UVC->CH0_CMD[5].UVC_CH0_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH0_CMD[5].UVC_CH0_CMDx_DATA: 0x%08x\n", &UVC->CH0_CMD[5].UVC_CH0_CMDx_DATA, UVC->CH0_CMD[5].UVC_CH0_CMDx_DATA);

	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[0].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[0].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[0].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[0].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[0].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[0].UVC_CH1_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[1].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[0].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[1].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[1].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[1].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[1].UVC_CH1_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[2].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[2].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[2].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[2].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[2].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[2].UVC_CH1_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[3].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[3].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[3].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[3].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[3].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[3].UVC_CH1_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[4].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[4].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[4].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[4].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[4].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[4].UVC_CH1_CMDx_DATA);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[5].UVC_CH1_CMDx_CTRL: 0x%08x\n", &UVC->CH1_CMD[5].UVC_CH1_CMDx_CTRL, UVC->CH1_CMD[5].UVC_CH1_CMDx_CTRL);
	RTK_LOGI(TAG, "[0x%08x]CH1_CMD[5].UVC_CH1_CMDx_DATA: 0x%08x\n", &UVC->CH1_CMD[5].UVC_CH1_CMDx_DATA, UVC->CH1_CMD[5].UVC_CH1_CMDx_DATA);

	return 0;
}
#endif
#ifdef  AMEBAL2_BUG_FIX
#define STATS_BUFFER_SIZE     ( 2000 )
static char cBuffer[ STATS_BUFFER_SIZE ];

void vGetStatsAndPrint(void *para)
{
	UNUSED(para);
	memset(cBuffer, 0x00, sizeof(cBuffer));

	while (1) {
		rtos_time_delay_ms(1000);
		extern void vTaskGetRunTimeStats(char *);
		vTaskGetRunTimeStats(cBuffer);

		RTK_LOGS(TAG, RTK_LOG_INFO, "%s\n", cBuffer);
		memset(cBuffer, 0, STATS_BUFFER_SIZE);
	}
}

u32 usbh_task_runtime(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	rtos_task_create(NULL, "vGetStatsAndPrint", vGetStatsAndPrint, NULL, 1024 * 4U, 6U);
	return 0;
}
#endif
u32 lcdc_test(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
#if USE_LCDC
	//u32* rgb_data =(u32*) 0x60200000;
	u8 *rgb_data = (u8 *) 0x60000000;

#if 0
	for (int i = 0; i < 800 * 480 * 3; i = i + 3) {
		if (i < 800 * 480) {
			rgb_data[i] = 0xff;//0x12+i%256;
			rgb_data[i + 1] = 0x00;
			rgb_data[i + 2] = 0x00;
		} else if (i > 800 * 480 * 2) {
			rgb_data[i] = 0x00;
			rgb_data[i + 1] = 0xff; //0x34+i%256;
			rgb_data[i + 2] = 0x00;
		} else {
			rgb_data[i] = 0x00;
			rgb_data[i + 1] = 0x00;
			rgb_data[i + 2] = 0xff; //0x56+i%256;
		}
	}
#endif
	//for(int i = 0; i < 128; i = i + 3){
	//		rgb_data[i] = 0xff;
	//		rgb_data[i+1] = 0x00;
	//		rgb_data[i+2] = 0x00;
	//	}

	for (int i = 0; i < 800 * 480 * 2; i += 2) {
		rgb_data[i] = 0xf8;
		rgb_data[i + 1] = 0x00;

	}

	//DCache_Clean(0x60200000, 800*480*3);
	uvc_rgb_init();
#endif
	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_uvc_test[] = {
	{
		(const u8 *)"uvc", 10, usbh_uvc_dec_test, (const u8 *)"\tuvc test\n"
		"\t\t cmd: uvc instance(0 1) if_num(0 1)  format(0=mjpeg 1=h264 2=yuv)  width  height  fps  frame_num \n"
		"\t\t example: uvc   0    0     0   1280    720   30    2000 \n"
	},
	{
		(const u8 *)"uvc_stop", 10, usbh_uvc_stop_test, (const u8 *)"\tuvc test stop\n"
		"\t\t cmd: uvc_stop \n"
	},
#if UVC_USE_HW
	{
		(const u8 *)"uvc_dump", 3, usbh_uvc_dec_dump, (const u8 *)"\tuvc reg dump\n"
		"\t\t cmd: uvc_dump \n"
	},
#endif
#ifdef AMEBAL2_BUG_FIX
	//amebaL2 compile err. remove AMEBAL2_BUG_FIX when fix err
	{
		(const u8 *)"uvc_runtime", 3, usbh_task_runtime, (const u8 *)"\tuvc show runtime\n"
		"\t\t cmd: uvc_runtime \n"
	},
#endif
	{
		(const u8 *)"uvc_lcd", 3, lcdc_test, (const u8 *)"\tuvc test lcd\n"
		"\t\t cmd: uvc_lcd \n"
	},
	{
		(const u8 *)"uvc_restart", 3, usbh_uvc_restart_test, (const u8 *)"\tuvc test restart\n"
		"\t\t cmd: uvc_restart \n"
	},
};


