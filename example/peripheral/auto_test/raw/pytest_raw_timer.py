"""
Pytest tests for raw General Timer (GTimer) example firmware.

Test cases:

- test_raw_gtimer — raw_gtimer: 验证周期定时器 (TIM2, 1s) 与单次定时器
  (TIM3, 500ms) 的中断触发及间隔
- test_raw_pwm — raw_pwm (RAW_PWM_SELFTEST): 单板跳线自检，统计 2s 内 50Hz PWM 上升沿数
- test_raw_tim_capture_pwm_32k — raw_tim_capture_pwm_32k: 验证 32K PWM 输入捕获频率
- test_raw_tim_pulse_num_32k — raw_tim_pulse_num_32k: 验证 32K PWM 脉冲计数
- test_raw_tim_pulse_width_32k — raw_tim_pulse_width_32k: 验证 32K PWM 脉冲宽度捕获
- test_raw_tim_pwm_one_pulse — raw_tim_pwm_one_pulse + helper_gpio_out: 验证外部负边沿触发 one pulse 输出

硬件需求：
- raw_gtimer: 无（单板纯软件测试）
- mark tim_loopback: 单板跳线 (RTL8720F: _PA_23 --> _PA_24)，无需第二块板；需避开 SWD pin
- raw_tim_pwm_one_pulse: 需要两块板，DUT PWM 输出 pin 连接 DUT GPIO interrupt pin；helper 板 GPIO 输出 pin 连接 DUT BUTTON_PIN

Usage::

    cd example/peripheral/auto_test/
    pytest raw/pytest_raw_timer.py \\
        --target RTL8720F \\
        --port COM6
"""

import time
import pytest


@pytest.mark.generic
@pytest.mark.app_name("raw_gtimer")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gtimer"], ids=["gtimer"], indirect=True)
def test_raw_gtimer(dut):
    """GTimer：验证 periodic (TIM2, 1s) 和 oneshot (TIM3, 500ms)
    定时中断均触发，且间隔符合预期。"""
    dut.hard_reset()

    # 两个定时器并发运行，串口输出可能交错。
    # 收集足够事件以分别验证两个通道的间隔。
    events = []  # [(name, timestamp), ...]
    # ~6s 内应有 6×oneshot + 6×periodic → 收集 8 条已足够
    for _ in range(8):
        match = dut.expect(r"(GTIMER_PERIOD_EXPIRED|GTIMER_ONESHOT_EXPIRED)", timeout=8)
        events.append((match.group(1), time.time()))

    # --- 验证 periodic 间隔 ---
    period_t = [ts for name, ts in events if name == "GTIMER_PERIOD_EXPIRED"]
    assert len(period_t) >= 2, (
        f"Expected ≥2 PERIOD events, got {len(period_t)}"
    )
    # 最后两个 PERIOD 事件之间的间隔应 ≈ 1s
    p_interval = period_t[-1] - period_t[-2]
    assert 0.9 <= p_interval <= 1.1, (
        f"Periodic interval {p_interval:.2f}s out of range [0.9, 1.1]s"
    )

    # --- 验证 oneshot 间隔 ---
    oneshot_t = [ts for name, ts in events if name == "GTIMER_ONESHOT_EXPIRED"]
    assert len(oneshot_t) >= 2, (
        f"Expected ≥2 ONESHOT events, got {len(oneshot_t)}"
    )
    o_interval = oneshot_t[-1] - oneshot_t[-2]
    assert 0.4 <= o_interval <= 0.6, (
        f"Oneshot interval {o_interval:.2f}s out of range [0.4, 0.6]s"
    )


@pytest.mark.tim_loopback
@pytest.mark.app_name("pwm_selftest")
@pytest.mark.parametrize("image_dir", ["./bin/pwm_selftest"], ids=["pwm_selftest"], indirect=True)
def test_raw_pwm_selftest(dut):
    """raw_pwm 单板跳线自检（公共 pwm_selftest 固件）。

    PWM channel-2 输出 pin 跳线到捕获 pin（RTL8720F PA23-PA24），
    自检在 2s 窗口内统计上升沿数，≈100（50Hz）即通过。
    """
    dut.hard_reset()
    dut.expect(r"RAW_PWM_PASS edges=\d+", timeout=15)


@pytest.mark.tim_loopback
@pytest.mark.app_name("raw_tim_capture_pwm_32k")
@pytest.mark.parametrize("image_dir", ["./bin/raw_tim_capture_pwm_32k"], ids=["capture_pwm_32k"], indirect=True)
def test_raw_tim_capture_pwm_32k(dut):
    """Timer capture：捕获例程内部生成的 32K PWM，并验证串口输出频率。"""
    dut.hard_reset()
    dut.expect(r"frequency: 32000", timeout=8)


@pytest.mark.tim_loopback
@pytest.mark.app_name("raw_tim_pulse_num_32k")
@pytest.mark.parametrize("image_dir", ["./bin/raw_tim_pulse_num_32k"], ids=["pulse_num_32k"], indirect=True)
def test_raw_tim_pulse_num_32k(dut):
    """Pulse Timer：统计 100ms 内 32K PWM 脉冲数，例程输出 success 即通过。"""
    dut.hard_reset()
    dut.expect(r"Pulse number: (3199|3200), success", timeout=8)


@pytest.mark.tim_loopback
@pytest.mark.app_name("raw_tim_pulse_width_32k")
@pytest.mark.parametrize("image_dir", ["./bin/raw_tim_pulse_width_32k"], ids=["pulse_width_32k"], indirect=True)
def test_raw_tim_pulse_width_32k(dut):
    """Pulse Timer：捕获 32K PWM 脉冲宽度，例程输出 success 即通过。"""
    dut.hard_reset()
    dut.expect(r"Pulse width: \d+, success", timeout=8)


@pytest.mark.skip(reason="双板测试+单板跳线，需手动连接 helper 板")
@pytest.mark.tim_loopback
@pytest.mark.multi_board  # PA17 作为 helper 板 GPIO 输出，拉低 DUT BUTTON_PIN 触发 one pulse
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("raw_tim_pwm_one_pulse|helper_gpio_out")
@pytest.mark.app_defines(
    # 延时 8s：给 helper 稳定高电平 + DUT 重启 arm 留足余量。
    "|HELPER_GPIO_PIN=_PA_17,HELPER_DELAY_MS=8000,HELPER_LEVEL=0,HELPER_PULSE_MS=100"
)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_tim_pwm_one_pulse|./bin/helper_gpio_out"],
    ids=["pwm_one_pulse|helper"],
    indirect=True,
)
def test_raw_tim_pwm_one_pulse(dut):
    """[双板] PWM one pulse：helper 板拉低 BUTTON_PIN 后，DUT GPIO interrupt 捕获 PWM 输出。"""
    test_board, helper_board = dut

    # 1) 先复位 helper 并等它稳定在 init 高电平：helper GPIO_Init 的上电毛刺
    helper_board.hard_reset()
    helper_board.expect(r"HELPER1: init level=1", timeout=10)

    # 2) 再 arm DUT；此后 BUTTON_PIN 一直被 helper 拉高，无边沿。
    test_board.hard_reset()
    test_board.expect(r"PWM_ONE_PULSE_READY", timeout=10)

    # 3) helper 延时结束打出唯一一个干净下降沿 → 触发单脉冲 → 自环回 GPIO 中断。
    helper_board.expect(r"HELPER1: drive level=0", timeout=10)
    test_board.expect(r"gpio_int_ISR pin:\d+ ==> event=\d+", timeout=15)
    helper_board.expect(r"HELPER1: pulse end, level=1", timeout=15)