"""
Pytest tests for raw Watchdog (WDG) example firmware.

Test cases:

- test_raw_watchdog  — raw_watchdog:  窗口保护复位 → 中断喂狗 → SW 定时器喂狗全流程

硬件需求：无（单板纯软件测试）

Usage::

    cd example/peripheral/auto_test/
    pytest raw/pytest_raw_watchdog.py --target RTL8720F --port COM6
"""

import pytest


@pytest.mark.generic
@pytest.mark.app_name("raw_watchdog")
@pytest.mark.parametrize("image_dir", ["./bin/raw_watchdog"], ids=["watchdog"], indirect=True)
def test_raw_watchdog(dut):
    """Watchdog 全流程：窗口保护复位 → 中断喂狗 → SW 定时器喂狗 → 成功。

    验证序列：
      1. "doing small task"                             — 冷启动，小任务开始
      2. "reset by refresh watchdog inside Protected Window"
                                                        — 小任务在窗口期内喂狗触发 WDG 复位重启
      3. "doing big task"                               — 复位后大任务启动
      4. "WDG refreshed by watchdog interrupt"          — WDG 早期中断（≥1 次）自动喂狗
      5. "big task done, watchdog is refreshed in big task"
                                                        — 10s 大任务结束
      6. "WDG refreshed by sw timer" ×5                — SW 定时器喂狗 5 次
      7. "stop sw timer, refresh watchdog by interrupt" — 定时器停止，切回中断喂狗
      8. "watchdog raw example exec succeed!"           — 示例通过
    """
    dut.hard_reset()
    dut.expect(r"WDG refreshed by sw timer", timeout=40)  # 含启动+WDG复位重启+big_task(10s)+首次timer触发
    for _ in range(4):
        dut.expect(r"WDG refreshed by sw timer", timeout=8)
    dut.expect(r"watchdog raw example exec succeed!", timeout=5)
