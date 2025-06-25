# Example Description

This example describes how to use the RTC Alarm by RAW RTC API.

# HW Configuration

None.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
   
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
