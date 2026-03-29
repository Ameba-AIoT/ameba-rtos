#  Example Description

This example shows how to use SDIO device. It will receive data from SDIO host and loopback data to SDIO host each 1 second.

# HW Configuration

User should prepare a SDIO host with Linux System. Realtek SDIO host driver should be compiled into Kernel and set to loopback mode. Then connect SDIO device with SDIO host. Refer to SDIO application note to get more information.

Pin defination:
|  PIN    | Description |
|  ----   | ----        |
| _PB_26  | SD_CLK      |
| _PB_25  | SD_CMD      |
| _PB_27  | SD_D0       |
| _PB_28  | SD_D1       |
| _PB_23  | SD_D2       |
| _PB_24  | SD_D3       |


#  SW configuration

Realtek SDIO host driver should set to loopback mode. Refer to SDIO application note to get more information.

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

#  Expect result

SDIO host will send data to SDIO device, and SDIO device will check if data is correct. Then it will loop it back to SDIO host and SDIO also check the loopback data.

1. If data is received corretly, SDIO device log output is:
> loopback package, size = 2032 (cnt = 1) heap=xxx
> loopback package, size = 2032 (cnt = 2) heap=xxx
> loopback package, size = 2032 (cnt = 3) heap=xxx
> ...

Or it will echo:
> Loopback data check fail:x, x, x
> Loopback data check fail:x, x, x
> ...

2. If data is send and received corretly, SDIO Host log output is:
> Loopback send successed [1]!, sz=2048
> Loopback recv successed [1], size=2048!
> Loopback send successed [2]!, sz=2048
> Loopback recv successed [2], size=2048!
> ....

Or it will echo:
> fail: x, x, x
> fail: x, x, x
> ...

#  Note

- Refer to sdio_loopback_test_guide.pdf to get more information about running SDIO host.
- SDIO host sends 2048 bytes, including 16-byte tx descriptor and 2032-byte payload.

#  Supported IC

RTL8721Dx


