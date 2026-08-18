"""
Pytest tests for raw RTC example firmware.

Test cases:

- test_raw_rtc           — raw_rtc: 时间设置与每秒递增
- test_raw_rtc_alarm     — raw_rtc_alarm: 闹钟中断触发

Usage::

    cd example/peripheral/raw/RTC/
    pytest ../../auto_test/raw/pytest_raw_rtc.py \\
        --port COM6 \\
        --target RTL8720F
"""

import pytest

RE_FORMAT = (
    r"Time as a custom formatted string = "
    r"(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})"
)

@pytest.mark.generic
@pytest.mark.app_name("raw_rtc")
@pytest.mark.parametrize("image_dir", ["./bin/raw_rtc"], ids=["rtc"], indirect=True)
def test_raw_rtc(dut):
    """RTC 时间每秒递增。抓两轮格式化时间，验证 RTC 秒数递增。"""
    m1 = dut.expect(RE_FORMAT, timeout=10)
    t1 = [int(m1.group(i)) for i in range(1, 7)]

    m2 = dut.expect(RE_FORMAT, timeout=5)
    t2 = [int(m2.group(i)) for i in range(1, 7)]

    # 将 HH:MM:SS 转为秒数，计算差值（容忍分钟翻卷）
    s1 = t1[3] * 3600 + t1[4] * 60 + t1[5]
    s2 = t2[3] * 3600 + t2[4] * 60 + t2[5]
    delta = s2 - s1 if s2 >= s1 else s2 + 3600 - s1

    # DelayMs(1001) 保证两次输出间隔 >1s，RTC 秒数必定递增
    assert delta >= 1, f"time not advancing: delta={delta}s"


@pytest.mark.generic
@pytest.mark.app_name("raw_rtc_alarm")
@pytest.mark.parametrize("image_dir", ["./bin/raw_rtc_alarm"], ids=["alarm"], indirect=True)
def test_raw_rtc_alarm(dut):
    """RTC 闹钟中断触发。抓两轮 ISR（间隔~60s），验证分钟数递增。"""
    RE_ISR = r"RTC_ISR time: (\d+):(\d+):(\d+):(\d+) \((\d+)\)"

    dut.hard_reset()
    m1 = dut.expect(RE_ISR, timeout=10)
    t1 = int(m1.group(3))  # Minutes

    m2 = dut.expect(RE_ISR, timeout=70)
    t2 = int(m2.group(3))  # Minutes

    delta = t2 - t1 if t2 >= t1 else t2 + 60 - t1
    assert delta >= 1, f"alarm period does not match 60s: minute delta={delta}"
