# Example Description

This example is designed for firmware update via UART Connection. Different from the `ota_http/ota_https`, it doesn't need to connect the network. Once the board connect to the serial port with script executed on the PC side, the board can start receiving the firmware and programing. It is applicable in scenarios of user-customized upgrades.

Download `ota_all.bin` from the download server (in `tools\DownloadServer(UART)`) automatically.

# HW Configuration

Required Components: UART adapter. Configure the UART PINs with the actual board.

Connect the UART adapter with PC.

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. Configure the script according to `tools\DownloadServer(UART)\readme.txt` first then execute the script.

3. Reset the board and start the download through serial port.

# Expect result

A uart download example thread will be started automatically when booting. When transfer finished, the board will boot from the new firmware directly if the verificaction is successful.

Using the example with the tool in `tools\DownloadServer(UART)` with ota_all.bin file.

# Note

Remember to Set the script `uart_user_ota.py` with the actual `port`.

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
