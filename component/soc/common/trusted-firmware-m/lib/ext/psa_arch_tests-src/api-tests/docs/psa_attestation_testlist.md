# PSA Initial Attestation Testcase checklist

| Test      | Return value                         | API                                                               | Test Algorithm                                                                                                                                                  | Test Cases                                                                                                                                                                                             |
|-----------|--------------------------------------|-------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| test_a001 | PSA_SUCCESS               | psa_initial_attest_get_token()<br />psa_initial_attest_get_token_size() | 1. Provide correct inputs to API with described challenge sizes  <br />2. Expect API to return this define as return value each time  <br />3. Verify the token | 1. Challenge_size = 32 <br />2. Challenge_size = 48 <br />3. Challenge_size = 64                                                                                                         |
|           | PSA_ERROR_INVALID_ARGUMENT         | psa_initial_attest_get_token()<br />psa_initial_attest_get_token_size() | 1. Provide described challenge sizes to the API along with other valid parameters <br />2. Expect API to return this define as return value each time           | 1. Challenge_size is zero <br />2. Invalid challenge size between 0 and 32 <br />3. Invalid challenge size between 32 and 64  <br />4. Challenge_size is greater than MAX_CHALLENGE_SIZE |
|           | PSA_ERROR_BUFFER_TOO_SMALL | psa_initial_attest_get_token()                                    | 1. Provide described taken size to the API along with other valid parameters <br />2. Expect API to return this define as return value each time                | 1. Token_size as zero<br />2. Token_size less than challenge size                                                                                                                                         |
|           | PSA_ERROR_SERVICE_FAILURE           | psa_initial_attest_get_token()<br />psa_initial_attest_get_token_size() | Can't simulate. Test can't generate stimulus where attestation initialisation fails                                                                             |                                                                                                                                                                                                       |
|           | PSA_ERROR_GENERIC_ERROR               | psa_initial_attest_get_token()<br />psa_initial_attest_get_token_size() | Can't simulate. Test can't generate stimulus where unexpected error happened during API operation                                                               |                                                                                                                                                                                                       |

## Note

1. In verifying the token, only the data type of claims and presence of the mandatory claims are checked and the values of the claims are not checked. <br />
2. Specify the version of underlying PSA crypto in  <psa-arch-tests>/api-tests/platform/targets/<target_name>/nspe/pal_config.h <br />
3. Supported crypto versions are CRYPTO_VERSION_BETA1, CRYPTO_VERSION_BETA2 and CRYPTO_VERSION_BETA3 <br />
4. Signer ID and Version fields of the software component must be present to be compliant with the PSA-SM <br />
5. Define the macro PLATFORM_OVERRIDE_ATTEST_PK in <psa-arch-tests>/api-tests/platform/targets/<target_name>/nspe/pal_config.h to use hardcoded public key <br />

# License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2019-2020, Arm Limited and Contributors. All rights reserved.*
