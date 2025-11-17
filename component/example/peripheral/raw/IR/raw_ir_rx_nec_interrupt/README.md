# Example Description

1. This example describes how to use IR receiving data in interrupt mode by RAW IR API.
2. This data are encoded with NEC protocol

# HW Configuration

Connect the rx pin to another board's tx pin (as defined in the header file of the current example).
- The relevant header file can be found in the directory:
  `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
2. In this example, the IR functions as a receiver and uses `IR_PINMUX_S1_RX` by default.
3. Additional configurations:
   - data waveform reverse
     reverse lR output idle level by modifying `IR_InitStruct->IR_TxIdleLevel ` to `IR_IDLE_OUTPUT_LOW `or `IR_IDLE_OUTPUT_HIGH`
   - revsese IR carrier and non-carrier level by modifying `IR_InitStruct->IR_TxInverse` to following optional value:
     - `IR_TX_DATA_NORMAL`: carrier symbol from high to low; non-carrier symbol default low
     - `IR_TX_DATA_INVERSE`: carrier symbol from low to high; non-carrier symbol default high
     - `IR_TX_DATA_CARRIER_NORMAL`: default carrier and non-carrier waveform
     - `IR_TX_DATA_CARRIER_INVERSE`: there is a pulse from carrier to non-carrier
4. Other IR protocol configuration, refer to `ir_nec_protocol.h` located in this demo folder.

   - struct and macros:

     - `IR_ProtocolTypeDef`: the struct of IR protocol data format, which depends on the IR protocol custom choose
     - `IR_DataTypeDef`: the struct used to store IR decoding/endcoding data
     - `MAX_IR_BUF_SIZE`: the maxium length of carrier symbol data to store in one data command and data format
     - `MAX_HEADDER_LEN`: header length if there is header
     - `MAX_LOG_WAVFORM_SIZE`: always be 2. to differ from space symbol high or low.
     - `MAX_CODE_SIZE`: the data length of `DATA_CODE_WDTH`
     - `DATA_CODE_WDTH`: NEC protocol is 8bits as default
   - IR encode and decode functions:

     - `NEC_EncodePacket`: encode the space symbol of IR waveform data with carrier symbol data
     - `NEC_DecodePacket`: decode the received IR waveform with carrier symbol to space symbol

# Expect result

This example will print out data received.

# Note

- NEC protocol works at

  - sample frequency = 10MHz
  - receive carrier symbol data when `IR_RX_FIFO_LEVEL_INT` interrupt happens.
  - rx stop at `IR_RX_CNT_THR_INT` interrupt.r
  - rx stop threshold is controlled by received `IR_RxCntThrType`(default Low level) count > `IR_RxCntThr`(default 0xa1644, that is about 66ms)
  - decoding tolerance is 30% by default.
  - When using IR receiver module, set ir rx pin no-pull state.
- On AmebaSmart: Pin in example is only supported as `IR_TX` or `IR_RX` function.
- On AmebaDplus: Pin in example is supported as both `IR_TX ` and `IR_RX` function.

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F
