#  Example Description

This example shows how to use QSPI to driver LCD SH8601A. It use PPE to write data to LCD.

This example does not support the loguart command response.

# HW Configuration

PPE will carry data from memory to QSPI, user should connect QSPI pins with LCD SH8601A pins as following pin definition:

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

How to use:
* Copy `main.c` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
* Run `make all` and `download` image to board.

#  Expect result

SH8601A will show RED, GREEN, BLUE cyclically.

#  Note

None

#  Supported IC

* AmebaDplus


