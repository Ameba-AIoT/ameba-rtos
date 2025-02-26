# Example Description

1. In this example, the control of the AT command module via SPI is simply demonstrated. In the example, the input to SPI is sourced from the UART interface, and the return output from the AT command module is sent to the UART interface.

3. In this example, two synchronized GPIOs need to be configured to notify the AT command SPI slave to send data and to receive the signal indicating that the slave is ready.

# HW Configuration

None

# SW configuration

1. Modify the SPI and UART configurations as needed. (like pins, spi clock frequence, uart baudrate).

2. How to use:
   - Run `./build.py -a atcmd_spi_master` under project path, e.g. `amebadplus_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

You can send AT command instructions through the UART port and view the results.

# Note

None

# Supported IC

- AmebaDplus
