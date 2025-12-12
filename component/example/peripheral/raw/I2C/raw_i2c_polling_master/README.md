# Example Description

1. This example describes how to use i2c as the master by using raw api.
2. In this case, i2c serves as the master and works in polling mode for reading or writing.

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

* On AMEBADGREEN2
  - master's`SDA (_PA_26)` connect to slave's `SDA`
  - master's `SCL (_PA_25)` connect to slave's `SCL`
  - master's `GND` connect to slave's `GND`

# SW Configuration

1. In this example, we use config I2C_RESTART_DEMO to decide if the master will generate a restart condition.
   		If `#define I2C_RESTART_DEMO`, then master will write ->restart->write -> stop ->write -> restart -> read -> stop.
   		If not `#define I2C_RESTART_DEMO`, then master will write ->stop->read -> stop.
2. In this example, we use config master_addr_retry to decide if the master will retry sending addr.
3. In this example, we use config I2C_LOOP_TEST to decide if the master will tx and rx 3 times.
4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
5. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. `i2c_master_rx_Check` will check if the received data is correct, and if it is consistent with src, it will print "`Master receive: Result is Success`"

# Note

1. This example can be tested with the i2c_polling_slave

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
