# Example Description

This example describes how to use keyscan when there is key stuck.

This example does not support the loguart command response.

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

1. Copy main.c to project\realtek_amebaxxx_va0_example\src\src_xxx, and replace old main.c.

2. make all and download new image.

# Expect result

Key stuck will generate interrupt, mask stuck key, other keys work normal.

# Note

Since the PA 30 and PA 31 pins are used for SWD, they need to be turned off before use, and can be turned off using API  `void Pinmux_Swdoff(void)`.

# Supported IC

- AmebaDplus
