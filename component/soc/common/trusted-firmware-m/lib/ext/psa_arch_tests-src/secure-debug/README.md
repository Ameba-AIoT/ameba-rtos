
# PSA Secure Debug: Arm ADAC Test Suite

## Introduction

### ADAC Specification

The [ADAC specification](https://developer.arm.com/documentation/den0101/0001) defines the protocol that allows a target to securely authenticate a debug host. The specification targets software-only layers that are above the physical debug link.

### ADAC test suite

The ADAC test suite checks if a device-side implementation conforms to the behavior described in the ADAC specification. The tests:
 - Are self-checking and portable, written in C with directed stimulus.
 - Are expected to run on the host platform only.
 - Drive the commands from the host platform through the debug link and verify the response obtained from the target platform.
 - Are available as open source.
 - And the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.

## Release details
 - Release Version - 0.8
 - Code Quality: Beta <br/>

 - The ADAC tests are written for version beta of the ADAC specification.
 - This test suite is not a substitute for design verification.

## Layers

ADAC tests use a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are:
 - Test suite
 - Validation Abstraction Layer (VAL)
 - Platform Abstraction Layer (PAL)

These tests are written on top of VAL and PAL.

The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In this release, PAL implementation for the 'emulation' target uses Unix sockets as the link layer to send commands and receive responses. Arm also provides support for executing the test suite on the 'Musca-b1' hardware board by adding a 'musca_b1' target which uses memory window protocol as the link layer.

You can also write your own abstraction layer implementations to allow ADAC tests to run on other host platforms and support the debug link layer.

## Scenarios

The mapping of the rules to the test cases and the steps followed in the tests are mentioned in the [Scenario document](docs/psa_adac_scenarios.md) present in the docs/ folder.

## Getting started


Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br/>
See [Arm Authenticated Debug Access Control Test Suite User guide](docs/Arm_Authenticated_Debug_Access_Control_Test_Suite_User_Guide.pdf) to get details on the steps involved in porting the test suite to your platform.
This test suite is provided as a separate directory within the psa-arch-tests repository.

### Prerequisites

Before starting the test suite build, ensure that the following requirements are met:

- Host Operating System: Ubuntu 16.04.4, RHEL 7, Windows 10
- Scripting tools: Python 3.7.1
- Other open-source tools: GNUARM 6.3.1, GCC 5.4.0 32-Bit (Linux Host), MinGW 6.3.0 32-Bit (Windows Host)
                           CMake 3.10 or later


## Download source
To download the main branch of the repository, type the following command:

	git clone https://github.com/ARM-software/psa-arch-tests.git

To download the main branch of the psa-adac repository, navigate to the secure debug directory and type the following command:

	git clone git@github.com:ARMmbed/psa-adac.git

Note:
    To receive access to the psa-adac repository and some of its dependent submodules, Arm licensees may contact Arm through their partner managers.


## Porting steps

See the [Porting Guide](docs/porting_guide_adac_host.md) for porting steps for your host platform.
See the [User Guide](docs/Arm_Authenticated_Debug_Access_Control_Test_Suite_User_Guide.pdf) in the docs/ folder for additional details.

## Build steps

To build ADAC test suite for a given host platform, execute the following commands: <br/>
~~~
    cd psa-arch-tests/secure-debug
    git clone git@github.com:ARMmbed/psa-adac.git --recurse-submodules
    cp psa_adac_ats.patch psa-adac/.
    cd psa-adac/
    git apply psa_adac_ats.patch
    cd ..
    mkdir <host_build_dir>
    cd <host_build_dir>
	cmake ../ -G<generator-name> -DTARGET=<target-name> -DSUITE=<suite-selection> -DLINK_LAYER_COMM=<link-layer>
	    where:
        - <generator-name>          "Unix Makefiles"  - to generate Makefiles for Linux and Cygwin
                                    "MinGW Makefiles" - to generate Makefiles for cmd.exe on Windows
        - <target-name>             target to build, as created in the platform/hosts directory
        - <suite-selection>         ADAC              - specify the ADAC suite
        - <link-layer>              link layer for the host platform, optional for emulation target

	To build project
	   cmake --build .
	To clean
	   cmake --build . -- clean
~~~

Note:
    Arm recommends running different build configurations in separate directories. Refer to the host-platform specific README for information on additional build options.

### Build output

The following ADAC test suite build outputs are available under host build directory, as created.

	- psa_adac_test executable
	- val and platform layer libraries

For running the test suite on the emulation platform, see the [Readme](platform/hosts/emulation/unix_socket/README.md).<br/>
For running the test suite on the Musca-b1 hardware platform, see the [Readme](platform/hosts/musca_b1/memw_pyocd/README.md).<br/>

Note:
 - See the psa-adac directory for information on other target platforms.
 - The keys and the certificate credentials for a supported cryptosystem can be generated using the tools provided in the psa-adac repository.
 - The test suite is designed to run once per supported cryptosystem. Ensure that the device must be provisioned with the correct certificates installed in the device's RoT.

## Security implication

ADAC test suite may run at higher privilege level. An attacker can utilize these tests to elevate privilege which can potentially reveal the platform Secure attests. To prevent such security vulnerabilities into the production system, it is recommended that ADAC test suite is run on development platforms. If it is run on production system, make sure system is scrubbed after running the test suite.

## License

Arm ADAC Architecture test suite is distributed under Apache v2.0 license.
The psa-adac code repository is distributed under BSD-3-Clause license.

## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.*
