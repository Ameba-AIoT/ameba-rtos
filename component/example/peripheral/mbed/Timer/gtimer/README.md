# Description
This example describes how to use general timer TIM2&TIM3 to generate periodic signal by mbed api.

This example does not support the loguart command response.

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

# SW configuration

How to compile :

1. copy `main.c` and `tim_ext.h`  under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

2. make and download new image

   - in {SDK}\project\realtek_amebaxxx_va0_example\GCC-RELEASE\auto_build path, input

   ```shell
   make xip //or make all
   ```

   - Use Realtek AmebaImageTool to download the image under auto_build folder to development board
   - press reset/chip_en button, then example will be start running.

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
