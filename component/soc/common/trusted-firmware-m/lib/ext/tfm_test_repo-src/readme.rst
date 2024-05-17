########################
Trusted Firmware-M Tests
########################
The Trusted Firmware-M(TF-M) Tests repo is meant to hold various tests for the
`Trusted Firmware-M`_.
The TF-M tests mainly focus on functionalities of various TF-M componentes such
as the TF-M SPM and various Secure Partitions.

There is also the `psa-arch-tests`_ suite which mainly focuses on the
implementation compliance of the Platform Security Architecture (PSA).

****************
Folder Structure
****************
- app - The application code that executes the testing, including both the TF-M
  tests and the `psa-arch-tests`_.
- docs - The documents about test developments.
- CMSIS - The libraries of RTX which is the Non-secure RTOS of the tests
- test - The TF-M test codes including test framework, test suites and test
  services
- lib/ext - 3rd party projects. May contain either imported source codes or CMake files to fetch the
  projects.

#######
License
#######
The software is provided under a BSD-3-Clause :doc:`License </license>`.
Contributions to this project are accepted under the same license with developer sign-off as
described in the
`TF-M Contributing Guidelines <https://tf-m-user-guide.trustedfirmware.org/contributing/contributing_process.html>`__

This project contains code or pre-built binaries from other projects as listed below.
The code from external projects is limited to ``CMSIS`` and ``lib`` folders.
The original license texts are included in those folders.

  - ``CMSIS`` - `Apache License <http://www.apache.org/licenses/>`__ Version 2.0 license
  - The ``lib/ext`` folder may contain 3rd party projects and files with diverse licenses.
    Here are some that are different from the BSD-3-Clause and may be a part of the runtime image.
    The source code for these projects is fetched from upstream at build time only.

    - ``erpc`` - `Modified BSD-3-Clause license <https://github.com/EmbeddedRPC/erpc/blob/develop/LICENSE>`__

####################
Feedback and support
####################
Feedback can be submitted via email to
`TF-M mailing list <tf-m@lists.trustedfirmware.org>`__.

.. _Trusted Firmware-M: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/
.. _psa-arch-tests: https://github.com/ARM-software/psa-arch-tests

*Copyright (c) 2020-2023, Arm Limited. All rights reserved.*
