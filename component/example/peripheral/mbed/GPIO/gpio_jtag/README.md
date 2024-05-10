# Example Description

This example describes how to use disable JTAG module and use GPIO pin to blink led by MBED GPIO API.

This example does not support the loguart command response.

# HW Configuration

- Requirement Components:

  - a LED
  - a push button
- Connect wires:

  - Connect a push button to `GPIO_JTAG_ENABLE_PIN` defined in `gpio_ext.h` and GND
  - Connect a LED to `GPIO_LED_PIN` defined in `gpio_ext.h` and GND

  For example:

  - On AmebaSmart, connect as below
    - Connect a push button to `GPIO_JTAG_ENABLE_PIN _PA_5` and GND
    - Connect a LED to `GPIO_LED_PIN _PA_14` and GND
  - On AmebaLite, connect as below
    - Connect a push button to `GPIO_JTAG_ENABLE_PIN _PA_29` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_1` and GND
  - On AmebaDplus, connect as below
    - Connect a push button to `GPIO_JTAG_ENABLE_PIN _PA_12` and GND
    - Connect a LED to `GPIO_LED_PIN _PA_30` and GND

# SW configuration

- How to compile :
  - Copy main.c and gpio_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
- `PA_12` map to `GPIOA_12`, `PB_8`  map to `GPIOB_8` and etc
- `GPIO_JTAG_ENABLE_PIN` is configured as input with internal pull-high
- `GPIO_LED_PIN` is configured as output

# Expect result

1. If button is not pressed while device boot up, then we don't turn off jtag module.
2. If button is pressed while device boot up, then jtag module is turned off.
3. If jtag module is turned off, then we blink led.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
