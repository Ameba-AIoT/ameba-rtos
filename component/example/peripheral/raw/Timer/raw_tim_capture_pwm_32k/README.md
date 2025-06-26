# Description
This example describes how to use PWM input capture function.

# HW Configuration

  - For AmebaSmart, AmebaDplus, AmebaLite
  - PWM_GEN_32KHz_CH5_PIN(channel 5) as PWM channel output 32K, connect it to TIM8_TRIG_PIN with UPS(Unmanned Peripheral System) or PRS(Peripheral Reflect System).
  - TIM8_TRIG_PIN as PWM input capture pin.

  - For AmebaGreen2
  - PWM_GEN_32KHz_CH3_PIN(channel 3) as PWM channel output 32K, connect it to TIM4_TRIG_PIN with UPS(Unmanned Peripheral System) or PRS(Peripheral Reflect System).
  - TIM4_TRIG_PIN as PWM input capture pin.
  for example:

1. If you run this example in AmebaSmart, then connect as below
   
   PB_10 <--> PA_13

2. If you run this example in AmebaLite, then connect as below
   
   PB_5 <--> PA_22

3. If you run this example in AmebaDplus, then connect as below
   
   PB_18 <--> PB_20

4. If you run this example in AmebaGreen2, then connect as below

   PA_3 <--> PB_16
# SW configuration
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

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
  - AmebaGreen2
