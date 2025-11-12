# Example Description

This example describes how to use GDMA's gather and scatter function.

# HW Configuration

None.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

GDMA will display "transfer over " in serial port tool,  and displays the results of source data and destination data.

# Note
1. Amebagreen2 only supports channel 0~channel 3 to use the gather/scatter function.
2. Users must first use GDMA_Init, then use GDMA_DestinationScatter or GDMA_SourceGather, and finally use GDMA_Cmd to enable GDMA.
3. Src_GatherCount (SGC)/Src_GatherInterval (SCI) is aligned with the source data transmission width, for example, SGC = 8, SCI=8, Src_TrWidth = 2Bytes, then continuously transfer $SGC\times 2=16$Bytes, jump to the interval of $SCI \times 2= 16$Bytes, and then Continue to the next SGC*2Bytes transfer.
4. Dst_ScatterCount (DSC)/Dst_ScatterInterval (DSI) is aligned with the destination data transmission width, for example, DSC = 8, DSI = 8, Dst_TrWidth = 2Bytes, then continuously transport $DSC\times2=16$Bytes, jump to the interval of $DSI\times2 = 16$Bytes, and then Continue to the next $DSC\times2$Bytes transfer.
5. When using the Source Gather function, users need to ensure that the `src data size` aligned to the source data transmission width.
6. When using the Destination Scatter function, users need to ensure that `dst data size` must be $((DSI + DSC)/DSI)$ of `src data size`, and `src data size` needs to be aligned with the transmission width of the destination. Otherwise, the destination has insufficient space to accommodate the scattered data.
# Supported IC

- AmebaGreen2
  * RTL8721F
