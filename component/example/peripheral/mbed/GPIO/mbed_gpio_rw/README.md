# Example Description

This example describes how to use GPIO read/write by MBED GPIO API.

# HW Configuration

- Requirement Components:
  - a LED
  - a push button
- Connect wires:
  - Connect a push button to `GPIO_PUSHBT_PIN` defined in `gpio_ext.h `and GND
  - Connect a LED to `GPIO_LED_PIN` defined in `gpio_ext.h` and GND
  
  For example:
  - On AmebaSmart, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_10` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_10` and GND
  - On AmebaLite, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_29` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_5` and GND
  - On AmebaDplus, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_12` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_10` and GND
  - On AmebaGreen2, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_26` and GND
    - Connect a LED to `GPIO_LED_PIN _PA_25` and GND

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
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
- AmebaD
- AmebaGreen2