# Example Description

1. This example describes how to use i2c as the slave by using raw api.
2. In this demo code, i2c as the slave, can perform rx and tx 3 times in interrupt mode.

# HW Configuration

slave's SDA connect to master's SDA 
slave's SCL connect to master's SCL 
slave's GND connect to master's GND 

* On AMEBASMART
    - slave's `SDA (_PA_9)` connect to master's `SDA`
    - slave's  `SCL (_PA_10)` connect to master's `SCL` 
    - slave's  `GND` connect to slave's `GND` 

* On AMEBALITE
  - slave's `SDA (_PA_29)` connect to master's `SDA`
  - slave's `SCL (_PA_30)` connect to master's `SCL` 
  - slave's `GND` connect to master's `GND` 

* On AMEBADPLUS
  - slave's`SDA (_PB_15)` connect to master's `SDA`
  - slave's `SCL (_PB_16)` connect to master's `SCL` 
  - slave's `GND` connect to master's `GND` 

# SW Configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. i2c has successfully transmitted the data.

# Note

1. This example can be tested with the i2c_int_master.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
* AmebaGreen2
