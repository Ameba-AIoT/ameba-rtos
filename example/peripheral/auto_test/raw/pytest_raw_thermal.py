"""
Pytest tests for raw Thermal example firmware.

Test cases:

- test_raw_thermal — raw_thermal: 验证三种温度输出格式

硬件需求：无（单板纯软件测试）

Usage::

    cd example/peripheral/raw/Thermal/
    pytest ../../auto_test/raw/pytest_raw_thermal.py \\
        --port COM6 \\
        --target RTL8720F
"""

import pytest


@pytest.mark.generic
@pytest.mark.app_name("raw_thermal")
@pytest.mark.parametrize("image_dir", ["./bin/raw_thermal"], ids=["thermal"], indirect=True)
def test_raw_thermal(dut):
    """温度传感器：读取 SoC 内部温度，验证三种格式输出。"""
    dut.hard_reset()

    # TM_Display_Result: "Temperature: +25.1234 C"
    dut.expect(r"Temperature: [+-]\d+\.\s*\d+ C", timeout=10)
    # TM_GetCdegree: "Temperature = 25.123456 C"
    dut.expect(r"Temperature = [+-]?\d+\.\d+ C", timeout=5)
    # TM_GetFdegree: "Temperature = 77.123456 F"
    dut.expect(r"Temperature = [+-]?\d+\.\d+ F", timeout=5)
