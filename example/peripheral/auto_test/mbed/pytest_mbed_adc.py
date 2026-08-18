"""
Pytest tests for mbed ADC example firmware.

Test cases:

- test_mbed_adc_norm_vol_dual — mbed_adc_norm_vol + helper_gpio_out (dual board)

硬件需求（双板）：
  两块开发板引脚一一对接 + GND 共地。

  Board A (DUT): mbed_adc_norm_vol 固件
  Board B:       helper_gpio_out 固件，双 GPIO 输出

  接线（以 RTL8720F 为例）：
    Board B PA_13 (GPIO1, HIGH) ──── Board A PA_13 (ADC_CH0, AD0)
    Board B PA_14 (GPIO2, LOW)  ──── Board A PA_14 (ADC_CH1, AD1)
    Board B GND                  ──── Board A GND

  ADC 输入范围：0 ~ 3.3V（RTL8720F），GPIO 直连安全。

Usage::

    cd example/peripheral/auto_test

    # 双板测试
    pytest mbed/pytest_mbed_adc.py::test_mbed_adc_norm_vol_dual \\
        --port 'COM6|COM9' --target RTL8720F -k "norm_vol"
"""

import pytest


# ── dual-board: mbed_adc_norm_vol + helper_gpio_out ──────────────────────

# app_defines 的引脚配置仅供参考，用户需根据实际 SoC 修改：
#   HELPER_GPIO_PIN=_PA_13   — 输出 HIGH，接 ADC_CH0(AD0)
#   HELPER_GPIO_PIN2=_PA_14  — 输出 LOW， 接 ADC_CH1(AD1)


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("mbed_adc_norm_vol|helper_gpio_out")
@pytest.mark.app_defines(
    "|HELPER_GPIO_PIN=_PA_13,HELPER_LEVEL=1,"
    "HELPER_GPIO_PIN2=_PA_14,HELPER_LEVEL2=0,"
    "HELPER_DELAY_MS=5000"
)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_adc_norm_vol|./bin/helper_gpio_out"],
    ids=["norm_vol|helper"],
    indirect=True,
)
def test_mbed_adc_norm_vol_dual(dut):
    """[双板] GPIO 直连 ADC 功能验证。

    接线：板 B 两路 GPIO（HIGH/LOW）分别接板 A 两路 ADC 输入，GND 共地。

    板 B 运行 helper_gpio_out，驱动：
      - GPIO1: init=LOW  → 5s 后 → HIGH
      - GPIO2: init=HIGH → 5s 后 → LOW

    板 A 运行 mbed_adc_norm_vol，每 500ms 打印两路 ADC 读数和换算 mV。

    验证序列：
      1. hard_reset 两板 → 等 helper 配完 GPIO → 抓 ADC（init 阶段）
      2. 等 helper 翻转 → 抓 ADC（target 阶段）
    """
    test_board, peer_board = dut
    peer_board.hard_reset()
    test_board.hard_reset()

    # ============================================================
    # 阶段 1：等 helper 启动并配好 GPIO，再读 ADC（init 阶段）
    # ============================================================
    peer_board.expect(r"HELPER1: init level=0", timeout=15)
    peer_board.expect(r"HELPER2: init level=1", timeout=5)

    ADC_RE = (
        r"AD0:0x[0-9a-fA-F]+ => (\d+)mV, "
        r"AD1:0x[0-9a-fA-F]+ => (\d+)mV"
    )

    m = test_board.expect(ADC_RE, timeout=5)
    ad0_mv = int(m.group(1))
    ad1_mv = int(m.group(2))

    faults = []
    if ad0_mv > 100:
        faults.append(f"AD0 expected ~0mV (GPIO LOW init), got {ad0_mv}mV")
    if ad1_mv < 3000:
        faults.append(f"AD1 expected ~3300mV (GPIO HIGH init), got {ad1_mv}mV")
    if faults:
        pytest.fail(" | ".join(faults))

    # ============================================================
    # 阶段 2：等 helper 翻转，再读 ADC（target 阶段）
    # ============================================================

    peer_board.expect(r"HELPER1: drive level=1", timeout=12)
    peer_board.expect(r"HELPER1: done", timeout=8)
    peer_board.expect(r"HELPER2: drive level=0", timeout=5)
    peer_board.expect(r"HELPER2: done", timeout=5)

    ADC_TARGET_RE = (
        r"AD0:0x[0-9a-fA-F]+ => (3[0-3]\d{2})mV, "
        r"AD1:0x[0-9a-fA-F]+ => (\d{1,2})mV"
    ) # AD0 只匹配 3000~3399mV， AD1 只匹配 0~99mV（2 位以内）

    m = test_board.expect(ADC_TARGET_RE, timeout=8)
    ad0_mv = int(m.group(1))
    ad1_mv = int(m.group(2))

    faults = []
    if ad0_mv < 3000:
        faults.append(f"AD0 expected ~3300mV (GPIO HIGH target), got {ad0_mv}mV")
    if ad1_mv > 100:
        faults.append(f"AD1 expected ~0mV (GPIO LOW target), got {ad1_mv}mV")
    if faults:
        pytest.fail(" | ".join(faults))
