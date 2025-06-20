# Example Description

This example describes how to get ADC sample data in normal channels `under software-trigger mode`.

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

# SW Configuration

1. By default, `2 normal channels` are selected to sample data, `ADC_CH_CNT`, `ADC_CHAN` and `ADC_PAD` can be modified to select desired channels.

2. For more info of ADC channel pins, refer to pinmux spec.
   For example, on AmebaSmart, refer to `UM0602_RTL8730E_pinmux.xls` for more info.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Under software-trigger mode, ADC conversion is triggered by setting ADC software trigger register. Every time software trigger register is set, ADC switches to the next channel in the conversion list.
2. ADC ready/busy register should be checked before reading conversion data.
3. ADC sample data and calculated voltage value in `mV` would be printed out.

# Note

1. Input voltage of single-ended normal channels:
* On AmebaSmart
	`0 ~ 1.8V`
* On AmebaLite
	`0 ~ 3.3V`

2. Each ADC channel has a unique corresponding pad. If `ADC_CHAN` is modified, `ADC_PAD` must be modified accordingly.

# Supported IC

* AmebaSmart
* AmebaLite
