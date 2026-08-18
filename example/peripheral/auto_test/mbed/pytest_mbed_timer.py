"""
Pytest tests for mbed General Timer (GTimer) and PWM example firmware.

Test cases:

- test_mbed_gtimer — mbed_gtimer: 验证周期定时器 (TIM2, 1s) 与单次定时器
  (TIM3, 500ms) 的中断触发及间隔
- test_mbed_pwm_selftest — mbed_pwm: 单板跳线自检，2s 窗口内统计上升沿数，≈100（50Hz）即通过
    mark tim_loopback: 单板跳线 (RTL8720F: _PA_23 --> _PA_24)

Usage::

    cd example/peripheral/auto_test/
    pytest mbed/pytest_mbed_timer.py \\
        --target RTL8720F \\
        --port COM6
"""

import time
import pytest


@pytest.mark.generic
@pytest.mark.app_name("mbed_gtimer")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gtimer"], ids=["gtimer"], indirect=True)
def test_mbed_gtimer(dut):
    """GTimer (mbed API)：验证 periodic (TIM2, 1s) 和 oneshot (TIM3, 500ms)
    定时中断均触发，且间隔符合预期。"""
    dut.hard_reset()

    # 两个定时器并发运行，串口输出可能交错。
    # 收集足够事件以分别验证两个通道的间隔。
    events = []  # [(name, timestamp), ...]
    for _ in range(8):
        match = dut.expect(r"(GTIMER_PERIOD_EXPIRED|GTIMER_ONESHOT_EXPIRED)", timeout=8)
        events.append((match.group(1), time.time()))

    # --- 验证 periodic 间隔 ---
    period_t = [ts for name, ts in events if name == "GTIMER_PERIOD_EXPIRED"]
    assert len(period_t) >= 2, (
        f"Expected ≥2 PERIOD events, got {len(period_t)}"
    )
    p_interval = period_t[-1] - period_t[-2]
    assert 0.8 <= p_interval <= 1.5, (
        f"Periodic interval {p_interval:.2f}s out of range [0.8, 1.5]"
    )
    print(f"  [OK] Periodic interval: {p_interval:.2f}s")

    # --- 验证 oneshot 间隔 ---
    oneshot_t = [ts for name, ts in events if name == "GTIMER_ONESHOT_EXPIRED"]
    assert len(oneshot_t) >= 2, (
        f"Expected ≥2 ONESHOT events, got {len(oneshot_t)}"
    )
    o_interval = oneshot_t[-1] - oneshot_t[-2]
    assert 0.35 <= o_interval <= 1.0, (
        f"Oneshot interval {o_interval:.2f}s out of range [0.35, 1.0]"
    )
    print(f"  [OK] Oneshots interval: {o_interval:.2f}s")


@pytest.mark.tim_loopback
@pytest.mark.app_name("pwm_selftest")
@pytest.mark.app_defines("USE_MBED_PWM=1")
@pytest.mark.parametrize("image_dir", ["./bin/pwm_selftest_mbed"],
                         ids=["pwm_selftest_mbed"], indirect=True)
def test_mbed_pwm_selftest(dut):
    """mbed_pwm 单板跳线自检（公共 pwm_selftest 固件）。

    PWM ch-2 输出 pin (PWM_3) 跳线到捕获 pin（RTL8720F PA23-PA24），
    自检在 2s 窗口内统计上升沿数，≈100（50Hz）即通过。
    """
    dut.hard_reset()
    dut.expect(r"MBED_PWM_PASS edges=\d+", timeout=15)
