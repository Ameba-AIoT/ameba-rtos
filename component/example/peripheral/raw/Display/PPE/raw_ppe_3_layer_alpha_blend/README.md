#  Example Description

This example describes how to use 3 layer alpha blending function. All 3 input layers are ARGB8888. After blending by PPE, It will output ARGB8888 format data. It's interface is memory to memory. Software will checkout if data is right. User can select polling mode or interrupt mode to check if it blends over. 

# HW Configuration

None

#  SW configuration

1. User can choose macro PPE_MODE to choose polling mode or interrupt mode. Value POLLING_MODE means polling mode, and value INTR_MODE means interrupt mode.
2. How to use:
* Copy `main.c` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
* Run `make all` and `download` image to board.

#  Expect result

It only blend one time. 
- If in interrupt mode, it will echo:
> Frame over interrupt occurs
> 3 layer alpha blending over

- If in polling mode, it will echo:
> 3 layer alpha blending over

- If failed it will echo:
> [error] has occured!

#  Note

None

#  Supported IC

* AmebaDplus


