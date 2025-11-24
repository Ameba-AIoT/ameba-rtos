#  Example Description

This example shows how to use QSPI to driver LCD SH8601A. CPU will write data to QSPI FIFO and then to LCD.

# HW Configuration

CPU will carry data from memory to QSPI, user should connect QSPI pins with LCD SH8601A pins as following pin definition:

|  PIN   | Description |
|  ----  | ----        |
| PA_12  | OSPI_CLK    |
| PA_24  | OSPI_CSN    |
| PA_25  | OSPI_DC_SEL |
| PA_26  | OSPI_D0     |
| PA_27  | OSPI_D1     |
| PB_6   | OSPI_D2     |
| PB_7   | OSPI_D3     |
| PA_22  | LCD_RST     |

Also, the other pin of LCD should be connected correctly, like GND and VCC.

#  SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#  Expect result
SH8601A will show RED, GREEN, BLUE in turn.

#  Note

None

#  Supported IC

- AmebaDplus
  * RTL8721Dx


