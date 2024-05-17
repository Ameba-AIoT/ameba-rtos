# Example Description

1. This example describes how to set and get RTC time by RAW RTC API.
2. The RTC function is implemented by an independent BCD timer/counter.

This example does not support the loguart command response.

# HW Configuration

None.

# SW configuration

How to compile :

- Copy main.c under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

# Expect result

1. This example set initial calendar time: `2009.10.28 11:35:37`
2. This example will print out the time information every second.
3. Current time information shown should be larger 1 second than last time information shown.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
