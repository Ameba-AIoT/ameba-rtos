# Example Description

In this example, the control of the AT command module via SDIO is simply demonstrated. In the example, the input to SDIO is sourced from the STM32 UART interface, and the return output from the AT command module is sent to the STM32 UART interface.

# HW Configuration

None

# SW configuration

1. About SDIO device, modify the SDIO configurations in atcmd_config.json as needed.
2. About SDIO host:
   a. Based on STM32F429 platform, compile and burn using Keil5.
   b. The files in the following directory of SDIO host project can be obtained from stm32 or freertos official website
      - STARTUP
      - CMSIS
      - STM32F4xx_StdPeriph_Driver
      - FreeRTOS-Source (v9.0.0)
      - FreeRTOS-Source-portable (v9.0.0)

# Expect result

You can send AT command instructions through the UART port and view the results.

# Note

For more details, please refer to the atcmd SDIO section of the online document.

# Supported IC

RTL8721Dx
