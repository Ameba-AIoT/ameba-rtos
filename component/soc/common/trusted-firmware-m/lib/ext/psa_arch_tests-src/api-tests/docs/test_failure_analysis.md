
# Test failure analysis document

This file contains list of failures identified when testing the release
on tgt_dev_apis_tfm_an521 and  tgt_dev_apis_tfm_musca_b1
targets with TFM commit Hash - 6fb14a14140f94150f959c88e3b880f48372da06 (tag: TF-Mv1.5.0).
The reason for each failing test is listed here in this file.

## Known Failures

| Test | Fail description                                                                | Github issue |
|------|---------------------------------------------------------------------------------| ------------ |
|test_c016 | This test will pass with the mbed TLS 3.1.0, TF-M 1.5.0 is not using mbed TLS 3.1.0. So the test is failing. | https://jira.arm.com/browse/PJ03206-534 |
|test_c026 | psa_sign_message not supported | https://jira.arm.com/browse/IOTPSW-4275 |
|test_c027 | psa_mac_update not supported | https://jira.arm.com/browse/IOTPSW-4275 |
|test_c028 | PSA_KEY_USAGE_SIGN_MESSAGE not found | https://jira.arm.com/browse/IOTPSW-4275 |
|test_c029 | PSA_KEY_USAGE_VERIFY_MESSAGE not supported | https://jira.arm.com/browse/IOTPSW-4275 |
|test_c030 | PSA_KEY_USAGE_VERIFY_MESSAGE not supported | https://jira.arm.com/browse/IOTPSW-4275 |
|test_c052 | psa_aead_encrypt_setup is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c053 | psa_aead_decrypt_setup is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c054 | psa_aead_generate_nonce is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c055 | psa_aead_set_nonce is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c056 | psa_aead_set_lengths is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c057 | psa_aead_update_ad is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c058 | psa_aead_update is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c059 | psa_aead_finish is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c060 | psa_aead_abort is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c061 | psa_aead_verify is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c062 | psa_hash_suspend is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c063 | psa_hash_resume is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://jira.arm.com/browse/IOTPSW-4100 |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.*
