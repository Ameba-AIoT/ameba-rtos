# Example Description

This example describes how to use GPIO Port read/write by RAW GPIO API.

# HW Configuration

- Requirement Components:

  - LEDs
  - Dupont line
- Connect wires:
  When test GPIO port output:

  - GPIO ports as output pins connect to the LED long leg, and the LED short leg connect to GND
  - Selected pins are decided by both `GPIO_TEST_PORT` and `pin_mask` (as defined in the header file of the current example)
     - The relevant header file can be found in the directory:
      `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`

    - About macro `GPIO_TEST_PORT`:

      - `1`: `PortB`
      - `0`: `PortA`
    - About global variable `pin_mask`:

      - firstly convert it to binary value from hex
      - secondly, each bit map to one pin: 0: pin disable; 1: pin enable
        for example:
        - if `GPIO_TEST_PORT` is set 0 (tested port is PortA), and `pin_mask` equals 0x0F (coverted to binary 00001111). So the selected pins are `PA0~PA3`
        - if `GPIO_TEST_PORT` is set 1 (tested port is PortB), and `pin_mask` equals 0xF0 (coverted to binary 11110000). So the selected pins are `PB4~PB7`
  - About global variable led_pattern[idx], each bit map to one pin for output test: 0- pin output low; 1- pin output high
    for example:

    - if the selected pins are `PA0~PA3`, and led_pattern[idx] equals 0x0F, then `PA0~PA3` will output high
    - if the selected pins are `PA0~PA3`, and led_pattern[idx] equals 0x00, then `PA0~PA3` will output low

  When test GPIO port input:

  - Connect the selected GPIO pins to signal source
  - the selected pins  are the same with above output test

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool. 
2. Modify macro `PORT_OUTPUT_TEST` to change output/input test
   `1`: for output test
   `0`: for input test
3. Modify macro `GPIO_TEST_PORT` to change test Port
   `1`: `PortB`
   `0`: `PortA`

# Expect result

- When test GPIO port output:
  - The LED is on when corresponding GPIO pin outputs `1`
  - The LED is off when corresponding GPIO pin outputs `0`
- When test GPIO port input:
  - If current input value of selected GPIO pins is different from the original input value, trace tool will print out current input value.

# Note

- On AmebaSmart, led_pattern is only supported in `PortA`
- On AmebaLite, the pin of number for `PortB` is 20

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
