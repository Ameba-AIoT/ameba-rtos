# Example Description

This example describes how to use BOD `under interrupt mode` or `under reset mode`.

This example does not support the loguart command response.

# HW Configuration

Required Components: power supply device.

1. Connect power supply `GND` to chip `GND`.

2. Power supply:
* On AmebaSmart
	- Remove resistor and jumper between `VDD33` and `VDD33_S`, connect `power supply output` to chip `VDD33_S`.
* On AmebaLite
	- Remove resistor and jumper between `VD33` and `VD33_WL`, connect `power supply output` to chip `VD33_WL`.
* On AmebaDPlus
	- Remove resistor and jumper between `VD1833` and `VD1833_WL`, connect `power supply output` to chip `VD1833_WL`.

	For more info, refer to EVB schematic design file.

3. Provide `3.3V` signal to download images to board at first and then change the output voltage.

# SW Configuration

By default, BOD works `under interrupt mode`.

1. `BOD_INTR_MODE` can be set to `1` or `0` to use BOD `under interrupt mode` or `under reset mode`.
2. `BOD_FALL_THRE` and `BOD_RISE_THRE` can be modified to set falling and rising threshold, which ranges from `0x0` to `0x1F`.
3. How to use:
    * Copy `main.c` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

* On AmebaSmart
  - Under interrupt mode
	1. After Boot-up, log `"Supply voltage under 2.67V triggers bod interrupt!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.67V` to `trigger bod interrupt`, and registered `"bor_irq_handler"` will be called, log "BOD interrupt" will be displayed.
	3. Recover supply voltage to `above 2.87V` to make the circuit work properly.

  - Under reset mode
	1. After Boot-up, log `"Supply voltage under 2.67V and recover it to above 2.87V causes bod reset!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.67V`, and recover voltage to `above 2.87V` to `reset chip`.
	3. Reset log `"KM4 BOOT Reason: 10"` is displayed, where BIT4 represents BOD reset.
* On AmebaLite
  - Under interrupt mode
	1. After Boot-up, log `"Supply voltage under 2.54V triggers bod interrupt!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.54V` to `trigger bod interrupt`, and registered `"bor_irq_handler"` will be called, log "BOD interrupt" will be displayed.
	3. Recover supply voltage to `above 2.83V` to make the circuit work properly.

  - Under reset mode
	1. After Boot-up, log `"Supply voltage under 2.54V and recover it to above 2.83V causes bod reset!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.54V`, and recover voltage to `above 2.83V` to `reset chip`.
	3. Reset log `"KM4 BOOT Reason: 10"` is displayed, where BIT4 represents BOD reset.

* On AmebaDplus
  - Under interrupt mode
	1. After Boot-up, log `"Supply voltage under 2.21V triggers bod interrupt!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.21V` to `trigger bod interrupt`, and registered `"bor_irq_handler"` will be called, log "BOD interrupt" will be displayed.
	3. Recover supply voltage to `above 2.50V` to make the circuit work properly.

  - Under reset mode
	1. After Boot-up, log `"Supply voltage under 2.21V and recover it to above 2.50V causes bod reset!!!"` will be displayed.
	2. Change supply voltage `from 3.3V to 1.80V~2.21V`, and recover voltage to `above 2.50V` to `reset chip`.
	3. Reset log `"KM4 BOOT Reason: 1"` is displayed, where BIT0 represents BOD reset.

# Note

1. As to corresponding voltage of threshold, refer to comment of function `BOR_ThresholdSet();`
2. If Reset log from one CPU is mixed with log from other CPU, it is recommended to set `Boot_Agg_En` in `component/soc/xx/usrcfg/ameba_bootcfg.c` as `TRUE` and log from various CPU will be divided.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus