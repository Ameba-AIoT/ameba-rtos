# Description
This example describes how to use PULSE_TIM to capture pulse number.

# HW Configuration

  - PWM_32K_CH5_PIN as PWM channel output 32KHz periodical signal.
  - TIM9_TRIG_PIN as PULSE timer input capture pin.
  - Connect TIM9_TRIG_PIN to PWM_32K_CH5_PIN.

  for example:

1. If you run this example in AmebaSmart, then connect as below
     _PB_11 <--> _PA_13
2. If you run this example in AmebaLite, then connect as below
     _PA_21 <--> _PA_22
3. If you run this example in AmebaDplus, then connect as below
     _PB_17 <--> _PB_19

# SW configuration
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2.  How to modify gpio pin:
       - you can also modify pinmux group settings according to correct pinmux table. For example: 
    
        If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result
This example will print out the pulse number captured every 100ms.

# Note
none

# Supported IC

  - AmebaSmart
  - AmebaLite
  - AmebaDplus
