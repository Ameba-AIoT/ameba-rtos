# Description
This example describes how to use pwm timer.
Use the four channels of PWM to generate PWM signals and dynamically modify the PWM duty cycle.

# HW Configuration
Connect LED to below PWM pins and ground

for example:
1. If you run this example in AmebaSmart, then connect as below
	GND <--> LED0 <---> PA_9
	GND <--> LED1 <---> PA_10
	GND <--> LED2 <---> PA_4
	GND <--> LED3 <---> PA_5
2. If you run this example in AmebaLite, then connect as below
	GND <--> LED0 <---> PA_28
	GND <--> LED1 <---> PA_29
	GND <--> LED2 <---> PA_30
	GND <--> LED3 <---> PA_31
3. If you run this example in AmebaDplus, then connect as below
	GND <--> LED0 <---> PB_17
	GND <--> LED1 <---> PB_18
	GND <--> LED2 <---> PB_19
	GND <--> LED3 <---> PB_30
4. If you run this example in AmebaGreen2, then connect as below
	GND <--> LED0 <---> PA_6
	GND <--> LED1 <---> PA_7
	GND <--> LED2 <---> PA_8
	GND <--> LED3 <---> PA_10

# SW configuration
Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

How to modify gpio pin:
1. you can also modify pinmux group settings according to correct pinmux table. for example: 
   - If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result
the LED would gradually become brighter and then darker with different speed.

# Note
1. Only certain pins of AmebaLite can output PWM waveforms. For example, the PB0, PB1, PB2, PB3  (or PA28, PA29, PA30, PA31) pins can only output PWM0~PWM3. For details, refer to the corresponding pinmux table.
2. Only a few pins of AmebaSmart can output PWM waveforms, and only specific pins can output specific PWM channels. For details, please refer to its pinmux table.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
