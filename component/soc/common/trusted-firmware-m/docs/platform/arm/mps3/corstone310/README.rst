Corstone SSE-310 with Ethos-U55 Example Subsystem for Arm Virtual Hardware, and for MPS3 (AN555)
================================================================================================

Introduction
------------

Corstone-310 (formerly Corstone-Polaris) is an Arm reference subsystem for
secure System on Chips containing an Armv8.1-M Cortex-M85 processor and an
Ethos-U55 neural network processor. It is an MPS3 based platform with the
usual MPS3 peripherals.

This platform port supports all TF-M regression tests (Secure and Non-secure)
with Isolation Level 1 and 2.

.. note::

   This platform support does not provide software for Ethos-U55 IP, only
   contains base address and interrupt number for it.

.. note::

   For Armclang compiler v6.18 or later version is required.

Building TF-M
-------------

Follow the instructions in :doc:`Building instructions </building/tfm_build_instruction>`.

For Corstone-310 Ethos-U55 Arm Virtual Hardware use the following platform name:

``-DTFM_PLATFORM=arm/mps3/corstone310/fvp``

.. note::

   The built binaries can be run on the Corstone-310 Arm Virtual Hardware
   (VHT_Corstone_SSE-310). At least VHT version 11.17 is required.

For AN555 use the following platform name:

``-DTFM_PLATFORM=arm/mps3/corstone310/an555``

To run the example code on Corstone-310 Ethos-U55 Arm Virtual Hardware
----------------------------------------------------------------------

To utilize the `Arm Virtual Hardware (AVH) <https://arm-software.github.io/AVH/main/simulation/html/Using.html>`_, you will need to create an `AWS Account <https://aws.amazon.com/>`_ if you don’t already have one.

Launching the instance in EC2 `(AWS on getting started) <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html>`_
1. Go to `EC2 <https://console.aws.amazon.com/ec2/v2/>`_ in the AWS Web Console.
2. Select **Launch Instances** which will take you to a wizard for launching the instance.

     1. **Choose an Amazon Machine Image** `(AMI) <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AMIs.html>`_  In the Search box, type `Arm Virtual Hardware` then find the item called "Arm Virtual Hardware" that is by Arm, and press Select for that item.
        This will raise a subscription page/pop-up titled, **Arm Virtual Hardware**. You will note that the subscription is free from Arm, but AWS does charge for the costs of the instances themselves according to the pricing chart provided.

        You must select Continue if you want to move forward.

     2. **Choose an Instance Type** - Select one of the instance types from the list. Keep in mind that there are charges that accrue while the instance is running.
        From here you may select **Review and Launch** to move directly to the launch page or select **Next: Configure Instance Details** if you need to set any custom settings for this instance.


Once you complete the wizard by initiating the instance launch you will see a page that allows you to navigate directly to the new instance. You may click this link or go back to your list of instances and find the instance through that method.

Whichever way you choose find your new instance and select its instance ID to open the page to manage the instance.

Connecting to the instance:
   1. Select **Connect** to open an SSH terminal session to the instance in your browser.
   2. Ensure the **User name** field is set to `ubuntu`.
   3. Select the **Connect** button to open the session. This will put you in a browser window where you will have an SSH terminal window ready for your input.

The TF-M can be cloned and built in the instance after connecting.
To run the built binaries:

#. Execute the following command to start VHT::

    $ VHT_Corstone_SSE-310 -a cpu0*="<path-to-build-directory>/bl2.axf" --data "<path-to-build-directory>/tfm_s_ns_signed.bin"@0x01020000

#. The  serial port's output can be redirected to a file with::

    $ VHT_Corstone_SSE-310 -a cpu0*="<path-to-build-directory>/bl2.axf" --data "<path-to-build-directory>/tfm_s_ns_signed.bin"@0x01020000 -C mps3_board.uart0.unbuffered_output=1 -C mps3_board.uart0.out_file="output.log"

   The output should contain the following messages::

    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    [INF] Starting bootloader
    [INF] Beginning BL2 provisioning
    [INF] Swap type: none
    [INF] Swap type: none
    [INF] Bootloader chainload address offset: 0x40000
    [INF] Jumping to the first image slot
    [INF] Beginning TF-M provisioning
    [Sec Thread] Secure image initializing!
    TF-M isolation level is:0x00000001
    Booting TF-M v1.6.0
    Creating an empty ITS flash layout.
    Creating an empty PS flash layout.
    Non-Secure system starting...


.. note::

   Some of the messages above are only visible when ``CMAKE_BUILD_TYPE`` is set
   to ``Debug``.

To run the example code on AN555
--------------------------------
FPGA image is available for download from `here <https://developer.arm.com/downloads/view/AN555>`__

If the link above is not working just go to `Arm PDH <https://developer.arm.com/downloads>`__ and search for AN555.

To run BL2 bootloader, TF-M example application and tests in the MPS3 board,
it is required to have AN555 image in the MPS3 board SD card. The image should
be located in ``<MPS3 device name>/MB/HBI<BoardNumberBoardrevision>/AN555``

The MPS3 board tested is HBI0309C.

#. Execute the following command to create the tfm.bin binary
   which fills the entire available space on the MPS3 onboard QSPI.
   This way the whole flash content (PS, ITS, OTP, NV counters) is in a known state.
   (If anything left in the QSPI, this binary will overwrite it.)
   Also, MPS3 can only handle SFN 8.3 format, so the binary name must be shortened.::

   $ cd <build dir>/bin
   $ cp tfm_s_ns_signed.bin tfm.bin
   $ truncate -s 8M tfm.bin

#. Copy ``bl2.bin`` and ``tfm.bin`` files from
   build dir to ``<MPS3 device name>/SOFTWARE/``
#. Open ``<MPS3 device name>/MB/HBI0309C/AN555/images.txt``
#. Update the ``images.txt`` file as follows::

    [IMAGES]
    TOTALIMAGES: 2

    IMAGE0ADDRESS: 0x01_00_1100_0000
    IMAGE0UPDATE: RAM
    IMAGE0FILE: \SOFTWARE\bl2.bin

    IMAGE1ADDRESS: 0x01_00_0000_0000
    IMAGE1UPDATE: FORCEQSPI
    IMAGE1FILE: \SOFTWARE\tfm.bin

#. Close ``<MPS3 device name>/MB/HBI0309C/AN555/images.txt``
#. Unmount/eject the ``<MPS3 device name>`` unit
#. Reset the board to execute the TF-M example application
#. After completing the procedure you should be able to see similar messages
   to this on the serial port (baud 115200 8n1)::

    [INF] Starting bootloader
    [INF] Beginning BL2 provisioning
    [WRN] TFM_DUMMY_PROVISIONING is not suitable for production! This device is NOT SECURE
    [INF] Swap type: none
    [INF] Swap type: none
    [INF] Bootloader chainload address offset: 0x0
    [INF] Jumping to the first image slot
    [INF] Beginning TF-M provisioning
    [WRN] TFM_DUMMY_PROVISIONING is not suitable for production! This device is NOT SECURE
    [Sec Thread] Secure image initializing!
    TF-M isolation level is: 0x00000002
    Booting TF-M <TF-M version and git hash>
    Creating an empty ITS flash layout.
    Creating an empty PS flash layout.
    [INF][Crypto] Provisioning entropy seed... complete.
    Non-Secure system starting...

.. note::

   Some of the messages above are only visible when ``CMAKE_BUILD_TYPE`` is set
   to ``Debug``.

--------------

*Copyright (c) 2021-2023, Arm Limited. All rights reserved.*
