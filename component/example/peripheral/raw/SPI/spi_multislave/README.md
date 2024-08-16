# Example Description

1. This example shows master sends data to multiple slaves by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Master.
3. This interface can be used for communication with SPI slave devices, such as FLASH memory, LCD screens and other modules or integrated circuits.

This example does not support the loguart command response.

# HW Configuration

- master's `MOSI` connect to slave1's `MOSI` & slave2's `MOSI`
- master's `MISO` connect to slave1's `MISO `& slave2's `MISO`
- master's `SCLK` connect to slave1's `SCLK` & slave2's `SCLK`
- master's `SPI_GPIO_CS0` connect to slave1's `CS`
- master's `SPI_GPIO_CS1` connect to slave2's `CS`
- the related Master's and Slave's pins are defined in spi_ext.h

For example:

- On AmebaSmart, connect as below

  - master's `MOSI (_PA_5)` connect to slave1's `MOSI (_PA_13)` & slave2's `MOSI (_PA_13)`
  - master's `MISO (_PA_4)` connect to slave1's `MISO (_PA_14)`& slave2's `MISO (_PA_14)`
  - master's `SCLK (_PA_3)` connect to slave1's `SCLK (_PA_15)` & slave2's `SCLK (_PA_15)`
  - master's `SPI_GPIO_CS0 (PA_0)` connect to slave1's `CS (_PA_16)`
  - master's `SPI_GPIO_CS1 (PA_1)` connect to slave2's `CS (_PA_16)`
- On AmebaLite, connect as below

  - master's `MOSI (_PB_3)` connect to slave1's `MOSI (_PA_29)` & slave2's `MOSI (_PA_29)`
  - master's `MISO (_PB_4)` connect to slave1's `MISO (_PA_30)` & slave2's `MISO (_PA_30)`
  - master's `SCLK (_PB_2)` connect to slave1's `SCLK (_PA_28)`& slave2's `SCLK (_PA_28)`
  - master's `SPI_GPIO_CS0 (PA_0)` connect to slave1's `CS (_PA_31)`
  - master's `SPI_GPIO_CS1 (PA_1)` connect to slave2's `CS (_PA_31)`
- On AmebaDplus, connect as below

  - master's `MOSI (_PB_19)` connect to slave1's `MOSI (_PB_24)`& slave2's `MOSI (_PB_24)`
  - master's `MISO (_PB_20)` connect to slave1's `MISO (_PB_25)`& slave2's `MISO (_PB_25)`
  - master's `SCLK (_PB_18)` connect to slave1's `SCLK (_PB_23)`& slave2's `SCLK (_PB_23)`
  - master's `SPI_GPIO_CS0 (_PB_30)` connect to slave1's `CS (_PB_26)`
  - master's `SPI_GPIO_CS1 (_PB_31)` connect to slave2's `CS (_PB_26)`

# SW configuration

1. How to compile :
   - Copy main.c and spi_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
   - To build image for master device, `SPI_IS_AS_MASTER` need to be set 1
   - To build image for slave device, `SPI_IS_AS_MASTER` need to be set 0
2. In this example, we use config SPI_IS_AS_MASTER to decide if device is master or slave.
   - If `SPI_IS_AS_MASTER` is 1, then device is master.
   - If `SPI_IS_AS_MASTER` is 0, then device is slave.
3. By default it use SPI0 as slave, and use SPI1 as master.
4. User can configure several slave selected lines of the master.

# Expect result

1. At first, slave1 would receive data in decreasing order in a loop, while slave2 which is not selected by master receives nothing and generates rx timeout at the same time.
2. In the next loop, slave2 selected by the master receives data in increasing order, while slave1 receives nothing and generates rx timeout.

# Note

1. Para spi_idx should be assigned first in the initialization process.
2. SPI1 is set as Master and SPI0 is set as Slave.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
