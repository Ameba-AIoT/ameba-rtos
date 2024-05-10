
# Verifying test suite on Musca-b1 hardware platform

## Prerequisites

Before executing the test suite on host, ensure that the following requirements are met:

- Python 3.7.1 and above with pyOCD is installed.
- Debug probe is connected from the target to the host system using a USB cable.
- The target image is flashed onto the board.

## Build steps for target image

The steps to build and flash the image for the musca-b1 hardware target are described in the psa-adac repository in ports/targets/musca-b1/README.md.

## Build steps for host image

To build ADAC test suite, execute the following commands: <br/>
~~~
    cd psa-arch-tests/secure-debug
    mkdir <host_build_dir>
    cd <host_build_dir>
    cmake ../ -G"Unix Makefiles" -DTARGET=musca_b1 -DSUITE=ADAC -DLINK_LAYER_COMM=memw_pyocd
    cmake --build .
~~~

## Test Suite Execution

Arm uses UDP sockets to transmit and receive test information from the host to pyOCD. pyOCD is used for communication with the target board. The pyOCD script communicates with the board using a memory window-based debug mailbox.
To launch the pyOCD script, run the following in another terminal as shown:
~~~
	cd platform/hosts/musca_b1/mem_pyocd
	python host_connect.py <port_no>
	where:
        - <port_no>          - The port number for communicating between host and pyOCD. Default is 7777
~~~

To run the test suite, execute the following command from your host build directory <host_build_dir>:
~~~
    ./psa_adac_test ../psa-adac/tools/test/resources/keys/EcdsaP256Key-3.pem \../psa-adac/tools/test/resources/chains/chain.EcdsaP256-3 <ip_address> <port_no>
	where:
		- <ip_address>       - The IP address of the network connection running the server instance
        - <port_no>          - The port number for communicating between host and pyOCD. Default is 7777
~~~
--------------

*Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.*
