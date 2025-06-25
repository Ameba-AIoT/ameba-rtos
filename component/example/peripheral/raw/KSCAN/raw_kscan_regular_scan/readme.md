# Example Description

This example describes how to use key-Scan regular scan mode.

# HW Configuration

Support up to 8*8 keyboard, and this example hardware connection is as follows:

| Row   | Pin   | Col   | Pin   |
| ----- | ----- | ----- | ----- |
| Row 1 | PA 25 | Col 1 | PA 23 |
| Row 2 | PA 22 | Col 2 | PA 21 |
| Row 3 | PA 26 | Col 3 | PA 19 |
| Row 4 | PA 20 | Col 4 | PB 6  |
| Row 5 | PB 9  | Col 5 | PA 30 |
| Row 6 | PB 8  | Col 6 | PB 1  |
| Row 7 | PA 24 | Col 7 | PB 2  |
| Row 8 | PB 7  | Col 8 | PB 3  |

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

key press will generate interrupt, any key press event is stored in the key event FIFO until it is released.

# Note

Since the PA 30 and PA 31 pins are used for SWD, they need to be turned off before use, and can be turned off using API  `void Pinmux_Swdoff(void)`.

# Supported IC

- AmebaDplus

