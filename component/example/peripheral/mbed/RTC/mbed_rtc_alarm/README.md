# Example Description

This example describes how to use the RTC Alarm by MBED RTC API.

# HW Configuration

None.

# SW configuration

How to compile :

   - in {SDK}\amebaxxx_gcc_project path, input
   
   ```shell
   ./build.py -a mbed_rtc_alarm
   ```

# Expect result

1. RTC Alarm Interrupt will be triggered When second counts upto 10 after reset.
2. When Interrupt triggered, time information will be printed in `RTC_Handler`.
3. The Interrupt will be triggered only once, and stopped in `rtc_alarm_intr_handler`.

# Note

None.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
