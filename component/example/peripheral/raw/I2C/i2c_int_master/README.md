# Example Description

1. This example describes how to use i2c as the master by using raw api.
2. In this demo code, i2c as the master, can perform tx and rx 3 times in interrupt mode. The interrupt handler and driver alternately use semaphore, which reduces cycle waiting and is simple to use.
3. The user needs to provide the semaphore of tx or rx and interface for acquiring and releasing semaphores.

This example does not support the loguart command response.

# HW Configuration

master's SDA connect to slave's SDA 

master's SCL connect to slave's SCL 

master's GND connect to slave's GND 

* On AMEBASMART
  - master's `SDA (_PA_9)` connect to slave's `SDA`
  - master's  `SCL (_PA_10)` connect to slave's `SCL` 
  - master's  `GND` connect to slave's `GND` 

* On AMEBALITE
  - master's `SDA (_PA_29)` connect to slave's `SDA`
  - master's `SCL (_PA_30)` connect to slave's `SCL` 
  - master's `GND` connect to slave's `GND` 

* On AMEBADPLUS
  - master's`SDA (_PB_15)` connect to slave's `SDA`
  - master's `SCL (_PB_16)` connect to slave's `SCL` 
  - master's `GND` connect to slave's `GND` 

# SW Configuration

1. How to compile :
   	(1)copy main.c and i2c_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

   ​ (2) make and download new image.

2. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. i2c has successfully transmitted the data.

# Note

1. This example can be tested with the i2c_int_slave

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus