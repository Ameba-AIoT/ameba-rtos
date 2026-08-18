"""
Pytest tests for raw I2C example firmware.

Test cases:

- test_raw_i2c_polling  — raw_i2c_polling_master + raw_i2c_polling_slave
- test_raw_i2c_int      — raw_i2c_int_master + raw_i2c_int_slave
- test_raw_i2c_dma_write — raw_i2c_dma_mode (master) writes, slave reads
- test_raw_i2c_dma_read  — raw_i2c_dma_mode (master) reads, slave writes

Usage::
    1. Build the example firmware for master and slave, save to specified directories, e.g.:
        ameba.py build -a raw_i2c_polling_master -D FINAL_IMAGE_DIR=<path/to/bin>
    2. cd example/peripheral/raw/I2C/ &&
        pytest --target RTL8720F --port "COM6|COM7"
    use --auto-build to auto-build the firmware before flashing, e.g.:
        pytest --target RTL8720F --port "COM6|COM7" --auto-build
    3. if need longrun, use "@pytest.mark.app_defines("LOOP_COUNT=<N>")" to define the number of loops for the test.
        --auto-build will use the defined LOOP_COUNT to build the firmware.
"""

import pytest
from ameba_pytest import ameba_parametrize

LOOP_COUNT = 3  # Number of loops for each test, take effect by 'cmake -D'

RE_ANY = r"(?:Result is success|Result is fail)"


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_i2c_polling_master|raw_i2c_polling_slave")
@pytest.mark.app_defines(f"LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir", ["./bin/raw_i2c_polling_master|./bin/raw_i2c_polling_slave"], ids=["polling"], indirect=True)
def test_raw_i2c_polling(dut):
    """I2C polling mode: master + slave data transfer, 3 rounds."""
    master, slave = dut
    # hard reset master after slave ready
    slave.hard_reset()
    master.hard_reset()
    for i in range(LOOP_COUNT):
        for board in (slave, master):
            m = board.expect(RE_ANY, timeout=40)
            if "fail" in m.group(0):
                pytest.fail(f"{board.port}: Result is fail (round {i+1})")


@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("raw_i2c_int_master|raw_i2c_int_slave")
@pytest.mark.app_defines(f"LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir", ["./bin/raw_i2c_int_master|./bin/raw_i2c_int_slave"], ids=["int"], indirect=True)
def test_raw_i2c_int(dut):
    """I2C interrupt mode: master + slave data transfer, 3 rounds.

    Only the slave firmware (raw_i2c_int_slave) prints result strings;
    the master (raw_i2c_int_master) transfers silently. Verification is
    done on the slave side via ``i2c_rx_check()`` after each read cycle.
    """
    master, slave = dut
    # hard reset master after slave ready
    slave.hard_reset()
    master.hard_reset()

    for i in range(LOOP_COUNT):
        for board in (slave, master):
            m = board.expect(RE_ANY, timeout=40)
            if "fail" in m.group(0):
                pytest.fail(f"{board.port}: Result is fail (round {i+1})")


@pytest.mark.generic
@pytest.mark.multi_board
@ameba_parametrize('target', ['RTL8721Dx', 'RTL8721F', 'RTL8720F'])
@pytest.mark.app_name("raw_i2c_dma_mode")
@pytest.mark.app_defines(f"I2C_MASTER_DEVICE=1,MASTER_SEND=1,LOOP_COUNT={LOOP_COUNT}|"
                         f"I2C_MASTER_DEVICE=0,MASTER_SEND=1,LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_i2c_dma_mode/master_write|./bin/raw_i2c_dma_mode/slave_read"],
    ids=["dma_write"], indirect=True,
)
def test_raw_i2c_dma_write(dut):
    """I2C DMA mode: master writes → slave reads, single transfer."""
    master, slave = dut
    # hard reset master after slave ready
    slave.hard_reset()
    master.hard_reset()

    for i in range(LOOP_COUNT):
        for board in (slave, master):
            m = board.expect(RE_ANY, timeout=60)
            if "fail" in m.group(0):
                pytest.fail(f"{board.port}: Result is fail (round {i+1})")
    master.expect(f"All {LOOP_COUNT} TX loops completed", timeout=60)
    slave.expect(f"All {LOOP_COUNT} RX loops completed", timeout=60)


@pytest.mark.generic
@pytest.mark.multi_board
@ameba_parametrize('target', ['RTL8721Dx', 'RTL8721F', 'RTL8720F'])
@pytest.mark.app_name("raw_i2c_dma_mode")
@pytest.mark.app_defines(f"I2C_MASTER_DEVICE=1,MASTER_SEND=0,LOOP_COUNT={LOOP_COUNT}|"
                         f"I2C_MASTER_DEVICE=0,MASTER_SEND=0,LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize(
    "image_dir",
    ["./bin/raw_i2c_dma_mode/master_read|./bin/raw_i2c_dma_mode/slave_write"],
    ids=["dma_read"], indirect=True,
)
def test_raw_i2c_dma_read(dut):
    """I2C DMA mode: master reads → slave writes, single transfer."""
    master, slave = dut
    # hard reset master after slave ready
    slave.hard_reset()
    master.hard_reset()

    for i in range(LOOP_COUNT):
        for board in (slave, master):
            m = board.expect(RE_ANY, timeout=60)
            if "fail" in m.group(0):
                pytest.fail(f"{board.port}: Result is fail (round {i+1})")
    slave.expect(f"All {LOOP_COUNT} TX loops completed", timeout=60)
    master.expect(f"All {LOOP_COUNT} RX loops completed", timeout=60)
