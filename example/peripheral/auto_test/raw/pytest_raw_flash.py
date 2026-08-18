"""
Pytest test for raw Flash read/write and OTP example firmware (single-board).

硬件需求：
  单板，无需外部接线

Usage::

    cd example/peripheral/auto_test
    pytest raw/pytest_raw_flash.py --port COM6 --target RTL8720F
"""

import pytest
from ameba_pytest import ameba_parametrize


@pytest.mark.generic
@pytest.mark.app_name("raw_flash_read_write")
@pytest.mark.parametrize("image_dir", ["./bin/raw_flash_read_write"], ids=["flash_rw"], indirect=True)
def test_raw_flash_read_write(dut):
    """Flash 扇区擦除 + 写入 0x13572468 + 回读验证，重复 10 轮。
    预期每轮输出 "Result is success"，10 轮全部通过为 PASS。
    """
    dut.hard_reset()

    for i in range(10):
        m = dut.expect_exact("Result is success", timeout=30)
        if m is None:
            pytest.fail(f"Loop {i + 1} failed")


@ameba_parametrize('target', ['RTL8730E','RTL8726E','RTL8720E','RTL8713E','RTL8710E','RTL8721Dx','RTL8721F'])
@pytest.mark.generic
@pytest.mark.app_name("raw_flash_otp")
@pytest.mark.parametrize("image_dir", ["./bin/raw_flash_otp"], ids=["flash_otp"], indirect=True)
def test_raw_flash_otp(dut):
    """Security Register OTP 功能验证：擦除 → 写入随机数据 → 回读比对。
    预期输出 "########Test security registers before OTP finished########"
    表示测试成功完成。若回读比对发现 mismatch，固件会打印 "verify failed"，
    但不会 halt，pytest 同时检测以捕获此假绿场景。
    """
    dut.hard_reset()

    m = dut.expect([r"verify failed", r"########Test security registers before OTP finished########"], timeout=30)
    if m is None:
        pytest.fail("OTP test failed: timeout waiting for completion")
    if "verify failed" in m.group():
        pytest.fail("OTP test failed: data verification mismatch detected")
