# Example Description

This example shows how to use freertos tickless with aontimer interruptable interface.

# HW Configuration


# SW Configuration
How to compile:

1. Run cmd: `./build.py -a raw_pmc_pg` in directory {sdk_path}/amebaXXX_gcc_project.
2. Copy the current file `example_xxx.c` to the amebaXXX_project/project_xxx/src path, and then include the current file in the main.c file, which can also be compiled. amebaxxx_gcc_project\project_xxx\src_xxx
		* On AmebaSmart, src_xxx should be src_ap
		* On AmebaLite, src_xxx should be src_KM4 if KM4 is AP core, or src_xxx should be src_KR4 if KR4 is AP core
		* On AmebaDplus,src_xxx should be src_KM4

# Expected Result

IC will enter PG mode, and wakeup after 2s. The wakeup source is aontimer.

# Note

1. Freertos enter tickless if the wakelock bit map is 0.By default there is PMU_OS keep system awake.So we need release this PMU_OS to enable tickless mode.
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
