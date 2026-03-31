#include "ameba_soc.h"

#include "bsp_mcu_com.h"
#include "bsp_mcu_ili9806.h"

_lcd_dev lcd_dev;

void LCD_WR_REG(u32 cmd)
{
	LCDC_MCUIOWriteCmd(LCDC, cmd);
}

void LCD_WR_DATA(u32 data)
{
	LCDC_MCUIOWriteData(LCDC, data);
}

u32 LCD_RD_DATA(void)
{
	return LCDC_MCUIOReadData(LCDC);
}

void LCD_Display_Dir(u8 dir)
{
	if (dir == 0) {
		lcd_dev.dir = 0;

		lcd_dev.width = LCD_ILI9806_WIDTH;
		lcd_dev.height = LCD_ILI9806_HEIGHT;

		lcd_dev.wramcmd = 0x2C;
		lcd_dev.setxcmd = 0x2A;
		lcd_dev.setycmd = 0x2B;
	} else {
		lcd_dev.dir = 1;

		lcd_dev.width = LCD_ILI9806_HEIGHT;
		lcd_dev.height = LCD_ILI9806_WIDTH;

		lcd_dev.wramcmd = 0x2C;
		lcd_dev.setxcmd = 0x2A;
		lcd_dev.setycmd = 0x2B;
	}
	//LCD_Scan_Dir(DFT_SCAN_DIR);
}

void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcd_dev.wramcmd);
}

/* for 24bit IF, RGB888 */
void LCD_WriteRAM(u32 RGB_Code)
{
	if (lcd_dev.McuLcdBitMode == MCU_LCD_24BIT_IF) {
		RGB_Code &= 0x00FFFFFF;//RGB888_MASK;
		LCD_WR_DATA(RGB_Code);

	} else if (lcd_dev.McuLcdBitMode == MCU_LCD_16BIT_IF) {
		RGB_Code &= 0x0000FFFF;//RGB565_MASK
		LCD_WR_DATA(RGB_Code);

	} else if (lcd_dev.McuLcdBitMode == MCU_LCD_8BIT_IF) {
		RGB_Code &= 0x00FFFFFF;//RGB888_MASK;
		LCD_WR_DATA((RGB_Code >> 16) & 0xFF);
		LCD_WR_DATA((RGB_Code >> 8) & 0xFF);
		LCD_WR_DATA((RGB_Code) & 0xFF);
	}
}

void LCD_SetWindow(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd)
{
	LCD_WR_REG(lcd_dev.setxcmd);
	LCD_WR_DATA(xStar >> 8);		//SC[17:8]
	LCD_WR_DATA(xStar & 0XFF);		//SC[7:0]
	LCD_WR_DATA(xEnd >> 8);			//EC[17:8]
	LCD_WR_DATA(xEnd & 0XFF);		//EC[7:0]

	LCD_WR_REG(lcd_dev.setycmd);
	LCD_WR_DATA(yStar >> 8);		//SP[17:8]
	LCD_WR_DATA(yStar & 0XFF); 		//SP[7:0]
	LCD_WR_DATA(yEnd >> 8);			//EP[17:8]
	LCD_WR_DATA(yEnd & 0XFF);		//EP[8:0]
}

void LCD_Clear(u32 color)
{
	u32 index = 0;
	u32 totalpoint = lcd_dev.width;
	totalpoint *= lcd_dev.height;

	/* send cmd and para for (x1,y1) and(x2, y2) */
	LCD_SetWindow(0, 0, lcd_dev.width - 1, lcd_dev.height - 1);

	/* send cmd: write mem */
	LCD_WriteRAM_Prepare();

	/* send cmd's parameter: pixel data */
	for (index = 0; index < totalpoint; index++) {
		/* clear screen to white */
		LCD_WriteRAM(color);
	}
}

u32 LCD_ReadID(u8 cmd)
{
	u32 id;

	LCD_WR_REG(cmd);

	LCD_RD_DATA();	//dummy read

	id = LCD_RD_DATA();
	id <<= 16;
	id |= (LCD_RD_DATA() << 8);
	id |= LCD_RD_DATA();

	return id;
}

u32 LCD_ReadPixelFormat(u8 cmd)
{
	u32 bpp;

	LCD_WR_REG(cmd);

	LCD_RD_DATA();	//dummy read

	bpp = (u32)LCD_RD_DATA();

	return bpp;
}