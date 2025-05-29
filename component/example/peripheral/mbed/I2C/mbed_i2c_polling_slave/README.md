# Example Description

1. This example describes how to use i2c as the slave by using mbed api.
2. In this case, i2c serves as the slave and works in polling mode for reading or writing.

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

1. In this example, we use config I2C_RESTART_DEMO to decide if the slave will cooperate with the master for restart transmission.
   		If `#define I2C_RESTART_DEMO`, then slave will read ->read->write 
   		If not `#define I2C_RESTART_DEMO`, then slave will read -> write.

2. How to compile :
   		perform cmd: './build.py -a mbed_i2c_polling_slave' in directory {sdk_path}/amebaXXX_gcc_project

3. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. i2c_slave_rx_check will check if the received data is correct, and if it is consistent with src, it will print
   	"`Slave receive: Result is Success`"

# Note

1. This example can be tested with the i2c_polling_master
2. First run Slave and then Master.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
