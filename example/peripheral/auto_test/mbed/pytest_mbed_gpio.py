"""
Pytest tests for mbed GPIO example firmware.

Test cases:

- test_mbed_gpio_rw            — mbed_gpio_rw: loopback read/write (GPIO_LOOPBACK=1)
- test_mbed_gpio_level_irq     — mbed_gpio_level_irq: level trigger interrupt
- test_mbed_gpio_edge_irq      — mbed_gpio_edge_irq: edge trigger interrupt
- test_mbed_gpio_pulse_measure — mbed_gpio_pulse_measure: pulse width measurement
- test_mbed_gpio_port          — mbed_gpio_port: dual-board port read/write
- test_mbed_gpio_jtag_dual    — mbed_gpio_jtag: dual-board JTAG off + LED pin test

硬件需求（RTL8720F）：
  gpio_loopback — 跳线连接 SIGNAL_SOURCE 与 IRQ 引脚
                  level_irq / edge_irq / pulse_measure 均适用（RTL8720F PA_23 <-> PA_24）
  multi_board   — 两块板子，对应 GPIO 引脚一一对接
                  jtag 测试：辅助板 GPIO_JTAG_ENABLE_PIN -> DUT GPIO_JTAG_ENABLE_PIN（拉低触发关 JTAG）
                  DUT GPIO_LED_PIN 编译时覆盖为 PA_24，避免 SWD (RTL8720F PA_15)冲突

Usage::

    cd example/peripheral/mbed/GPIO/
    pytest pytest_mbed_gpio.py \\
        --port COM6 \\
        --target RTL8720F \\
        --auto-build

    # 双板测试
    pytest pytest_mbed_gpio.py -m multi_board \\
        --port 'COM6|COM9' --target RTL8720F \\
        --auto-build
"""

import pytest


# ---------------------------------------------------------------------------
#  0. mbed_gpio_rw — loopback 读写
# ---------------------------------------------------------------------------

@pytest.mark.gpio_loopback
@pytest.mark.app_name("mbed_gpio_rw")
@pytest.mark.app_defines("GPIO_LOOPBACK=1")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gpio_rw"], ids=["rw"], indirect=True)
def test_mbed_gpio_rw(dut):
    """GPIO mbed read/write loopback：写 GPIO_LED_PIN → 读 GPIO_PUSHBT_PIN（跳线），3 轮全 PASS。

    需跳线 GPIO_LED_PIN <-> GPIO_PUSHBT_PIN（RTL8720F: PA_23 <-> PA_24）。
    固件每秒翻转输出并打印 PASS/FAIL，只要出现 FAIL 立即报错。
    """
    for i in range(3):
        m = dut.expect(r"(PASS|FAIL): Output=([01]), Input=([01])", timeout=15)
        if m.group(1) == "FAIL":
            pytest.fail(
                f"{dut.port}: FAIL detected (round {i + 1}) — "
                f"Output={m.group(2)}, Input={m.group(3)}"
            )


# ---------------------------------------------------------------------------
#  1. mbed_gpio_level_irq — 电平触发中断
# ---------------------------------------------------------------------------

@pytest.mark.gpio_loopback
@pytest.mark.app_name("mbed_gpio_level_irq")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gpio_level_irq"], ids=["level_irq"], indirect=True)
def test_mbed_gpio_level_irq(dut):
    """GPIO 电平中断：low/high level event 交替出现 3 轮。

    需跳线 GPIO_SIGNAL_SOURCE <-> GPIO_IRQ_LEVEL_PIN
    （RTL8720F: PA_23 <-> PA_24）。
    """
    for i in range(3):
        dut.expect("low level event", timeout=5)
        dut.expect("high level event", timeout=5)


# ---------------------------------------------------------------------------
#  2. mbed_gpio_edge_irq — 边沿触发中断
# ---------------------------------------------------------------------------

@pytest.mark.gpio_loopback
@pytest.mark.app_name("mbed_gpio_edge_irq")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gpio_edge_irq"], ids=["edge_irq"], indirect=True)
def test_mbed_gpio_edge_irq(dut):
    """GPIO 边沿中断：falling/rising event 交替出现 3 轮。

    需跳线 GPIO_SIGNAL_SOURCE <-> GPIO_IRQ_EDGE_PIN
    （RTL8720F: PA_23 <-> PA_24）。
    """
    for i in range(3):
        dut.expect("falling edge event", timeout=5)
        dut.expect("rising edge event", timeout=5)


# ---------------------------------------------------------------------------
#  3. mbed_gpio_pulse_measure — 脉冲宽度测量
# ---------------------------------------------------------------------------

@pytest.mark.gpio_loopback
@pytest.mark.app_name("mbed_gpio_pulse_measure")
@pytest.mark.parametrize("image_dir", ["./bin/mbed_gpio_pulse_measure"], ids=["pulse"], indirect=True)
def test_mbed_gpio_pulse_measure(dut):
    """GPIO 脉冲宽度测量：OUT_PIN -> IRQ_PIN 跳线。

    固件每 500ms 输出一个 1000us 高电平脉冲，IRQ handler 测 rising→falling
    间隔后打印 ``<值>us``。受中断延迟影响实际值略大于 1000us，用 900-1100us
    区间判定。
    """
    for i in range(3):
        m = dut.expect(r"(\d+)us", timeout=5)
        duration = int(m.group(1))
        if not (900 <= duration <= 1100):
            pytest.fail(
                f"{dut.port}: pulse width out of range (round {i + 1}) — "
                f"expected ~1000us, got {duration}us"
            )


# ---------------------------------------------------------------------------
#  4. mbed_gpio_port — Port 模式双板对测
#  Helper functions 逻辑与 raw GPIO port 测试保持一致
# ---------------------------------------------------------------------------


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
@pytest.mark.app_name("mbed_gpio_port")
@pytest.mark.app_defines("PORT_OUTPUT_TEST=1|PORT_OUTPUT_TEST=0")
@pytest.mark.parametrize("image_dir",
                         ["./bin/mbed_gpio_port/output|./bin/mbed_gpio_port/input"],
                         ids=["out|in"], indirect=True)
def test_mbed_gpio_port(dut):
    """GPIO Port 双板对测（固件零改动，验证在 host 侧）。

    输出板按序驱动 led_pattern[] 并打印 ``[i]set port: 0xV``；输入板电平变化时
    打印 ``< 0xY``。两板遍历同一张表、同样顺序，故输入读到的有序序列必是输出
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

    # 2. 从输入板采足够覆盖旋转匹配的变化值（约两轮，留裕量）。
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


# ---------------------------------------------------------------------------
#  5. mbed_gpio_jtag — JTAG disable + GPIO reuse（双板）
# ---------------------------------------------------------------------------

@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.app_name("mbed_gpio_jtag|helper_gpio_out")
@pytest.mark.app_defines(
    "GPIO_LED_PIN=_PA_24|HELPER_GPIO_PIN=_PA_23,HELPER_LEVEL=0,HELPER_DELAY_MS=0"
)
@pytest.mark.parametrize("image_dir",
    ["./bin/mbed_gpio_jtag|../tools/helper_gpio_out/bin"],
    ids=["jtag|helper"], indirect=True)
def test_mbed_gpio_jtag_dual(dut):
    """[双板] RTL8720F 辅助板拉低 PA_23 -> DUT 关 JTAG -> GPIO 读写 3 轮验证。注意：换IC需要换PIN(TODO: 适配其他芯片)。

    RTL8720F 接线(双板榫卯全对接):
      helper PA_23  -> DUT PA_23 (GPIO_JTAG_ENABLE_PIN, 拉低触发关 JTAG)
      helper GND    -> DUT GND (共地)

    编译时 DUT 的 GPIO_LED_PIN 覆盖为 _PA_24, 避开双板全对接下 SWD 引脚
    (PA_15) 的冲突。DUT 在 PA_24 上做 GPIO 翻转并自读回验证。
    """
    target, helper = dut

    # 双板 hard_reset：清空缓存，确保从启动开始捕获输出
    helper.hard_reset()
    target.hard_reset()

    # 1. 辅助板: 确认 PA_23 已输出 LOW
    helper.expect(r"HELPER1: init level=1", timeout=15)
    helper.expect(r"HELPER1: drive level=0", timeout=5)
    helper.expect(r"HELPER1: done", timeout=5)

    # 2. DUT: 确认进入 JTAG-off 路径
    target.expect("jtag off", timeout=15)

    # 3. DUT: 验证 3 轮 GPIO 读写 (LED_PIN 翻转后自读回)
    for i in range(3):
        m1 = target.expect(r"Expect 1, Read (\d)", timeout=5)
        if int(m1.group(1)) != 1:
            pytest.fail(
                f"{target.port}: round {i+1} write 1 but read {m1.group(1)}")

        m2 = target.expect(r"Expect 0, Read (\d)", timeout=5)
        if int(m2.group(1)) != 0:
            pytest.fail(
                f"{target.port}: round {i+1} write 0 but read {m2.group(1)}")
