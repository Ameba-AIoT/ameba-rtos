# Example Description

This example shows how to enter deepsleep mode and configure wakeup source

# HW Configuration
AON_WAKEPIN_WAKEUP wakeup operation:
* On AmebaSmart,
	- Connect PB_21/PB_22 to GND
	- Or connect PB_21/PB_22 to V3.3
	- PB_23 and PB_24 is for loguart now, contact RTK if need
* On AmebaLite,
	- Connect PA_0/PA_1 to to GND
	- Or connect PA_0/PA_1 to V3.3
* On AmebaDplus,
	- Connect PB_30/PB_31 to to GND
	- Or connect PB_30/PB_31 to V3.3
* On AmebaGreen2,
	- Connect PA_3/PA_2/PA_1/PA_0 to to GND
	- Or connect PA_3/PA_2/PA_1/PA_0 to V3.3

# SW Configuration

1. Choose wakeup source by setting AON_WAKEPIN_IDX: AON_TIMER_WAKEUP, AON_WAKEPIN_WAKEUP
2. Choose Pin Index for wakepin by static parameter :

|    | WAKEPIN_0   | WAKEPIN_1   | WAKEPIN_2 | WAKEPIN_3|
| ----- | ----- | ----- |----- | ----- |
| AmebaSmart | PB 21 | PB 22 | | |
| AmebaLite | PA 0 | PA 1 | | |
| AmebaDplus| PB30 | PB 31 | | |
| AmebaGreen2 | PA 3 | PA 2 | PA 1| PA 0 |

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. AON_TIMER_WAKEUP: chip will wakeup from dslp after timeout, log will show "dslp wake from aontimer"
2. AON_WAKEPIN_WAKEUP: chip will wakeup from dslp after wakepin level change to the target level, log will show "dslp wake from wakepin"
	- if sleep_wakepin_config[] set to HIGH_LEVEL_WAKEUP, chip will wakeup from dslp after corresponding wakepin level change from low level to high level
	- if sleep_wakepin_config[] set to LOW_LEVEL_WAKEUP, chip will wakeup from dslp after corresponding wakepin level change from high level to low level

# Note

1. if AON wakepin is wakeup source, sleep_wakepin_config[] in ameba_sleepcfg.c and pmap_func in ameba_pinmapcfg.c also need be set.
  - for wakepin high level wakeup

  	* set "config" attribute of corresponding wakepin to HIGH_LEVEL_WAKEUP in sleep_wakepin_config[]
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_DOWN in pmap_func[]
  - for wakepin low level wakeup

  	* set "config" attribute of corresponding wakepin to LOW_LEVEL_WAKEUP in sleep_wakepin_config[]
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_UP in pmap_func[]
2. This example will not run in loop, which means it will keep active after wake up once.
3. If there is any information that needs to be saved before the system sleeps, it can be saved in the three registers BKUP1 ~ BKUP3. BKUP0 has a special purpose and cannot be used to store information.

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F

