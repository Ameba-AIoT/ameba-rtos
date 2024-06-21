# Example Description

This example describes how to get ADC sample data in `vbat channel` `under timer-trigger mode`.

ADC could cooperate with `timer` modules which drive ADC module to do conversions according to a pre-configured timer event `period`. Every time a timer event is sent to ADC module, a conversion would be executed.

This example does not support the loguart command response.

# HW Configuration

In this example, ADC is configured to sample data in `vbat channel`.

* On AmebaSmart
	- Connect `VBAT_MEAS` to `to-be-measured signal`
	- Connect `GND` to `GND` of to-be-measured device

For more info of `VBAT_MEAS`, refer to EVB schematic design file.

# SW Configuration

1. By default, `TIMER2` with a period of `500ms` is selected to trigger ADC to sample data `periodically`.
2. `ADC_TIM_IDX` and `PERIOD_MS` can be modified to configure desired TIMER and sample period.
3. How to use:
    * Copy `main.c` and `adc_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. Timer trigger control register is set to enable timer-trigger mode and an appropriate `timer module index` is set in trigger timer selection register.
2. ADC sample data and calculated voltage value in `mV` would be printed out periodically.

# Note

1. Input voltage of vbat channel: `0 ~ 5V`.
2. Conversion in normal channels can also be executed under timer-trigger mode
3. Supported timer: `TIMER0 ~ TIMER7`.
4. Vbat channel `is not supported` on `AmebaLite`.

# Supported IC

* AmebaSmart