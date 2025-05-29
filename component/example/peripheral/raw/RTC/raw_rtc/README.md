# Example Description

1. This example describes how to set and get RTC time by RAW RTC API.
2. The RTC function is implemented by an independent BCD timer/counter.

# HW Configuration

None.

# SW configuration

How to compile :
   
   - make and download new image
   
     - in {SDK}\amebaxxx_gcc_project path, input
   
     ```shell
    ./build.py -a raw_rtc
     ```
   
     - Use Realtek AmebaImageTool to download the image under {SDK}\amebaxxx_gcc_project folder to development board
     - press reset/chip_en button, then example will be start running.
   

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
