/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "st7701s.h"
#include "os_wrapper.h"

#define ST7701S_DEBUG                               0
#define LCDC_TEST_IMG_BUF_X                         480 //240 //(272)
#define LCDC_TEST_IMG_BUF_Y                         800 //320 //(480)

#define LCDC_IMG_BUF_ALIGNED64B(x)                  (((x) & ~0x3F) + 0x40)/*Start Addr shall aligned to 64Byte*/
#define LCDC_IMG_BUF_SIZE                           LCDC_IMG_BUF_ALIGNED64B(LCDC_TEST_IMG_BUF_X * LCDC_TEST_IMG_BUF_Y * 4)

#define REGFLAG_DELAY                               0xFC
#define REGFLAG_END_OF_TABLE                        0xFD    // END OF REGISTERS MARKER

#define Mhz                                         1000000UL
#define T_LPX                                       5
#define T_HS_PREP                                   6
#define T_HS_TRAIL                                  8
#define T_HS_EXIT                                   7
#define T_HS_ZERO                                   10

#define MIPI_DSI_RTNI                               2//4
#define MIPI_DSI_HSA                                4
#define MIPI_DSI_HBP                                30
#define MIPI_DSI_HFP                                30
#define MIPI_DSI_VSA                                5
#define MIPI_DSI_VBP                                20
#define MIPI_DSI_VFP                                15

enum {
	MIPI_DSI_V_SYNC_START                           = 0x01,
	MIPI_DSI_V_SYNC_END                             = 0x11,
	MIPI_DSI_H_SYNC_START                           = 0x21,
	MIPI_DSI_H_SYNC_END                             = 0x31,

	MIPI_DSI_COLOR_MODE_OFF                         = 0x02,
	MIPI_DSI_COLOR_MODE_ON                          = 0x12,
	MIPI_DSI_SHUTDOWN_PERIPHERAL                    = 0x22,
	MIPI_DSI_TURN_ON_PERIPHERAL                     = 0x32,

	MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM            = 0x03,
	MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM            = 0x13,
	MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM            = 0x23,

	MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM           = 0x04,
	MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM           = 0x14,
	MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM           = 0x24,

	MIPI_DSI_DCS_SHORT_WRITE                        = 0x05,
	MIPI_DSI_DCS_SHORT_WRITE_PARAM                  = 0x15,

	MIPI_DSI_DCS_READ                               = 0x06,

	MIPI_DSI_DCS_COMPRESSION_MODE                   = 0x07,
	MIPI_DSI_PPS_LONG_WRITE                         = 0x0A,

	MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE         = 0x37,

	MIPI_DSI_END_OF_TRANSMISSION                    = 0x08,

	MIPI_DSI_NULL_PACKET                            = 0x09,
	MIPI_DSI_BLANKING_PACKET                        = 0x19,
	MIPI_DSI_GENERIC_LONG_WRITE                     = 0x29,
	MIPI_DSI_DCS_LONG_WRITE                         = 0x39,

	MIPI_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20    = 0x0c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR24            = 0x1c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR16            = 0x2c,

	MIPI_DSI_PACKED_PIXEL_STREAM_30                 = 0x0d,
	MIPI_DSI_PACKED_PIXEL_STREAM_36                 = 0x1d,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR12            = 0x3d,

	MIPI_DSI_PACKED_PIXEL_STREAM_16                 = 0x0e,
	MIPI_DSI_PACKED_PIXEL_STREAM_18                 = 0x1e,
	MIPI_DSI_PIXEL_STREAM_3BYTE_18                  = 0x2e,
	MIPI_DSI_PACKED_PIXEL_STREAM_24                 = 0x3e,
};

typedef struct {
	u8 cmd;
	u8 count;
	u8 para_list[128];
} LCMSettingTable;

static u32 g_ST7701S_init_done = 0;
static u32 g_ST7701S_send_cmd = 1;
static u32 g_under_flow_cnt = 0;
static LCDC_InitTypeDef g_lcdc_init_struct;
static MIPI_InitTypeDef g_mipi_init_struct;

static ST7701SVBlankCallback *g_callback = NULL;
static void *g_data = NULL;
static int g_color_depth = 0;

static int g_first_flag = 1;

/* Resulation : 480x800
 * Inversion  : 2dot
 * Porch      : 125>vbp>=12, vfp>=20
 * Line Time  : 20us
 * Frame Rate : 60hz
 */
static LCMSettingTable g_ST7701S_init_cmd[] = {/* DCS Write Long */
	/* ST7701S Reset Sequence */
	/* LCD_Nreset (1); Delayms (1); */
	/* LCD_Nreset (0); Delayms (1); */
	/* LCD_Nreset (1); Delayms (120); */
	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},/* Delayms (120) */

	/* Bank0 Setting */
	/* Display Control setting */
	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x10}},
	{0xC0, 2, {0x63, 0x00}},
	{0xC1, 2, {0x0C, 0x02}},
	{0xC2, 2, {0x31, 0x08}},
	{0xCC, 1, {0x10}},

	/* Gamma Cluster Setting */
	{0xB0, 16, {0x40, 0x02, 0x87, 0x0E, 0x15, 0x0A, 0x03, 0x0A, 0x0A, 0x18, 0x08, 0x16, 0x13, 0x07, 0x09, 0x19}},
	{0xB1, 16, {0x40, 0x01, 0x86, 0x0D, 0x13, 0x09, 0x03, 0x0A, 0x09, 0x1C, 0x09, 0x15, 0x13, 0x91, 0x16, 0x19}},
	/* End Gamma Setting */
	/* End Display Control setting */
	/* End Bank0 Setting */

	/* Bank1 Setting */
	/* Power Control Registers Initial */
	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x11}},
	{0xB0, 1, {0x4D}},

	/* Vcom Setting */
	{0xB1, 1, {0x64}},
	/* End End Vcom Setting */

	{0xB2, 1, {0x07}},
	{0xB3, 1, {0x80}},
	{0xB5, 1, {0x47}},
	{0xB7, 1, {0x85}},
	{0xB8, 1, {0x21}},
	{0xB9, 1, {0x10}},
	{0xC1, 1, {0x78}},
	{0xC2, 1, {0x78}},
	{0xD0, 1, {0x88}},
	/* End Power Control Registers Initial */
	{REGFLAG_DELAY, 100, {}},/* Delayms (100) */
	/* GIP Setting */
	{0xE0, 3, {0x00, 0x84, 0x02}},
	{0xE1, 11, {0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20}},
	{0xE2, 13, {0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
	{0xE3, 4, {0x00, 0x00, 0x33, 0x33}},
	{0xE4, 2, {0x44, 0x44}},
	{0xE5, 16, {0x09, 0x31, 0xBE, 0xA0, 0x0B, 0x31, 0xBE, 0xA0, 0x05, 0x31, 0xBE, 0xA0, 0x07, 0x31, 0xBE, 0xA0}},
	{0xE6, 4, {0x00, 0x00, 0x33, 0x33}},
	{0xE7, 2, {0x44, 0x44}},
	{0xE8, 16, {0x08, 0x31, 0xBE, 0xA0, 0x0A, 0x31, 0xBE, 0xA0, 0x04, 0x31, 0xBE, 0xA0, 0x06, 0x31, 0xBE, 0xA0}},
	{0xEA, 16, {0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00}},
	{0xEB, 7, {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}},
	{0xEC, 2, {0x02, 0x00}},
	{0xED, 16, {0xF5, 0x47, 0x6F, 0x0B, 0x8F, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xF8, 0xB0, 0xF6, 0x74, 0x5F}},
	{0xEF, 12, {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
	/* End GIP Setting */

	{0xFF, 5, {0x77, 0x01, 0x00, 0x00, 0x00}},
	{0x29, 0, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}},
};

static void mipi_dsi_ST7701S_isr(void)
{
	u32 reg_val = MIPI_DSI_INTS_Get(MIPI);
	MIPI_DSI_INTS_Clr(MIPI, reg_val);

	u32 reg_val2 = MIPI_DSI_INTS_ACPU_Get(MIPI);
	MIPI_DSI_INTS_ACPU_Clr(MIPI, reg_val2);

	if (reg_val & MIPI_BIT_CMD_TXDONE) {
		reg_val &= ~MIPI_BIT_CMD_TXDONE;
		if (ST7701S_DEBUG) {
			printf("irq command tx done !\n");
		}
		g_ST7701S_send_cmd = 1;
	}

	if (reg_val2 & MIPI_BIT_VID_DONE) {
		printf("irq video tx done !\n");
	}

	u32 reg_dphy_err;
	if (reg_val & MIPI_BIT_ERROR) {
		reg_dphy_err = MIPI->MIPI_DPHY_ERR;
		MIPI->MIPI_DPHY_ERR = reg_dphy_err;
		if (g_first_flag != 1) {
			printf("LPTX Error: 0x%lu, DPHY Error: 0x%lu\n", reg_val, reg_dphy_err);
		}

		if (MIPI->MIPI_CONTENTION_DETECTOR_AND_STOPSTATE_DT & MIPI_MASK_DETECT_ENABLE) {
			MIPI->MIPI_CONTENTION_DETECTOR_AND_STOPSTATE_DT &= ~MIPI_MASK_DETECT_ENABLE;

			MIPI->MIPI_DPHY_ERR = reg_dphy_err;
			MIPI_DSI_INTS_Clr(MIPI, MIPI_BIT_ERROR);
			if (g_first_flag != 1) {
				printf("LPTX Error CLR: 0x%lu, DPHY: 0x%lu\n", MIPI->MIPI_INTS, MIPI->MIPI_DPHY_ERR);
			}
		}

		if (MIPI->MIPI_DPHY_ERR == reg_dphy_err) {
			printf("LPTX Still Error\n");
			MIPI_DSI_INT_Config(MIPI, ENABLE, DISABLE, FALSE);
		}
		reg_val &= ~MIPI_BIT_ERROR;
	}

	if (ST7701S_DEBUG) {
		if (reg_val) {
			printf("LPTX Error Occur: 0x%lx\n", reg_val);
		}

		if (reg_val2) {
			printf("irq id=0x%lx#\n", reg_val2);
		}
	}
}

static void MIPI_InitStruct_Config(MIPI_InitTypeDef *MIPI_init_type_def)
{
	u32 bit_per_pixel;
	switch (MIPI_init_type_def->MIPI_VideoDataFormat) {
	case MIPI_VIDEO_DATA_FORMAT_RGB565:
		bit_per_pixel = 16;
		break;
	case MIPI_VIDEO_DATA_FORMAT_RGB666_PACKED:
		bit_per_pixel = 18;
		break;
	case MIPI_VIDEO_DATA_FORMAT_RGB666_LOOSELY:
	case MIPI_VIDEO_DATA_FORMAT_RGB888:
	default:
		bit_per_pixel = 24;
		break;
	}

	MIPI_init_type_def->MIPI_LaneNum = 2;
	MIPI_init_type_def->MIPI_FrameRate = 60;

	MIPI_init_type_def->MIPI_HSA = MIPI_DSI_HSA * bit_per_pixel / 8 ;//- 10; /* here the unit is pixel but not us */
	if (MIPI_init_type_def->MIPI_VideoModeInterface == MIPI_VIDEO_NON_BURST_MODE_WITH_SYNC_PULSES) {
		MIPI_init_type_def->MIPI_HBP = MIPI_DSI_HBP * bit_per_pixel / 8 ;//- 10;
	} else {
		MIPI_init_type_def->MIPI_HBP = (MIPI_DSI_HSA + MIPI_DSI_HBP) * bit_per_pixel / 8 ;//-10 ;
	}

	MIPI_init_type_def->MIPI_HACT = LCDC_TEST_IMG_BUF_X;
	MIPI_init_type_def->MIPI_HFP = MIPI_DSI_HFP * bit_per_pixel / 8 ;//-12;

	MIPI_init_type_def->MIPI_VSA = MIPI_DSI_VSA;
	MIPI_init_type_def->MIPI_VBP = MIPI_DSI_VBP;
	MIPI_init_type_def->MIPI_VACT = LCDC_TEST_IMG_BUF_Y;
	MIPI_init_type_def->MIPI_VFP = MIPI_DSI_VFP;

	/*DataLaneFreq * LaneNum = FrameRate * (VSA+VBP+VACT+VFP) * (HSA+HBP+HACT+HFP) * PixelFromat*/
	u32 vtotal = MIPI_init_type_def->MIPI_VSA + MIPI_init_type_def->MIPI_VBP + MIPI_init_type_def->MIPI_VACT + MIPI_init_type_def->MIPI_VFP;
	u32 htotal_bits = (MIPI_DSI_HSA + MIPI_DSI_HBP + MIPI_init_type_def->MIPI_HACT + MIPI_DSI_HFP) * bit_per_pixel;
	u32 overhead_cycles = T_LPX + T_HS_PREP + T_HS_ZERO + T_HS_TRAIL + T_HS_EXIT;
	u32 overhead_bits = overhead_cycles * MIPI_init_type_def->MIPI_LaneNum * 8;
	u32 total_bits = htotal_bits + overhead_bits;

	MIPI_init_type_def->MIPI_VideDataLaneFreq = MIPI_init_type_def->MIPI_FrameRate * total_bits * vtotal / MIPI_init_type_def->MIPI_LaneNum / Mhz + 20;

	MIPI_init_type_def->MIPI_LineTime = (MIPI_init_type_def->MIPI_VideDataLaneFreq * Mhz) / 8 / MIPI_init_type_def->MIPI_FrameRate / vtotal;
	MIPI_init_type_def->MIPI_BllpLen = MIPI_init_type_def->MIPI_LineTime / 2;

	if (MIPI_DSI_HSA + MIPI_DSI_HBP + LCDC_TEST_IMG_BUF_X + MIPI_DSI_HFP < (512 + MIPI_DSI_RTNI * 16)) {
		printf("!!ERROR!!, LCM NOT SUPPORT\n");
	}

	if (MIPI_init_type_def->MIPI_LineTime * MIPI_init_type_def->MIPI_LaneNum < total_bits / 8) {
		printf("!!ERROR!!, LINE TIME TOO SHORT!\n");
	}

	if (ST7701S_DEBUG) {
		printf("DataLaneFreq: %d, LineTime: %d\n", MIPI_init_type_def->MIPI_VideDataLaneFreq, MIPI_init_type_def->MIPI_LineTime);
	}
}

static void mipi_lcm_set_reset_pin(u8 new_status)
{
	u32 pin_name = _PA_14;

	Pinmux_Swdoff();
	Pinmux_Config(pin_name, PINMUX_FUNCTION_GPIO);

	GPIO_InitTypeDef reset_pin;
	reset_pin.GPIO_Pin = pin_name;
	reset_pin.GPIO_PuPd = GPIO_PuPd_NOPULL;
	reset_pin.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&reset_pin);

	/* Reset */
	if (new_status) {
		GPIO_WriteBit(pin_name, 1);
	} else {
		GPIO_WriteBit(pin_name, 0);
	}
}

static void mipi_dsi_ST7701S_send_dcs(MIPI_TypeDef *MIPI_type_def, u8 cmd, u8 payload_len, u8 *para_list)
{
	u32 word0, word1, addr, idx;
	u8 cmd_addr[128];

	if (payload_len == 0) {
		MIPI_DSI_CMD_Send(MIPI_type_def, MIPI_DSI_DCS_SHORT_WRITE, cmd, 0);
		return;
	} else if (payload_len == 1) {
		MIPI_DSI_CMD_Send(MIPI_type_def, MIPI_DSI_DCS_SHORT_WRITE_PARAM, cmd, para_list[0]);
		return;
	}

	cmd_addr[0] = cmd;
	for (idx = 0; idx < payload_len; idx++) {
		cmd_addr[idx + 1 ] = para_list[idx];
	}

	payload_len = payload_len + 1;

	/* the addr payload_len 1 ~ 8 is 0 */
	for (addr = 0; addr < (u32)(payload_len + 7) / 8; addr++) {
		idx = addr * 8;
		word0 = (cmd_addr[idx + 3] << 24) + (cmd_addr[idx + 2] << 16) + (cmd_addr[idx + 1] << 8) + cmd_addr[idx + 0];
		word1 = (cmd_addr[idx + 7] << 24) + (cmd_addr[idx + 6] << 16) + (cmd_addr[idx + 5] << 8) + cmd_addr[idx + 4];

		MIPI_DSI_CMD_LongPkt_MemQWordRW(MIPI_type_def, addr, &word0, &word1, FALSE);
	}

	MIPI_DSI_CMD_Send(MIPI_type_def, MIPI_DSI_DCS_LONG_WRITE, payload_len, 0);
}

static void mipi_dsi_ST7701S_send_cmd(MIPI_TypeDef *MIPI_type_def, LCMSettingTable *table)
{
	static u8 send_cmd_idx_s = 0;
	u32 payload_len;
	u8 cmd, send_flag = FALSE;
	u8 *cmd_addr;

	while (1) {
		cmd = table[send_cmd_idx_s].cmd;

		switch (cmd) {
		case REGFLAG_DELAY:
			DelayMs(table[send_cmd_idx_s].count);
			break;
		case REGFLAG_END_OF_TABLE:
			send_cmd_idx_s = 0;
			printf("CMD Send Done\n");
			g_ST7701S_init_done = 1;
			g_first_flag = 0;
			return;
		default:
			if (send_flag) {
				return;
			}
			cmd_addr = table[send_cmd_idx_s].para_list;
			payload_len = table[send_cmd_idx_s].count;

			mipi_dsi_ST7701S_send_dcs(MIPI_type_def, cmd, payload_len, cmd_addr);

			send_flag = TRUE;
		}
		send_cmd_idx_s++;
	}
}


static void mipi_dsi_ST7701S_push_table(MIPI_TypeDef *MIPI_type_def, MIPI_InitTypeDef *MIPI_InitStruct, LCMSettingTable *table)
{
	MIPI_DSI_TO1_Set(MIPI_type_def, DISABLE, 0);
	MIPI_DSI_TO2_Set(MIPI_type_def, ENABLE, 0x7FFFFFFF);
	MIPI_DSI_TO3_Set(MIPI_type_def, DISABLE, 0);

	/*register MIPI ISR*/
	InterruptDis(MIPI_DSI_IRQ);
	InterruptUnRegister(MIPI_DSI_IRQ);
	InterruptRegister((IRQ_FUN)mipi_dsi_ST7701S_isr, MIPI_DSI_IRQ, (u32)MIPI, 10);
	InterruptEn(MIPI_DSI_IRQ, 10);
	MIPI_DSI_INT_Config(MIPI_type_def, DISABLE, ENABLE, FALSE);

	MIPI_DSI_init(MIPI_type_def, MIPI_InitStruct);

	g_ST7701S_init_done = FALSE;
	g_ST7701S_send_cmd = TRUE;

	while (1) {
		if (g_ST7701S_send_cmd) {
			g_ST7701S_send_cmd = 0;

			if (!g_ST7701S_init_done) {
				mipi_dsi_ST7701S_send_cmd(MIPI_type_def, table);
				rtos_time_delay_ms(1);
			} else {
				break;
			}
		}
	}
}

static void mipi_dsi_ST7701S_lcm_init(void)
{
	/* to prevent electric leakage */
	mipi_lcm_set_reset_pin(1);
	DelayMs(10);
	mipi_lcm_set_reset_pin(0);
	DelayMs(10);
	mipi_lcm_set_reset_pin(1);
	DelayMs(120);

	mipi_dsi_ST7701S_push_table(MIPI, &g_mipi_init_struct, g_ST7701S_init_cmd);
}

static void mipi_dsi_underflow_reset(void)
{
	u32 reg_val = MIPI_DSI_INTS_ACPU_Get(MIPI);
	MIPI_DSI_INTS_ACPU_Clr(MIPI, reg_val);

	if (reg_val) {
		g_under_flow_cnt = 0;
		MIPI_DSI_INT_Config(MIPI, DISABLE, DISABLE, DISABLE);

		/*Disable the LCDC*/
		LCDC_Cmd(LCDC, DISABLE);
		printf("Reset Mipi...\n");

		LCDC_Cmd(LCDC, ENABLE);
		while (!LCDC_CheckLCDCReady(LCDC));

		MIPI_DSI_Mode_Switch(MIPI, ENABLE);
	}
}

static u32 lcdc_underflow_isr(void *data)
{
	LCDC_TypeDef *LCDCx = (LCDC_TypeDef *)data;
	/*get LCDC interrupt status*/
	volatile u32 IntId = LCDC_GetINTStatus(LCDCx);

	if (IntId & LCDC_BIT_DMA_UN_INTS) {
		g_under_flow_cnt++;
		if (g_under_flow_cnt == 1) {
			printf("[warn] DMA Under--------------------\n");
			InterruptRegister((IRQ_FUN)mipi_dsi_underflow_reset, MIPI_DSI_IRQ, (u32)MIPI, 5);
			InterruptEn(MIPI_DSI_IRQ, 5);
			MIPI_DSI_INTS_ACPU_Clr(MIPI, MIPI_DSI_INTS_ACPU_Get(MIPI));
			MIPI_DSI_Mode_Switch(MIPI, DISABLE);
			MIPI_DSI_INT_Config(MIPI, ENABLE, ENABLE, ENABLE);
		}
	}

	if (IntId & LCDC_BIT_LCD_LIN_INTEN) {
		LCDC_ClearINT(LCDCx, LCDC_BIT_LCD_LIN_INTEN);
		if (g_callback) {
			g_callback->VBlank(g_data);
		}
	}

	return 0;
}

static void lcdc_enable(void)
{
	/*enable the LCDC*/
	LCDC_Cmd(LCDC, ENABLE);
	while (!LCDC_CheckLCDCReady(LCDC));

	MIPI_DSI_Mode_Switch(MIPI, ENABLE);
}

static void lcdc_init_config(int width, int height, int image_format)
{
	UNUSED(height);
	LCDC_StructInit(&g_lcdc_init_struct);
	g_lcdc_init_struct.LCDC_ImageWidth = width;
	g_lcdc_init_struct.LCDC_ImageHeight = height;

	g_lcdc_init_struct.LCDC_BgColorRed = 0;
	g_lcdc_init_struct.LCDC_BgColorGreen = 0;
	g_lcdc_init_struct.LCDC_BgColorBlue = 0;

	g_lcdc_init_struct.layerx[0].LCDC_LayerEn = ENABLE;

	if (image_format == RGB565) {
		image_format = LCDC_LAYER_IMG_FORMAT_RGB565;
		g_color_depth = 2;
	} else {
		image_format = LCDC_LAYER_IMG_FORMAT_ARGB8888;
		g_color_depth = 4;
	}

	g_lcdc_init_struct.layerx[0].LCDC_LayerImgFormat = image_format;
	g_lcdc_init_struct.layerx[0].LCDC_LayerHorizontalStart = 1;/*1-based*/
	g_lcdc_init_struct.layerx[0].LCDC_LayerHorizontalStop = width;
	g_lcdc_init_struct.layerx[0].LCDC_LayerVerticalStart = 1;/*1-based*/
	g_lcdc_init_struct.layerx[0].LCDC_LayerVerticalStop = height;

	LCDC_Init(LCDC, &g_lcdc_init_struct);
}



static void lcdc_display_init(int image_format)
{
	lcdc_init_config(LCDC_TEST_IMG_BUF_X, LCDC_TEST_IMG_BUF_Y, image_format);

	LCDC_DMAModeConfig(LCDC, LCDC_LAYER_BURSTSIZE_4X64BYTES);
	LCDC_DMADebugConfig(LCDC, LCDC_DMA_OUT_DISABLE, NULL);

	/*register LCDC ISR*/
	InterruptRegister((IRQ_FUN)lcdc_underflow_isr, LCDC_IRQ, (u32)LCDC, 10);
	InterruptEn(LCDC_IRQ, 10);

	/*line number*/
	LCDC_LineINTPosConfig(LCDC, LCDC_TEST_IMG_BUF_Y);

	/*enbale LCDC interrupt*/
	LCDC_INTConfig(LCDC, LCDC_BIT_LCD_LIN_INTS | LCDC_BIT_DMA_UN_INTS, ENABLE);

	lcdc_enable();
}

void st7701s_init(int image_format)
{
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_HPERI_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDCMIPI_CLOCK, ENABLE);

	MIPI_StructInit(&g_mipi_init_struct);
	MIPI_InitStruct_Config(&g_mipi_init_struct);
	MIPI_Init(MIPI, &g_mipi_init_struct);

	mipi_dsi_ST7701S_lcm_init();

	lcdc_display_init(image_format);
}

static void lcdc_layer_config(u8 *buffer)
{
	DCache_CleanInvalidate(buffer, LCDC_TEST_IMG_BUF_X * LCDC_TEST_IMG_BUF_Y * g_color_depth);
	g_lcdc_init_struct.layerx[0].LCDC_LayerImgBaseAddr = (u32)buffer;
	LCDC_LayerConfig(LCDC, LCDC_LAYER_LAYER1, &g_lcdc_init_struct.layerx[LCDC_LAYER_LAYER1]);
	LCDC_TrigerSHWReload(LCDC);
}

void st7701s_clean_invalidate_buffer(u8 *buffer)
{
	lcdc_layer_config(buffer);
}

void st7701s_get_info(int *width, int *height)
{
	*width = LCDC_TEST_IMG_BUF_X;
	*height = LCDC_TEST_IMG_BUF_Y;
}

void st7701s_register_callback(ST7701SVBlankCallback *callback, void *data)
{
	g_callback = callback;
	g_data = data;
}
