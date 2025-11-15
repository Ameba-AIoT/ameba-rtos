#  Example Description

This example describes how to use PPE as a DMA, which means no other process, just carry origin data. 

It has 2 cases, case1 shows how to carry data from memory to memory, case2 shows that from memory to QSPI (use LCD SH8601A as an example). 

# HW Configuration

For the case that carry data from memory to QSPI, user should connect QSPI pins with LCD SH8601A pins with following pin definition:

|  PIN   | Description |
|  ----  | ----        |
| PA_12  | OSPI_CLK    |
| PA_24  | OSPI_CSN    |
| PA_25  | OSPI_DC_SEL |
| PA_26  | OSPI_D0     |
| PA_27  | OSPI_D1     |
| PB_6   | OSPI_D2     |
| PB_7   | OSPI_D3     |

#  SW configuration

1. User can choose macro ACCESS to choose these 2 case. Value MEM2MEM means case1, and value MEM2QSPI means case2.
2. Build and Download:
* Refer to the SDK Examples section of the online documentation to generate images.
* `Download` images to board by Ameba Image Tool.

#  Expect result

- For case1, PPE will carry origin data each 500ms, and then echo "ppe transfer over". If there are data error, It will echo
" [error] has occured in alpha blending function!".
- For case2, PPE will carry RED, GREEN, BLUE data in turn each 500ms, and echo "Red" "Green" "Blue" on console. Also, SH8601A will show red, green, blue color in turn.

#  Note

None

#  Supported IC

- AmebaDplus
  * RTL8721Dx


