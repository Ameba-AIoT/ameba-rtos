# Example Description

1. This example shows master sends data to multiple slaves by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Master.
3. This interface can be used for communication with SPI slave devices, such as FLASH memory, LCD screens and other modules or integrated circuits.

# HW Configuration

- master's `MOSI` connect to slave1's `MOSI` & slave0's `MOSI`
- master's `MISO` connect to slave1's `MISO `& slave0's `MISO`
- master's `SCLK` connect to slave1's `SCLK` & slave0's `SCLK`
- master's `SPI_GPIO_CS0` connect to slave0's `CS`
- master's `SPI_GPIO_CS1` connect to slave1's `CS`
- The related Master's and Slave's pins are defined in the current example's header file
  - The relevant header file can be found in the directory:
    `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
In this way, slave1 will be selected first as software configuration.

For example:

- On AmebaSmart, connect as below

  - master's `MOSI (_PA_5)` connect to slave1's `MOSI (_PA_13)` & slave0's `MOSI (_PA_13)`
  - master's `MISO (_PA_4)` connect to slave1's `MISO (_PA_14)`& slave0's `MISO (_PA_14)`
  - master's `SCLK (_PA_3)` connect to slave1's `SCLK (_PA_15)` & slave0's `SCLK (_PA_15)`
  - master's `SPI_GPIO_CS0 (PA_0)` connect to slave0's `CS (_PA_16)`
  - master's `SPI_GPIO_CS1 (PA_1)` connect to slave1's `CS (_PA_16)`
- On AmebaLite, connect as below

  - master's `MOSI (_PB_3)` connect to slave1's `MOSI (_PA_29)` & slave0's `MOSI (_PA_29)`
  - master's `MISO (_PB_4)` connect to slave1's `MISO (_PA_30)` & slave0's `MISO (_PA_30)`
  - master's `SCLK (_PB_2)` connect to slave1's `SCLK (_PA_28)`& slave0's `SCLK (_PA_28)`
  - master's `SPI_GPIO_CS0 (PA_0)` connect to slave0's `CS (_PA_31)`
  - master's `SPI_GPIO_CS1 (PA_1)` connect to slave1's `CS (_PA_31)`
- On AmebaDplus, connect as below

  - master's `MOSI (_PB_19)` connect to slave1's `MOSI (_PB_24)`& slave0's `MOSI (_PB_24)`
  - master's `MISO (_PB_20)` connect to slave1's `MISO (_PB_25)`& slave0's `MISO (_PB_25)`
  - master's `SCLK (_PB_18)` connect to slave1's `SCLK (_PB_23)`& slave0's `SCLK (_PB_23)`
  - master's `SPI_GPIO_CS0 (_PB_30)` connect to slave0's `CS (_PB_26)`
  - master's `SPI_GPIO_CS1 (_PB_31)` connect to slave1's `CS (_PB_26)`
- On AmebaGreen2, connect as below

  - master's `MOSI (_PB_30)` connect to slave1's `MOSI (_PA_30)`& slave0's `MOSI (_PA_30)`
  - master's `MISO (_PB_31)` connect to slave1's `MISO (_PA_31)`& slave0's `MISO (_PA_31)`
  - master's `SCLK (_PB_29)` connect to slave1's `SCLK (_PA_29)`& slave0's `SCLK (_PA_29)`
  - master's `SPI_GPIO_CS0 (_PB_26)` connect to slave0's `CS (_PB_0)`
  - master's `SPI_GPIO_CS1 (_PB_27)` connect to slave1's `CS (_PB_0)`

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
   - To build image for master device, `SPI_IS_AS_MASTER` need to be set 1
   - To build image for slave device, `SPI_IS_AS_MASTER` need to be set 0
2. In this example, we use config SPI_IS_AS_MASTER to decide if device is master or slave.
   - If `SPI_IS_AS_MASTER` is 1, then device is master.
   - If `SPI_IS_AS_MASTER` is 0, then device is slave.
3. By default it use SPI0 as slave, and use SPI1 as master.
4. User can configure several slave selected lines of the master.

# Expect result

1. At first, slave1 would receive data in decreasing order in a loop, while slave0 which is not selected by master receives nothing and generates rx timeout at the same time.
2. In the next loop, slave0 selected by the master receives data in increasing order, while slave1 receives nothing and generates rx timeout.

# Note

1. Para spi_idx should be assigned first in the initialization process.
2. SPI1 is set as Master and SPI0 is set as Slave.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
