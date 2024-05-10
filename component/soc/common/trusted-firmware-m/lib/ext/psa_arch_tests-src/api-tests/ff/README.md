
# PSA Firmware Framework Architecture Test Suite

## PSA Firmware Framework

*PSA Firmware Framework* (PSA-FF) defines a standard interface and framework to isolate trusted functionality within constrained IoT devices.

The framework provides:
- Architecture that describes isolated runtime environments (partitions) for trusted and untrusted firmware.
- A standard model for describing the functionality and resources in each partition.
- A Secure IPC interface to request services from other partitions.
- A model that describes how the partitions can interact with one another, as well as the hardware and the firmware framework implementation itself.
- A standard interface for the PSA RoT services such as PSA RoT lifecycle service.

This specification enables the development of Secure firmware functionality which can be reused on different devices that use any conforming implementation of the Firmware Framework. For more information, download the [PSA-FF Specification](https://pages.arm.com/psa-resources-ff.html?_ga=2.97388575.1220230133.1540547473-1540784585.1540547382).

### Architecture test suite

The architecture test suite is a set of examples of the invariant behaviors that are specified by the PSA-FF specification. Use this suite to verify whether these behaviors are implemented correctly in your system. This suite contains self-checking and portable C-based tests with directed stimulus. The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.

This test suite is not a substitute for design verification. To review the test logs, Arm licensees can contact Arm directly through their partner managers.

For more information on architecture test suite specification, refer to the [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf) document.

## This release
 - Code Quality : REL v1.4
 - This release contains the PSA-FF tests that are written for the PSA FF 1.1 Extensions specification.

##  Release Tags

| Release version | Release tag  | PSA FF specification version |
|-----------------|---------------|----------------|
| REL v1.4 | [v22.01_API1.4_ADAC_BETA](https://github.com/ARM-software/psa-arch-tests/tree/v22.01_API1.4_ADAC_BETA/api-tests/ff) | 1.1-Alpha0 |
| REL v1.3 | [v21.10_API1.3_ADAC_ALPHA-1](https://github.com/ARM-software/psa-arch-tests/tree/v21.10_API1.3_ADAC_ALPHA-1/api-tests/ff) | 1.1-Alpha0 |
| REL v1.2 | [v21.07_API1.2_ADAC_ALPHA](https://github.com/ARM-software/psa-arch-tests/tree/v21.07_API1.2_ADAC_ALPHA/api-tests/ff) | 1.1-Alpha0 |
| REL v1.1 | [v20.11_API1.1](https://github.com/ARM-software/psa-arch-tests/tree/v20.11_API1.1/api-tests/ff) | 1.0 |
| REL v1.0 | [v20.03_API1.0](https://github.com/ARM-software/psa-arch-tests/tree/v20.03_API1.0/api-tests/ff) | 1.0 |
| v0.9 | [v19.06_API0.9](https://github.com/ARM-software/psa-arch-tests/tree/v19.06_API0.9/api-tests/ff) | 1.0-Beta1 |
| v0.8 | [v19.02_API0.8](https://github.com/ARM-software/psa-arch-tests/tree/v19.02_API0.8/api-tests/ff) | 1.0-Beta0 |

## Tests scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario Document](../docs/) present in the **docs/** folder.


## Getting started

Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. Make sure you have all required software installed as explained in the [Software Requirements](../docs/sw_requirements.md).

### Porting steps

Refer to the [PSA-FF Test Suite Porting Guide](../docs/porting_guide_ff.md) document for porting steps.

### Build steps

To build the test suite for your target platform, perform the following steps.

1. Execute `cd api-tests`.

2. Execute `python tools/scripts/manifest_update.py` to remove heap_size field from PSA test suite manifest files if your platform doesn't support the dynamic memory functions for the secure partition. Otherwise, skip this step.

3. Using your Secure partition build tool, parse the following test suite partition manifest files and generate manifest output files. The manifest parsing tool must be compliant with the manifest rules defined in the PSA FF specification.<br />
   <br />The test suite manifests to be parsed are:<br />
   - **platform/manifests/driver_partition_psa.json**
   - **platform/manifests/client_partition_psa.json**
   - **platform/manifests/server_partition_psa.json**

4. Compile the tests as shown below. <br />
```
    cd api-tests
    mkdir <build_dir>
    cd <build_dir>
    cmake ../ -G"<generator_name>" -DTARGET=<platform_name> -DCPU_ARCH=<cpu_architecture_version> -DSUITE=<suite_name> -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>"
    cmake --build .
```
<br />Options information:<br />

-   -G"<generator_name>" : "Unix Makefiles" to generate Makefiles for Linux and Cygwin. "MinGW Makefiles" to generate Makefiles for cmd.exe on Windows  <br />
-   -DTARGET=<platform_name> is the same as the name of the target-specific directory created in the **platform/targets/** directory. The current release has been tested on **tgt_dev_apis_tfm_an521**, **tgt_dev_apis_tfm_musca_b1** and **tgt_dev_apis_tfm_musca_a** platforms except for the tests written for PSA isolation level-3 and secure partition dynamic memory APIs as these features are unsupported by the mentioned platforms. However, it can still be possible to run them if the platform supports these features.<br />
-   -DTOOLCHAIN=<tool_chain> Compiler toolchain to be used for test suite compilation. Supported values are GNUARM (GNU Arm Embedded), ARMCLANG (ARM Compiler 6.x) and HOST_GCC. Default is GNUARM.<br />
-   -DCPU_ARCH=<cpu_architecture_version> is the Arm Architecture version name for which the tests should be compiled. Supported CPU arch are armv8m_ml, armv8m_bl and armv7m. Default is empty. This option is unused when TOOLCHAIN type is HOST_GCC.<br />
-   -DSUITE=<suite_name> is the test suite name. To compile PSA FF tests, use -DSUITE=IPC<br >
-   -DVERBOSE=<verbose_level>. Print verbosity level. Default is 3. Supported print levels are 1(INFO & above), 2(DEBUG & above), 3(TEST & above), 4(WARN & ERROR) and 5(ERROR).
-   -DBUILD=<BUILD_DIR> : To select the build directory to keep output files. Default is BUILD/ inside current directory.
-   -DINCLUDE_PANIC_TESTS=<0|1> : The default compilation flow includes the functional API tests to build the test suite. It does not include panic tests that check for the API's PROGRAMMER ERROR(Panic) conditions as defined in the PSA-FF specification. You can include the panic tests for building the test suite by setting this option to 1.
-   -DPLATFORM_PSA_ISOLATION_LEVEL=<1|2|3> : PSA Firmware Framwork isolation level supported by the platform. Default is highest level of isolation which is three.
-   -DSP_HEAP_MEM_SUPP=<0|1> : Are dynamic memory functions available to secure partition? 0 means no and 1 means yes. This skips the secure partition dynamic memory functions related tests if this is marked as zero.
-   -DWATCHDOG_AVAILABLE=<0|1>: Test harness may require to access watchdog timer to recover system hang. 0 means skip watchdog programming in the test suite and 1 means program the watchdog. Default is 1. Note, If the system under test doesn't support the reboot of the system when it encounters the panic situation, a watchdog must be available to the tests if INCLUDE_PANIC_TESTS set to 1.
-   -DSUITE_TEST_RANGE="<test_start_number>;<test_end_number>" is to select range of tests for build. All tests under -DSUITE are considered by default if not specified.
-   -DTFM_PROFILE=<profile_small/profile_medium> is to work with TFM defined Pofile Small/Medium definitions. Supported values are profile_small and profile_medium. Unless specified Default Profile is used.
-   -DSPEC_VERSION=<spec_version> is test suite specification version. Which will build for given specified spec_version. Supported values for FF test suite are 1.0 and 1.1 . Default is empty. <br/>
     If -DSPEC_VERSION option is not given it will build for latest version of testsuite.
     For spec version corresponds test list will be in testsuite.db file in api-tests/ff/ipc/ folder.
     Note: For FF 1.1 make sure to do the manifests changes and use SPEC_VERSION=1.1 .
-   -DSTATELESS_ROT_TESTS=<stateless_rot> is the flag for enabling stateless rot service for FF suite. Supported values are 0 and 1. 0 for connection based services and 1 for stateless rot services.
     Note: For using STATELESS ROT service must use -DSPEC_VERSION = 1.1 .
-   -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>" is an additional directory to be included into the compiler search path. To compile IPC tests, the include path must point to the path where **psa/client.h**, **psa/service.h**,  **psa/lifecycle.h** and test partition manifest output files(**psa_manifest/sid.h**, **psa_manifest/pid.h** and **psa_manifest/<manifestfilename>.h**) are located in your build system. Bydefault, PSA_INCLUDE_PATHS accepts absolute path. However, relative path can be provided using below format:<br />
```
    -DPSA_INCLUDE_PATHS=`readlink -f <relative_include_path>`
```

For using FF-1.1 do the following manifests changes in api-tests/platform/manifests files.
	Change "psa_framework_version" attribute from 1.0 to 1.1 in all manifests files.
	Add "model": "IPC" attribute in manifests files.
	Add "connection_based" attribute in all services of manifest file. Give value true or false accroding to your requirement. True for connection based services and false for stateless rot services.
	Replace signal to name in irq attribute of manifest file.

To compile IPC tests for **tgt_ff_tfm_an521** platform, execute the following commands:
```
    cd api-tests
    mkdir BUILD
    cd  BUILD
    cmake ../ -G"Unix Makefiles" -DTARGET=tgt_ff_tfm_an521 -DCPU_ARCH=armv8m_ml -DSUITE=IPC -DPLATFORM_PSA_ISOLATION_LEVEL=2 -DSP_HEAP_MEM_SUPP=0 -DPSA_INCLUDE_PATHS="<include_path1>;<include_path2>;...;<include_pathn>"
    cmake --build .
```
**Note**: The default compilation flow includes the functional API tests to build the test suite. It does not include panic tests that check for the API's PROGRAMMER ERROR conditions as defined in the PSA-FF specification. You can include the panic tests for building the test suite just by passing **-DINCLUDE_PANIC_TESTS=1** to CMake.

### Build output
The test suite build generates the following binaries:<br />

NSPE libraries:<br />
1. **<build_dir>/val/val_nspe.a**
2. **<build_dir>/platform/pal_nspe.a**
3. **<build_dir>/ff/<suite_name>/test_combine.a**

SPE libraries explicitly for IPC test suite:<br />
1. **<build_dir>/partition/driver_partition.a**
2. **<build_dir>/partition/client_partition.a**
3. **<build_dir>/partition/server_partition.a**

### Integrating the libraries into your target platform

1. Integrate the test partition (SPE archives) with your software stack containing SPM so that the partition code gets access to PSA-defined client and Secure partition APIs. This forms an SPE binary.
2. Integrate **val_nspe.a**, **pal_nspe.a** and **test_combine.a** libraries with your Non-secure OS so that these libraries get access to PSA client APIs. This forms an NSPE binary.
3. Load NSPE binary into Non-secure memory.
4. Load SPE binary into Secure memory.

## Test suite execution
The following steps describe the execution flow before the test execution: <br />

1. The target platform must load the above binaries into appropriate memory. <br />
2. The *System Under Test* (SUT) boots to an environment that initializes the SPM and the test suite partitions are ready to accept requests. <br />
3. On the Non-secure side, the SUT boot software gives control to the test suite entry point **int32_t val_entry(void);** as an application entry point returning test status code. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

For details on test suite integration, refer to the **Integrating the test suite with the SUT** section of [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf).

## Security implication

PSA FF test suite may run at higher privilege level. An attacker can utilize these tests as a means to elevate privilege which can potentially reveal the platform secure attests. To prevent such security vulnerabilities into the production system, it is strongly recommended that PSA FF test suite is run on development platforms. If it is run on production system, make sure system is scrubbed after running the test suite.

## License

Arm PSA test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.*
