# Example Description

This example demonstrates how to wake up via wakepin.

# HW Configuration
wakeup operation:
* On RTL8730E,
	- Connect PB_21/PB_22 to GND
	- Or connect PB_21/PB_22 to V3.3
	- PB_23 and PB_24 is for loguart now, contact RTK if need
* On RTL8726E/RTL8720E/RTL8713E/RTL8710E,
	- Connect PA_0/PA_1 to to GND
	- Or connect PA_0/PA_1 to V3.3
* On RTL8721Dx,
	- Connect PB_30/PB_31 to to GND
	- Or connect PB_30/PB_31 to V3.3
* On RTL8721F,
	- Connect PA_3/PA_2/PA_1/PA_0 to to GND
	- Or connect PA_3/PA_2/PA_1/PA_0 to V3.3

# SW Configuration

1. Choose Pin Index for wakepin by static parameter :

|    | WAKEPIN_0   | WAKEPIN_1   | WAKEPIN_2 | WAKEPIN_3|
| ----- | ----- | ----- |----- | ----- |
| RTL8730E | PB 21 | PB 22 | | |
| RTL8726E/RTL8720E/RTL8713E/RTL8710E | PA 0 | PA 1 | | |
| RTL8721Dx| PB30 | PB 31 | | |
| RTL8721F | PA 3 | PA 2 | PA 1| PA 0 |

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

chip will wakeup from dslp after wakepin level change to the target level, log will show "dslp wake from wakepin"
	- If wake pin is set to HIGH_LEVEL_WAKEUP, the chip will wake up from the DSLP after the corresponding wake-up pin level changes from low to high.
	- If wake pin is set to LOW_LEVEL_WAKEUP, chip will wakeup from dslp after corresponding wakepin level change from high level to low level
    - If the wake pin is set to DISABLE_WAKEPIN, then this pin cannot wake up the system.

# Note

1. When AON wakepin is wakeup source, pmap_func in `ameba_pinmapcfg.c` also need be set.
  - for wakepin high level wakeup

	* set "config" attribute of corresponding wakepin to HIGH_LEVEL_WAKEUP via API `Wakepin_Setting`
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_DOWN in pmap_func[]
  - for wakepin low level wakeup

	* set "config" attribute of corresponding wakepin to LOW_LEVEL_WAKEUP via API `Wakepin_Setting`
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_UP in pmap_func[]
2. This example will not run in loop, which means it will keep active after wake up once.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F

