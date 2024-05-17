RSS key management
==================

The RSS has a set of hardware components that act together to prevent hardware
protected keys being disclosed to compromised software. This chain involves the
LifeCycle Manager "LCM", the Key Management Unit "KMU", and the cryptographic
accelerator in the integration layer of the RSS (which in reference RSS builds
is a CryptoCell-3XX series accelerator).

Hardware protected keys (henceforth "Hardware Keys") are stored in the LCM, in
protected keyslots. On production-mode "PCI" chips these slots are write-only.
They are protected during provisioning from disclosure by Secure Provisioning
"SP" mode disabling debug, and the encryption with either the RTL key or the CM
provisioning key. On boot, the LCM checks the integrity of the stored hardware
keys by comparing the amount of zeroes with a stored zero-count which, because
of the property of the OTP that does not allow 1-bits to be set to 0-bits,
allows detection of any changes in the keys. If the check succeeds it exports
the keys to the KMU over a private memory bus (one that is not accessible
programs running on the RSS' CPU or other controllers on the main bus such as
the DMA).

The KMU has between 2 and 32 hardware keyslots, which are 256 bit (32 byte) in
size. Between 0 and 8 of these are hardware keyslots, and correspond to the
hardware keyslots in the LCM. These slots are filled by the key export from the
LCM, and have their permissions preconfigured so they are not readable or
writable by software. Software KMU keyslots (keyslots that are not defined in
hardware as hardware keyslots) can be read and written, and then subsequently
locked to prevent reading and writing, so that they behave like hardware
keyslots. This allows trusted immutable firmware to set up keys for less-trusted
later stage code, which can be used but cannot be read. KMU keyslots can be
exported over another private bus to the cryptographic accelerator. Hardware
keyslots can be invalidated, making them unusable until a hard reset.

The cryptographic accelerator receives the key into a write-only key register,
and then can use it to perform cryptographic operations. For the CryptoCell-3XX
series, this is limited to AES. As with the previous stages, software cannot
retrieve the key, only use it. Note that currently this can only be used with
the cc3xx_rom_lib driver, not the cryptocell-312-runtime driver, as the latter
requires modification. Note that this path is the only way to use hardware
protected keys on the RSS, they cannot be used by software cryptographic
libraries that require key material to be accessible by software.

The RSS uses this functionality to allow key derivation (based on NIST SP800-108
with an RSA-based PRF that can utilise the hardware keys as described above)
from the HUK and GUK. This allows the derivation of the CCA platform attestation
key / delegated attestation root key without allowing access to the GUK
directly. As a further security measure, these keys are derived by ROM/OTP code,
and the HUK and GUK are then invalidated so they cannot be used by the runtime
firmware and BL2.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
