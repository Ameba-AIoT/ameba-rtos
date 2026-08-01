"""
Pytest tests for mbed UART example firmware.

Test cases:

- test_mbed_uart_poll_trx       — mbed_uart_poll_trx: polling loopback
- test_mbed_uart_int_trx        — mbed_uart_int_trx: interrupt loopback
- test_mbed_uart_int_stream     — mbed_uart_int_stream: 8-byte stream echo
- test_mbed_uart_int_rx_timeout — mbed_uart_int_rx_timeout: timeout (<13B) / full echo (13B)
- test_mbed_uart_dma_stream     — mbed_uart_dma_stream: 2~33B variable-length DMA echo
- test_mbed_uart_dma_rx_fc      — mbed_uart_dma_rx_fc: DMA flow controller echo
- test_mbed_uart_poll_autoctrl  — mbed_uart_poll_autoctrl: dual-board full-duplex, polling RX + flow control

All single-board tests use the USB-to-TTL adapter connected to PC:
  UART_TX -> RXD, UART_RX -> TXD, GND -> GND

Usage::
    1. Build the example firmware and save to bin/ directories, e.g.:
        ameba.py build -a mbed_uart_poll_trx -D FINAL_IMAGE_DIR=<path>/bin/mbed_uart_poll_trx
    2. cd example/peripheral/mbed/UART/ &&
        pytest --target RTL8720F --port "COM6"
       Auto-build:
        pytest --target RTL8720F --port "COM6" --auto-build
    3. if need longrun, use "@pytest.mark.app_defines("LOOP_COUNT=<N>")" to define the number of loops for the test.
"""

import pytest
import time

LOOP_COUNT = 1  # Number of loops for each test (can be overridden with pytest env var)


def _wait_for_prompt(dut, timeout: int = 10) -> None:
    """Poll for shell prompt after boot/reset by sending Enter until '#' appears."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        dut.write("")
        try:
            dut.expect("#", timeout=0.5)
            return
        except Exception:
            continue


# ---------------------------------------------------------------------------
#  1. mbed_uart_poll_trx — 轮询方式收发
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_poll_trx")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_poll_trx"],
                         ids=["poll_trx"], indirect=True)
def test_mbed_uart_poll_trx(dut):
    """UART polling mode: dual-board loopback echo test."""
    peer_board, test_board = dut
    test_board.hard_reset()
    peer_board.expect('Hello World!!!', timeout=20)

    _wait_for_prompt(peer_board)
    for _ in range(LOOP_COUNT):
        peer_board.write("uarttool 123456")
        peer_board.expect('123456', timeout=20)
        peer_board.expect('PASS: 6/6 bytes matched', timeout=20)


# ---------------------------------------------------------------------------
#  2. mbed_uart_int_trx — 中断方式收发
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_int_trx")
@pytest.mark.parametrize("count", [2], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_int_trx"],
                         ids=["int_trx"], indirect=True)
def test_mbed_uart_int_trx(dut):
    """UART interrupt mode: dual-board loopback echo test."""
    peer_board, test_board = dut
    peer_board.expect('Hello World!!!', timeout=20)

    _wait_for_prompt(peer_board)
    for _ in range(LOOP_COUNT):
        peer_board.write("uarttool 123456")
        peer_board.expect('123456', timeout=20)
        peer_board.expect('PASS: 6/6 bytes matched', timeout=20)


# ---------------------------------------------------------------------------
#  3. mbed_uart_int_stream — 中断方式流式收发 (8 字节双板对测)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_int_stream")
@pytest.mark.parametrize("count", [2], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_int_stream"],
                         ids=["int_stream"], indirect=True)
def test_mbed_uart_int_stream(dut):
    """UART interrupt stream: dual-board loopback echo test."""
    peer_board, test_board = dut
    _wait_for_prompt(peer_board)
    data = "12345678"
    for _ in range(LOOP_COUNT):
        peer_board.write(f"uarttool {data}")
        peer_board.expect(data, timeout=20)
        peer_board.expect('PASS: 8/8 bytes matched', timeout=20)


# ---------------------------------------------------------------------------
#  4. mbed_uart_int_rx_timeout — 中断超时接收 (13 字节 / 超时)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_int_rx_timeout")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_int_rx_timeout"],
                         ids=["int_rx_timeout"], indirect=True)
def test_mbed_uart_int_rx_timeout(dut):
    """UART interrupt: dual-board test.

    Sends 13 bytes to trigger full-receive path, then sends 5 bytes
    to exercise the timeout path (< 13 bytes within 1000 ms).
    """
    peer_board, test_board = dut
    _wait_for_prompt(peer_board)
    data = "UART_13_bytes"
    for _ in range(LOOP_COUNT):
        peer_board.write(f"uarttool {data}")  # 13 bytes, DUT should receive and echo
        test_board.expect("Recv all 13 Bytes", timeout=15)
        # --- Verify PEER received and validated the echo ---
        peer_board.expect(data, timeout=10)
        peer_board.expect("PASS: 13/13 bytes matched", timeout=10)

        peer_board.write("uarttool 12345")  # 5 bytes, DUT should timeout
        test_board.expect(r"Serial Rcv Timeout, Got 5 bytes", timeout=10)


# ---------------------------------------------------------------------------
#  5. mbed_uart_dma_stream — DMA 流式收发 (2~33 字节循环)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_dma_stream")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_dma_stream"],
                         ids=["dma_stream"], indirect=True)
def test_mbed_uart_dma_stream(dut):
    """UART DMA stream: dual-board variable-length DMA echo (2~33 bytes)."""
    peer_board, test_board = dut
    _wait_for_prompt(peer_board)
    test_board.hard_reset()
    for n in [2] + list(range(2, 34)):
        test_board.expect(f"Ready to receive {n}-byte-data", timeout=10)
        peer_board.write(f"uarttool {'a' * n}")
        peer_board.expect(f"PASS: {n}/{n} bytes matched", timeout=10)


# ---------------------------------------------------------------------------
#  6. mbed_uart_dma_rx_fc — DMA 流控接收
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("uart_tool|mbed_uart_dma_rx_fc")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["../tools/uart_tool/bin|./bin/mbed_uart_dma_rx_fc"],
                         ids=["dma_rx_fc"], indirect=True)
def test_mbed_uart_dma_rx_fc(dut):
    """UART DMA flow controller: dual-board echo test."""
    peer_board, test_board = dut
    _wait_for_prompt(peer_board)
    data = "UART_FC_DMA"
    for _ in range(LOOP_COUNT):
        peer_board.write(f"uarttool {data}")
        peer_board.expect(data, timeout=20)
        peer_board.expect(f'PASS: {len(data)}/{len(data)} bytes matched', timeout=20)


# ---------------------------------------------------------------------------
#  7. mbed_uart_poll_autoctrl — 轮询自动流控 (双板全双工, RX 轮询 + 优先级演示)
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_uart_poll_autoctrl")
@pytest.mark.app_defines("PEER_BOARD=0|PEER_BOARD=1")
@pytest.mark.parametrize("count", [2], indirect=True)
@pytest.mark.parametrize("image_dir",
                         ["./bin/mbed_uart_poll_autoctrl/dut|./bin/mbed_uart_poll_autoctrl/peer"],
                         ids=["autoctrl"], indirect=True)
def test_mbed_uart_poll_autoctrl(dut):
    """UART polling auto flow control: dual-board full-duplex test.

    Both boards run identical firmware. Each board has two tasks:
      - TX task (pri 2, HIGH): sends 1000 bytes with periodic DelayMs
      - RX task (pri 1, LOW): receives data via polling

    The TX task uses busy-wait to starve the RX task. Hardware
    flow control (RTS/CTS) protects against FIFO overflow when
    the RX task is starved and cannot drain the FIFO in time.

    Pass criteria:
      - Both boards print "UART TX task done".
      - Both boards print "UART RX done: all 1000 bytes match".
      - No "mismatch" or "Failed" on either board.
    """
    board0, board1 = dut

    board0.hard_reset()
    board1.hard_reset()

    for board in (board0, board1):
        found_tx = found_rx = False
        for _ in range(2):
            match = board.expect([r"UART TX task done",
                                    r"UART RX done: all \d+ bytes match",
                                    r"UART RX done: \d+ / \d+ bytes mismatch"],
                                    timeout=120)
            txt = match.group(0)
            if "TX task done" in txt:
                found_tx = True
            elif "all " in txt:
                found_rx = True
            else:
                pytest.fail(f"{board.port}: {txt}")
        if not (found_tx and found_rx):
            pytest.fail(f"{board.port}: missing TX done or RX done")

