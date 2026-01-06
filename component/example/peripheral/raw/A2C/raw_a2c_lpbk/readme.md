# Example Description

This example describes how to use A2C0 to perform a loopback test.

# HW Configuration

No hardware configuration required.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A2C0 receives a standard data frame per second, with a StdId of 0x55 and data values ​​of 0-7, sent by itself.

```shell
Clear Interrupt Status = 0
****TEC = 0, REC = 0, ErrPassive = 0, ErrBusoff = 0, ErrWarning = 0
######################RX message start######################
RxMsg->StdId = 55
RxMsg->ExtId = 0
RxMsg->MsgBufferIdx = 15
RxMsg->IDE = 0 (standard frame)
RxMsg->RTR = 0 (data frame)
RxMsg->DLC = 8 (data length)
RxMsg->data[0] = 0
RxMsg->data[1] = 1
RxMsg->data[2] = 2
RxMsg->data[3] = 3
RxMsg->data[4] = 4
RxMsg->data[5] = 5
RxMsg->data[6] = 6
RxMsg->data[7] = 7
RxMsg->ID_MASK = 0
RxMsg->RTR_Mask = 0
RxMsg->IDE_Mask = 0
RxMsg->RxTimStamp = 121
RxMsg->RxLost = 0
##########################RX message end######################
```
# Note

None.

# Supported IC

RTL8721F
