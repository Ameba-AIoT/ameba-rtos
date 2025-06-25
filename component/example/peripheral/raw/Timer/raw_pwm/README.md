# Description
This example describes how to use pwm.

# HW Configuration
Connect LED to below PWM pins and ground

For example:
1. If you run this example in AmebaSmart, then connect as below

	GND <--> LED0 <---> PA_14 //pwm0
	GND <--> LED1 <---> PA_9  //pwm1
	GND <--> LED2 <---> PA_10 //pwm2
	GND <--> LED3 <---> PA_4  //pwm3
	GND <--> LED4 <---> PA_5  //pwm4
	GND <--> LED5 <---> PA_13 //pwm5
2. If you run this example in AmebaLite, then connect as below

	GND <--> LED0 <---> PA_28
	GND <--> LED1 <---> PA_29
	GND <--> LED2 <---> PA_30
	GND <--> LED3 <---> PA_31
	GND <--> LED4 <---> PA_26
	GND <--> LED5 <---> PA_27
	GND <--> LED6 <---> PA_21
	GND <--> LED7 <---> PA_22
3. If you run this example in AmebaDplus, then connect as below

	GND <--> LED0 <---> PA_12
	GND <--> LED1 <---> PA_26
	GND <--> LED2 <---> PA_27
	GND <--> LED3 <---> PA_30
	GND <--> LED4 <---> PA_31
	GND <--> LED5 <---> PB_17
	GND <--> LED6 <---> PB_18
	GND <--> LED7 <---> PB_19

# SW configuration
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. How to modify gpio pin:
   - you can also modify pinmux group settings according to correct pinmux table. For example:
    If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expect result

  - The LED would gradually become brighter and then darker with different speed.
  - Connect the logic analyzer, and PWM waveforms on all six/eight channels can be displayed. The Waveform duty cycle gradually increases and then decreases.

# Note
1. The initial brightness setting of the AmebaSmart is different from that of other chips, so the output of the LED is also different from other chips.

2. Only a few pins of AmebaSmart can output PWM waveforms, and only specific pins can output specific PWM channels. For details, please refer to its pinmux table.

3. Only certain pins of AmebaLite can output PWM waveforms. For example, the PB0, PB1, PB2, PB3 (or PA28, PA29, PA30, PA31) pins can only output PWM0~PWM3. For details, refer to the corresponding pinmux table.

4. If the AmebaLite board is 8720E, you need to replace the pins.

   ```shell
    //8726E, _PA_26;  8720E, _PA_15
    //8726E, _PA_27; 8720E, _PA_16
   ```

# Supported IC
  - AmebaSmart
  - AmebaLite
  - AmebaDplus
