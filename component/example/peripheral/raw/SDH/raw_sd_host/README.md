# Example Description

This example shows how to use SD host. It will erase, read and write SD card and check data. 

# HW Configuration

User should prepare a SD card and plug it into SD card socket before runnig this example. 

Pin defination:
|  PIN    | Description |
|  ----   | ----        |
| _PB_28  | SD_CLK      |
| _PB_27  | SD_CMD      |
| _PB_29  | SD_D0       |
| _PB_30  | SD_D1       |
| _PB_25  | SD_D2       |
| _PB_26  | SD_D3       |
| _PC_0   | SD_CD       |


# SW Configuration

# Expected Result

This example will erase and then read blocks to check if erase succeed. Then it will write erase and then read blocks to check if write succeed.

If all operation is successfull, log output is:
> Card Detect
> SD Host erase/read/write start.
> Erase Succeed
> Read Succeed
> Write Succeed
> Read Succeed

If fail, it will echo:
> Data check fail. Something wrong when erase or read data.

or it will echo:
> Data check fail. Something wrong when write or read data.

# Note

User may select different configuration by modify sdioh_config. The HW configuration should also be changed samely.

# Supported IC

* AmebaSmart  
