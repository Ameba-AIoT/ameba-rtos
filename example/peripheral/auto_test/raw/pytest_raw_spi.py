"""
Pytest tests for raw SPI example firmware (dual-board).

Test cases:

- test_raw_spi_dma_sblk      — raw_spi_dma_sblk_tx_mstr + raw_spi_dma_sblk_rx_slv
- test_raw_spi_dma_mblk      — raw_spi_dma_mblk_tx_mstr + raw_spi_dma_mblk_rx_slv
- test_raw_spi_trx_polling   — raw_spi_trx_polling_mstr + raw_spi_trx_polling_slv
- test_raw_spi_trx_interrupt — raw_spi_trx_interrupt_mstr + raw_spi_trx_interrupt_slv
- test_raw_spi_trx_dma       — raw_spi_trx_dma_mstr + raw_spi_trx_dma_slv
- test_raw_spi_multislave    — raw_spi_multislave (SPI_IS_AS_MASTER=1) + raw_spi_multislave (SPI_IS_AS_MASTER=0)

Usage::
    1. Build the example firmware for master and slave, save to specified directories, e.g.:
        ameba.py build -a raw_spi_dma_sblk_tx_mstr -D FINAL_IMAGE_DIR=<path/to/bin/master>
        ameba.py build -a raw_spi_dma_sblk_rx_slv -D FINAL_IMAGE_DIR=<path/to/bin/slave>
    2. cd example/peripheral/raw/SPI/ &&
        pytest --target RTL8720F --port "COM6|COM7"
    use --auto-build to auto-build the firmware before flashing, e.g.:
        pytest --target RTL8720F --port "COM6|COM7" --auto-build
"""

import pytest

LOOP_COUNT = 3  # Number of loops for each test, take effect by 'cmake -D'

RE_RX_RESULT = r"SPI RX test: (OK|fail)!"
RE_TX_DONE = r"SPI tx Demo finished"
RE_SPI_DEMO = r"SPI Demo: (success|fail)"


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_spi_dma_sblk_tx_mstr|raw_spi_dma_sblk_rx_slv")
@pytest.mark.app_defines(f"LOOP_COUNT={LOOP_COUNT}|LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_dma_sblk_tx_mstr|./bin/raw_spi_dma_sblk_rx_slv"],
    ids=["dma_sblk"], indirect=True,
)
def test_raw_spi_dma_sblk(dut):
    """SPI DMA single-block: master TX → slave RX, single transfer.

    Master board runs ``raw_spi_dma_sblk_tx_mstr``, slave runs
    ``raw_spi_dma_sblk_rx_slv``.

    The slave firmware internally compares received data against
    the expected pattern (0~2047) and prints the verdict.
    The master firmware prints completion after the DMA TX finishes.
    """
    master, slave = dut

    # Hard reset master after slave is ready (Slave first, then Master).
    # The slave board started earlier and will have exited its 5-second
    # startup delay by the time the master resets and restarts.
    slave.hard_reset()
    master.hard_reset()

    # Slave prints "SPI RX test: OK!" or "SPI RX test: fail!" for each loop
    for i in range(LOOP_COUNT):
        m = slave.expect(RE_RX_RESULT, timeout=60)
        if "fail" in m.group(1):
            pytest.fail(f"{slave.port}: SPI RX test: fail (round {i+1})")

    # Master prints "SPI tx Demo finished." after DMA TX completes
    master.expect(RE_TX_DONE, timeout=60)


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_spi_dma_mblk_tx_mstr|raw_spi_dma_mblk_rx_slv")
@pytest.mark.app_defines(f"LOOP_COUNT={LOOP_COUNT}|LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_dma_mblk_tx_mstr|./bin/raw_spi_dma_mblk_rx_slv"],
    ids=["dma_mblk"], indirect=True,
)
def test_raw_spi_dma_mblk(dut):
    """SPI DMA multi-block: master TX → slave RX, single transfer.

    Master board runs ``raw_spi_dma_mblk_tx_mstr``, slave runs
    ``raw_spi_dma_mblk_rx_slv``.

    Multi-block mode splits 8192 bytes into 4 blocks of 2048 bytes
    each, transferred via GDMA auto-reload mechanism.  The slave
    firmware compares received data against the expected pattern
    (0~8191) and prints the verdict.
    """
    master, slave = dut

    # Hard reset master after slave is ready
    slave.hard_reset()
    master.hard_reset()

    # Slave prints "SPI RX test: OK!" or "SPI RX test: fail!" for each loop
    for i in range(LOOP_COUNT):
        m = slave.expect(RE_RX_RESULT, timeout=60)
        if "fail" in m.group(1):
            pytest.fail(f"{slave.port}: SPI RX test: fail (round {i+1})")

    # Master prints "SPI tx Demo finished." after multi-block DMA TX completes
    master.expect(RE_TX_DONE, timeout=60)


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_spi_trx_polling_mstr|raw_spi_trx_polling_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_trx_polling_mstr|./bin/raw_spi_trx_polling_slv"],
    ids=["trx_polling"], indirect=True,
)
def test_raw_spi_trx_polling(dut):
    """SPI polling dual-board: full-duplex + unidirectional.

    Master board runs ``raw_spi_trx_polling_mstr``, slave runs
    ``raw_spi_trx_polling_slv``. Both use SPI1 (master side as
    SSI_MASTER, slave side as SSI_SLAVE), connected pin-to-pin.

    Phase 1: full-duplex — both sides TX+RX simultaneously.
    Phase 2: unidirectional — master writes, slave reads.
    Each phase prints \"success\" or \"fail\"; final verdict is
    \"SPI Demo: success\" or \"SPI Demo: fail\".
    """
    master, slave = dut

    # Hard reset master after slave is ready
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
@pytest.mark.app_name("raw_spi_trx_interrupt_mstr|raw_spi_trx_interrupt_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_trx_interrupt_mstr|./bin/raw_spi_trx_interrupt_slv"],
    ids=["trx_interrupt"], indirect=True,
)
def test_raw_spi_trx_interrupt(dut):
    """SPI interrupt dual-board: full-duplex + unidirectional.

    Master board runs ``raw_spi_trx_interrupt_mstr``, slave runs
    ``raw_spi_trx_interrupt_slv``. Both use SPI1, connected pin-to-pin.

    Phase 1: full-duplex via interrupt-driven stream API.
    Phase 2: unidirectional — slave writes, master reads.
    Each phase prints \"success\" or \"fail\"; final verdict is
    \"SPI Demo: success\" or \"SPI Demo: fail\".
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
@pytest.mark.app_name("raw_spi_trx_dma_mstr|raw_spi_trx_dma_slv")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_trx_dma_mstr|./bin/raw_spi_trx_dma_slv"],
    ids=["trx_dma"], indirect=True,
)
def test_raw_spi_trx_dma(dut):
    """SPI DMA dual-board: full-duplex + unidirectional.

    Master board runs ``raw_spi_trx_dma_mstr``, slave runs
    ``raw_spi_trx_dma_slv``. Both use SPI1, connected pin-to-pin.

    Phase 1: full-duplex via GDMA (master TX+RX, slave RX+TX).
    Phase 2: unidirectional — slave writes, master reads via GDMA.
    Each phase prints \"success\" or \"fail\"; final verdict is
    \"SPI Demo: success\" or \"SPI Demo: fail\".
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
@pytest.mark.app_name("raw_spi_multislave")
@pytest.mark.app_defines(f"SPI_IS_AS_MASTER=1, LOOP_COUNT={LOOP_COUNT}|SPI_IS_AS_MASTER=0, LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_spi_multislave/mstr|./bin/raw_spi_multislave/slv"],
    ids=["multislave"], indirect=True,
)
def test_raw_spi_multislave(dut):
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

    Total transfers: ``LOOP_COUNT × MASTER_NR_SLAVES = {LOOP_COUNT * 2}``; scheduling is
    non-deterministic so the exact split per CS varies, but all received
    data must pass byte-for-byte verification.
    """
    master, slave = dut

    slave.hard_reset()
    master.hard_reset()

    # LOOP_COUNT transfers
    m = slave.expect(r"success:\s*(\d+),\s*fail:\s*(\d+),\s*deselected:\s*(\d+)", timeout=100+LOOP_COUNT*2)
    cs_low_fail = int(m.group(2))
    cs_deselected = int(m.group(3))
    if cs_low_fail > 0 or cs_deselected > 0:
        pytest.fail(f"{slave.port}: data check failed {cs_low_fail} time(s), deselected {cs_deselected} time(s)")

    slave.expect(r"SPI Demo finished", timeout=15)
    master.expect(r"SPI Demo finished", timeout=15)

