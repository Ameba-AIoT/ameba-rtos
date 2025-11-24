#  Example Description

This example shows how to use SPI interface to driver ILI9341 LCD Module through PPE. PPE will carry data from SRAM to QSPI FIFO and then to LCD.

# HW Configuration

The following is PIN definition and how to connect with ILI9341 LCD module:

|  PIN   | Description | ILI9341 LCD module |
|  ----  | ----        | ----               |
| PA_19  | OSPI_CLK    | CLK                |
| PA_20  | OSPI_CSN    | CS1                |
| PA_21  | OSPI_DC_SEL | DC                 |
| PB_6   | OSPI_D0     | MOSI               |
| PA_22  | LCD_RST     | RES                |

Also, GND should connect with each other and VCC of ILI9341 LCD module should connect with DUT 3.3V.

#  SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#  Expect result

ILI9341 LCD will show RED, GREEN, BLUE in turn.

#  Note

None

#  Supported IC

- AmebaDplus
  * RTL8721Dx

