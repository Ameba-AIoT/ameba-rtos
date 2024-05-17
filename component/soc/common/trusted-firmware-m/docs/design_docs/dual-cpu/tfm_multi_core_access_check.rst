################################################################
Memory Access Check of Trusted Firmware-M in Multi-Core Topology
################################################################

:Author: David Hu
:Organization: Arm Limited
:Contact: david.hu@arm.com

************
Introduction
************

TF-M memory access check function ``tfm_has_access_to_region()`` checks whether an access has proper
permission to read or write the target memory region.

On single Armv8-M core platform based on Trustzone, TF-M memory access check implementation relies
on `Armv8-M Security Extension`_ (CMSE) intrinsic ``cmse_check_address_range()``.
The secure core may not implement CMSE on multi-core platforms. Even if CMSE is
implemented on a multi-core platform, additional check on system-level security
and memory access management units are still necessary since CMSE intrinsics and
TT instructions are only aware of MPU/SAU/IDAU inside the secure core.

As a result, TF-M in multi-core topology requires a dedicated access check
process which can work without CMSE support. This document discuss about the
design of the memory access check in multi-core topology.

.. _Armv8-M Security Extension: https://developer.arm.com/architectures/cpu-architecture/m-profile/docs/100720/0100/secure-software-guidelines/armv8m-security-extension

**************
Overall Design
**************

Memory Access Check Policy
==========================

The policies vary in diverse Isolation Levels.

When TF-M works in Isolation Level 1, the access check in multi-core topology
checks

- Memory region is valid according to system settings
- Non-secure client call request should not access secure memory.
- Secure services should not directly access non-secure memory. According to PSA
  Firmware Framework, Secure services should call Secure Partition APIs to ask
  TF-M SPM to fetch non-secure input/output buffer for them.
- Whether read/write attribute match between access and memory region

When TF-M works in Isolation Level 2, the access check in multi-core topology
checks:

- Memory region is valid according to system settings
- Non-secure client call request should not access secure memory.
- Secure services should not directly access non-secure memory. According to PSA
  Firmware Framework, Secure services should call Secure Partition APIs to ask
  TF-M SPM to fetch non-secure input/outputs buffer for them.
- Whether read/write attribute match between access and memory region
- Unprivileged secure access should not access privileged secure memory region

The check policy in Isolation Level 3 will be defined according to TF-M future
implementation.

The above policies will be adjusted according to TF-M implementation and PSA
specs.

General Check Process in TF-M Core
==================================

In multi-core topology, ``tfm_has_access_to_region()`` is still invoked to keep an uniform interface
to TF-M SPM. The function implementation should be placed in multi-core topology specific files
separated from Trustzone-based access check.

Multi-core platform specific ``tfm_hal_memory_check()`` can invoke ``tfm_has_access_to_region()`` to
implement the entire memory access check routine.

During the check process, ``tfm_has_access_to_region()`` compares the access permission with memory
region attributes and determines whether the access is allowed to access the region according to
policy described in `Memory Access Check Policy`_ above.

``tfm_has_access_to_region()`` invokes 3 HAL APIs to retrieve attributes of target memory region.

- ``tfm_hal_get_mem_security_attr()`` retrieves the security attributes of the target memory region.
- ``tfm_hal_get_secure_access_attr()`` retrieves secure access attributes of the target memory
  region.
- ``tfm_hal_get_ns_access_attr()`` retrieves non-secure access attributes of the target memory
  region.

All three functions are implemented by multi-core platform support. The definitions are specified in
the section `HAL APIs`_ below.

The pseudo code of ``tfm_has_access_to_region()`` is shown below.

.. code-block:: c
    :linenos:
    :emphasize-lines: 19,36,46

    enum tfm_status_e tfm_has_access_to_region(const void *p, size_t s, uint8_t flags)
    {
        struct security_attr_info_t security_attr;
        struct mem_attr_info_t mem_attr;

        /* Validate input parameters */
        if (Validation failed) {
            return error;
        }

        /* The memory access check should be executed inside TF-M PSA RoT */
        if (Not in privileged level) {
            abort;
        }

        /* Set initial value */
        security_attr_init(&security_attr);

        /* Retrieve security attributes of memory region */
        tfm_hal_get_mem_security_attr(p, s, &security_attr);

        /* Compare according to current Isolation Level */
        if (Input flags mismatch security attributes) {
            return error;
        }

        /* Set initial value */
        mem_attr_init(&mem_attr);

        if (The target memory region is in secure memory space) {
            /* Retrieve access attributes of secure memory region */
            tfm_hal_get_secure_access_attr(p, s, &mem_attr);

            if (Not in Isolation Level 1) {
                /* Secure memory protection unit(s) must be enabled in Isolation Level 2 and 3 */
                if (Protection unit not enabled) {
                    abort;
                }
            }
        } else {
            /* Retrieve access attributes of non-secure memory region. */
            tfm_hal_get_ns_access_attr(p, s, &mem_attr);
        }

        /* Compare according to current Isolation Level and non-secure/secure access. */
        if (Input flags match memory attributes) {
            return success;
        }

        return error;
    }

.. note::
   It cannot be guaranteed that TF-M provides a comprehensive memory access
   check on non-secure memory for NSPE client call. If non-secure memory
   protection or isolation is required in a multi-core system, NSPE software
   should implement and execute the check functionalities in NSPE, rather than
   relying on TF-M access check.

   For example, all the access from NSPE client calls to non-secure memory are
   classified as unprivileged in current TF-M implementation. Multi-core access
   check may skip the privileged/unprivileged permission check for non-secure
   access.

   If a multi-core system enforces the privileged/unprivileged isolation and
   protection of non-secure area, NSPE software should execute the corresponding
   check functionalities before submitting the NSPE client call request to SPE.

*******************
Data Types and APIs
*******************

Data Types
==========

Access Permission Flags
-----------------------

The following flags are defined to indicate the access permission attributes.
Each flag is mapped to the corresponding CMSE macro. Please refer to
`ARMv8-M Security Extensions: Requirements on Development Tools
<http://infocenter.arm.com/help/topic/com.arm.doc.ecm0359818/ECM0359818_armv8m_security_extensions_reqs_on_dev_tools_1_0.pdf>`_
for details of each CMSE macro.

``MEM_CHECK_MPU_READWRITE``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Mapped to CMSE macro ``CMSE_MPU_READWRITE`` to indicate that the access requires
both read and write permission to the target memory region.

.. code-block:: c

    #define MEM_CHECK_MPU_READWRITE   (1 << 0x0)


``MEM_CHECK_MPU_UNPRIV``
^^^^^^^^^^^^^^^^^^^^^^^^

Mapped to CMSE macro ``CMSE_MPU_UNPRIV`` to indicate that it is an unprivileged
access.

.. code-block:: c

    #define MEM_CHECK_MPU_UNPRIV      (1 << 0x2)


``MEM_CHECK_MPU_READ``
^^^^^^^^^^^^^^^^^^^^^^

Mapped to CMSE macro ``CMSE_MPU_READ``. It indicates that it is a read-only
access to target memory region.

.. code-block:: c

    #define MEM_CHECK_MPU_READ        (1 << 0x3)


``MEM_CHECK_NONSECURE``
^^^^^^^^^^^^^^^^^^^^^^^

Mapped to CSME macro ``CMSE_NONSECURE`` to indicate that it is a access from
non-secure client call request.
If this flag is unset, it indicates the access is required from SPE.

.. code-block:: c

    #define MEM_CHECK_AU_NONSECURE    (1 << 0x1)
    #define MEM_CHECK_MPU_NONSECURE   (1 << 0x4)
    #define MEM_CHECK_NONSECURE       (MEM_CHECK_AU_NONSECURE | \
                                       MEM_CHECK_MPU_NONSECURE)

Security Attributes Information
-------------------------------

The structure ``security_attr_info_t`` contains the security attributes
information of the target memory region.
``tfm_hal_get_mem_security_attr()`` implementation should fill the structure
fields according to the platform specific secure isolation setting.

.. code-block:: c

    struct security_attr_info_t {
        bool is_valid;
        bool is_secure;
    };

| ``is_valid`` indicates whether the target memory region is valid according to
  platform resource assignment and security isolation configurations.
| ``is_secure`` indicates the target memory region is secure or non-secure. The
  value is only valid when ``is_valid`` is ``true``.

Memory Attributes Information
-----------------------------

The structure ``mem_attr_info_t`` contains the memory access attributes
information of the target memory region.
``tfm_hal_get_secure_access_attr()`` and ``tfm_hal_get_ns_access_attr()`` implementations should
fill the structure fields according to the memory protection settings.

.. code-block:: c

    struct mem_attr_info_t {
        bool is_mpu_enabled;
        bool is_valid;
        bool is_xn;
        bool is_priv_rd_allow;
        bool is_priv_wr_allow;
        bool is_unpriv_rd_allow;
        bool is_unpriv_wr_allow;
    };

| ``is_mpu_enabled`` indicates whether the MPU and other management unit are
  enabled and work normally.
| ``is_valid`` indicates whether the target memory region is valid according to
  platform resource assignment and memory protection configurations.
| ``is_xn`` indicates whether the target memory region is Execute Never. This
  field is only valid when ``is_valid`` is ``true``.
| ``is_priv_rd_allow`` and ``is_priv_wr_allow`` indicates whether the target
  memory region allows privileged read/write. Both the fields are valid only
  when  ``is_valid`` is ``true``.
| ``is_unpriv_rd_allow`` and ``is_unpriv_wr_allow`` indicates whether the target
  memory region allows unprivileged read/write. Both the fields are valid only
  when  ``is_valid`` is ``true``.

HAL APIs
========

``tfm_hal_get_mem_security_attr()``
-----------------------------------

``tfm_hal_get_mem_security_attr()`` retrieves the current active security configuration information
and fills the ``security_attr_info_t``.

.. code-block:: c

    void tfm_hal_get_mem_security_attr(const void *p, size_t s,
                                       struct security_attr_info_t *p_attr);

+--------------------------------------------------------------------+
| **Parameters**                                                     |
+-------------+------------------------------------------------------+
| ``p``       | Base address of the target memory region             |
+-------------+------------------------------------------------------+
| ``s``       | Size of the target memory region                     |
+-------------+------------------------------------------------------+
| ``p_attr``  | Pointer to the ``security_attr_info_t`` to be filled |
+-------------+------------------------------------------------------+
| **Return**                                                         |
+-------------+------------------------------------------------------+
| ``void``    | None                                                 |
+-------------+------------------------------------------------------+

The implementation should be decoupled from TF-M current isolation level or
access check policy.

All the fields in ``security_attr_info_t`` shall be explicitly set in
``tfm_hal_get_mem_security_attr()``.

If the target memory region crosses boundaries of different security regions or levels in security
isolation configuration, ``tfm_hal_get_mem_security_attr()`` should determine whether the memory
region violates current security isolation.
It is recommended to mark the target memory region as invalid in such case, even if the adjoining
regions or levels have the same security configuration.

If the target memory region is not explicitly specified in memory security configuration,
``tfm_hal_get_mem_security_attr()`` can return the following values according to actual use case:

- Either set ``is_valid = false``
- Or set ``is_valid = true`` and set ``is_secure`` according to platform specific policy.

``tfm_hal_get_secure_access_attr()``
----------------------------------------

``tfm_hal_get_secure_access_attr()`` retrieves the secure memory protection configuration
information and fills the ``mem_attr_info_t``.

.. code-block:: c

    void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr);

+--------------------------------------------------------------+
| **Parameters**                                               |
+------------+-------------------------------------------------+
| ``p``      | Base address of the target memory region        |
+------------+-------------------------------------------------+
| ``s``      | Size of the target memory region                |
+------------+-------------------------------------------------+
| ``p_attr`` | Pointer to the ``mem_attr_info_t`` to be filled |
+------------+-------------------------------------------------+
| **Return**                                                   |
+------------+-------------------------------------------------+
| ``void``   | None                                            |
+------------+-------------------------------------------------+

The implementation should be decoupled from TF-M current isolation level or
access check policy.

All the fields in ``mem_attr_info_t`` shall be explicitly set in
``tfm_hal_get_secure_access_attr()``, according to current active memory protection configuration.
It is recommended to retrieve the attributes from secure MPU and other hardware memory protection
unit(s). The implementation can also be simplified by checking static system-level memory layout.

If the target memory region is not specified in current active secure memory protection
configuration, ``tfm_hal_get_secure_access_attr()`` can select the following values according to
actual use case.

- Either directly set ``is_valid`` to ``false``
- Or set ``is_valid`` to ``true`` and set other fields according to other memory assignment
  information, such as static system-level memory layout.

If secure memory protection unit(s) is *disabled* and the target memory region is a valid area
according to platform resource assignment, ``tfm_hal_get_secure_access_attr()`` must set
``is_mpu_enabled`` to ``false`` and set other fields according to current system-level memory
layout.

``tfm_hal_get_ns_access_attr()``
--------------------------------

``tfm_hal_get_ns_access_attr()`` retrieves the non-secure memory protection configuration
information and fills the ``mem_attr_info_t``.

.. code-block:: c

    void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr);

+--------------------------------------------------------------+
| **Parameters**                                               |
+------------+-------------------------------------------------+
| ``p``      | Base address of the target memory region        |
+------------+-------------------------------------------------+
| ``s``      | Size of the target memory region                |
+------------+-------------------------------------------------+
| ``p_attr`` | Pointer to the ``mem_attr_info_t`` to be filled |
+------------+-------------------------------------------------+
| **Return**                                                   |
+------------+-------------------------------------------------+
| ``void``   | None                                            |
+------------+-------------------------------------------------+

The implementation should be decoupled from TF-M current isolation level or
access check policy.

Since non-secure core runs asynchronously, the non-secure MPU setting may be modified by NSPE OS and
therefore the attributes of the target memory region can be unavailable during
``tfm_hal_get_ns_access_attr()`` execution in TF-M.
When the target memory region is not specified in non-secure MPU, ``tfm_hal_get_ns_access_attr()``
can set the fields according to other memory setting information, such as static system-level memory
layout.

If non-secure memory protection unit(s) is *disabled* and the target memory region is a valid area
according to platform resource assignment, ``tfm_hal_get_ns_access_attr()`` can set the following
fields in ``mem_attr_info_t`` to default values:

- ``is_mpu_enabled = false``
- ``is_valid = true``
- ``is_xn = true``
- ``is_priv_rd_allow = true``
- ``is_unpriv_rd_allow = true``

``is_priv_wr_allow`` and ``is_unpriv_wr_allow`` can be set according to current system-level memory
layout, such as whether it is in code section or data section.

General retrieval functions
===========================

TF-M implements 3 general retrieval functions to retrieve memory region security attributes or
memory protection configurations, based on static system-level memory layout. Platform specific HAL
functions can invoke those 3 general functions to simplify implementations.

- ``tfm_get_mem_region_security_attr()`` retrieves general security attributes from static
  system-level memory layout.
- ``tfm_get_secure_mem_region_attr()`` retrieves general secure memory protection configurations
  from static system-level memory layout.
- ``tfm_get_ns_mem_region_attr()`` retrieves general non-secure memory protection configurations
  from static system-level memory layout.

If a multi-core platform's memory layout may vary in runtime, it shall not rely on these 3 functions
to retrieve static configurations.
These 3 functions run through memory layout table to check against each memory section one by one,
with pure software implementation. It might cost more time compared to hardware-based memory access
check.

``tfm_get_mem_region_security_attr()``
--------------------------------------

``tfm_get_mem_region_security_attr()`` retrieves security attributes of target memory region
according to the static system-level memory layout and fills the ``security_attr_info_t``.

.. code-block:: c

    void tfm_get_mem_region_security_attr(const void *p, size_t s,
                                          struct security_attr_info_t *p_attr);

+--------------------------------------------------------------------+
| **Parameters**                                                     |
+-------------+------------------------------------------------------+
| ``p``       | Base address of the target memory region             |
+-------------+------------------------------------------------------+
| ``s``       | Size of the target memory region                     |
+-------------+------------------------------------------------------+
| ``p_attr``  | Pointer to the ``security_attr_info_t`` to be filled |
+-------------+------------------------------------------------------+
| **Return**                                                         |
+-------------+------------------------------------------------------+
| ``void``    | None                                                 |
+-------------+------------------------------------------------------+

``tfm_get_secure_mem_region_attr()``
------------------------------------

``tfm_get_secure_mem_region_attr()`` retrieves general secure memory protection configuration
information of the target memory region according to the static system-level memory layout and fills
the ``mem_attr_info_t``.

.. code-block:: c

    void tfm_get_secure_mem_region_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr);

+--------------------------------------------------------------+
| **Parameters**                                               |
+------------+-------------------------------------------------+
| ``p``      | Base address of the target memory region        |
+------------+-------------------------------------------------+
| ``s``      | Size of the target memory region                |
+------------+-------------------------------------------------+
| ``p_attr`` | Pointer to the ``mem_attr_info_t`` to be filled |
+------------+-------------------------------------------------+
| **Return**                                                   |
+------------+-------------------------------------------------+
| ``void``   | None                                            |
+------------+-------------------------------------------------+

``tfm_get_ns_mem_region_attr()``
--------------------------------

``tfm_get_ns_mem_region_attr()`` retrieves general non-secure memory protection configuration
information of the target memory region according to the static system-level memory layout and fills
the ``mem_attr_info_t``.

.. code-block:: c

    void tfm_get_ns_mem_region_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr);

+--------------------------------------------------------------+
| **Parameters**                                               |
+------------+-------------------------------------------------+
| ``p``      | Base address of the target memory region        |
+------------+-------------------------------------------------+
| ``s``      | Size of the target memory region                |
+------------+-------------------------------------------------+
| ``p_attr`` | Pointer to the ``mem_attr_info_t`` to be filled |
+------------+-------------------------------------------------+
| **Return**                                                   |
+------------+-------------------------------------------------+
| ``void``   | None                                            |
+------------+-------------------------------------------------+

--------------

*Copyright (c) 2019-2023, Arm Limited. All rights reserved.*
