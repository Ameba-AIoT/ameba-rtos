# Example Description

This example provide a stress ui to test the performance of display refresh rate.

# HW Configuration

Required Components: display device

Connect display to board
* On AmebaSmart
The model of display is st7701s, insert it through mipi port.
* On AmebaDPlus
The model of display is ILI9341, connect it by dupont lines as follow:
connect `GND` of display to `GND` of board
connect `VCC` of display to `3.3V` of board
connect `CLK` of display to `A19` of board
connect `MOSI` of display to `B6` of board
connect `RES` of display to `A22` of board
connect `DC` of display to `A21` of board
connect `CS1` of display to `A20` of board

# SW Configuration
* On AmebaSmart
1. enable ui/lvgl configuration:
cd amebasmart_gcc_project
MENUCONFIG FOR AP CONFIG
GUI config -> Enable GUI ->  chose LVGL

1. Modify the stress configuration in
component/ui/LVGL/port/amebasmart/lv_conf.h
Change
#define LV_USE_DEMO_STRESS 0
to
#define LV_USE_DEMO_STRESS 1

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

* On AmebaDPlus
1.Disable XIP_FLASH
cd amebadplus_gcc_project
configure menuconfig
CONFIG Link Option  --->
choise PSRAM AS HEAP

2. enable ui/lvgl configuration:
cd amebadplus_gcc_project
MENUCONFIG FOR KM4 CONFIG
GUI config -> Enable GUI ->  chose LVGL ->  chose drivers

3. Modify the stress configuration in
component/ui/LVGL/port/amebadplus/lv_conf.h
Change
#define LV_USE_DEMO_STRESS 0
to
#define LV_USE_DEMO_STRESS 1

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result
The stress ui has shown in the display