#  Example Description

This example shows how to use LCDC to drive LCD ST7272A of 8-bit RGB888 mode.

# HW Configuration

LCDC will carry data from memory to LCD, user should connect LCDC pins with LCD ST7272A pins as following pin definition:

+--------+-------------+
| PIN    | Description |
+========+=============+
| PB_17  | LCD_D0      |
| PB_16  | LCD_D1      |
| PB_15  | LCD_D2      |
| PA_29  | LCD_D3      |
| PA_31  | LCD_D4      |
| PA_30  | LCD_D5      |
| PA_18  | LCD_D6      |
| PA_26  | LCD_D7      |
| PB_22  | LCD_HSYNC   |
| PA_27  | LCD_VSYNC   |
| PB_30  | LCD_DCLK    |
| PA_20  | LCD_DE      |
| PC_1   | LCD_BLE     |
+--------+-------------+

Also, the other pin of LCD should be connected correctly, like GND and VCC.

#  SW configuration

Cancel the comment about macro `ALLOC_BUFFER_FOR_DISPLAY` in src file of this demo.

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#  Expect result
ST7272A will show RED, GREEN, BLUE in turn.

#  Note
None

# Trouble shooting
When a display abnormality occurs, please check whether the pixel data in the frame buffer allocated by PSARM has been altered.

In this demo, three frame buffers are used, indicated by `PIX_DATA0`, `PIX_DATA1`, and `PIX_DATA2`. Their addresses are `0x60000000`, `0x60100000`, and `0x60200000`, respectively.

#  Supported IC

* AmebaGreen2


