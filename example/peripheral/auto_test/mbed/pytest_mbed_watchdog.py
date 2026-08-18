"""
Pytest tests for mbed Watchdog (WDG) example firmware.

Test cases:

- test_mbed_watchdog — mbed_watchdog: WDG 超时触发中断复位 → 成功打印

硬件需求：无（单板纯软件测试）

Usage::

    cd example/peripheral/auto_test/
    pytest mbed/pytest_mbed_watchdog.py --target RTL8720F --port COM6
"""

import pytest


@pytest.mark.generic
@pytest.mark.app_name("mbed_watchdog")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_watchdog"], ids=["watchdog"], indirect=True)
def test_mbed_watchdog(dut):
    """mbed Watchdog：big_task 超时未喂狗 → WDG 中断打印 → 系统复位 → 成功。

    验证序列：
      1. "watchdog barks!!!"              — WDG 5s 超时，中断回调触发
      2. "watchdog mbed example exec succeed!" — 复位重启后确认成功
    """
    dut.hard_reset()
    # boot(~5s) + small_task(1s喂狗) + big_task(5s超时) ≈ 11s，留余量
    dut.expect(r"watchdog barks!!!", timeout=30)
    dut.expect(r"reset by watchdog barks", timeout=30)
    dut.expect(r"watchdog mbed example exec succeed!", timeout=20)