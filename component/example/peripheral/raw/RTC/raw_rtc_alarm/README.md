# Example Description

This example describes how to use the RTC Alarm by RAW RTC API.

This example does not support the loguart command response.

# HW Configuration

None.

# SW configuration

How to compile :
   
   - make and download new image
   
     - in {SDK}\amebaxxx_gcc_project path, input
   
     ```shell
    ./build.py -a raw_rtc_alarm
     ```
   
     - Use Realtek AmebaImageTool to download the image under {SDK}\amebaxxx_gcc_project folder to development board
     - press reset/chip_en button, then example will be start running.
   

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
