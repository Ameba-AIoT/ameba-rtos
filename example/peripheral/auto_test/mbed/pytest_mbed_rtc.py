"""
Pytest tests for mbed RTC example firmware.

Test cases:

- test_mbed_rtc           — mbed_rtc: 时间设置与每秒递增
- test_mbed_rtc_alarm     — mbed_rtc_alarm: 闹钟 10 秒后触发

Usage::

    cd example/peripheral/auto_test/
    pytest mbed/pytest_mbed_rtc.py \\
        --port COM6 \\
        --target RTL8720F
"""

import pytest


@pytest.mark.generic
@pytest.mark.app_name("mbed_rtc")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_rtc"], ids=["rtc"], indirect=True)
def test_mbed_rtc(dut):
    """RTC 时间每秒递增。抓两轮格式化时间，验证秒数递增。"""
    RE_FORMAT = (
        r"Time as a custom formatted string = "
        r"(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})"
    )

    m1 = dut.expect(RE_FORMAT, timeout=10)
    t1 = [int(m1.group(i)) for i in range(1, 7)]

    m2 = dut.expect(RE_FORMAT, timeout=5)
    t2 = [int(m2.group(i)) for i in range(1, 7)]

    # 将 HH:MM:SS 转为秒数，计算差值（容忍分钟翻卷）
    s1 = t1[3] * 3600 + t1[4] * 60 + t1[5]
    s2 = t2[3] * 3600 + t2[4] * 60 + t2[5]
    delta = s2 - s1 if s2 >= s1 else s2 + 3600 - s1

    # wait(1.0) 保证两次输出间隔 >1s，RTC 秒数必定递增
    assert delta >= 1, f"time not advancing: delta={delta}s"


@pytest.mark.generic
@pytest.mark.app_name("mbed_rtc_alarm")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_rtc_alarm"], ids=["alarm"], indirect=True)
def test_mbed_rtc_alarm(dut):
    """RTC 闹钟 10 秒后触发。先检查初始时间，再等闹钟输出。"""
    dut.hard_reset()
    m_now = dut.expect(r"now time = (\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)", timeout=10)
    assert m_now, "initial time not printed"

    m_alarm = dut.expect(r"alarm time = (\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)", timeout=15)
    assert m_alarm, "alarm not triggered within 15s"

    # 验证闹钟触发时秒数为 10
    sec = int(m_alarm.group(6))
    assert sec == 10, (
        f"alarm triggered at second {sec}, expected 10"
    )
