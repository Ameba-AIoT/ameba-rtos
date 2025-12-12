# Example Description

This example describes how to use disable JTAG module and use GPIO pin to blink led by MBED GPIO API.

# HW Configuration

- Requirement Components:

  - a LED
  - a push button
- Connect wires:

  - Connect a push button to `GPIO_JTAG_ENABLE_PIN` (as defined in the header file of the current example) and GND
    - The relevant header file can be found in the directory:
      `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
  - Connect a LED to `GPIO_LED_PIN` (as defined in the header file of the current example) and GND

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
  - On AmebaGreen2, connect as below
    - Connect a push button to `GPIO_JTAG_ENABLE_PIN _PA_26` and GND
    - Connect a LED to `GPIO_LED_PIN _PA_18` and GND

# SW configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
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

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
