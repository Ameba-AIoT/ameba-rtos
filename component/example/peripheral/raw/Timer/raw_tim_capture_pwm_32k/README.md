# Description
This example describes how to use PWM input capture function.

# HW Configuration

  - For RTL8730E, RTL8721Dx, RTL8726E/RTL8720E/RTL8713E/RTL8710E
  - PWM_GEN_32KHz_CH5_PIN(channel 5) as PWM channel output 32K, connect it to TIM8_TRIG_PIN with UPS(Unmanned Peripheral System) or PRS(Peripheral Reflect System).
  - TIM8_TRIG_PIN as PWM input capture pin.

  - For RTL8721F
  - PWM_GEN_32KHz_CH3_PIN(channel 3) as PWM channel output 32K, connect it to TIM4_TRIG_PIN with UPS(Unmanned Peripheral System) or PRS(Peripheral Reflect System).
  - TIM4_TRIG_PIN as PWM input capture pin.
  for example:

1. If you run this example in RTL8730E, then connect as below
   
   PB_10 <--> PA_13

2. If you run this example in RTL8726E/RTL8720E/RTL8713E/RTL8710E, then connect as below
   
   PB_5 <--> PA_22

3. If you run this example in RTL8721Dx, then connect as below
   
   PB_18 <--> PB_20

4. If you run this example in RTL8721F, then connect as below

   PA_3 <--> PB_16
# SW configuration
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. How to modify gpio pin:

   - you can also modify pinmux group settings according to correct pinmux table. For example: 

     If you want to modify pinmux group setting in RTL8721Dx, then you should refer to UM1002_RTL8721Dx_pin_mux.xlsx

# Expect result
This example will print out the frequency of pwm captured in trace tool.

# Note
none

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
