# Description
This example describes how to use Pulse Timer to capture pulse width.

# HW Configuration

For RTL8730E,RTL8726E/RTL8720E/RTL8713E/RTL8710E,RTL8721Dx
- PWM_32K_CH5_PIN as PWM channel output 32KHz periodical signal.
- TIM9_TRIG_PIN as PULSE timer input capture pin.
- Connect TIM9_TRIG_PIN to PWM_32K_CH5_PIN.
For RTL8721F:
- PWM_32K_CH3_PIN as PWM channel output 32KHz periodical signal.
- TIM8_TRIG_PIN as PULSE timer input capture pin.
- Connect TIM8_TRIG_PIN to PWM_32K_CH3_PIN.
for example:

1. If you run this example in RTL8730E, then connect as below
   _PB_11 <--> _PA_13
2.  If you run this example in RTL8726E/RTL8720E/RTL8713E/RTL8710E, then connect as below
    _PA_21 <--> _PA_22
3. If you run this example in RTL8721Dx, then connect as below
   _PB_17 <--> _PB_19
4. If you run this example in RTL8721F, then connect as below
   _PA_3 <--> _PB_16
# SW configuration
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. How to modify gpio pin:
   - you can also modify pinmux group settings according to correct pinmux table.
     For example:
     If you want to modify pinmux group setting in RTL8730E, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result
This example will print out the pulse width of PWM channel output signal.

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
