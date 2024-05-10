.. _source_structure:

################
Source Structure
################

TF-M is designed to provide a user-friendly source structure to ease
integration and service development. This document introduces the source
structure and its design intention of TF-M. Additionally, the folders below are
important for TF-M integration and descibed in a separate documents:

.. toctree::
    :maxdepth: 1

    Details for the /platform folder     <platform_folder.rst>
    Details for the /platform/ext folder <platform_ext_folder.rst>

/ (TF-M root)
=============
This table describes the structure under the root folder with part of
possible folders.

========================= ====================================
Folder name               Description
========================= ====================================
bl1                       The 1st stage immutable bootloader
bl2                       MCUBoot based 2nd stage bootloader
cmake                     Cmake files of the build system
config                    Configuration system files
docs                      The documentation
interface                 RoT service API for client calls
lib                       The 3rd party libraries
`platform`_               Platform intermedia files
`secure_fw`_              The secure firmware
tools                     Tools in scripts for building
========================= ====================================

platform
========
The `platform` folder contains SW ports of all :ref:`supported platforms
<supported_platforms>` and the files necessary for :ref:`adding a new platform
<adding_platform>`. Please refer to :ref:`platform folder document
<platform_folder>` for more information.

========================= =============================================
Folder name               Description
========================= =============================================
include                   HAL and platform public headers
ext                       Platform ports and related files
========================= =============================================

platform/ext
------------
This folder can include imported files licensed differently from TF-M's
BSD-3 license. More details are in the dedicated document :ref:`platform_ext_folder`.

========================= =============================================
Folder name               Description
========================= =============================================
accelerator               Supported Crypto HW accelerators
cmsis                     A copy of essential CMSIS headers
common                    Common HAL implementation
driver                    Driver headers for porting
target/<vendor>           Vendor specific folders with ported platforms
========================= =============================================

Each `vendor` is assigned one folder for usage under `target/`.
A `vendor` contributes platform's ports and manages the structure inside it.

secure_fw
=========
This folder contains components needed by secure firmware and the exported
interfaces for application, service development and HAL integration.

========================= =============================================
Folder name               Description
========================= =============================================
include                   Public headers of `secure_fw`
partitions                Default services and SPRTL
spm                       PSA FF-M SPM implementation
shared                    Sources shared out of SPRTL
========================= =============================================

The shared sources can be referenced by the building system out of SPRTL.
Generally, they are runtime and PSA APIs.

secure_fw/include
-----------------
This folder holds public headers for external references by clients,
services and platforms. Avoid putting private headers, not referenced by
other modules in this `include` folder.

secure_fw/partitions
--------------------
This folder contains default services implemented as partitions and runtime
utilities used and provided by TF-M.

============================== =============================================
Folder name                    Description
============================== =============================================
lib/runtime                    The SPRTL sources and intermedia files
<partition_x>                  Sources of `partition_x`
<partition_x>/include          RoT Service API headers of `partition_x`
<partition_y>                  Sources of `partition_y`
<partition_y>/include          RoT Service API headers of `partition_y`
============================== =============================================

Here `partition_x` and `partition_y` are examples of RoT services without
detailed structure of them.

secure_fw/spm
-------------
The SPM is the core component to provide a mechanism for providing secure
services complied with PSA FF-M.

============================== =============================================
Folder name                    Description
============================== =============================================
include                        SPM public headers.
ffm                            SPM logic complies with PSA FF-M specification
cmsis_psa                      CMSIS implementation for PSA FF-M SPM
============================== =============================================

--------------

*Copyright (c) 2020-2023, Arm Limited. All rights reserved.*
