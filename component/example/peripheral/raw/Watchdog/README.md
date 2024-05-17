# Example Description

This example describes how to use watchdog API. In this example, watchdog is setup to 5s timeout. Watchdog won't bark if we refresh it before timeout. The timer is also reloaded after refresh. Otherwise it will restart system in default .

This example does not support the loguart command response.

# HW Configuration

None

# SW configuration

The steps to build Watchdog image are as following:
1. Copy `main.c` to override `project\realtek_chipname_va0_example\src\src_xx\main.c`

# Expect result

1. watchdog will reset system due to very small task refresh WDG (Refresh WDG between [window - reload] will trigger reset). loguart output: 
	```
	doing very small task...
	...
	reset by refresh watchdog inside Protected Window
	````
2. After wdg reset system, big task will start and the early interrupt will be triggerd to avoid watchdog barks. loguart output:
	```
	doing big task...
	WDG refreshed by watchdog interrupt
	WDG refreshed by watchdog interrupt
	big task done, watchdog is refreshed in big task
	```
3. After big task done, sw timer will start to refresh watchdog, and the early interrupt will not be triggerd. loguart output:
	```
	WDG refreshed by sw timer
	WDG refreshed by sw timer
	WDG refreshed by sw timer
	WDG refreshed by sw timer
	WDG refreshed by sw timer
	stop sw timer, refresh watchdog by interrupt
	watchdog raw example exec succeed!
	```

# Note

* WDG support window protection, refresh between [window - reload] will reset the system.
* IWDG can only trigger global reset which reset whole chip, other watchdogs can trigger global reset or cpu reset in AmebaLite.
* IWDG may be enabled by HW, so window protection cannot be enabled again for IWDG already enabled.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
