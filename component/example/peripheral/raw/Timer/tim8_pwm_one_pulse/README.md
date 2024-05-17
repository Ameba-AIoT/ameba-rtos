# Description
  This example describes how to use pwm one pulse mode, default negative edge will trigger one pulse.

This example does not support the loguart command response.

# HW Configuration
  - GPIO_Pin as GPIO interrupt pin, connect it to PWM_GEN_CH5_PIN.
  - PWM_GEN_CH5_PIN(channel 5) as one pulse output pin.
  - BUTTON_PIN as one pulse input trigger pin with internal pull-high, connect a push button to this pin and ground.

  for example:

1. If you run this example in AmebaSmart, then connect as below
   
   PB_21<--> PA_13
   GND <--> button <---> PB_10

2. If you run this example in AmebaLite, then connect as below
   
   PB_4 <--> PA_22
   GND <--> button <---> PB_5

3. If you run this example in AmebaDplus, then connect as below
   
   PB_19 <--> PB_20
   GND <--> button <---> PB_18

# SW configuration
1. How to compile :
   - copy `main.c` and `tim_ext.h` under floder src to project\realtek_amebaxxx_va0_example\src\src_xxx
   - make and download new image
   
     - in {SDK}\project\realtek_amebaxxx_va0_example\GCC-RELEASE\auto_build path, input
   
     ```shell
     make xip //or make all
     ```
   
     - Use Realtek AmebaImageTool to download the image under auto_build folder to development board
     - press reset/chip_en button, then example will be start running.
2. How to modify gpio pin:
   - you can also modify pinmux group settings according to correct pinmux table.For example: 
     If you want to modify pinmux group setting in AmebaDplus, then you should refer to UM1002_RTL8721Dx_pin_mux.xlsx

# Expect result
  This example will print out interrupt information when button is pushed.

# Note
  none

# Supported IC

  - AmebaSmart
  - AmebaLite
  - AmebaDplus