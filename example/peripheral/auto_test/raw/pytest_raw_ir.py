"""
Pytest tests for raw IR example firmware (dual-board).

Test cases:

- test_raw_ir_nec_interrupt  — raw_ir_tx_nec_interrupt + raw_ir_rx_nec_interrupt
- test_raw_ir_rx_learning    — raw_ir_tx_nec_polling + raw_ir_rx_learning

硬件需求：
  两块板子，IR_TX <-> IR_RX 一一对接（默认 PINMUX_S1）

Usage::

    1. Build the example firmware for both boards, save to specified directories, e.g.:
        ameba.py build -a raw_ir_tx_nec_interrupt -D FINAL_IMAGE_DIR=<path/to/bin/tx>
        ameba.py build -a raw_ir_rx_nec_interrupt -D FINAL_IMAGE_DIR=<path/to/bin/rx>
    2. cd example/peripheral/raw/IR/ &&
        pytest --target RTL8720F --port "COM6|COM7"
    use --auto-build to auto-build the firmware before flashing, e.g.:
        pytest --target RTL8720F --port "COM6|COM7" --auto-build
"""

import pytest
from ameba_pytest import ameba_parametrize


@ameba_parametrize('target', ['RTL8730E', 'RTL8721Dx', 'RTL8721F', 'RTL8720F'])
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_ir_tx_nec_interrupt|raw_ir_rx_nec_interrupt")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_ir_nec_interrupt/tx|./bin/raw_ir_nec_interrupt/rx"],
    ids=["nec_interrupt"], indirect=True,
)
def test_raw_ir_nec_interrupt(dut):
    """IR NEC interrupt mode: TX encodes + RX decodes via interrupt.

    TX 连续发送 NEC 编码数据，每包打印 ``TX 0x{code}``；
    RX 每包解码打印 ``result 0 RX 0x{code}``。
    用 TX 提取的 code 构造 RX 的 expect 字符串，一轮搞定收发一致性。
    """
    tx, rx = dut
    tx.hard_reset()
    tx.expect('start TX')
    for _ in range(50):
        code = tx.expect(r"TX 0x([0-9a-f]{4})").group(1)
        rx.expect(f"result 0 RX 0x{code}", timeout=30)


@ameba_parametrize('target', ['RTL8730E', 'RTL8721Dx', 'RTL8721F', 'RTL8720F'])
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_ir_tx_nec_polling|raw_ir_rx_learning")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_ir_rx_learning/tx|./bin/raw_ir_rx_learning/rx"],
    ids=["rx_learning"], indirect=True,
)
def test_raw_ir_rx_learning(dut):
    """IR waveform learning: TX sends one NEC burst (polling mode),
    RX learns and reports waveform frequency.

    TX 默认配置 IR_TEST_LEARNNING=1，发送一包 NEC 数据（0x12, 0x34）后退出。
    RX 捕获波形并计算载波频率（NEC 标准约 38KHz），打印频率数据。
    接线：TX(IR_PINMUX_S1_TX) -> RX(IR_PINMUX_S1_RX)。
    """
    tx, rx = dut
    FREQUENCY = 38000

    rx.hard_reset()  # RX 先就绪；TX 上电有 ~3s 延时
    tx.hard_reset()

    # TX 发出 NEC 码 {0x12, 0x34}
    assert tx.expect(r"TX BYTE1\s*(\w+) BYTE0\s*(\w+)", timeout=15).groups() == ("34", "12")

    # RX 学习波形；命中任一错误日志即失败，否则校验载波频率 == 38000
    SUCCESS = r"save data flag: \d+, 0 success, or failed, frequency: (\d+)"
    m = rx.expect([SUCCESS, "IR learning timeout", "IR wave err", "resetting the trigger threshold"], timeout=60)
    assert m.re.pattern == SUCCESS, f"RX 学习失败: {m.group(0)!r}"
    assert int(m.group(1)) == FREQUENCY, f"载波频率异常: {m.group(1)} Hz"
