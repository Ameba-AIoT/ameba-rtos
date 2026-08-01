"""
Pytest tests for mbed SPI example firmware (dual-board).

Test cases:

- test_mbed_spi_trx_polling   — mbed_spi_trx_polling_mstr + mbed_spi_trx_polling_slv
- test_mbed_spi_trx_interrupt — mbed_spi_trx_interrupt_mstr + mbed_spi_trx_interrupt_slv
- test_mbed_spi_trx_dma       — mbed_spi_trx_dma_mstr + mbed_spi_trx_dma_slv
- test_mbed_spi_multislave    — mbed_spi_multislave (SPI_IS_AS_MASTER=1) + mbed_spi_multislave (SPI_IS_AS_MASTER=0)

Usage::

    1. Build the example firmware for master and slave, save to specified directories, e.g.::

        ameba.py build -a example mbed_spi_trx_polling_mstr -D FINAL_IMAGE_DIR=<path/to/bin/master>
        ameba.py build -a example mbed_spi_trx_polling_slv -D FINAL_IMAGE_DIR=<path/to/bin/slave>

    2. cd example/peripheral/mbed/SPI/ &&
        pytest --target RTL8720F --port "COM6|COM7"
    use --auto-build to auto-build the firmware before flashing, e.g.::

        pytest --target RTL8720F --port "COM6|COM7" --auto-build
"""

import pytest

LOOP_COUNT = 3  # Number of loops for each test, take effect by 'cmake -D'

RE_SPI_DEMO = r"SPI Demo: (success|fail)"
RE_MULTISLAVE_SUMMARY = r"success:\s*(\d+),\s*fail:\s*(\d+),\s*deselected:\s*(\d+)"


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_spi_trx_polling_mstr|mbed_spi_trx_polling_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_spi_trx_polling_mstr|./bin/mbed_spi_trx_polling_slv"],
    ids=["trx_polling"], indirect=True,
)
def test_mbed_spi_trx_polling(dut):
    """mbed SPI polling dual-board: full-duplex + unidirectional.

    Both boards use the same SPI0 pins, connected pin-to-pin.
    Master board runs ``mbed_spi_trx_polling_mstr`` (SPI0 as master),
    slave runs ``mbed_spi_trx_polling_slv`` (SPI0 as slave).

    Phase 1: full-duplex — both sides TX+RX simultaneously via byte-by-byte
    polling (``spi_master_write`` / ``spi_slave_write``+``spi_slave_read``).
    Phase 2: unidirectional — master writes, slave reads.
    Each phase prints "success" or "fail"; final verdict is
    "SPI Demo: success" or "SPI Demo: fail".
    """
    master, slave = dut

    # Hard reset master after slave is ready (Slave first, then Master)
    slave.hard_reset()
    master.hard_reset()

    # Check both sides print final verdict
    m = slave.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{slave.port}: SPI Demo: fail")

    m = master.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{master.port}: SPI Demo: fail")


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_spi_trx_interrupt_mstr|mbed_spi_trx_interrupt_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_spi_trx_interrupt_mstr|./bin/mbed_spi_trx_interrupt_slv"],
    ids=["trx_interrupt"], indirect=True,
)
def test_mbed_spi_trx_interrupt(dut):
    """mbed SPI interrupt dual-board: full-duplex + unidirectional.

    Both boards use the same SPI0 pins, connected pin-to-pin.
    Master board runs ``mbed_spi_trx_interrupt_mstr`` (SPI0 as master),
    slave runs ``mbed_spi_trx_interrupt_slv`` (SPI0 as slave).

    Phase 1: full-duplex via interrupt-driven stream API
    (``spi_master_write_read_stream`` / ``spi_slave_read_stream``+``spi_slave_write_stream``).
    Phase 2: unidirectional — slave writes, master reads.
    Each phase prints "success" or "fail"; final verdict is
    "SPI Demo: success" or "SPI Demo: fail".
    """
    master, slave = dut
    slave.hard_reset()
    master.hard_reset()

    m = slave.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{slave.port}: SPI Demo: fail")

    m = master.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{master.port}: SPI Demo: fail")


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_spi_trx_dma_mstr|mbed_spi_trx_dma_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_spi_trx_dma_mstr|./bin/mbed_spi_trx_dma_slv"],
    ids=["trx_dma"], indirect=True,
)
def test_mbed_spi_trx_dma(dut):
    """mbed SPI DMA dual-board: full-duplex + unidirectional.

    Both boards use the same SPI0 pins, connected pin-to-pin.
    Master board runs ``mbed_spi_trx_dma_mstr`` (SPI0 as master),
    slave runs ``mbed_spi_trx_dma_slv`` (SPI0 as slave).

    Phase 1: full-duplex via GDMA (``spi_master_write_read_stream_dma`` /
    ``spi_slave_read_stream_dma``+``spi_slave_write_stream_dma``).
    Phase 2: unidirectional — slave writes, master reads via GDMA.
    Each phase prints "success" or "fail"; final verdict is
    "SPI Demo: success" or "SPI Demo: fail".
    """
    master, slave = dut
    slave.hard_reset()
    master.hard_reset()

    m = slave.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{slave.port}: SPI Demo: fail")

    m = master.expect(RE_SPI_DEMO, timeout=60)
    if "fail" in m.group(1):
        pytest.fail(f"{master.port}: SPI Demo: fail")


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_spi_multislave")
@pytest.mark.app_defines(f"SPI_IS_AS_MASTER=1, LOOP_COUNT={LOOP_COUNT}|SPI_IS_AS_MASTER=0, LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/mbed_spi_multislave/mstr|./bin/mbed_spi_multislave/slv"],
    ids=["multislave"], indirect=True,
)
def test_mbed_spi_multislave(dut):
    """SPI GPIO CS multi-slave: master dual-threaded, slave single-threaded.

    On the master board, two independent worker threads share the SPI bus
    via a mutex: each thread owns one GPIO CS line (CS0 or CS1) and sends
    ``LOOP_COUNT`` bursts of data.  This simulates an RTOS scenario where
    multiple tasks contend for a shared SPI bus, each addressing a different
    slave device.

    On the slave, a single listening thread receives data from whichever
    master worker wins the bus (both CS lines are tied to the same physical
    SPI0 slave on the test board), auto-detects the data pattern, and
    verifies every byte.

    Total transfers: ``LOOP_COUNT × MASTER_NR_SLAVES``; scheduling is
    non-deterministic so the exact split per CS varies, but all received
    data must pass byte-for-byte verification.
    """
    master, slave = dut

    slave.hard_reset()
    master.hard_reset()

    # LOOP_COUNT transfers
    m = slave.expect(RE_MULTISLAVE_SUMMARY, timeout=100 + LOOP_COUNT * 2)
    cs_low_fail = int(m.group(2))
    cs_deselected = int(m.group(3))
    if cs_low_fail > 0 or cs_deselected > 0:
        pytest.fail(f"{slave.port}: data check failed {cs_low_fail} time(s), deselected {cs_deselected} time(s)")

    slave.expect(r"SPI Demo finished", timeout=15)
    master.expect(r"SPI Demo finished", timeout=15)
