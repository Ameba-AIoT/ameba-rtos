# Example Description

This example provide a widgets ui to show the interaction of widgets.

# HW Configuration

Required Components: display device

Connect display to board
* On AmebaSmart
The model of display is st7701s, insert it through mipi port.
* On AmebaDPlus
The model of display is ILI9341, connect it by dupont lines as follow:
connect `GND` of display to `GND` of board
connect `VCC` of display to `3.3V` of board
connect `CLK` of display to `PA19` of board
connect `MOSI` of display to `PB6` of board
connect `RES` of display to `PA22` of board
connect `DC` of display to `PA21` of board
connect `CS1` of display to `PA20` of board
* On AmebaLite
The model of display is st7789v, connect it by dupont lines as follow:
connect `GND` of display to `GND` of board
connect `VCC` of display to `3.3V` of board
connect `SCL` of display to `PA28` of board
connect `SDA` of display to `PA29` of board
connect `RES` of display to `PA21` of board
connect `DC` of display to `PA22` of board
connect `CS` of display to `PA31` of board
connect `BLK` of display to `PA23` of board

# BUILD COMMAND
./build.py -a lv_demo_widgets -p

# Expected Result
The widgets ui has shown in the display
