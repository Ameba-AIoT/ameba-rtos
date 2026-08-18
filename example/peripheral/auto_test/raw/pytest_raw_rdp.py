"""
Pytest tests for raw RDP example firmware.

Test cases:

- test_raw_rdp_service — rdp_service: secure compute and non-secure callback
  results must match for idx 0..3.
- test_raw_rdp_ecdsa   — rdp_ecdsa: secure-world ECDSA signing and
  non-secure-world verification.

硬件需求：无（单板 TrustZone/RDP 示例测试）

Usage::

    cd example/peripheral/auto_test/
    pytest raw/pytest_raw_rdp.py \
        --target RTL8720F \
        --port COM6 \
        --auto-build
"""

import pytest
from ameba_pytest import ameba_parametrize


@pytest.mark.generic
@ameba_parametrize('target', ['RTL8721F', 'RTL8720F'])
@pytest.mark.app_name("rdp_service")
@pytest.mark.parametrize("image_dir", ["./bin/rdp_service"], ids=["service"], indirect=True)
def test_raw_rdp_service(dut):
    """RDP service: secure compute equals registered non-secure callback result."""
    dut.hard_reset()
    dut.expect(r"=== RDP Service Demo ===", timeout=10)

    for idx in range(4):
        m = dut.expect(
            rf"idx={idx}: secure=0x[0-9A-Fa-f]{{8}}, callback=0x[0-9A-Fa-f]{{8}} (OK|FAIL)",
            timeout=10,
        )
        if m.group(1) == "FAIL":
            pytest.fail(f"RDP service mismatch at idx={idx}: {m.group(0)}")

    dut.expect(r"Demo complete!", timeout=5)


@pytest.mark.generic
@pytest.mark.app_name("rdp_ecdsa")
@pytest.mark.parametrize("image_dir", ["./bin/rdp_ecdsa"], ids=["ecdsa"], indirect=True)
def test_raw_rdp_ecdsa(dut):
    """ECDSA TrustZone demo: secure signing and non-secure verification succeed."""
    dut.hard_reset()
    dut.expect(r"=== ECDSA TrustZone Demo Start ===", timeout=10)
    dut.expect(r"Creating secure context for task\.\.\.", timeout=5)
    dut.expect(r"Message to sign: Hello from Non-Secure World!", timeout=5)
    dut.expect(r"Calling secure world for signing\.\.\.", timeout=5)
    dut.expect(r"=== Secure ECDSA Sign Service ===", timeout=10)
    dut.expect(r"ECDSA key pair generated successfully", timeout=10)
    dut.expect(r"SHA256 hash computed", timeout=10)
    dut.expect(r"Signature created \(len=\d+\)", timeout=10)
    dut.expect(r"Public key exported \(len=65\)", timeout=10)
    dut.expect(r"=== Secure Sign Complete ===", timeout=10)
    dut.expect(r"Signature received from secure world \(len=\d+\)", timeout=5)
    dut.expect(r"Public key length: 65", timeout=5)
    dut.expect(r"Verifying signature in non-secure world\.\.\.", timeout=5)
    dut.expect(r"Non-secure verification succeeded!", timeout=10)
    dut.expect(r"=== ECDSA TrustZone Demo Complete ===", timeout=5)
    dut.expect(
        r"Sign in Secure World, Verify in Non-Secure World: SUCCESS!",
        timeout=5,
    )
