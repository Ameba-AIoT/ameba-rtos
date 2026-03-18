# Example Description

This example describes how to use GPIO read/write in a light weight way by MBED GPIO API.

# HW Configuration

- Requirement Components:

  - a LED
  - a push button
- Connect wires:

  - Connect a push button to `GPIO_PUSHBT_PIN` and GND
  - Connect a LED to `GPIO_LED_PIN` and GND

  For example:

  - On RTL8730E, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_10 ` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_10` and GND
  - On RTL8726E/RTL8720E/RTL8713E/RTL8710E, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_29` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_5` and GND
  - On RTL8721Dx, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_12` and GND
    - Connect a LED to `GPIO_LED_PIN _PB_10` and GND
  - On RTL8721F, connect as below
    - Connect a push button to `GPIO_PUSHBT_PIN _PA_26` and GND
    - Connect a LED to `GPIO_LED_PIN _PA_25` and GND

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
2. `PA_12` map to `GPIOA_12`, `PB_8`  map to `GPIOB_8` and etc
3. `GPIO_PUSHBT_PIN` as input with internal pull-high
4. `GPIO_LED_PIN` as output

# Expect result

In this example, the LED is on when the push button is pressed.

# Note

None.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
