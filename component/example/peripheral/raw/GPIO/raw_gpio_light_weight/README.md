# Example Description

This example describes how to use GPIO read/write in a light weight way by RAW GPIO API.

# HW Configuration

- Requirement Components:
  - a LED
  - a push button
- Connect wires:
  - Connect a push button to `GPIO_PUSHBT_PIN` defined in the current example's header file and GND
  - connect a LED to `GPIO_LED_PIN` defined in the current example's header file and GND

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

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaD
