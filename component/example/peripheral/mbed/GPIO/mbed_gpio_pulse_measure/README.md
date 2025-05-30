# Example Description

This example describes how to use GPIO read/write to generate a pulse and to measure the pulse width by MBED GPIO API.

# HW Configuration

- Requirement Components:

  - Dupont line
- Connect wires:

  - Connect `GPIO_IRQ_PIN` to `GPIO_OUT_PIN`

  For example:
  - On AmebaSmart, connect as below
    - Connect `GPIO_IRQ_PIN (_PA_10)` to `GPIO_OUT_PIN (_PB_10)`
  - On AmebaLite, connect as below
    - Connect `GPIO_IRQ_PIN (_PA_29)` to `GPIO_OUT_PIN (_PB_5)`
  - On AmebaDplus, connect as below
    - Connect `GPIO_IRQ_PIN (_PA_12)` to `GPIO_OUT_PIN (_PB_10)`

# SW configuration

1. How to compile :

   - perform cmd: './build.py -a mbed_gpio_pulse_measure' in directory {sdk_path}/amebaXXX_gcc_project 
2. `PA_12` map to `GPIOA_12`, `PB_8`  map to `GPIOB_8` and etc
3. `GPIO_IRQ_PIN` is configured as input with no pull (High-Z)
4. `GPIO_OUT_PIN` is configured as output

# Expect result

In this example, trace tool will print out the measured width (in us) of the pulse which is generated by `GPIO_OUT_PIN`.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaD
