# Description
This example describes how to use PWM input capture function.

This example does not support the loguart command response.

# HW Configuration

  - PWM_GEN_32KHz_CH5_PIN(channel 5) as PWM channel output 32K, connect it to TIM8_TRIG_PIN with UPS(Unmanned Peripheral System) or PRS(Peripheral Reflect System).
  - TIM8_TRIG_PIN as PWM input capture pin.

  for example:

1. If you run this example in AmebaSmart, then connect as below
   
   PB_10 <--> PA_13

2. If you run this example in AmebaLite, then connect as below
   
   PB_5 <--> PA_22

3. If you run this example in AmebaDplus, then connect as below
   
   PB_18 <--> PB_20

# SW configuration
1. How to compile :

   - make and download new image

     - in {SDK}\amebaxxx_gcc_project path, input
   
     ```shell
    ./build.py -a raw_tim8_capture_pwm_32k
     ```

     - Use Realtek AmebaImageTool to download the image under auto_build folder to development board
     - press reset/chip_en button, then example will be start running.

2. How to modify gpio pin:

   - you can also modify pinmux group settings according to correct pinmux table. For example: 

     If you want to modify pinmux group setting in AmebaDplus, then you should refer to UM1002_RTL8721Dx_pin_mux.xlsx

# Expect result
This example will print out the frequency of pwm captured in trace tool.

# Note
none

# Supported IC

  - AmebaSmart
  - AmebaLite
  - AmebaDplus