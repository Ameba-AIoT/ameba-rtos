# Example Description

This example describes how to use IR receive and learn the received waveform frequency or data format by RAW IR API.

This example does not support the loguart command response.

# HW Configuration

Connect the rx pin to another board's tx pin defined in `ir_ext.h`.

# SW configuration

- How to compile :

  - Copy main.c, ir_ext.h, ir_learn.h and ir_nec_protocol.h, ir_nec_protocol.c and Makefile under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
- IR learn data Flow:

  1. Enable IR RX mode.
  2. IR recieve data at `IR_RX_FIFO_LEVEL_INT` and store data to queue.
  3. When `IR_RX_CNT_THR_INT` happens, IR stop receiving data and start decodeing received data.
  4. IR decode packet will count all carrier symbol (data between `TIME_LOWEST_VALUE` and `TIME_HIGHEST_VALUE` regard as carrier symbol) together; and record non-carrier symbol value (data larger than `TIME_LOW_VOLTAGE_VALUE`).
  5. IR decode frequncy is caculated from the IR sample frequency and received carrier symbol number.
     Note: there may be some accuarcy when rx learning from the received data.
  6. Display the carrier symbol data to space symbol data.
     Note: when the non-carrier symbol is large enough, there may bring caculation overflow. when this happens, force (uint64_t)detect_time or (uint64_t)frequency data type to uint64_t.

- If you choose another tx signal, then this step can be ignored. As well, a tx image should be build as following:
  - By means of example located in sdk/component/example/peripheral/raw/IR/ir_tx_nec_polling/src, some modifications are necessary:
    - Define `IR_LOOP_TEST` to  `0`
    - Define `IR_TEST_LEARNNING` to `1`
    - Rebuild the image as descriped in sdk/component/example/peripheral/raw/IR/ir_tx_nec_polling/README.md
# Expect result

This example will print out detect_time and frequency of received data.

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
