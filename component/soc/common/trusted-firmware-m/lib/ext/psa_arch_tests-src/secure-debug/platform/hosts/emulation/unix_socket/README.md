
# Verifying test suite on emulation platform

## Build steps for target image

To generate the target image for the virtual target, follow these steps:
~~~
    cd psa-arch-tests/secure-debug/psa-adac
    make native
~~~

## Build steps for host image

To build ADAC test suite, execute the following commands: <br/>
~~~
    cd psa-arch-tests/secure-debug
    mkdir <host_build_dir>
    cd <host_build_dir>
    cmake ../ -G"Unix Makefiles" -DTARGET=emulation -DSUITE=ADAC
    cmake --build .
~~~

## Test Suite Execution

The current release provides a reference implementation of ADAC target which communicates with the host platform using Unix sockets.
To launch the target to run as a server instance, run the target executable in another terminal as shown:
~~~
    cd psa-arch-tests/secure-debug/psa-adac
    ./build/native/Debug/psa_adac_server coms.socket tools/test/resources/chains/chain.EcdsaP256-3
~~~

To run the test suite, execute the following command from your host build directory <host_build_dir>:
~~~
    ./psa_adac_test ../psa-adac/tools/test/resources/keys/EcdsaP256Key-3.pem \../psa-adac/tools/test/resources/chains/chain.EcdsaP256-3 ../psa-adac/coms.socket
~~~
--------------

*Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.*
