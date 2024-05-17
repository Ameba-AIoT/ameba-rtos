######################################
Adding partitions for regression tests
######################################

************
Introduction
************

Some test group may need specific test services. These test services may support
one or more groups thus developers shall determine the proper test scope.
Currently, TF-M test services are located under ``tf-m-tests/test/secure_fw``.

+---------------------------------------+---------------------------------------------------------------+
| Folder name                           | Description                                                   |
+=======================================+===============================================================+
| test/secure_fw/suites/<suite>/service | Test service divided into corresponding suite subdirectories. |
+---------------------------------------+---------------------------------------------------------------+
| test/secure_fw/common_test_services   | Common test services.                                         |
+---------------------------------------+---------------------------------------------------------------+

**************
Implementation
**************

Adding a test partition to provide test services is same as adding a secure
partition, generally the process can be referenced from the document
`Adding Secure Partition <https://tf-m-user-guide.trustedfirmware.org/integration_guide/services/tfm_secure_partition_addition.html>`_.

Test Partition Specific Manifest Attributes
===========================================

Each test service must have resource requirements declared in a manifest file,
the contents of test services are the same as secure partitions, but their
locations are different. Test service manifests shall be set in
``tf-m-tests/test/secure_fw/tfm_test_manifest_list.yaml``.

There are some test purpose attributes in Secure Partition manifests that are
**NOT** compatible with FF-M.
They should be used in Test Partitions only.

weak_dependencies
-----------------
A TF-M regression test Partition calls other RoT services for test. But it
can still run other tests if some of the RoT services are disabled.
TF-M defines a ``"weak_dependencies"`` attribute in partition manifests of
regression test partitions to describe test service access to other RoT
services. It *shall* be only used for TF-M regression test services.

model
-----
A TF-M regression test Partition may support both the SFN and IPC model.
The actual model being used follows the SPM backend enabled.

The TF-M build system supports this by allowing Secure Partitions to set
the ``model`` attribute to ``dual``.
The manifest tool will then change it to the corresponding value according
to the current backend selected.

The Test Partitions use the following definitions to know what model is being
built:

- ``<<partition_name>>_MODEL_IPC``, ``1`` if IPC model is used.
- ``<<partition_name>>_MODEL_SFN``, ``1`` if SFN model is used.

Test service implementation
===========================

Test service of individual test
-------------------------------

An individual test dedicated test service should be put under the corresponding
test folder ``test/secure_fw/suites/<test_name>``.

``add_subdirectory(suites/<test_name>/<service_dir>)`` shall be added into
``tf-m-tests/test/secure_fw/secure_tests.cmake`` to make sure that the test
service is built with secure side configuration.

Common test service
-------------------

If a new test service is required by multiple test suites, the code should be
put under ``test/secure_fw/common_test_services``. If the new test suite relies
on a common test service, please make sure that the build implementation of the
test service is linked correctly, including the header files and libraries.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
