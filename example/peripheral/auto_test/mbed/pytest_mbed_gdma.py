"""
Pytest tests for mbed GDMA example firmware.

Test cases:

- test_mbed_gdma_singleblock  — mbed_gdma_singleblock

硬件需求：
  single-board only，MemToMem 模式，无需跳线或双板。

Usage::

    cd example/peripheral/mbed/GDMA/
    pytest pytest_mbed_gdma.py \\
        --port COM6 \\
        --target RTL8720F \\
    use --auto-build to auto-compile before flashing:
    pytest pytest_mbed_gdma.py \\
        --target RTL8720F --port COM6 --auto-build
"""

import re
import pytest


def _ok_or_err(dut, ok_pat, err_pat, timeout, msg):
    """Expect either *ok_pat* (pass) or *err_pat* (fail), fail on err."""
    m = dut.expect(rf"(?:{ok_pat}|{err_pat})", timeout=timeout)
    if re.search(err_pat, m.group(0)):
        pytest.fail(f"{dut.port}: {msg}")


@pytest.mark.generic
@pytest.mark.app_name("mbed_gdma_singleblock")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gdma_singleblock"], ids=["single"], indirect=True)
def test_mbed_gdma_singleblock(dut):
    """GDMA single-block MemToMem via mbed API: expect OK, not Err."""
    dut.reset()
    dut.expect(r"DMA Copy Done!!", timeout=30)
    _ok_or_err(dut,
               ok_pat=r"DMA Copy Memory Compare OK!!",
               err_pat=r"Compare Err",
               timeout=10,
               msg="singleblock data mismatch")
