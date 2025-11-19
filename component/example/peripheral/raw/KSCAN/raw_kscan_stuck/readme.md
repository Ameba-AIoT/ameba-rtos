# Example Description

This example describes how to use keyscan when there is key stuck.

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

Key stuck will generate interrupt, mask stuck key, other keys work normal. For example, Press and hold the key (8,6), then release it. It's observed that a stuck interrupt is generated for key press and release. Bit[8] = 1 indicates that the key is pressed, and 0 indicates that the key is released.
```shell
11:43:58.606  Stuck Intr
11:43:58.616  Default Intr
11:43:58.616   186
11:44:01.238  Scan Intr
11:44:01.238   86
11:44:01.542  Stuck Intr
11:44:01.558  Release Intr
11:44:01.558  Default Intr
```

Press and hold the key (8,6), then press any key (8,y), and there is no interrupt. This is because row 8 is blocked. Pressing keys on other rows will generate an interrupt normally.

```shell
11:49:13.774  Stuck Intr
11:49:13.797  Default Intr
11:49:13.797   186
11:49:17.731  Scan Intr
11:49:17.731   176
11:49:17.925  Scan Intr
11:49:17.925   76
11:49:20.089  Scan Intr
```
# Note

1. Since the PA 30 and PA 31 pins are used for SWD, they need to be turned off before use, and can be turned off using API  `void Pinmux_Swdoff(void)`.
2. When a key is stuck, this row will be temporarily blocked, and attempts to press other keys on the same row will not generate events. However, keys on other rows can work normally.
3. A stuck interrupt is generated when a key is stuck or leaves the stuck state.

# Supported IC

- AmebaDplus
  * RTL8721Dx
