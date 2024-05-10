
# Architecture Test Suite Software Requirements

Current release has been tested againt the below tools versions: <br />

- Host Operating System     : Ubuntu 16.04, Windows 10
- Scripting tools           : Python 3.7.1
- Host Compiler toolchain   : GCC 5.4.0 (Linux Host) or MinGW 6.3.0 (Windows Host)
- Cross Compiler toolchain  : GNU Arm Embedded Toolchain 7.3.1 or Arm Compiler 6.14
- Build tools               : CMake 3.15

**Note**: To compile the test suite code, at least one of the above supported compiler toolchains
        must be available in the build environment.

### Setting up a shell to enable compiler toolchain after installation

To import GNU Arm in your bash shell console, execute:
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

To import Arm Compiler in your bash shell console, execute:
~~~
export PATH=<ARM_COMPILER_PATH>/bin:$PATH
~~~

## Download source

To download the main branch of the repository, type the following command: <br />
~~~
git clone https://github.com/ARM-software/psa-arch-tests.git
~~~

## Cloning the external repository

To build Initial Attestation tests, the PSA APIs test suite clones the following
git repository. The cloning is automated as part of the test suite cmake build:

**Repo** <br />
Name      : https://github.com/laurencelundblade/QCBOR.git <br />
Commit id : 42272e466a8472948bf8fca076d113b81b99f0e0

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.*
