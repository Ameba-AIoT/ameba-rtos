# Example Description

This example describes how to get ADC sample data in normal channels by mbed API.

This example does not support the loguart command response.

# HW Configuration

In this example, ADC is configured to sample data in `2 normal channels`.

* On AmebaSmart
	- Connect `ADC_CH2_PIN(_PA_2)` to `to-be-measured signal`
	- Connect `ADC_CH3_PIN(_PA_3)` to `to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device
* On AmebaLite
	- Connect `ADC_CH0_PIN(_PB_5)` to `to-be-measured signal`
	- Connect `ADC_CH1_PIN(_PB_4)` to `to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device
* On AmebaDplus
	- Connect `ADC_CH0_PIN(_PB_19)` to` to-be-measured signal`
	- Connect `ADC_CH1_PIN(_PB_18)` to` to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device

# SW Configuration

1. By default, `2 normal channels` are selected to sample data, `ADC_PAD` can be modified to select desired channels.

2. For more info of ADC channel pins, refer to pinmux spec.
   For example, on AmebaSmart, refer to `UM0602_RTL8730E_pinmux.xls` for more info.

3. How to use:
    * Copy `main.c` and `adc_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

ADC sample data and calculated voltage value in `mV` would be printed out `periodically`.

# Note

Input voltage of single-ended normal channels:
* On AmebaSmart
	`0 ~ 1.8V`
* On AmebaLite
	`0 ~ 3.3V`
* On AmebaDplus
	`0 ~ 3.3V`

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus