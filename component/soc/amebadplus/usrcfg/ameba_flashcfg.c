/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/**
* @brif Indicate the flash baudrate. It can be one of the following value:
*	CLKDIV(10): => SPIC clock = 1/10 pll
*	CLKDIV(9): => SPIC clock = 1/9 pll
*	CLKDIV(8): => SPIC clock = 1/8 pll
*	CLKDIV(7): => SPIC clock = 1/7 pll
*	CLKDIV(6): => SPIC clock = 1/6 pll
*	CLKDIV(5): => SPIC clock = 1/5 pll
*	CLKDIV(4): => SPIC clock = 1/4 pll
*	CLKDIV(3): => SPIC clock = 1/3 pll
*	CLKDIV(2): => SPIC clock = 1/2 pll
*	other value is not support.
*/
const u16 Flash_Speed = CLKDIV(2);
const u16 Data_Flash_Speed = CLKDIV(2);

/**
* @brif Indicate the flash read I/O mode. It can be one of the following value:
*	0xFFFF: Read quad IO, Address & Data 4 bits mode
*	0x7FFF: Read quad O, Just data 4 bits mode
*	0x3FFF: Read dual IO, Address & Data 2 bits mode
*	0x1FFF: Read dual O, Just data 2 bits mode
*	0x0FFF: 1 bit mode
* @note If the configured read mode is not supported, other modes would be searched until find the appropriate mode.
*/
const u16 Flash_ReadMode = 0xFFFF;
const u16 Data_Flash_ReadMode = 0xFFFF;


/**
* @brif Flash_AVL maintains the flash IC supported by SDK.
*	If users want to adpot new flash, add item in the following AVL.
*	Note that, if new flash can be classfied as one of the Realtek-defined categories according to Classification SPEC,
*	filling in the defined class index is necessary. Otherwise, FlashClassUser can be used to indicate new class.
*	If (Status Register of flash & status_mask in Flash_AVL) != (1 << QE Bit), set (1 << QE Bit) to Status Register of flash
*/
const FlashInfo_TypeDef Flash_AVL[] = {
	/*flash_id,		flash_id_mask,	flash_class,		status_mask,	FlashInitHandler */

	/* case1: Realtek defined class, any modification is not allowed */
	{0xEF,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* Winbond: MANUFACTURER_ID_WINBOND */
	{0xA1,			0x000000FF,		FlashClass1,		0x0000FFFC,		NULL}, 	/* Fudan Micro: MANUFACTURER_ID_FM */
	{0x0B,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX */
	{0x0E,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX(FT) */
	{0xC8,			0x000000FF,		FlashClass2,		0x000043FC,		NULL},	/* GD normal: MANUFACTURER_ID_GD */
	{0x28C2,		0x0000FFFF,		FlashClass6,		0x000200FC,		NULL},	/* MXIC wide-range VCC: MANUFACTURER_ID_MXIC */
	{0xC2,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* MXIC normal: MANUFACTURER_ID_BOHONG */
	{0x68,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* Hua Hong */
	{0x51,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* GD MD serial */
	{0x1C,			0x000000FF,		FlashClass4,		0x000000FC,		NULL},	/* ESMT: MANUFACTURER_ID_EON */
	{0x20,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XMC: MANUFACTURER_ID_WINBOND */
	{0x85,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* Puya: 85-20-16 */
	{0x5E,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* Zbit: 5E-50-16 */
	//{0x20,			0x000000FF,		FlashClass5,		0x000000FC,		NULL},	/* Micron: MANUFACTURER_ID_MICRON */

	/* case2: new flash, ID is not included in case1 list, but specification is compatible with FlashClass1~FlashClass6 */
	//{0xXX,			0x0000XXXX,		FlashClassX,		0x0000XXXX,		NULL},

	/* case3: new flash, ID is not included in case1 list, and specification is not compatible with FlashClass1~FlashClass6 */
	{0x00,			0x000000FF,		FlashClassUser,		0xFFFFFFFF,		&flash_init_userdef},

	/* End */
	{0xFF,			0xFFFFFFFF,		FlashClassNone,		0xFFFFFFFF,		NULL},
};

/*
* @brif	Flash layout is set according to Flash Layout in User Manual
*  In each entry, the first item is flash regoin type, the second item is start address, the second item is end address */
const FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/*Region_Type,	[StartAddr,	EndAddr]		*/
	{IMG_BOOT, 		0x08000000, 0x08013FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1, 0x08014000, 0x081F3FFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA1 + Manifest(4K) + RDP IMG OTA1

	{IMG_BOOT_OTA2, 0x08200000, 0x08213FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K) OTA
	{IMG_APP_OTA2, 0x08214000, 0x083DCFFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA2 + Manifest(4K) + RDP IMG OTA2

	{FTL,			0x083DD000, 0x083DFFFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1, 			0x083E0000, 0x083FFFFF}, //VFS region 1 (128K)
	{VFS2, 			0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER, 			0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user

	/* End */
	{0xFF, 			0xFFFFFFFF, 0xFFFFFFFF},
};

/**
 * @brief  To adopt user defined class, need to initialize the parameters in the function below
 *       with its default values according to flash Spec, and add additional config code if need.
 * @param none.
 * @retval none
 * @note	The following parameter setting is an example of MXIC flash. For users reference.
 */
void flash_init_userdef(void)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/*1. Initialize CMD parameters */

	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;

	/*1.1 status bit define */
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(6);
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/*1.2 dummy cycle */
	FLASH_InitStruct->FLASH_rd_dummy_cyle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[1] = FLASH_DM_CYCLE_2O;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[2] = FLASH_DM_CYCLE_2IO;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[3] = FLASH_DM_CYCLE_4O;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[4] = FLASH_DM_CYCLE_4IO;

	/*1.3 set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;

	/*1.4 set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;

	/*1.5 other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = 0x06;
	FLASH_InitStruct->FLASH_cmd_rd_id = 0x9F;
	FLASH_InitStruct->FLASH_cmd_rd_status = 0x05;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = 0x01;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0x60;
	FLASH_InitStruct->FLASH_cmd_block_e = 0xD8;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0x20;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0xAB;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0xB9;

	/*2. Add other configuration code here if needed for flash */
	//TODO

}

u32 flash_get_readmode(u32 ReadMode)
{
	u32 flash_bit_mode;
	switch (ReadMode) {
	case 0xFFFF:
		flash_bit_mode = Spic4IOBitMode;
		break;
	case 0x7FFF:
		flash_bit_mode = Spic4OBitMode;
		break;
	case 0x3FFF:
		flash_bit_mode = Spic2IOBitMode;
		break;
	case 0x1FFF:
		flash_bit_mode = Spic2OBitMode;
		break;
	default:
		flash_bit_mode = Spic1IOBitMode;
		break;
	}

	return flash_bit_mode;
}

FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id)
{
	u32 temp, i = 0;
	FlashInfo_TypeDef *pAVL = (FlashInfo_TypeDef *)Flash_AVL;

	while (pAVL[i].flash_class != FlashClassNone) {
		temp = flash_id & pAVL[i].id_mask;
		if (pAVL[i].flash_id == temp) {
			return &pAVL[i];
		}

		i++;
	}

	return NULL;
}

void flash_get_layout_info(u32 type, u32 *start, u32 *end)
{
	u32 i = 0;
	u32 tmp_type;
	while (Flash_Layout[i].region_type != 0xFF) {
		tmp_type = Flash_Layout[i].region_type;
		if (tmp_type == type) {
			if (start != NULL) {
				*start = Flash_Layout[i].start_addr;
			}
			if (end != NULL) {
				*end = Flash_Layout[i].end_addr;
			}
			return;
		}
		i++;
	}
}