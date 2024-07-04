# Example Description

This example shows how to enter deepsleep mode and configure wakeup source 

This example does not support the loguart command response.

# HW Configuration
AON_WAKEPIN_WAKEUP wakeup operation:
* On AmebaSmart,
	- Connect PB_21 or PB_22 to GND
	- Connect PB_21 or PB_22 to V3.3
	- PB_23 and PB_24 is for loguart now, contact RTK if need
* On AmebaLite,
	- Connect PA_0 or PA_1 to to GND
	- Connect PA_0 or PA_1 to V3.3
* On AmebaDplus,
	- Connect PB_30 or PB_31 to to GND
	- Connect PB_30 or PB_31 to V3.3

# SW Configuration

1. Choose wakeup source by setting AON_WAKEPIN_IDX in main.c: AON_TIMER_WAKEUP, AON_WAKEPIN_WAKEUP
2. Choose Pin Index for wakepin by static parameter :
	* On AmebaSmart, WAKEPIN_0 for PB_21, WAKEPIN_1 for PB_22
	* On AmebaLite, WAKEPIN_0 for PA_0, WAKEPIN_1 for PA_1
	* On AmebaDplus, WAKEPIN_0 for PB_30, WAKEPIN_1 for PB_31

3. How to compile :
	- Copy main.c under folder src to amebaxxx_gcc_project\project_xxx\src
		* On AmebaSmart, project_xxx should be project_ap
		* On AmebaLite, project_xxx should be project_km4 if KM4 is AP core, or project_xxx should be project_kr4 if KR4 is AP core
		* On AmebaDplus, project_xxx should be project_km4


# Expected Result

1. AON_TIMER_WAKEUP: chip will wakeup from dslp after timeout, log will show "dslp wake from aontimer"
2. AON_WAKEPIN_WAKEUP: chip will wakeup from dslp after wakepin level change to the target level, log will show "dslp wake from wakepin"
	- if sleep_wakepin_config[] set to HIGH_LEVEL_WAKEUP, chip will wakeup from dslp after corresponding wakepin level change from low level to high level
	- if sleep_wakepin_config[] set to LOW_LEVEL_WAKEUP, chip will wakeup from dslp after corresponding wakepin level change from high level to low level

# Note

1. if AON wakepin is wakeup source, sleep_wakepin_config[] in ameba_sleepcfg.c and pmap_func in ameba_pimapcfg.c also need be set.
  - for wakepin high level wakeup

  	* set "config" attribute of corresponding wakepin to HIGH_LEVEL_WAKEUP in sleep_wakepin_config[]
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_DOWN in pmap_func[]
  - for wakepin low level wakeup

  	* set "config" attribute of corresponding wakepin to LOW_LEVEL_WAKEUP in sleep_wakepin_config[]
  	* set Func PU/PD and Slp PU/PD of of corresponding wakepin to GPIO_PuPd_UP in pmap_func[]
2. For AmebaSmart, enter cmd `~tickps dslp` firstly after boot, let KM4 release lock is needed.
3. This example will not run in loop, which means it will keep active after wake up once.
4. If there is any information that needs to be saved before the system sleeps, it can be saved in the three registers BKUP1 ~ BKUP3. BKUP0 has a special purpose and cannot be used to store information.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus

