
# PSA Certified APIs Architecture Test Suite

## PSA Certified APIs

PSA defines security service APIs for application developers. Some of these services are Crypto, Attestation, and Secure storage. For more information on API specifications, refer to the [PSA Certified APIs Specifications](https://arm-software.github.io/psa-api/).

## Architecture test suite

The architecture test suite is a set of examples of the invariant behaviors that are specified in the PSA Certified APIs specifications. Use this suite to verify whether these behaviors are implemented correctly in your system. This  suite contains self-checking and portable C-based tests with directed stimulus. These tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license, allowing external contribution.

This test suite is not a substitute for design verification. To review the test logs, Arm licensees can contact Arm directly through their partner managers.

For more information on the architecture test suite framework and methodology to run the tests, refer to the [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf) document.

## This release
 - Code Quality : REL v1.4
 - This release contains following API tests: <br />

| Test Category            | Specification Version                |
|--------------------------|--------------------------------------|
| Crypto                   | [Crypto API 1.0.1](https://arm-software.github.io/psa-api/crypto/)     |
| Storage (PS and ITS)     | [Storage API 1.0.0](https://arm-software.github.io/psa-api/storage/) |
| Attestation              | [Attestation API 1.0.2](https://arm-software.github.io/psa-api/attestation/)  |


##  Release Tags

 - For API certification, use release tag from below table:

| Release version | Release tag  | Crypto API | Storage API | Attestation API |
|-----------------|---------------|----------------|-----------------|---------------------|
| REL v1.4 | [v22.01_API1.4_ADAC_BETA](https://github.com/ARM-software/psa-arch-tests/tree/v22.01_API1.4_ADAC_BETA/api-tests/dev_apis) | 1.0.1  | 1.0.0 | 1.0.2 |
| REL v1.3 | [v21.10_API1.3_ADAC_ALPHA-1](https://github.com/ARM-software/psa-arch-tests/tree/v21.10_API1.3_ADAC_ALPHA-1/api-tests/dev_apis) | 1.0.0  | 1.0.0 | 1.0.2 |
| REL v1.2 | [v21.07_API1.2_ADAC_ALPHA](https://github.com/ARM-software/psa-arch-tests/tree/v21.07_API1.2_ADAC_ALPHA/api-tests/dev_apis) | 1.0.0  | 1.0.0 | 1.0.2 |
| REL v1.1 | [v20.11_API1.1](https://github.com/ARM-software/psa-arch-tests/tree/v20.11_API1.1/api-tests/dev_apis) | 1.0-Beta3  | 1.0.0 | 1.0.0 |
| REL v1.0 | [v20.03_API1.0](https://github.com/ARM-software/psa-arch-tests/tree/v20.03_API1.0/api-tests/dev_apis) | 1.0-Beta3  | 1.0.0 | 1.0.0 |
| v0.9 | [v19.06_API0.9](https://github.com/ARM-software/psa-arch-tests/tree/v19.06_API0.9/api-tests/dev_apis) | 1.0-Beta2 | 1.0-Beta2 | 1.0-Beta0 |
| v0.8 | [v19.02_API0.8](https://github.com/ARM-software/psa-arch-tests/tree/v19.02_API0.8/api-tests/dev_apis) | 1.0-Beta1 | 1.0-Beta0 | 1.0-Beta0 |

## Test scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario Document](../docs/) that is present in the **docs/** folder.


## Getting started

Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. Make sure you have all required software installed as explained in the [Software Requirements Document](../docs/sw_requirements.md).

### Porting steps

Refer to the [PSA Certified APIs Test Suite Porting Guide](../docs/porting_guide_dev_apis.md) document for porting steps.

### Build steps

To build the test suite for your target platform, execute the following commands:

```
    cd api-tests
    mkdir <build_dir>
    cd <build_dir>
    cmake ../ -G"<generator_name>" -DTARGET=<platform_name> -DCPU_ARCH=<cpu_architecture_version> -DSUITE=<suite_name> -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>"
    cmake --build .
```
<br />Options information:<br />

-   -G"<generator_name>" : "Unix Makefiles" to generate Makefiles for Linux and Cygwin. "MinGW Makefiles" to generate Makefiles for cmd.exe on Windows  <br />
-   -DTARGET=<platform_name> is the same as the name of the target-specific directory created in the **platform/targets/** directory. The current release has been tested on **tgt_dev_apis_tfm_an521**, **tgt_dev_apis_tfm_musca_b1** and **tgt_dev_apis_tfm_musca_a** platforms. Refer [Test_failure analysis](../docs/test_failure_analysis.md) document to know the reason for any known test fail.<br />
-   -DTOOLCHAIN=<tool_chain> Compiler toolchain to be used for test suite compilation. Supported values are GNUARM (GNU Arm Embedded), ARMCLANG (ARM Compiler 6.x) , HOST_GCC and GCC_LINUX . Default is GNUARM.<br />
-   -DCPU_ARCH=<cpu_architecture_version> is the Arm Architecture version name for which the tests should be compiled. Supported CPU arch are armv8m_ml, armv8m_bl, armv7m and armv8a. Default is empty. This option is unused when TOOLCHAIN type is HOST_GCC.<br />
-   -DSUITE=<suite_name> is the test suite name. Supported values are CRYPTO, INITIAL_ATTESTATION, STORAGE(INTERNAL_TRUSTED_STORAGE and PROTECTED_STORAGE), INTERNAL_TRUSTED_STORAGE and PROTECTED_STORAGE .<br />
-   -DVERBOSE=<verbose_level>. Print verbosity level. Default is 3. Supported print levels are 1(INFO & above), 2(DEBUG & above), 3(TEST & above), 4(WARN & ERROR) and 5(ERROR).
-   -DBUILD=<BUILD_DIR> : To select the build directory to keep output files. Default is BUILD/ inside current directory.
-   -DWATCHDOG_AVAILABLE=<0|1>: Test harness may require to access watchdog timer to recover system hang. 0 means skip watchdog programming in the test suite and 1 means program the watchdog. Default is 1. Note, watchdog must be available for the tests which check the API behaviour on the system reset.
-   -DSUITE_TEST_RANGE="<test_start_number>;<test_end_number>" is to select range of tests for build. All tests under -DSUITE are considered by default if not specified.
-   -DTFM_PROFILE=<profile_small/profile_medium> is to work with TFM defined Pofile Small/Medium definitions. Supported values are profile_small and profile_medium. Unless specified Default Profile is used.
-   -DSPEC_VERSION=<spec_version> is test suite specification version. Which will build for given specified spec_version. Supported values for CRYPTO test suite are 1.0-BETA1, 1.0-BETA2, 1.0-BETA3 , for INITIAL_ATTESATATION test suite are 1.0-BETA0, 1.0.0, 1.0.1, 1.0.2, for STORAGE, INTERNAL_TRUSTED_STORAGE, PROTECTED_STORAGE test suite are 1.0-BETA2, 1.0 . Default is empty. <br/>
     If -DSPEC_VERSION option is not given it will build for latest version of testsuite.
     For every spec version corresponds test list will be in spec_version_testsuite.db file in api-tests/dev_apis/test_suite_name/ folder.
-   -DCOMPILER_NAME=<compiler_name> Compiler name to be use for selecting compiler. Supported values are gcc. By defualt it will take gcc if not specified.
     Note: -DCOMPILER_NAME only applicable for linux i.e. -DTOOLCHAIN=GCC_LINUX and DTARGET=tgt_dev_apis_linux.
-   -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>" is an additional directory to be included into the compiler search path.You must provide API header files implementation to the test suite build system using this option. For example, to compile Crypto tests, the include path must point to the path where **psa/crypto.h** is located in your build system. Bydefault, PSA_INCLUDE_PATHS accepts absolute path. However, relative path can be provided using below format:<br />
-   -DPSA_TARGET_QCBOR=<path for pre-fetched cbor folder, this is option used where no network connectivity is possible during the build:<br />
```
    -DPSA_INCLUDE_PATHS=`readlink -f <relative_include_path>`
```
-   -DTESTS_COVERAGE=<tests_coverage_value> is used to skip known failure tests by selecting value PASS. Supported values are ALL and PASS. ALL value will include all the tests and PASS value will skip the known failure tests and will include pass tests. Default is ALL.

To compile Crypto tests for **tgt_dev_apis_tfm_an521** platform, execute the following commands:
```
    cd api-tests
    mkdir BUILD
    cd  BUILD
    cmake ../ -G"Unix Makefiles" -DTARGET=tgt_dev_apis_tfm_an521 -DCPU_ARCH=armv8m_ml -DSUITE=CRYPTO -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>"
    cmake --build .
```

### Build output
Building the test suite generates the following NSPE binaries:<br />
- **<build_dir>/val/val_nspe.a**
- **<build_dir>/platform/pal_nspe.a**
- **<build_dir>/dev_apis/<suite_name>/test_combine.a**

### Integrating the libraries into your target platform

1. Integrate **val_nspe.a**, **pal_nspe.a**, and **test_combine.a** libraries with your Non-secure OS so that these libraries get access to the APIs. For example, Crypto tests require access to Crypto APIs. This forms an NSPE binary.
2. Load the NSPE binary to the Non-secure memory.
3. Build your SPE binary and load into the Secure memory.

## Test suite execution
The following steps describe the execution flow before the test execution: <br />

1. The target platform must load the above binaries into appropriate memory. <br />
3. Upon booting firmware and Non-secure OS, the SUT boot software gives control to the test suite entry point **int32_t val_entry(void);** as an Non-secure application entry point returning test status code. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

For details on test suite integration, refer to the **Integrating the test suite with the SUT** section of [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf).

## Security implication

The API test suite may run at higher privilege level. An attacker can utilize these tests as a means to elevate privilege which can potentially reveal the platform secure attests. To prevent such security vulnerabilities into the production system, it is strongly recommended that the API test suite is run on development platforms. If it is run on production system, make sure system is scrubbed after running the test suite.

## License

Arm PSA test suite is distributed under Apache v2.0 License.

## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.*
