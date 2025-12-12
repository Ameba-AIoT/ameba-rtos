# Example Description

This example describes how to use GPIO read/write by RAW GPIO API.

# HW Configuration

- Requirement Components:
  - a LED
  - a push button
- Connect wires:
  - Connect a push button to `GPIO_PUSHBT_PIN` (as defined in the header file of the current example) and GND
    - The relevant header file can be found in the directory:
      `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
  - Connect a LED to `GPIO_LED_PIN` (as defined in the header file of the current example) and GND

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

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
