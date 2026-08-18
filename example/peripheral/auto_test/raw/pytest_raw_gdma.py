"""
Pytest tests for raw GDMA example firmware.

Test cases:

- test_raw_gdma_singleblock                 — raw_gdma_singleblock
- test_raw_gdma_multiblock_reloaddst        — raw_gdma_multiblock (GDMA_TYPE=2, ReloadDst)
- test_raw_gdma_multiblock_reloadsrc        — raw_gdma_multiblock (GDMA_TYPE=3, ReloadSrc)
- test_raw_gdma_multiblock_reloadsrcdst     — raw_gdma_multiblock (GDMA_TYPE=4, ReloadSrcDst)
- test_raw_gdma_gather_scatter              — raw_gdma_gather_scatter
- test_raw_gdma_linear_link_list            — raw_gdma_linear_link_list
- test_raw_gdma_circle_link_list_abrt1      — raw_gdma_circle_link_list (ABORT_TRANSFER=1)
- test_raw_gdma_circle_link_list_abrt0      — raw_gdma_circle_link_list (ABORT_TRANSFER=0)
- test_raw_gdma_susp_rsm_abrt0              — raw_gdma_susp_rsm_abrt (ABORT_TRANSFER=0, suspend+resume)
- test_raw_gdma_susp_rsm_abrt1              — raw_gdma_susp_rsm_abrt (ABORT_TRANSFER=1)

硬件需求：
  single-board only，所有测试均使用 MemToMem 模式，无需跳线或双板。

Usage::

    cd example/peripheral/raw/GDMA/
    pytest example/peripheral/auto_test/raw/pytest_raw_gdma.py \\
        --port COM6 \\
        --target RTL8720F \\
    use --auto-build to auto-compile before flashing:
    pytest example/peripheral/auto_test/raw/pytest_raw_gdma.py \\
        --target RTL8720F --port COM6 --auto-build
"""

import re
import pytest
from ameba_pytest import ameba_parametrize


def _ok_or_err(dut, ok_pat, err_pat, timeout, msg):
    """Expect either *ok_pat* (pass) or *err_pat* (fail), fail on err."""
    m = dut.expect(rf"(?:{ok_pat}|{err_pat})", timeout=timeout)
    if re.search(err_pat, m.group(0)):
        pytest.fail(f"{dut.port}: {msg}")


# ---------------------------------------------------------------------------
# 1. Single block
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_singleblock")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_singleblock"], ids=["single"], indirect=True)
def test_raw_gdma_singleblock(dut):
    """GDMA single-block MemToMem: expect OK, not Err."""
    dut.reset()
    _ok_or_err(dut,
               ok_pat=r"DMA Copy Memory Compare OK!!",
               err_pat=r"Compare Err",
               timeout=30,
               msg="singleblock data mismatch")


# ---------------------------------------------------------------------------
# 2. Multi-block (3 variants: ReloadDst, ReloadSrc, ReloadSrcDst)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_multiblock")
@pytest.mark.app_defines("GDMA_TYPE=2")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_multiblock/rld_dst"], ids=["rld_dst"], indirect=True)
def test_raw_gdma_multiblock_reload_dst(dut):
    """GDMA multi-block ReloadDst."""
    _expect_multiblock_pass(dut)


@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_multiblock")
@pytest.mark.app_defines("GDMA_TYPE=3")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_multiblock/rld_src"], ids=["rld_src"], indirect=True)
def test_raw_gdma_multiblock_reload_src(dut):
    """GDMA multi-block ReloadSrc."""
    _expect_multiblock_pass(dut)


@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_multiblock")
@pytest.mark.app_defines("GDMA_TYPE=4")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_multiblock/rld_srcdst"], ids=["rld_srcdst"], indirect=True)
def test_raw_gdma_multiblock_reload_srcdst(dut):
    """GDMA multi-block ReloadSrcDst."""
    _expect_multiblock_pass(dut)


def _expect_multiblock_pass(dut):
    """Common multi-block checker: pass on MaxMuliBlock, fail on Error."""
    dut.reset()
    dut.expect(r"(ReloadDst|ReloadSrc|ReloadSrcDst) Test", timeout=10)
    _ok_or_err(dut,
               ok_pat=r"DMA Data MaxMuliBlock",
               err_pat=r"Check Data Error!!!",
               timeout=30,
               msg="multiblock data mismatch")


# ---------------------------------------------------------------------------
# 3. Gather / Scatter
# ---------------------------------------------------------------------------
@pytest.mark.generic
@ameba_parametrize('target', ['RTL8721F', 'RTL8720F'])
@pytest.mark.app_name("raw_gdma_gather_scatter")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_gather_scatter"], ids=["gather_scatter"], indirect=True)
def test_raw_gdma_gather_scatter(dut):
    """GDMA source gather and destination scatter.

    Both tasks run concurrently; their ``PASS``/``FAIL`` lines
    may appear in any order.  Match each by name.
    """
    dut.reset()
    for expected_name in ("Source Gather", "Dest Scatter"):
        m = dut.expect(rf"{expected_name} (PASS|FAIL)", timeout=30)
        if m.group(1) == "FAIL":
            pytest.fail(f"{dut.port}: {expected_name} reported FAIL")


# ---------------------------------------------------------------------------
# 4. Linear link list
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_linear_link_list")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_linear_link_list"], ids=["linear_ll"], indirect=True)
def test_raw_gdma_linear_link_list(dut):
    """GDMA linear link-list transfer."""
    dut.reset()
    _ok_or_err(dut,
               ok_pat=r"transfer over",
               err_pat=r"Check Data Error!!!",
               timeout=30,
               msg="linear link list data mismatch")


# ---------------------------------------------------------------------------
# 5. Circle link list (2 variants: abort, loop-limited)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_circle_link_list")
@pytest.mark.app_defines("ABORT_TRANSFER=1")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_circle_link_list/abrt1"], ids=["1"], indirect=True)
def test_raw_gdma_circle_link_list_abrt1(dut):
    """GDMA circle link-list with abort."""
    dut.reset()
    dut.expect(r"Last Block \d+ is over, and transmission terminated", timeout=30)
    _ok_or_err(dut,
               ok_pat=r"transfer over",
               err_pat=r"Check Data Error!!!",
               timeout=10,
               msg="circle link-list abort data error")


@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_circle_link_list")
@pytest.mark.app_defines("ABORT_TRANSFER=0")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_circle_link_list/abrt0"], ids=["0"], indirect=True)
def test_raw_gdma_circle_link_list_abrt0(dut):
    """GDMA circle link-list non-abort: runs forever; check one full cycle.

    The circular linked list never terminates.  Verify one full cycle
    (8 blocks) is error-free, then return.  The DUT fixture resets
    the board before the next test.
    """
    for _ in range(8):
        m = dut.expect(r"(?:Block \d+ Over|Check Data Error!!!)", timeout=15)
        if "Check Data Error!!!" in m.group(0):
            pytest.fail(f"{dut.port}: circle link-list loop data mismatch")


# ---------------------------------------------------------------------------
# 6. Suspend / Resume / Abort (2 variants)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_susp_rsm_abrt")
@pytest.mark.app_defines("ABORT_TRANSFER=0")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_susp_rsm_abrt/abrt0"], ids=["0"], indirect=True)
def test_raw_gdma_susp_rsm_abrt0(dut):
    """GDMA suspend+resume: suspend twice, resume twice, then complete."""
    dut.reset()
    for _ in range(2):
        dut.expect(r"transfer suspend, and \d+ bytes have been moved", timeout=15)
        dut.expect(r"transfer resume", timeout=10)
    _ok_or_err(dut,
               ok_pat=r"transfer over",
               err_pat=r"Check Data Error!!!",
               timeout=10,
               msg="suspend/resume data mismatch")


@pytest.mark.generic
@pytest.mark.app_name("raw_gdma_susp_rsm_abrt")
@pytest.mark.app_defines("ABORT_TRANSFER=1")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gdma_susp_rsm_abrt/abrt1"], ids=["1"], indirect=True)
def test_raw_gdma_susp_rsm_abrt1(dut):
    """GDMA suspend+abort: suspend once, abort mid-transfer.

    Abort terminates the DMA early — this test verifies the abort
    mechanism is crash-free.
    """
    dut.reset()
    dut.expect(r"transfer suspend, and \d+ bytes have been moved", timeout=15)
    dut.expect(r"transfer abort", timeout=10)
    dut.expect(r"transfer over", timeout=10)
