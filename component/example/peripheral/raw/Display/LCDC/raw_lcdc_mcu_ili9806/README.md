#  Example Description

This example shows how to use LCDC to drive LCD ILI9806 of 24-bit RGB888 mode.

# HW Configuration

LCDC will carry data from memory to LCD, user should connect LCDC pins with LCD ILI9806 pins as following pin definition:

+--------+------------------------+
| PIN    | Description           |
+========+=======================+
| PB_31  | LCD_D0 (B0)           |
| PB_29  | LCD_D1                |
| PB_27  | LCD_D2                |
| PB_25  | LCD_D3                |
| PB_24  | LCD_D4                |
| PB_23  | LCD_D5                |
| PB_18  | LCD_D6                |
| PB_14  | LCD_D7 (B7)           |
| PB_13  | LCD_D8 (G0)           |
| PB_0   | LCD_D9                |
| PA_25  | LCD_D10               |
| PA_28  | LCD_D11               |
| PA_19  | LCD_D12               |
| PA_23  | LCD_D13               |
| PA_22  | LCD_D14               |
| PA_21  | LCD_D15 (G7)          |
| PC_0   | LCD_D16 (R0)          |
| PB_30  | LCD_D17               |
| PB_28  | LCD_D18               |
| PB_26  | LCD_D19               |
| PA_27  | LCD_D20               |
| PB_22  | LCD_D21               |
| PB_21  | LCD_D22               |
| PB_17  | LCD_D23 (R7)          |
| PA_30  | LCD_MCU_DCX           |
| PA_31  | LCD_MCU_WR            |
| PA_29  | LCD_MCU_RD            |
| PB_15  | LCD_MCU_CSX           |
| PC_1   | LCD_BLE               |
+--------+-----------------------+

Also, the other pin of LCD should be connected correctly, like GND and VCC.

#  SW configuration

Cancel the comment about macro `ALLOC_BUFFER_FOR_DISPLAY` in src file of this demo.

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#  Expect result
ILI9806 will show RED, GREEN, BLUE in turn.

#  Note
None

# Trouble shooting
When a display abnormality occurs, please check whether the pixel data in the frame buffer allocated by PSARM has been altered.

In this demo, three frame buffers are used, indicated by `PIX_DATA0`. Its address is `0x60000000`.

#  Supported IC

* AmebaGreen2


