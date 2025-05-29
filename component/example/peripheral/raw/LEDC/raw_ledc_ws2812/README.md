# Example Description

This example describes how to use LEDC to light WS2812 series LED in the state of a breathing RGB light.

# HW Configuration

Required Components: a WS2812 series LED and a 4.5V-5.5V power supply.

* On AmebaSmart
	- Connect `LEDC_PIN(_PA_9)` to `Data_In` of LED.
	- Supply `5V` power for the LED
	- Connect `GND` to `GND` of LED.
* On AmebaLite
	- Connect `LEDC_PIN(_PA_21)` to `Data_In` of LED.
	- Supply `5V` power for the LED.
	- Connect `GND` to `GND` of LED.
* On AmebaDplus
	- Connect `LEDC_PIN(_PA_26)` to `Data_In` of LED.
	- Supply `5V` power for the LED.
	- Connect `GND` to `GND` of LED.

# SW Configuration

By default, `16 LEDs` will be lit in `red-off-green-off-blue-off` in turn and the time interval is about `1s`.

1. `LED_NUM` can be modified to set LED number.
2. `LEDC_MODE` can be modified as `LEDC_DMA_MODE` or `LEDC_CPU_MODE` to make LEDC work under DMA mode or CPU mode, and the former is more efficient.
3. `LEDC_PIN` in ledc_ext.h can be modified to select desired pin.
   For more info of LEDC pins, refer to pinmux spec.
4. `LED_T1H`/`LED_T1L`/`LED_T0H`/`LED_T0L` in ledc_ext.h can be modified to configure LEDC waveform features according to LED spec.
5. `F_INTERVAL_NS` and `LED_RST` in ledc_ext.h can be modified to set time interval between frames.
6. How to use:
    * Run `./build.py -a raw_ledc_ws2812` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
    * `Download` images to board by Ameba Image Tool.

# Expected Result

1. `16 LEDs` will be lit in `red-off-green-off-blue-off` `in turn` every `1ms`.
2. After `7 frames` are sent, LEDC will be enabled again and send `the next 7 frames`.

# Note

None

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
