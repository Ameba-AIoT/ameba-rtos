"""
Pytest tests for raw ADC example firmware (dual board, GPIO-driven).

Test cases:

- test_raw_adc_auto      — raw_adc_auto + helper_gpio_out (dual board, GPIO-driven)
- test_raw_adc_sw_trig   — raw_adc_sw_trig + helper_gpio_out (dual board, GPIO-driven)

硬件需求：双板，板 A PA_13 ↔ 板 B PA_13, 板 A PA_14 ↔ 板 B PA_14, GND ↔ GND

  板 A (DUT): raw_adc_auto / raw_adc_sw_trig
  板 B:       helper_gpio_out (PA_13=HIGH, PA_14=LOW, 5s 后翻转)

Usage::

    cd example/peripheral/auto_test

    pytest raw/pytest_raw_adc.py::test_raw_adc_auto \\
        --port 'COM6|COM17' --target RTL8720F

    pytest raw/pytest_raw_adc.py::test_raw_adc_sw_trig \\
        --port 'COM6|COM17' --target RTL8720F
"""

import pytest

# ── 通道 + 电压双重限定的 ADC 正则 ─────────────────────────────────────
# expect 自动跳过不匹配的行（天然排干 stale 数据）
# RTL8720F ADC 量程 0~3.3V，GPIO HIGH ≈ VDD ≈ 3300mV
ADC0_LOW_RE = r"AD0:0x[0-9a-fA-F]+ => (1[0-4][0-9]|[0-9]{1,2})mV"
"""AD0 值 ≤149mV（GPIO LOW ~0mV）。group(1)=电压mV。"""
ADC1_HIGH_RE = r"AD1:0x[0-9a-fA-F]+ => (3[0-3]\d{2})mV"
"""AD1 值 3000~3399mV（GPIO HIGH ~3300mV）。group(1)=电压mV。"""
ADC0_HIGH_RE = r"AD0:0x[0-9a-fA-F]+ => (3[0-3]\d{2})mV"
"""AD0 值 3000~3399mV（GPIO HIGH ~3300mV）。group(1)=电压mV。"""
ADC1_LOW_RE = r"AD1:0x[0-9a-fA-F]+ => (1[0-4][0-9]|[0-9]{1,2})mV"
"""AD1 值 ≤149mV（GPIO LOW ~0mV）。group(1)=电压mV。"""

# ══════════════════════════════════════════════════════════════════════════════
#  raw_adc_auto — one-shot: fills DMA buffer (256 entries) and exits
# ══════════════════════════════════════════════════════════════════════════════


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("raw_adc_auto|helper_gpio_out")
@pytest.mark.app_defines(
    "|HELPER_GPIO_PIN=_PA_13,HELPER_LEVEL=1,"
    "HELPER_GPIO_PIN2=_PA_14,HELPER_LEVEL2=0,"
    "HELPER_DELAY_MS=5000"
)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_adc_auto|./bin/helper_gpio_out"],
    ids=["auto|helper"],
    indirect=True,
)
def test_raw_adc_auto(dut):
    """[双板] raw_adc_auto + helper_gpio_out: GPIO 驱动 ADC 验证。

    raw_adc_auto 为 one-shot（打印一次即退出），因此必须错峰 boot：
    helper 先启动配置 GPIO，ADC 再启动采样。

    helper init phase: PA_13=0V(LOW), PA_14=3.3V(HIGH)
    """
    test_board, peer_board = dut

    # 错峰 boot：helper 先走，配好 GPIO 再 boot ADC
    peer_board.hard_reset()
    peer_board.expect(r"HELPER1: init level=0", timeout=15)
    peer_board.expect(r"HELPER2: init level=1", timeout=5)
    # 此时 GPIO 已在 init 电平稳定

    test_board.hard_reset()
    # ADC 在 GPIO 稳定的条件下采样，缓冲区数据正确

    # AD1=HIGH（3000~3399mV），自动跳过 buffer 中可能残留的噪声行
    test_board.expect(ADC1_HIGH_RE, timeout=10)

    # AD0=LOW（0~149mV）
    test_board.expect(ADC0_LOW_RE, timeout=5)


# ══════════════════════════════════════════════════════════════════════════════
#  raw_adc_sw_trig — continuous: 每 500ms 触发一次，单通道/行
# ══════════════════════════════════════════════════════════════════════════════


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("raw_adc_sw_trig|helper_gpio_out")
@pytest.mark.app_defines(
    "|HELPER_GPIO_PIN=_PA_13,HELPER_LEVEL=1,"
    "HELPER_GPIO_PIN2=_PA_14,HELPER_LEVEL2=0,"
    "HELPER_DELAY_MS=5000"
)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_adc_sw_trig|./bin/helper_gpio_out"],
    ids=["sw_trig|helper"],
    indirect=True,
)
def test_raw_adc_sw_trig(dut):
    """[双板] raw_adc_sw_trig + helper_gpio_out: GPIO 驱动 ADC 验证。

    sw_trig 为连续模式（每 500ms 触发一次），支持 init → target 两阶段验证。

    helper 行为:
      init:  PA_13=0(LOW), PA_14=1(HIGH)    [t=0]
      drive: PA_13=1(HIGH), PA_14=0(LOW)     [t=5s]

    接线：板 A PA_13 ↔ 板 B PA_13, 板 A PA_14 ↔ 板 B PA_14, GND ↔ GND
    """
    test_board, peer_board = dut
    test_board.hard_reset()
    peer_board.hard_reset()

    # ═══ 阶段 1：init — 等 helper GPIO 稳定后读取 ADC ═══
    peer_board.expect(r"HELPER1: init level=0", timeout=15)
    peer_board.expect(r"HELPER2: init level=1", timeout=5)

    # AD1=HIGH（3000~3399mV），自动跳过 ADC 先于 GPIO 配置时采样的噪声行
    test_board.expect(ADC1_HIGH_RE, timeout=15)

    # AD0=LOW（0~149mV）
    test_board.expect(ADC0_LOW_RE, timeout=10)

    # ═══ 阶段 2：target — 等 helper 翻转 GPIO ═══
    peer_board.expect(r"HELPER1: drive level=1", timeout=12)
    peer_board.expect(r"HELPER1: done", timeout=8)
    peer_board.expect(r"HELPER2: drive level=0", timeout=5)
    peer_board.expect(r"HELPER2: done", timeout=5)

    # 翻转后：AD0=HIGH（3000~3399mV），AD1=LOW（0~149mV）
    test_board.expect(ADC0_HIGH_RE, timeout=15)

    test_board.expect(ADC1_LOW_RE, timeout=10)
