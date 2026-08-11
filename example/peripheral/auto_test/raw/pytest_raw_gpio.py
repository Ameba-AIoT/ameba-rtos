"""
Pytest tests for raw GPIO example firmware.

Test cases:

- test_raw_gpio_rw           — raw_gpio_rw
- test_raw_gpio_edge_irq     — raw_gpio_edge_irq
- test_raw_gpio_level_irq    — raw_gpio_level_irq
- test_raw_gpio_port         — raw_gpio_port

硬件需求（RTL8720F）：
  gpio_loopback — 跳线 PA_23 <-> PA_24
  multi_board   — 两块板子，PAx 一一对接

Usage::

    cd example/peripheral/raw/GPIO/
    pytest pytest_example_gpio.py \\
        --port COM6\\
        --target RTL8720F \\
"""

import pytest

@pytest.mark.gpio_loopback
@pytest.mark.app_name("raw_gpio_rw")
@pytest.mark.app_defines("GPIO_LOOPBACK=1")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gpio_rw"], ids=["rw"], indirect=True)
def test_raw_gpio_rw(dut):
    """GPIO read/write loopback：写 PA_23 → 读 PA_24（跳线），输出=输入连续 3 轮。

    每轮固件输出 PASS 或 FAIL，3 轮全部 PASS 才算通过，**只要有一轮 FAIL 则报错**。
    注意：固件按 Output=1 → 0 → 1 → 0 交替，无跳线时偶数轮 Output=0 读回 1（上拉）
    会产生 FAIL，此时 3 轮 expect 可能全部命中 PASS 行而漏报；本实现用捕获组直判状态。
    """
    for i in range(3):
        m = dut.expect(r"(PASS|FAIL): Output=([01]), Input=([01])", timeout=15)
        if m.group(1) == "FAIL":
            pytest.fail(
                f"{dut.port}: FAIL detected (round {i + 1}) — "
                f"Output={m.group(2)}, Input={m.group(3)}"
            )


@pytest.mark.gpio_loopback
@pytest.mark.app_name("raw_gpio_edge_irq")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gpio_edge_irq"], indirect=True)
def test_raw_gpio_edge_irq(dut):
    """GPIO 边沿中断：falling/rising 交替出现 3 轮。需跳线 PA_23 <-> PA_24。"""
    for i in range(3):
        dut.expect("falling edge event", timeout=5)
        dut.expect("rising edge event", timeout=5)

@pytest.mark.gpio_loopback
@pytest.mark.app_name("raw_gpio_level_irq")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gpio_level_irq"], indirect=True)
def test_raw_gpio_level_irq(dut):
    """GPIO 电平中断：low/high 交替出现 3 轮。需跳线 PA_23 <-> PA_24。"""
    for i in range(3):
        dut.expect("low level event", timeout=5)
        dut.expect("high level event", timeout=5)


def _fold_cyclic(seq):
    """折叠相邻重复值（含首尾接缝）。输入序列 -> 无相邻重复的环形序列。"""
    out = []
    for v in seq:
        if not out or out[-1] != v:
            out.append(v)
    if len(out) > 1 and out[0] == out[-1]:
        out.pop()
    return out


def _rotation_match(reads, cyc):
    """reads 中是否存在长度 L 的连续窗口，恰为环形序列 cyc 的某个旋转。"""
    L = len(cyc)
    if L == 0 or len(reads) < L:
        return False
    for s in range(len(reads) - L + 1):
        window = reads[s:s + L]
        for k in range(L):
            if cyc[k] != window[0]:
                continue
            if all(cyc[(k + j) % L] == window[j] for j in range(L)):
                return True
    return False


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("raw_gpio_port")
@pytest.mark.app_defines("PORT_OUTPUT_TEST=1|PORT_OUTPUT_TEST=0")
@pytest.mark.parametrize("image_dir", ["./bin/raw_gpio_port/output|./bin/raw_gpio_port/input"], ids=["out|in"], indirect=True)
def test_raw_gpio_port(dut):
    """GPIO Port 双板对测（固件零改动，验证在 host 侧）。

    输出板按序驱动 led_pattern[] 并打印 ``[i]set port: 0xV``；输入板电平变化时
    打印 `` < 0xY``。两板遍历同一张表、同样顺序，故输入读到的有序序列必是输出
    序列的一个旋转（起点因上电先后不同）。用旋转匹配判定相符，天然吸收上电时间差。
    """
    out_board, in_board = dut

    # 1. 从输出板采一整轮 set port（含行内 index），以 index 回绕到 0 为一轮边界。
    s_out = []
    seen_zero = 0
    while True:
        m = out_board.expect(r'\[(\d+)\]set port: 0x([0-9a-f]+)', timeout=15)
        idx, val = int(m.group(1)), int(m.group(2), 16)
        if idx == 0:
            seen_zero += 1
            if seen_zero == 2:
                break
            s_out = [val]          # 从第一个 index 0 起，重置为整轮起点
        elif seen_zero >= 1:
            s_out.append(val)

    cyc = _fold_cyclic(s_out)
    if len(cyc) < 2:
        pytest.fail(f"{out_board.port}: output pattern degenerate, cyc={[hex(v) for v in cyc]}")

    # 2. 从输入板采足够覆盖旋转匹配的变化值（约两轮，留裕量）。输入固件仅在变化时
    #    打印，天然无相邻重复；忽略开机基线行 ` > value_old`。
    reads = []
    target = 2 * len(cyc)
    while len(reads) < target:
        m = in_board.expect(r'< 0x([0-9a-f]+)', timeout=15)
        reads.append(int(m.group(1), 16))

    # 3. 旋转匹配：输入序列须含一个完整周期窗口，恰为输出环形序列的某旋转。
    if not _rotation_match(reads, cyc):
        pytest.fail(
            f"{in_board.port}: sequence mismatch\n"
            f"  expected cycle: {[hex(v) for v in cyc]}\n"
            f"  input reads:    {[hex(v) for v in reads]}"
        )
