#################################
Adding TF-M Regression Test Suite
#################################

.. contents:: Table of Contents

*************************************
Introduction of TF-M regression tests
*************************************

TF-M regression tests test whether changes to TF-M code work as expected.
A new regression test can consist of following 3 components:

1. ``test suite``: A series of tests of a certain function.
2. ``test case``: A specific test instance in test suites.
3. ``test service or partition``: Optional secure services or partitions to
   support related test suites.

Source structure
================

+---------------------------------------+---------------------------------------------------------------+
| Folder name                           | Description                                                   |
+=======================================+===============================================================+
| test/bl1                              | TF-M bl1 test suites source code.                             |
+---------------------------------------+---------------------------------------------------------------+
| test/bl2                              | TF-M bL2 test suites source code.                             |
+---------------------------------------+---------------------------------------------------------------+
| test/config                           | The CMAKE test configurations files.                          |
+---------------------------------------+---------------------------------------------------------------+
| test/framework                        | Source code for test framework code, managing test suites.    |
+---------------------------------------+---------------------------------------------------------------+
| test/secure_fw/suites                 | Test suites divided into subdirectories.                      |
+---------------------------------------+---------------------------------------------------------------+
| test/secure_fw/suites/<suite>/service | Test service divided into corresponding suite subdirectories. |
+---------------------------------------+---------------------------------------------------------------+
| test/secure_fw/common_test_services   | Common test services.                                         |
+---------------------------------------+---------------------------------------------------------------+

***********************
Adding a new test suite
***********************

This section introduces how to add a new test suite and control its compilation
with a test configuration in ``tf-m-tests`` repository.

Source code
===========

The test suite example subdirectory named ``<test_name>`` is located under the
path ``tf-m-tests/test/secure_fw/suites``. If the new test suite includes both
non-secure and secure parts, the source code shall be divided shared code and
specific code. An example test suite folder can be organized as the figure
below.

.. code-block:: bash

    .
    ├── CMakeLists.txt
    ├── <test_name>_tests_common.c
    ├── non_secure
    │   ├── <test_name>_ns_interface_testsuite.c
    │   └── <test_name>_ns_tests.h
    └── secure
        ├── <test_name>_s_interface_testsuite.c
        └── <test_name>_s_tests.h

Creating test configurations
============================

A test configuration controls whether one or multiple test suites are enabled.
The doc `TF-M Build Instructions <https://tf-m-user-guide.trustedfirmware.org/technical_references/instructions/tfm_build_instruction.html>`_.
shows some test configurations which are already supported in current TF-M.
An example usage of test configuration shows below.

.. code-block:: bash

    cmake -S . -B cmake_build -DTFM_PLATFORM=arm/mps2/an521               \
                              -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake \
                              -DCMAKE_BUILD_TYPE=Release                  \
                              -DTEST_NS_ATTESTATION=ON

With this command, only non-secure initial attestation test suite will be built.

Developers shall assign corresponding test configurations to control the test
suites.

Naming test configurations
--------------------------

The test configurations of example test suites are ``TEST_NS_<TEST_NAME>``
in non-secure and ``TEST_S_<TEST_NAME>`` in secure.

.. Note::
    The test configurations must be named with the prefixes ``TEST_S_`` and
    ``TEST_NS_``, for secure regression tests and non-secure regression tests
    respectively. Otherwise, tf-m-tests build system may not recognize it.

Setting test configurations
---------------------------

When the test configurations have dependences, the default value need to be set.
The setting is performed in following four steps.

#. Command line input: The configuration can be enabled or disabled by the
   command ``-DTEST_NS_<TEST_NAME>=ON/OFF -DTEST_S_<TEST_NAME>=ON/OFF``, and
   the value cannot be changed throughout the whole compilation of TF-M tests.

#. ``tf-m-tests/config/set_config.cmake``: The test configurations shall be
   OFF if its dependences are not supported. The dependences are probably
   from:

    #. TF-M partitions configurations like ``TFM_PARTITION_CRYPTO``,
       ``TFM_PARTITION_INITIAL_ATTESTATION``, etc.
    #. TF-M build mode configuration like ``CONFIG_TFM_SPM_BACKEND``.
    #. TF-M other configurations like ``TFM_PARTITION_FIRMWARE_UPDATE``, etc.

#. ``tf-m-tests/config/default_ns_test_config.cmake`` or
   ``tf-m-tests/config/default_s_test_config.cmake``: It is required to give
   the default value of the new test configuration in these two files when
   ``TEST_NS`` or ``TEST_S`` is ON. The recommended value is ON unless the
   single test's code or data size is very large.

#. ``tf-m-tests/config/default_test_config.cmake``: It is required to give the
   default value of the new test configuration in the file when both
   ``TEST_NS`` and ``TEST_S`` are OFF. The default value must be OFF.

.. Note::
   The test configurations must be set as CACHE value in CMAKE files. The CACHE
   set cannot replace the value from command line, see
   `Set Cache Entry <https://cmake.org/cmake/help/latest/command/set.html#set-cache-entry>`_.

Checking test configurations
----------------------------

The new test configurations must be checked by function ``tfm_invalid_config()``
if they have any dependence. The value comes from command line may be wrong when
the dependences are conflicting. In addition to the dependences quoted in
``tf-m-tests/config/set_config.cmake``, some other test configurations may be
necessary.

Applying test configurations
============================

The mission of test configurations is to control the build. They are applied
in ``test/secure_fw/suites/<test_name>/CMakeLists.txt`` like the example below.

.. code-block:: cmake

    cmake_policy(SET CMP0079 NEW)

    if (NOT TEST_NS_<TEST_NAME> AND NOT TEST_S_<TEST_NAME>)
        return()
    endif()

    ####################### Non Secure #########################################

    if (TEST_NS_<TEST_NAME>)
        add_library(tfm_test_suite_<test_name>_ns STATIC EXCLUDE_FROM_ALL)
        # target_sources()
        # target_include_directories()
        target_compile_definitions(tfm_test_suite_<test_name>_ns
            INTERFACE
                TEST_NS_<TEST_NAME>
        )
        # target_link_libraries()
    endif()

    ####################### Secure #############################################

    if (TEST_S_<TEST_NAME>)
        add_library(tfm_test_suite_<test_name>_s STATIC EXCLUDE_FROM_ALL)
        # target_sources()
        # target_include_directories()
        target_compile_definitions(tfm_test_suite_<test_name>_s
            INTERFACE
                TEST_S_<TEST_NAME>
        )
        # target_link_libraries()
    endif()

The function ``target_compile_definitions`` will export the macros
``TEST_NS_<TEST_NAME>`` or ``TEST_S_<TEST_NAME>`` into source code. and in the
file ``tf-m-tests/framework/non_secure_suites.c`` or
``tests/framework/secure_suites.c``, the definitions of these macros will be
checked, and then the head file will be included and test cases will be
registered if the macro is defined.

.. code-block:: c

    #ifdef TEST_NS_<TEST_NAME>
    #include "<test_name>_ns_tests.h"
    #endif

    static struct test_suite_t test_suites[] = {
    /* Non-secure example test cases */
        // ......
    #ifdef TEST_NS_<TEST_NAME>
        {&register_testsuite_ns_<test_name>_interface, 0, 0, 0},
    #endif
    };

.. code-block:: c

    #ifdef TEST_S_<TEST_NAME>
    #include "<test_name>_s_tests.h"
    #endif

    static struct test_suite_t test_suites[] = {
    /* Secure example test cases */
        // ......
    #ifdef TEST_S_<TEST_NAME>
        {&register_testsuite_s_<test_name>_interface, 0, 0, 0},
    #endif
    };

.. Note::
    On most platforms non-secure tests and secure tests run on the same CPU
    core, but dual-core platform is an exception. So secure test library and
    secure services shall be linked together in the file
    ``tf-m-tests/test/secure_fw/secure_tests.cmake``. Thus they can be built on
    secure CPU core and non-secure tests library and RTOS are built on
    non-secure CPU core.

.. code-block:: cmake

    if (TEST_FRAMEWORK_S)
        # ...
        if (TEST_S_<TEST_NAME>)
            add_library(tfm_test_suite_<test_name>_s STATIC EXCLUDE_FROM_ALL)
        endif()
    endif()

************************************
Adding a new test case in test suite
************************************

The test cases usually express as a function in source code. They will be added
into an array with structure type called ``test_t`` defined in
``tf-m-tests/test/framework/test_framework.h``.

.. code-block:: c

    struct test_t {
        TEST_FUN * const test;         /*!< Test function to call */
        const char *name;              /*!< Test name */
        const char *desc;              /*!< Test description */
    };

For example, a new test case called ``TFM_NS_<TEST_NAME>_TEST_1001`` is created
and the function ``tfm_<test_name>_test_1001`` needs to be defined in file
``<test_name>_ns_interface_testsuite.c``. Then the function shall be appended
into the array which will be quoted in function
``register_testsuite_ns_<test_name>_interface``. See the reference code below.

.. code-block:: c

    /* List of test cases */
    static void tfm_<test_name>_test_1001(struct test_result_t *ret);

    /* Append test cases */
    static struct test_t <test_name>_tests[] = {
        {&tfm_<test_name>_test_1001, "TFM_NS_<TEST_NAME>_TEST_1001",
        "Example test case"},
    };

    /* Register test case into test suites */
    void register_testsuite_ns_<test_name>_interface(struct test_suite_t *p_test_suite)
    {
        uint32_t list_size;

        list_size = (sizeof(<test_name>_tests) / sizeof(<test_name>_tests[0]));

        set_testsuite("<TEST_NAME> non-secure interface test (TFM_NS_<TEST_NAME>_TEST_1XXX)",
                        <test_name>_tests, list_size, p_test_suite);
    }

    static void tfm_<test_name>_test_1001(struct test_result_t *ret)
    {
       /* test case code */
    }

********************
Adding test services
********************

Some test group may need specific test services. These test services may support
one or more groups thus developers shall determine the proper test scope. Refer
to
`Adding partitions for regression tests <https://git.trustedfirmware.org/TF-M/tf-m-tests.git/tree/docs/tfm_test_services_addition.rst>`_
to get more information.

**********************************
Out-of-tree regression test suites
**********************************

TF-M supports out-of-tree regression test suites build, whose source code
folder is outside tf-m-tests repo. There are two configurations for developers
to include the source code.

- ``EXTRA_NS_TEST_SUITE_PATH``

  An absolute directory of the out-of-tree non-secure test suite
  source code folder. TF-M build system searches ``CMakeLists.txt`` of
  non-secure test suite in the source code folder.

- ``EXTRA_S_TEST_SUITE_PATH``

  An absolute directory of the out-of-tree secure test suite
  source code folder.

Example usage
=============

Take non-secure test as an example in
`tf-m-extras <https://git.trustedfirmware.org/TF-M/tf-m-extras.git/>`_.
A single out-of-tree test suite folder can be organized as the figure below:

.. code-block:: bash

    extra_ns
    ├── CMakeLists.txt
    ├── ns_test.c
    └── ns_test_config.cmake

In the example above, ``EXTRA_NS_TEST_SUITE_PATH`` in the build command can be
specified as listed below.

.. code-block:: bash

  -DEXTRA_NS_TEST_SUITE_PATH=<Absolute-path-extra-test-folder>

Coding instructions
===================

This is a demo of source code so the structure has been simplified. Files like
``.c`` and ``.h`` can be expanded to ``src`` and ``include`` folders
respectively. The ``CMakeLists.txt`` is required in the root path and
``ns_test_config.cmake`` is optional.

Header Files
------------

The header file ``extra_ns_tests.h`` must be included by out-of-tree source
code. This file contains the declaration of
``void register_testsuite_extra_ns_interface(struct test_suite_t *p_test_suite)``.

Source code
-----------

To connect the out-of-tree source code and tf-m-tests framework, the test case
function/functions must be defined first. An example format is:

.. code-block:: c

    void ns_test(struct test_result_t *ret)
    {
        /* Add platform specific non-secure test suites code here. */

        ret->val = TEST_PASSED;
    }

This function follows the standard TF-M test case function prototype.

.. note::
    Extra tests can have one or more test cases. This is simplified example so
    only one test case is added.

After ``ns_test()`` is defined, a structure variable need to be created like:

.. code-block:: c

    static struct test_t plat_ns_t[] = {
        {&ns_test, "TFM_EXTRA_TEST_1001",
         "Extra Non-Secure test"},
    };

It will be used by function ``register_testsuite_extra_ns_interface()`` to
register the test by calling the ``set_testsuite()`` function:

.. code-block:: c

    void register_testsuite_extra_ns_interface(struct test_suite_t *p_test_suite)
    {
        uint32_t list_size;

        list_size = (sizeof(plat_ns_t) /
                     sizeof(plat_ns_t[0]));

        set_testsuite("Extra Non-Secure interface tests"
                      "(TFM_NS_EXTRA_TEST_1XXX)",
                      plat_ns_t, list_size, p_test_suite);
    }

The platform initialization code can be added in this function because it runs
before ``ns_test()``.

.. Note::
    Function ``register_testsuite_extra_ns_interface()`` is declared in
    tf-m-tests repository without definition. It is supplied to out-of-tree
    source code and need to be defined with no change of its format, like
    returns error code and parameter name.


CMakeLists.txt
--------------

After extra test suite file were created they must be linked to
``tfm_test_suite_extra_ns`` CMAKE target:

.. code-block:: cmake

    target_sources(tfm_test_suite_extra_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/ns_test.c
    )

ns_test_config.cmake
--------------------

The CMAKE configuration file is optional. If out-of-tree source already exists
another configuration file, a new one can be ignored.

--------------

*Copyright (c) 2021-2022, Arm Limited. All rights reserved.*
*Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
or an affiliate of Cypress Semiconductor Corporation. All rights reserved.*
