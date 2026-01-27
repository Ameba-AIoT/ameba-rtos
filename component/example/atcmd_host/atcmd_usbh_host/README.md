# Example Description

In this example, the control of the AT command module via USB is simply demonstrated. In the example, the input to SPI is sourced from the UART interface, and the return output from the AT command module is sent to the UART interface.


# HW Configuration

None

# SW configuration
1. For atcmd device:
   a. Modify the USB configurations in atcmd_config.json.

   b. Build and Download:
      * Refer to the SDK Examples section of the online documentation to generate images.
      * `Download` images to board by Ameba Image Tool.

2. For atcmd host:
   a. ATCMD Mode of menuconfig should be configured as `Manual Test Mode`.

# Expect result

You can send AT command instructions through the UART port and view the results.

# Note

For more details, please refer to the atcmd USB section of the online document.

# Supported IC

RTL8730E
RTL8721F
