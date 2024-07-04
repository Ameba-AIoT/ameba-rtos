# Example Description

This example describes how to get ADC sample data in normal channels `under comparator-assist mode`.

To reduce ADC power consumption, an additional comparator could cooperate with ADC as a first level check of input voltage.

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

# SW Configuration

1. By default, `2 normal channels` are selected to sample data with reference voltage `Vref0(1V)` and `Vref1(2V)`.
    * `ADC_CH_CNT`, `ADC_CHAN` and `ADC_PAD` can be modified to select desired channels.
    * `CMP_Ref0`, `CMP_Ref1` and `CMP_WakeADCCtrl` can be modified to configure desired compare criterion.

2. For more info of ADC channel pins, refer to pinmux spec.
   For example, on AmebaSmart, refer to `UM0602_RTL8730E_pinmux.xls` for more info.

3. How to use:
    * Copy `main.c` and `adc_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. Under comparator-assist mode, once comparison result matches a predefined criterion, a wake-up signal could be delivered to ADC module by comparator to trigger a ADC conversion.
2. When input voltage of set channel (`ONLY CH0 ~ CH3`) is `between Vref0 and Vref1`, compare criterion matches an comparator wakeup ADC to sample for the channel.
3. ADC sample data and calculated voltage value in `mV` would be printed out.

For example, when `CH1` is fed with `1.8V` signal, the following log will be printed out:
`AD1:0x998 => 1832 mV`

# Note

1. Input voltage of single-ended normal channels:
* On AmebaSmart
	`0 ~ 1.8V`
* On AmebaLite
	`0 ~ 3.3V`

2. Each ADC channel has a unique corresponding pad. If `ADC_CHAN` is modified, `ADC_PAD` must be modified accordingly.
3. Comparator can only detect `CH0 ~ CH3`.

# Supported IC

* AmebaSmart
* AmebaLite
