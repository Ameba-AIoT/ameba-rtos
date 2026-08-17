"""
Pytest test for mbed Flash read/write example firmware (single-board).

硬件需求：
  单板，无需外部接线

Usage::

    cd example/peripheral/auto_test
    pytest mbed/pytest_mbed_flash.py --port COM6 --target RTL8720F
"""

import pytest


@pytest.mark.generic
@pytest.mark.app_name("mbed_flash_read_write")
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_flash_read_write"],
    ids=["flash_rw"],
    indirect=True,
)
def test_mbed_flash_read_write(dut):
    """Flash 扇区擦除 + 写入 0x13572468 + 回读验证，重复 10 轮。
    预期每轮输出 "Result is success"，10 轮全部通过为 PASS。
    """
    dut.hard_reset()

    for i in range(10):
        m = dut.expect_exact("Result is success", timeout=30)
        if m is None:
            pytest.fail(f"Loop {i + 1} failed")
