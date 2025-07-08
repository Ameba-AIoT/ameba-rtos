#ifndef __BSP_MCU_LCD_COM_H
#define __BSP_MCU_LCD_COM_H

#define MCU_LCD_8BIT_IF		(0)
#define MCU_LCD_16BIT_IF	(1)
#define MCU_LCD_24BIT_IF	(2)

typedef struct {
	u16 width;
	u16 height;
	u32 id;
	u8  dir;
	u16	wramcmd;
	u16 setxcmd;
	u16  setycmd;
	volatile u32 McuLcdBitMode;
} _lcd_dev;

extern _lcd_dev lcd_dev;

#define RGB888_WHITE_MASK	0x00FFFFFF
#define RGB888_RED_MASK 	0x00FF0000
#define RGB888_GREEN_MASK 	0x0000FF00
#define RGB888_BLUE_MASK 	0x000000FF

void LCD_WR_REG(u32 cmd);
void LCD_WR_DATA(u32 data);
u32 LCD_RD_DATA(void);
void LCD_Display_Dir(u8 dir);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u32 RGB_Code);
void LCD_SetWindow(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd);
void LCD_Clear(u32 color);
u32 LCD_ReadPixelFormat(u8 cmd);

#endif