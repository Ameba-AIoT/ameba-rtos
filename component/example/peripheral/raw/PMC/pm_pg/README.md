# Example Description

This example shows how to use freertos tickless with aontimer interruptable interface. 

This example does not support the loguart command response.

# HW Configuration


# SW Configuration
How to compile :
	- Copy main.c under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
		* On AmebaSmart, src_xxx should be src_ap
		* On AmebaLite, src_xxx should be src_KM4 if KM4 is AP core, or src_xxx should be src_KR4 if KR4 is AP core
		* On AmebaDplus,src_xxx should be src_KM4

# Expected Result

IC will enter PG mode, and wakeup after 2s. The wakeup source is aontimer.

# Note

1. Freertos enter tickless if the wakelock bit map is 0.By default there is PMU_OS keep system awake.So we need release this PMU_OS enable tickless mode.
2. Keep alive time is adjustable by parameter of pmu_set_sysactive_time(), unit is ms.
3. This example will not run in loop, which means it will keep active after wake up once.
4. On AmebaLite and AmebaDplus
	- There are two core: NP and AP.
	- After bootup, enter cmd "tickps r debug" to release lock for AP.
5. On AmebaSmart
	- There are three core: LP, NP and AP.
	- After bootup, cmd "tickps r debug" to release lock for AP, which have already done in example code.
	- After bootup, enter cmd "~tickps r debug" to release lock for NP immediately if want NP and LP goto sleep mode at the sametime.
	- To reset after wakeup, long press the CHIP_EN(reset) button for at least ~3-5s.


# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus