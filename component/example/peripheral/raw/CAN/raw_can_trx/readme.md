# Example Description

This example describes how to use CAN0 and CAN1 to send and receive data to each other.

# HW Configuration

CAN0 and CAN1 use the following pins respectively:

|    | CAN0   | CAN1   |
| ----- | ----- | ----- |
| RX | PA 12 | PA 14 |
| TX | PA 13 | PA 15 |

These pins need to be connected to the CAN PHY device(chip). Then the CAN_H and CAN_L of the two CAN PHYs are connected via twisted pair wires without crossing.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

Normally, CAN0 will receive the standard data frame with StdId 0x55 and data 0-7 sent by CAN1.

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
RxMsg->RxTimStamp = 14569
RxMsg->RxLost = 0
##########################RX message end######################
```

CAN1 will receive the standard data frame with StdId 0x0 and data 0-7 sent by CAN0.

```shell
######################RX message start######################
RxMsg->StdId = 0
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
RxMsg->RxTimStamp = 14507
RxMsg->RxLost = 0
##########################RX message end######################
```
If the twisted pair is disconnected, ACK Error and TX Error will occur, and the TEC count will accumulate.

```shell
CAN0: Clear Interrupt Status = 0
ACK error
tx error
TEC = 8, REC = 0
CAN1: Clear rx Status = 0
ACK error
tx error
ErrStatus = 120, TEC = 8, REC = 0, ErrPassive = 0, ErrBusoff = 0, ErrWarning = 0
CAN0: Clear Interrupt Status = 0
ACK error
tx error
TEC = 16, REC = 0
CAN1: Clear rx Status = 0
ACK error
tx error
ErrStatus = 120, TEC = 16, REC = 0, ErrPassive = 0, ErrBusoff = 0, ErrWarning = 0
CAN0: Clear Interrupt Status = 0
ACK error
tx error
TEC = 24, REC = 0
CAN1: Clear rx Status = 0
ACK error
tx error
ErrStatus = 120, TEC = 24, REC = 0, ErrPassive = 0, ErrBusoff = 0, ErrWarning = 0
```
When the accumulated tx errors exceed 128, the error will become a passive error.
```shell
CAN0: Clear Interrupt Status = 0
ACK error
tx error
TEC = 120, REC = 0
CAN1: Clear rx Status = 0
ACK error
tx error
ErrStatus = 120, TEC = 120, REC = 0, ErrPassive = 0, ErrBusoff = 0, ErrWarning = 1
CAN0: Clear Interrupt Status = 0
ACK error
tx error
TEC = 128, REC = 0
CAN1: Clear rx Status = 0
ACK error
tx error
ErrStatus = 120, TEC = 128, REC = 0, ErrPassive = 1, ErrBusoff = 0, ErrWarning = 1

```
# Supported IC

- AmebaGreen2
