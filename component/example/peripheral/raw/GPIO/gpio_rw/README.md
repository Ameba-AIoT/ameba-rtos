# Example Description

This example describes how to use GPIO read/write by RAW GPIO API.

This example does not support the loguart command response.

# HW Configuration

- Requirement Components:
  - a LED
  - a push button
- Connect wires:
  - Connect a push button to `GPIO_PUSHBT_PIN` defined in gpio_ext.h and GND
  - Connect a LED to `GPIO_LED_PIN` defined in gpio_ext.h and GND

# SW configuration

1. How to compile :

   - Copy main.c and gpio_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
2. `PA_12` map to `GPIOA_12`, `PB_8`  map to `GPIOB_8` and etc
3. `GPIO_PUSHBT_PIN` is configured as input with internal pull-high
4. `GPIO_LED_PIN` is configured as output

# Expect result

In this example, the LED is on when the push button is pressed.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
