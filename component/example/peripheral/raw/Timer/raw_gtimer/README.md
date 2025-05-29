# Description
This example describes how to use general timer TIM2&TIM3 to generation periodic signal.

# HW Configuration
  Connect the two LEDs to port LED1_pin and LED2_pin respectively.
  for example:
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
1. How to compile :
   
   - make and download new image
   
     - in {SDK}\amebaxxx_gcc_project path, input
   
     ```shell
    ./build.py -a raw_gtimer
     ```
   
     - Use Realtek AmebaImageTool to download the image under {SDK}\amebaxxx_gcc_project folder to development board
     - press reset/chip_en button, then example will be start running.
   
2. How to modify gpio pin:
    you can also modify pinmux group settings according to correct pinmux table. for example: 
   - If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result

  - The LED will periodically turn on and off.
  - Link logic analyzer, and two PWM waveforms with periods of 1s and 500ms can be find.

# Note
  none

# Supported IC
  - AmebaSmart
  - AmebaLite
  - AmebaDplus
