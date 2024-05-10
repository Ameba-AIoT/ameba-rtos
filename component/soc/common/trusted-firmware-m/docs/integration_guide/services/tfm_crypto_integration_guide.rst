################################
Crypto Service Integration Guide
################################

************
Introduction
************
The TF-M Crypto service allows Non Secure world applications and Secure
services to use cryptographic functionalities such as symmetric ciphering,
message signing and verification, asymmetric encryption and decryption,
cryptographic hashes, message authentication codes (MACs), key derivation and
agreement, authenticated encryption with associated data (AEAD). It exposes the
PSA Crypto APIs [1]_.

.. _components-label:

The secure service resides in the Crypto partition as a single entry point and
is made of different components:

 - An interface layer that exposes the PSA Crypto API to either NS or S entities
   and is implemented in ``interface/src/tfm_crypto_api.c``. The interface is
   based on the Uniform Secure Service Signature and communicates with the
   Secure Partition Manager available in TF-M
 - An init module ``secure_fw/partitions/crypto/crypto_init.c`` that implements
   functionalities requested by TF-M during the initialisation phase, and an API
   dispatcher that at runtime receives the requests from the interface and
   dispatches them to the component that processes that particular API request
 - A set of components that process cryptographic API requests, each component
   dispatching to a subset of functionalities, i.e. AEAD, Asymmetric, Ciphering,
   Hashing, Key derivation, Key management, MACs, and Random Number Generation
 - An alloc module ``secure_fw/partitions/crypto/crypto_alloc.c`` that manages
   the partition secure memory, storing multipart application contexts, input /
   outputs of the APIs being requested, inaccessible from NS or other secure
   partitions
 - A library abstraction module ``secure_fw/partitions/crypto/crypto_library.c``
   which is used to abstract the details of the cryptographic library used by
   the service *backend* to provide the actual implementation of the crypto
   functionalities. The backend library must expose the PSA Crypto APIs, and
   must provide support to encode *key ownership* into key identifiers. This is
   not standardized by the PSA Crypto APIs so it must be provided as an
   extension to the APIs. The backend library also needs to provide the
   subsystem for key storage and retrieval, and, in case, the interface to the
   cryptographic accelerator of the underlying platform, using the PSA
   cryptoprocessor driver interface specification [2]_. For this reason, it must
   provide a mechanism to access platform *builtin* keys, and permanent key
   slots using the
   :ref:`TF-M Internal Trusted Storage (ITS) service <its-introduction-label>`,
   if available.

**************
Code structure
**************
The PSA interfaces for the Crypto service are located in ``interface/include``.
The only header to be included by applications that want to use functions from
the PSA API is ``psa/crypto.h``. The TF-M Crypto service source files are
located in ``secure_fw/partitions/crypto``.

PSA interfaces
==============
The TF-M Crypto service exposes the PSA interfaces detailed in the header
``psa/crypto.h``. This header, in turn, includes several other headers which
are not meant to be included directly by user applications. For a detailed
description of the PSA API interface, please refer to the comments in the
``psa/crypto.h`` header itself.

Service source files
====================
A brief description of what is implemented by each source file is as below:

 - ``crypto_cipher.c`` : Dispatcher for symmetric crypto operations
 - ``crypto_hash.c`` : Dispatcher for hash operations
 - ``crypto_mac.c`` : Dispatcher for MAC operations
 - ``crypto_aead.c`` : dispatcher for AEAD operations
 - ``crypto_key_derivation.c`` : Dispatcher for key derivation and key agreement
   operations
 - ``crypto_key_management.c`` : Dispatcher for key management operations
   towards the key slot management system provided by the backend library
 - ``crypto_rng.c`` : Dispatcher for the random number generation requests
 - ``crypto_asymmetric.c`` : Dispatcher for message signature/verification and
   encryption/decryption using asymmetric crypto
 - ``crypto_init.c`` : Init module for the service. The modules stores also the
   internal buffer used to allocate temporarily the IOVECs needed, which is not
   required in case of SFN model. The size of this buffer is controlled by the
   ``CRYPTO_IOVEC_BUFFER_SIZE`` config define
 - ``crypto_library.c`` : Library abstractions to interface the dispatchers
   towards the underlying library providing *backend* crypto functions.
   Currently this only supports the mbed TLS library. In particular, the mbed
   TLS library requires to provide a static buffer to be used as heap for its
   internal allocation. The size of this buffer is controlled by the
   ``CRYPTO_ENGINE_BUF_SIZE`` config define
 - ``crypto_alloc.c`` : Takes care of storing multipart operation contexts in a
   secure memory not visible outside of the crypto service. The
   ``CRYPTO_CONC_OPER_NUM`` config define determines how many concurrent
   contexts are supported at once. In a multipart operation, the client view of
   the contexts is much simpler (i.e. just an handle), and the Alloc module
   keeps track of the association between handles and contexts
 - ``tfm_crypto_api.c`` :  This module is contained in ``interface/src`` and
   implements the PSA Crypto API client interface exposed to both S/NS clients.
   This module allows a configuration option ``CONFIG_TFM_CRYPTO_API_RENAME``
   to be set to 1 in case the NS environment or integrators want to rename the
   API symbols exported by the TF-M Crypto service. The renaming adds a default
   prefix, ``tfm_crypto__`` to all functions. The prefix can be changed editing
   the interface file. This config option is for the NS environment or
   integration setup only, hence it is not accessible through the TF-M config
 - ``tfm_mbedcrypto_alt.c`` : This module is specific to the mbed TLS [3]_
   library integration and provides some alternative implementation of mbed TLS
   APIs that can be used when a optimised profile is chosen. Through the
   ``_ALT`` mechanism it is possible to replace at link time default
   implementations available in mbed TLS with the ones available in this file

   .. Note::
     The ``_ALT`` mechanism will be deprecated in future releases of the mbed
     TLS library

***************************************
Considerations on service configuration
***************************************

Crypto *backend* library configuration
======================================
The TF-M Crypto service relies on a cryptographic library to provide the
functionalities specific by the PSA Crypto API spec and the PSA cryptoprocessor
driver interface spec. At the moment, the only supported library is mbed
TLS [3]_.

The configuration of the backend library is supplied using the
``TFM_MBEDCRYPTO_CONFIG_PATH`` and ``TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH``
config option that point to configuration headers following the legacy mbed TLS
configuration scheme or the new PSA based configuration scheme.

Platforms can specify an extra config file by setting the
``TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH`` variable (which is a wrapper
around the ``MBEDTLS_USER_CONFIG_FILE`` option).  This is preferred for platform
configuration over ``TFM_MBEDCRYPTO_CONFIG_PATH`` and
``TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH`` as it does not interfere with
config changes due to TFM Profile.

.. Note::

    The default entropy source configured for mbed TLS is
    ``MBEDTLS_ENTROPY_NV_SEED`` with a unique seed. For production devices, an
    alternative hardware entropy source can be specified using the config option
    ``MBEDTLS_ENTROPY_HARDWARE_ALT``

.. Note::
    Starting from mbed TLS 3.3.0, the Python package ``jsonschema`` must be
    available when building as it is required by the autogen framework for the
    driver integrations into the PSA Crypto core and driver wrapper modules

Crypto service build time options
=================================

  - ``CRYPTO_STACK_SIZE`` : Defines the stack size of the Crypto Secure
    Partition. This value might depend on several parameters such as the build
    type, the compiler being used, the cryptographic functionalities that are
    enabled at build time
  - ``CRYPTO_<COMPONENT>_MODULE_ENABLED`` : A series of defines, one per each
    ``<COMPONENT>`` that processes cryptographic operations, that are used to
    disable modules at build time. Each define corresponds to a component as
    described in :ref:`the components list <components-label>`.


Crypto service *builtin* keys integration
=========================================
A detailed description of how the service interacts with *builtin* keys is
available in the ``tfm_builtin_key_loader``
:ref:`design document <tfm-builtin-keys-label>`.

.. Note::

    The crypto service integration with builtin keys relies on implementation
    details of mbed TLS that are not standardized in the spec and might change
    between releases due to ongoing work [4]_


References
----------

.. [1] PSA Crypto APIs: \ https://armmbed.github.io/mbed-crypto/html/
.. [2] PSA cryptoprocessor driver interface: \ https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-interface.md
.. [3] mbed TLS library: \ https://www.trustedfirmware.org/projects/mbed-tls/
.. [4] Interface for platform keys: \ https://github.com/ARM-software/psa-crypto-api/issues/550


--------------

*Copyright (c) 2018-2023, Arm Limited. All rights reserved.*
