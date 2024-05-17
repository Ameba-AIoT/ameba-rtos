# Example Description

This example describes how to use the RTC Alarm by RAW RTC API.

This example does not support the loguart command response.

# HW Configuration

None.

# SW configuration

How to compile :

- Copy main.c under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

# Expect result

1. RTC Alarm Interrupt will be triggered When second counts upto 0.
2. When Interrupt triggered, time information will be printed in `RTC_Handler`.
3. The Interrupt will be triggered every minute, and won't stop by default.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
