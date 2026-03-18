# Example Description

1. This example describes how to use i2c by using raw api.
2. This test needs two demo board, one as master and the other as slave.

# HW Configuration

slave's SDA connect to master's SDA 
slave's SCL connect to master's SCL 
slave's GND connect to master's GND 

* On RTL8730E
  - slave's `SDA (_PA_9)` connect to master's `SDA`
  - slave's  `SCL (_PA_10)` connect to master's `SCL` 
  - slave's  `GND` connect to slave's `GND` 

* On RTL8726E/RTL8720E/RTL8713E/RTL8710E
  - slave's `SDA (_PA_29)` connect to master's `SDA`
  - slave's `SCL (_PA_30)` connect to master's `SCL` 
  - slave's `GND` connect to master's `GND` 

* On RTL8721Dx
  - slave's`SDA (_PB_15)` connect to master's `SDA`
  - slave's `SCL (_PB_16)` connect to master's `SCL` 
  - slave's `GND` connect to master's `GND` 

* On RTL8721F
  - slave's`SDA (_PA_26)` connect to slave's `SDA`
  - slave's `SCL (_PA_25)` connect to slave's `SCL`
  - slave's `GND` connect to slave's `GND`

# SW Configuration

1. In this example, we use config I2C_RESTART_DEMO to decide if the slave will cooperate with the master for restart transmission.
   		If `#define I2C_RESTART_DEMO`, then slave will read ->read->write 
   		If not `#define I2C_RESTART_DEMO`, then slave will read -> write.
2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in RTL8730E, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. i2c_slave_rx_check will check if the received data is correct, and if it is consistent with src, it will print
   	"`Slave receive: Result is Success`"

# Note

1. This example can be tested with the i2c_polling_master.
2. First run Slave and then Master.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
