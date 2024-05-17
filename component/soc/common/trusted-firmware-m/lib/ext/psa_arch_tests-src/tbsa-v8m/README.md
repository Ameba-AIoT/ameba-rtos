
# TBSA-v8M : Architecture Test Suite


## Introduction

### TBSA-v8M Specification

The TBSA-v8M specification defines the behavior of an abstract machine referred to as a TBSA-v8M system. Implementations that are compliant with the TBSA-v8M architecture must conform to the described behavior of the TBSA-v8M System.

To receive a copy of the TBSA-v8M specification, Arm Licensees may contact Arm through their partner managers.

### Architecture test suite

The TBSA-v8M Architecture test suite checks whether an implementation conforms to the behaviors described in the TBSA-v8M specification.

TBSA-v8M compliance tests are self-checking, portable C-based tests
with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.


## Release details
 - Release Version - 0.9
 - Code Quality: Beta <br />
   The suite is in development. This indicates that the suite contains tests which have not been validated on any platform. Please use this opportunity to suggest enhancements and point out errors.

 - The TBSA-v8M tests are written for version 1.0 Beta 1 of the [TBSA-M specification](https://developer.arm.com/-/media/Files/pdf/PlatformSecurityArchitecture/Architect/DEN0083-PSA_TBSA-M_1.0-bet1.pdf?revision=804e230c-34b6-457d-9801-9608c1609015&la=en).
 - This test suite is not a substitute for design verification.

 - To review the test logs, Arm licensees can contact Arm directly through their partner managers.

## Layers

TBSA-v8M compliance tests use a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are:
         - Test suite
         - Validation Abstraction Layer (VAL)
         - Platform Abstraction Layer (PAL)


These tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL).

The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In this release, PAL is written on top of baremetal drivers and CMSIS.

Partners can also write their own abstraction layer implementations to allow TBSA-v8M tests to be run in other environments, for example, as raw workload on an RTL simulation.

## Scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario document](docs/Arm_TBSA-v8M_Arch_Test_Scenario_Document.pdf) present in the docs/ folder.

## Getting started


Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br />
See User guide document to get details on the steps involved in Porting the test suite to your platform.


### Prerequisites

Before starting the test suite build, ensure that the following requirements are met:

- Host Operating System     : Ubuntu 16.04.4, Windows 10
- Scripting tools           : Python3 3
- Other open-source tools   : GNUARM 6.3.1, GCC 5.4.0 32-Bit (Linux Host), mingw 6.3.0 32-Bit (Windows Host)
                              CMake 3.10


## Download source
To download the master branch of the repository, type the following command:

	git clone https://github.com/ARM-software/psa-arch-tests.git


## Porting steps

Refer to the [Porting Guide](docs/Arm_TBSA-v8M_Arch_Test_Porting_Guide.md) document for porting steps.
Refer to the [User Guide](docs/Arm_TBSA-v8M_Arch_Test_Validation_Methodology_and_User_Guide.pdf) document in the docs folder for additional details.

## Build steps

To build TBSA-v8M test suite for a given platform, execute the following commands: <br/>
~~~
	cd psa-arch-tests/tbsa-v8m
	mkdir cmake_build
	cd cmake_build
	cmake ../ -G<generator-name> -DTARGET=<target-name> -DCOMPILER=<compiler-selection> -DSUITE=<suite-selection> -DCMSIS_REPO_PATH=<CMSIS-repo-path> -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMSIS_DRIVER=OFF
	    where:
	        - <generator-name>          "Unix Makefiles"  - to generate Makefiles for Linux and Cygwin
                                            "MinGW Makefiles" - to generate Makefiles for cmd.exe on Windows
		- <target-name>             target to build, as created in the platform/board directory
		- <compiler-selection>      GNUARM
                                            Defaults to GNUARM if not specified
		- <suite-selection>         ALL                           - builds all test_pool components
                                            "<comp1>:<comp2>:..:<compn>"  - for selective component build or
                                            <comp1>\:<comp2>\:..\:<compn> - for selective component build
                                            Defaults to ALL if not specified
		- <CMSIS-repo-path>         Absolute CMSIS repo path
                                            If not specified CMake clones the CMSIS for itself
		- <CMAKE_VERBOSE_MAKEFILE>  ON  - To get detailed build log on console
                                            OFF - To get minimalistic log on console
                                            Defaults to OFF
		- <CMAKE_DRIVER>            ON  - Build takes CMSIS drivers as specified in target specific target.cmake
                                            OFF - Build takes non CMSIS drivers as specified in target specific target.cmake
                                            Defaults to OFF
	To build project
	   cmake --build .
	To clean
	   cmake --build . -- clean
~~~

~~~
Note:
    It is recommended to build each different build configurations in separate
    directories.
~~~

### Build output
TBSA build outputs are available under build directory: cmake_build - as created.

	- tbsa.elf, tbsa.map
	- tbsa_test_combined.bin
	- test specific executables and Map files

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution.
1. The target platform must load the tbsa.elf file.
2. The suite execution begins from the tbsa_entry.
3. The tests are executed sequentially in a loop in the tbsa_dispatcher function.

## Security implication

TBSA test suite may run at higher privilege level. An attacker can utilize these tests as a means to elevate privilege which can potentially reveal the platform secure attests. To prevent such security vulnerabilities into the production system, it is strongly recommended that TBSA test suite is run on development platforms. If it is run on production system, make sure system is scrubbed after running the test suite.

## License

Arm TBSA-v8M Architecture test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
