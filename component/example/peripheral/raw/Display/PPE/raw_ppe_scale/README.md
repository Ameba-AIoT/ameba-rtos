#  Example Description
This example describes how to use scale function. For this example, its scale ratios of x axis and y axis are both 2. It's interface is memory to memory. Software will checkout if output data is right. User can select polling mode or interrupt mode to check if it blends over.

# HW Configuration

None

#  SW configuration

1. User can choose macro PPE_MODE to choose polling mode or interrupt mode. Value POLLING_MODE means polling mode, and value INTR_MODE means interrupt mode.
2. Build and Download:
* Refer to the SDK Examples section of the online documentation to generate images.
* `Download` images to board by Ameba Image Tool.

#  Expect result

It only blend one time. 
- If in interrupt mode, it will echo:
> Frame over interrupt occurs
> scale over 

- If in polling mode, it will echo:
> scale over

- If failed it will echo:
> [error] has occured!

#  Note

None

#  Supported IC

* AmebaDplus


