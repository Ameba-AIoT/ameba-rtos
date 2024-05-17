# Example Description

1. This example describes how to use IR transmitting data in interrupt mode by RAW IR API
2. This data are encoded with NEC protocol

This example does not support the loguart command response.

# HW Configuration

Logic Analyser or oscilloscope.

# SW configuration

1. How to compile :

   - Copy main.c, ir_ext.h, ir_nec_protocol.h, ir_nec_protocol.c and Makefile under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
2. In this example, IR as transmitter, default using `IR_PINMUX_S1_TX`.
3. Additional configurations:

   - data waveform reverse
     reverse lR output idle level by modifying `IR_InitStruct->IR_TxIdleLevel ` to `IR_IDLE_OUTPUT_LOW `or `IR_IDLE_OUTPUT_HIGH`
   - revsese IR carrier and non-carrier level by modifying `IR_InitStruct->IR_TxInverse` to following optional value:
     - `IR_TX_DATA_NORMAL`: carrier symbol from high to low; non-carrier symbol default low
     - `IR_TX_DATA_INVERSE`: carrier symbol from low to high; non-carrier symbol default high
     - `IR_TX_DATA_CARRIER_NORMAL`: default carrier and non-carrier waveform
     - `IR_TX_DATA_CARRIER_INVERSE`: there is a pulse from carrier to non-carrier
4. Other IR protocol configuration, refer to `component\soc\amebasmart\verification\IR\protocol\ir_nec_protocol.c`

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

   When LA or oscilloscope is connected to `IR_PINMUX_S1_TX` pin, tx data will be captured.

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
- AmebaDplus
