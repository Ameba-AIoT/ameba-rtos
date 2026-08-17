"""
Pytest tests for raw PMC (Power Management Control) example firmware.

Test cases:

- test_raw_pmc_dslp       — raw_pmc_dslp (single board, no wiring)
- test_raw_pmc_pg         — raw_pmc_pg (single board, no wiring)
- test_raw_pmc_wakepin    — raw_pmc_wakepin + helper_gpio_out (dual board)

硬件需求：
  dslp / pg           — 单板，无需外部接线
  wakepin（双板）      — 板 A PA_21 ↔ 板 B PA_21 直连

  板 A (DUT): raw_pmc_wakepin 固件，配置 WAKEPIN_2 = PA_21, HIGH_LEVEL_WAKEUP
  板 B:       helper_gpio_out 固件，15 秒后驱动 PA_21 HIGH → 唤醒板 A

Usage::

    cd example/peripheral/auto_test

    # 单板测试（dslp / pg）
    pytest raw/pytest_raw_pmc.py::test_raw_pmc_dslp \\
        --port COM6 --target RTL8720F

    # 双板测试（wakepin）
    pytest raw/pytest_raw_pmc.py::test_raw_pmc_wakepin \\
        --port 'COM6|COM9' --target RTL8720F -k "test_raw_pmc_wakepin"
"""

import pytest


# ── single-board: raw_pmc_dslp ──────────────────────────────────────────────


@pytest.mark.generic
@pytest.mark.app_name("raw_pmc_dslp")
@pytest.mark.parametrize("image_dir", ["./bin/raw_pmc_dslp"], ids=["dslp"], indirect=True)
def test_raw_pmc_dslp(dut):
    """Deep Sleep + AON Timer 唤醒：芯片进入 DSLP，AON 定时器 5s 后唤醒，
    备份寄存器 BKUP_REG1 数据保持。

    验证序列：
      1. "enter deepsleep mode after 5S"  — 冷启动，准备进 DSLP
      2. "Save 0x12345678 into BKUP_REG1" — DSLP 前写入备份寄存器
      3. "dslp wake from aontimer"        — AON 定时器唤醒
      4. "BKUP_REG1's value = 0x12345678" — 唤醒后读回备份值一致
    """
    dut.hard_reset()
    dut.expect(r"enter deepsleep mode after 5S", timeout=10)
    dut.expect(r"Save 0x12345678 into BKUP_REG1", timeout=10)
    dut.expect(r"dslp wake from aontimer", timeout=15)
    dut.expect(r"BKUP_REG1's value = 0x12345678", timeout=5)


# ── single-board: raw_pmc_pg ────────────────────────────────────────────────


@pytest.mark.generic
@pytest.mark.app_name("raw_pmc_pg")
@pytest.mark.parametrize("image_dir", ["./bin/raw_pmc_pg"], ids=["pg"], indirect=True)
def test_raw_pmc_pg(dut):
    """Power Gating + AON Timer 唤醒：芯片进入 PG 模式，2s 后 AON 定时器唤醒。

    验证序列：
      1. "AP start enter sleep mode"    — suspend 回调，准备进 PG
      2. "app_peripheral_suspend"       — 外设暂停回调
      3. "app_peripheral_resume"        — 外设恢复回调
      4. "PG wakeup test pass"          — PG 唤醒成功
    """
    dut.expect(r"AP start enter sleep mode", timeout=15)
    dut.expect(r"app_peripheral_suspend", timeout=5)
    dut.expect(r"app_peripheral_resume", timeout=10)
    dut.expect(r"PG wakeup test pass", timeout=5)


# ── dual-board: raw_pmc_wakepin + helper_gpio_out ────────────────────────────


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("raw_pmc_wakepin|helper_gpio_out")
@pytest.mark.app_defines(
    "PMC_WAKEPIN=WAKEPIN_2|HELPER_GPIO_PIN=_PA_21,HELPER_DELAY_MS=15000,HELPER_LEVEL=1"
)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_pmc_wakepin|./bin/helper_gpio_out"],
    ids=["wakepin|helper"],
    indirect=True,
)
def test_raw_pmc_wakepin(dut):
    """[双板] Wake Pin 唤醒 DSLP：板 B 延时 15s 驱动 PA_21 HIGH，唤醒板 A。

    接线：板 A PA_21 ↔ 板 B PA_21

    板 A (raw_pmc_wakepin) 验证序列：
      1. "set aon wakepin to wakeup"    — 配置 Wakepin
      2. "enter deepsleep mode after 5S" — 5s 后进入 DSLP
      3. "dslp wake from wakepin"       — 板 B 驱动 PA_21 HIGH 唤醒
    """
    test_board, peer_board = dut

    # 板 A：冷启动，配置 Wakepin
    test_board.hard_reset()
    test_board.expect(r"set aon wakepin to wakeup", timeout=10)
    test_board.expect(r"enter deepsleep mode after 5S", timeout=5)

    # 板 B 计时 15 秒后驱动 PA_21 HIGH → 板 A 唤醒
    peer_board.expect(r"HELPER1: drive level=1", timeout=20)
    test_board.expect(r"dslp wake from wakepin", timeout=30)
