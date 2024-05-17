RSS provisioning
================

The LifeCycle Manager controls access to the RSS OTP, and includes a
state-machine that controls Lifecycle-state transitions. The LCM is derived from
the OTP management and state machine components of the CryptoCell-3XX series
accelerators, and will be familiar to those who have worked with them.

When the chip hasn't been provisioned, the OTP is blank, which means the LCM is
in "Virgin" Test/Production mode "TP mode" state. The first step for
provisioning must be to set the LCM to either test-chip mode "TCI" or
production-chip mode "PCI". In TCI mode the RTL key is masked to avoid
disclosure, several OTP fields are changed from write-only to read-write, to aid
in debugging, and debugging is not limited in secure provisioning mode.

Once the TP mode has been set, the chip is then is Chip Manufacturer
provisioning state "CM". This mode is intended for the provisioning of the HUK,
GUK, CM provisioning key, CM code-encryption key, the root-of-trust public key
and the CM config. To provision these fields, The firmware must first receive a
provisioning bundle via a side-band channel (UART or debug interface). This
bundle either contains just the keys, or more usually the keys and also
provisioning code. The chip must then enter secure provisioning mode by setting
the SP_ENABLE register. This causes a reset (but does not clear the RSS SRAMs),
and allows access to the RTL key by exporting it to the KMU. The RSS must then
decrypt and authenticate the bundle using the RTL key. Under TCI mode the RTL
key is zeroed, so encryption and signing must use a zeroed key. Once the CM
provisioning bundle has been unpacked, run if it contains code, and the CM
values have been set. The RSS must be cold-reset.

After the cold reset, the RSS will then be in Device Manufacturer provisioning
state "DM". This state is designed to provision the DM provisioning key, the DM
code-encryption key and the DM config. The procedure follows the same steps as
the CM provisioning flow, with the exception that the bundle will now be
encrypted and signed using the CM provisioning key. Once the provisioning bundle
has been unpacked/run, the RSS must be cold-reset.

After the cold reset, the device will now be in Secure Enable "SE" mode. Debug
may be limited based on the hardware DCU mask for SE state. Provisioning will
not be run on boot.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
