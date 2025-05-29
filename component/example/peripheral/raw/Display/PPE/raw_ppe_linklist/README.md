#  Example Description

This example show how to use PPE linklist function. User should create link list item structure, and then fill these link list items for each frame. For this example, it creates 5 link list item, which describe 5 frames. PPE will blend all frames configured by link list items. 

# HW Configuration

None

#  SW configuration

How to use:
* Copy `main.c` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
* Run `make all` and `download` image to board.

#  Expect result

This exmaple will blend 5 frames. Each frame blended over will trigger frame over interrupt. All frames blend over 
will trigger all over interrupt. 

Expect log output: 
> Frame over interrupt occurs
> Frame over interrupt occurs
> Frame over interrupt occurs
> Frame over interrupt occurs
> all frame over interrupt occurs
> linklist over

#  Note

Link list item structure should be built according the requirement.

#  Supported IC

* AmebaDplus


