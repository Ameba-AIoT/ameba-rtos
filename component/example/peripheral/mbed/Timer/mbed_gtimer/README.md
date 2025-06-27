# Description
This example describes how to use general timer TIM2&TIM3 to generate periodic signal by mbed api.

# HW Configuration

Connect the two LEDs to port LED1_pin and LED2_pin respectively. for example:

1. If you run this example in AmebaSmart, then connect as below
GND <--> LED1 <---> PB_10
GND <--> LED2 <---> PB_11
2. If you run this example in AmebaLite, then connect as below
GND <--> LED1 <---> PA_21
GND <--> LED2 <---> PA_22
3. If you run this example in AmebaDplus, then connect as below
GND <--> LED1 <---> PB_17
GND <--> LED2 <---> PB_18
3. If you run this example in AmebaGreen2, then connect as below
GND <--> LED1 <---> PA_13
GND <--> LED2 <---> PA_14
# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

How to modify gpio pin:

you can also modify pinmux group settings according to correct pinmux table. for example:

- If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result
The LED will periodically turn on and off.

# Note
none

# Supported IC
- AmebaSmart
- AebaLite
- AmebaDplus
- AmebaGreen2
