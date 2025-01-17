#  Example Description

This example show how to use PPE auto reload function. It blends 4 frames totally for this exmaple. After calling set valid, PPE will reload registers into internal register and start blending. It's interface is memory to memory. 

This example does not support the loguart command response.

# HW Configuration

None

#  SW configuration

How to use:
* Copy `main.c` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
* Run `make all` and `download` image to board.

#  Expect result

It will blend 4 frames totally. And expect log on console is:
> Load over interrupt occurs
> Frame over interrupt occurs
> Here we set set_valid manually for the second frame
> Load over interrupt occurs
> Frame over interrupt occurs
> Here we set set_valid manually for the third frame
> Load over interrupt occurs
> Frame over interrupt occurs
> Here we set set_valid manually for the forth frame
> Load over interrupt occurs
> Frame over interrupt occurs
> all frame over interrupt occurs
> auto reload over

#  Note

This example use polling PPE_GetValid() to check if layer registers for the previous frame has been loaded into internal registers, and then layer registers can be written for current frame. User can use load over interrupt to check it, too.

#  Supported IC

* AmebaDplus


