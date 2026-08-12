"""
Pytest tests for raw I2C example firmware.

Test cases:

- test_mbed_i2c_polling  — mbed_i2c_polling_master + mbed_i2c_polling_slave

Usage::
    1. Build the example firmware for master and slave, save to specified directories, e.g.:
        ameba.py build -a mbed_i2c_polling_master -D FINAL_IMAGE_DIR=<path/to/bin>
    2. cd example/peripheral/mbed/I2C/ &&
        pytest --target RTL8720F --port "COM6|COM7"
    use --auto-build to auto-build the firmware before flashing, e.g.:
        pytest --target RTL8720F --port "COM6|COM7" --auto-build
    3. if need longrun, use "@pytest.mark.app_defines("LOOP_COUNT=<N>")" to define the number of loops for the test.
        --auto-build will use the defined LOOP_COUNT to build the firmware.
"""

import pytest

RE_ANY = r"(?:Result is success|Result is fail)"
LOOP_COUNT = 3

@pytest.mark.generic
@pytest.mark.multi_board
@pytest.mark.app_name("mbed_i2c_polling_master|mbed_i2c_polling_slave")
@pytest.mark.app_defines(f"LOOP_COUNT={LOOP_COUNT}")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir", ["./bin/mbed_i2c_polling_master|./bin/mbed_i2c_polling_slave"], ids=["polling"], indirect=True)
def test_mbed_i2c_polling(dut):
    """I2C polling mode: master + slave data transfer."""
    master, slave = dut
    # hard reset master after slave ready
    slave.hard_reset()
    master.hard_reset()

    for _ in range(LOOP_COUNT):
        for board in (slave, master):
            m = board.expect(RE_ANY, timeout=40)
            if "fail" in m.group(0):
                pytest.fail(f"{board.port}: Result is fail")
