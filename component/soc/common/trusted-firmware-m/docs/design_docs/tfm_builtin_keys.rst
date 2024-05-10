#################
TF-M builtin keys
#################

:Author: Raef Coles
:Organization: Arm Limited
:Contact: raef.coles@arm.com

.. _tfm-builtin-keys-label:

************
Introduction
************

When TF-M is integrated on a platform, the platform itself can provide several
keys for usage which are bound to the device instead of being owned by a
specific secure partition. When those keys are readable by the secure
processing environment, the platform must provide a function to load these keys
in the HAL layer, either loading them from OTP or another platform specific
location or subsystem. These keys are henceforth referred to as "builtin keys",
and might include (but are not limited to) the following:

  1. The Hardware Unique Key (HUK)
  2. The Initial Attestation Key (IAK)

The ``tfm_builtin_key_loader`` component implements a mechanism to discover
those keys and make them available to the PSA Crypto APIs for usage. Note that
if a key is stored in a subsystem which can't be read by the secure
processing environment, a full opaque driver [1]_ must be used to be able to use
those keys through the PSA Crypto APIs. This document focuses only on the case
where the keys can be read by the SPE (i.e. running TF-M), so the driver applies
only on those cases, which can be considered as "_transparent_ builin keys".

In TF-M's legacy solution, the IAK is loaded by the attestation service as a
volatile key, which requires some key-loading logic to be implemented by that
partition. The HUK is not loaded in the crypto service, and is instead used by
an implementation of a TF-M specific KDF algorithm which then loads the key and
invokes Mbed TLS directly. Both solutions are far from ideal as they require an
effort to load (and duplicate) the keys per each user, or require a dedicated
code in TF-M that directly interacts with the HAL layer and invokes functions
from the crypto library bypassing the PSA Crypto interface. The aim of the
``tfm_builtin_key_loader`` driver is to provide a uniform interface to use the
builtin keys available in a platform.

Implementing a driver to deal with builtin keys allows to expand the legacy
solution for dealing with this type of keys in several ways. For example, it
avoids the need for partitions to implement dedicated mechanisms for probing the
HAL layer for builtin keys and load them as volatile. It removes the need to
have implementation specific call flows for deriving keys from the HUK (as that
requirement now is fulfilled by the driver itself transparently). It allows
uniform access to the keys also from the NS world (in any case, subject to the
policy dictated by the platform). Correctly abstracts away details of the key
handling mechanism from TF-M partitions into the PSA Crypto core key management
subsystem.

****************
PSA builtin keys
****************

The PSA Cryptographic API provides a mechanism for accessing keys that are
stored in platform-specific locations (often hardware accelerators or OTP). A
builtin key is assigned a specific key_id, i.e. a handle, which is hardcoded at
build time and must be selected in the range [MBEDTLS_PSA_KEY_ID_BUILTIN_MIN,
MBEDLTS_PSA_KEY_ID_BUILTIN_MAX]. A user of the PSA Crypto API can reference
those keys directly by using these handles. It is up to the platform to specify
policy restrictions for specific users of the keys, e.g. an NS entity, or a
secure partition. The PSA Crypto core will then check those policies to grant
or deny access to the builtin key for that user.

******************************
PSA cryptoprocessor driver API
******************************

The PSA specification allows the PSA Crypto APIs to defer their operation to an
accelerator driver, through a mechanism described in the PSA cryptoprocessor
driver interface specification [1]_. This specification defines the concept of
PSA Crypto core, i.e. "An implementation of the PSA Cryptography API is
composed of a core and zero or more drivers". The PSA specification also has
the concept of storage locations for keys, through the type
``psa_key_location_t``, which is used to access keys that don't have local
storage [2]_. This is leveraged mainly by opaque drivers mainly that use keys
for which the key material is not readable by the PSA crypto core layer.

TF-M defines a software driver called ``tfm_builtin_key_loader`` that provides
no acceleration but just defines a dedicated key location defined through the
``TFM_BUILTIN_KEY_LOADER_KEY_LOCATION`` define. By resorting to the entry points
provided by this driver, the PSA Crypto core slot management subsystem can
access keys stored in the underlying platform, validate key usage policies and
allow PSA Crypto APIs to uniformly access builtin keys using the same call flows
used with traditional local-storage keys.

This is implemented by hooking the entry points defined by the driver into the
``library/psa_crypto_driver_wrappers.c`` file provided by the PSA Crypto core.
This is currently done manually but eventually could be just autogenerated by
parsing a description of the driver entry points in the JSON format. These entry
points are:

  1. ``tfm_builtin_key_loader_init``
  2. ``tfm_builtin_key_loader_get_key_buffer_size``
  3. ``tfm_builtin_key_loader_get_builtin_key``

The call flow for the entry points from the driver wrapper layer is as follows:

  1. During the driver initialisation phase started by the PSA Crypto Core init,
     the ``tfm_builtin_key_loader_init`` function is called to probe the
     platform HAL and retrieve the builtin keys. Those keys are described by
     the types defined in the HAL layer interface header
     ``tfm_plat_crypto_keys.h``. In particular global tables describing key
     properties and user policies must be implemented by each platform and are
     retrieved by the two accessor functions
     ``tfm_plat_builtin_key_get_desc_table_ptr()`` and
     ``tfm_plat_builtin_key_get_policy_table_ptr()``. The keys are loaded from
     the platform in secure RAM in the TF-M Crypto partition with associated
     metadata. It's worth to note that the keys are loaded through callback
     functions which the platform lists in the key descriptor table.
  2. Once the TF-M Crypto service is initialised, at runtime it might receive a
     request through an API call to use one of the builtin key IDs. Those IDs
     are described in the ``tfm_builtin_key_ids.h`` header. Platforms can
     override the default values providing their own header.
  3. When the PSA Crypto core in the ``psa_get_and_lock_key_slot()`` function
     checks that the ``key_id`` being requested is in the builtin range region,
     it probes the platform through the function
     ``mbedtls_psa_platform_get_builtin_key`` which must be implemented by the
     TF-M Crypto service library abstraction layer. This function just checks
     the key descriptor table from the HAL to understand if such ``key_id`` is
     available in the platform and what are the corresponding ``slot`` and
     ``lifetime`` values associated to it. The lifetime contains the location of
     the key, which determines which driver is responsible for dealing with it
     (for the ``tfm_builtin_key_loader`` driver, that is
     ``TFM_BUILTIN_KEY_LOADER_KEY_LOCATION``), while the ``slot`` value maps the
     key to the slot in the driver internal storage.
  4. At this point, the PSA Crypto core knows that the key exists and is bound
     to the location associated to the driver. By calling into the driver
     wrappers layer is then able to retrieve the key attributes stored in the
     platform for that key ID, and the size required to allocate in its
     internal slot management system in order to load the key material in the
     core. This is done by calling ``tfm_builtin_key_loader_get_builtin_key``
     just with a valid key attributes pointer (and nothing else), to retrieve
     the attributes. Once the attributes are available, the required size is
     retrieved through the driver wrapper by calling
     ``tfm_builtin_key_loader_get_key_buffer_size``.
  5. At this stage, the slot management subsystem calls again into the driver
     wrapper layer through ``tfm_builtin_key_loader_get_builtin_key`` with a
     valid buffer to hold the key material returned by the
     ``tfm_builtin_key_loader`` driver. When loading the key, the user
     requiring that key_id is validated by the driver code against the policies
     defined by the platform. If the policies match, the builtin key material
     and metadata is loaded and is used like a transparent key available to the
     PSA Crypto core slot management subsystem.

*****************
Technical details
*****************

------------------------------
Builtin key IDs and overriding
------------------------------

TF-M builtin key IDs are defined in
``interface/include/crypto_keys/tfm_builtin_key_ids.h`` through the enum
``tfm_key_id_builtin_t``. They are allocated inside the range that PSA
specifies for the builtin keys, i.e. between ``MBEDTLS_PSA_KEY_ID_BUILTIN_MIN``
and ``MBEDLTS_PSA_KEY_ID_BUILTIN_MAX``. A platform can specify extra builtin key
IDs by setting the ``PLATFORM_DEFAULT_CRYPTO_KEYS`` variable to ``OFF``,
creating the header ``tfm_builtin_key_ids.h``, and specifying new keys and IDs.

--------------------------
Builtin key access control
--------------------------

A builtin key is accessible by all callers since the ``key_id`` associated to it
is public information. Access to the keys must be mediated, which is done by
matching the user requesting the ``key_id`` against the policies available for
that user on that particular key in the policy table. If no policies are
specified for a specific combination of user and ``key_id``, the usage flags in
the key attributes will be all set to zeros, meaning the key will be unusable
for any operation for that particular user.

------------------------------
Multi-partition key derivation
------------------------------

The HUK is used for key derivation by any secure partition or NS caller that
requires keys that are bound to a particular context. For example, Protected
Storage derives keys uniquely for each user of the service which are used to
encrypt user files. In order to provide HUK derivation to every secure
partition / NS caller, it must be ensured that no service that utilises HUK
derivation can derive the same key as another service (simply by using the same
inputs for the KDF APIs, i.e. accessing the same base key for derivation).

This is accomplished by deriving a further "platform key" for each builtin key
that has ``PSA_KEY_USAGE_DERIVE`` set in its attributes. These platform keys
are derived from the builtin key, using the partition ID as a KDF input, and
can then be used for safely for further derivations by the user, without risks
to derive the same keys as other users. This is enforced directly by the
``tfm_builtin_key_loader`` driver.

.. Note::
    If the NS client ID feature is disabled, all NS callers share a partition ID
    of ``-1``, and therefore will share a platform key and be therefore be able
    to derive the same keys as other NS callers.

For keys that are not exposed outside the device, this is transparent to the
service that is using the key derivation, as they have no access to the builtin
key material and cannot distinguish between keys derived directly from it and
keys derived from the platform key. For some builtin keys, deriving platform
keys is not acceptable, as the key is used outside the device (i.e. the IAK
public key is used to verify attestation tokens) so the actual builtin key is
used.

The decision has been taken to derive platform keys for any key that can be used
for key derivation (``PSA_KEY_USAGE_DERIVE``), and not derive platform keys
otherwise. For builtin keys that do not derive platform keys but are directly
used, care must be taken with access control where multiple partitions have
access to the same raw key material.

---------------------------------
Mbed TLS transparent builtin keys
---------------------------------

Mbed TLS does not natively support transparent builtin keys (transparent keys
are keys where the key material is directly accessible by the PSA Crypto core),
so some modifications had to be made. Opaque keyslots have the same basic
structure as standard transparent key slots, and can be passed to the functions
usually reserved for transparent keys, though this is a private implementation
detail of the Mbed TLS library and is not specified in the driver interface.
Therefore, the only modification required currently is to allow keys that have
the location ``TFM_BUILTIN_KEY_LOADER_KEY_LOCATION`` to be passed to the
functions that usually accept transparent keys only, i.e. with the location
``PSA_KEY_LOCATION_LOCAL_STORAGE``. This is due to the fact that the standard
assumption of the PSA Crypto core is that, if a driver that provides an
additional location, will also provide dedicated cryptographic mechanisms to act
on those keys, but this is not the case of the ``tfm_builtin_key_loader``, as it
just provides a mechanism to load keys (which act as a transparent key with
local storage, once loaded), but Mbed TLS does not support such "transparent
builtin key" concept. Note that the modifications on Mbed TLS are relying on non
standard implementation details hence this particular integration can change
between releases [3]_.

**********************************************
Using Opaque PSA crypto accelerators with TF-M
**********************************************

For platforms which have a cryptographic accelerator which has a corresponding
Opaque PSA crypto accelerator driver, the TF-M builtin key loader driver can be
disabled using the ``-DCRYPTO_TFM_BUILTIN_KEYS_DRIVER=OFF`` cmake option. The
platform can then redefine the HAL function
``tfm_plat_builtin_key_get_desc_table_ptr`` to point to a table where the
location and slot number of the keys corresponds instead to the opaque driver.
The PSA driver wrapper will then route the calls into the opaque driver, with no
other changes needed. If the description table is altered but the builtin key
loader driver is not disabled, it is possible to mix software builtin keys with
keys stored in opaque accelerators on a per-key level. Note that because the key
policy enforcement via ``tfm_plat_builtin_key_get_desc_table_ptr`` is currently
applied by the builtin key loader driver, other opaque drivers must apply either
this policy or their own policy (Though this may be changed in future).

**********
References
**********

.. [1] PSA cryptoprocessor driver interface: \ https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-interface.md
.. [2] Definition of psa_key_location_t type in the PSA spec: \ https://armmbed.github.io/mbed-crypto/html/api/keys/lifetimes.html#c.psa_key_location_t
.. [3] Interface for platform keys: \ https://github.com/ARM-software/psa-crypto-api/issues/550

--------------

*Copyright (c) 2022-2023, Arm Limited. All rights reserved.*
