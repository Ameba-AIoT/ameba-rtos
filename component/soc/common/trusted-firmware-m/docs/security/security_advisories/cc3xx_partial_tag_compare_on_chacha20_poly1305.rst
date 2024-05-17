Advisory TFMV-6
===============

+-----------------+------------------------------------------------------------+
| Title           | Partial tag comparison when using Chacha20-Poly1305 on the |
|                 | PSA driver API interface in CryptoCell enabled platforms   |
+=================+============================================================+
| CVE ID          | CVE-2023-40271                                             |
+-----------------+------------------------------------------------------------+
| Public          | 04/09/2023                                                 |
| Disclosure Date |                                                            |
+-----------------+------------------------------------------------------------+
| Versions        | TF-M v1.6.0, TF-M v1.6.1, TF-M v1.7.0, TF-M v1.8.0         |
| Affected        |                                                            |
+-----------------+------------------------------------------------------------+
| Configurations  | CC312 enabled platforms, where the legacy driver API is    |
|                 | disabled (``CC312_LEGACY_DRIVER_API_ENABLED=OFF``) and the |
|                 | single part AEAD APIs are implemented through a dedicated  |
|                 | function and not by leveraging the multipart functions     |
|                 | (``CC3XX_CONFIG_ENABLE_AEAD_ONE_SHOT_USE_MULTIPART`` not   |
|                 | set)                                                       |
+-----------------+------------------------------------------------------------+
| Impact          | It might allow for unauthenticated payloads to be deemed   |
|                 | as authentic by comparing only the first 4 bytes of the    |
|                 | authentication tag instead of the full length of 16 bytes  |
+-----------------+------------------------------------------------------------+
| Fix Version     | 2e82124af, TF-M v1.8.1                                     |
+-----------------+------------------------------------------------------------+
| Credit          | Nordic Semiconductor                                       |
+-----------------+------------------------------------------------------------+

Background
----------

AEAD algorithms
^^^^^^^^^^^^^^^

Authenticated Encryption with Associated Data (AEAD) is a common ciphering
method where the data to be encrypted is also authenticated as part of the
process by creating an authentication tag. When the encrypted data is then
decrypted, the authentication tag is verified and if it does not match the
expected value, then the entire operation fails. In this way, the operation
allows for Authenticity in addition to the confidentiality granted by the
encryption process. Using PSA Crypto APIs, it's possible to use several of
such algorithms, such as AES in Galois-Counter Mode (GCM), AES in Counter with
CBC-Mac Mode (CCM) or Chacha20-Poly1305, which is a combination of the Chacha20
cipher with the Poly1305 authenticator [1]_.

In particular for Chacha20-Poly1305 the corresponding macro defining the
algorithm in PSA Cryptographic API specification is PSA_ALG_CHACHA20_POLY1305.

Single part vs multipart API functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

PSA Crypto API specification [2]_ allows the usage of AEAD algorithms through
several possible APIs, that can be grouped generally in the single part, or
integrated, operation type, and in the multipart operation type. The main
difference is that for single part operations, the whole encryption and tag
production (and on the other hand, the whole decryption and tag authentication)
happen with a single API call. This type of approach is simpler but at the same
time less flexible than the multipart approach where an operation context must
be allocated by the application and the encryption/decryption processes require
the call to different APIs to setup the operation context, provide inputs to the
process and calculate the output. For example, the integrated APIs defined by
the PSA Crypto API spec are called ``psa_aead_encrypt()`` and ``psa_aead_decrypt``.

It is possible, to reduce the code size of an implementation, to implement the
single part APIs by calling the underlying multipart functions, effectively
encapsulating the multipart flow in the single part APIs.

PSA Unified Driver API for Cryptoprocessors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The PSA Unified Driver API for Cryptoprocessors spec [3]_ allows a PSA compliant
implementation to redirect some of the operations to the underlying hardware
accelerated platform. For example, CryptoCell is a cryptographic accelerator
available in some of the TF-M supported platforms that can accelerate crypto
operations, such as Chacha20-Poly1305. The driver code associated to it has
the responsibility of driving the hardware resources with inputs and collect
the outputs. When decrypting, the driver must compare the reconstructed
authentication tag with the expected value, and return failure in case of
mismatch during verification. For Chacha20-Poly1305, the tag size is 16 bytes.

For CryptoCell enabled platforms, the software component implementing the PSA
Unified Driver interface is located into the ``psa_driver_api`` directory in
``lib/ext/cryptocell-312-runtime/codesafe/src/`` in TF-M's codebase. This
components can be configured to have single part APIs implemented though the
corresponding multipart functions by setting the following define at build
time: ``CC3XX_CONFIG_ENABLE_AEAD_ONE_SHOT_USE_MULTIPART``.

By default, CryptoCell enabled platforms don't build the PSA Unified Driver
API interface layer but rely on the legacy interface. To enable the PSA Driver
interface the following TF-M build option during CMake config must be be set
to OFF: ``CC312_LEGACY_DRIVER_API_ENABLED=OFF``.

Impact
------

When the PSA Driver API interface is used and is not configured to rely on the
corresponding multipart functions, when performing the verification of the
authentication tag at the end of the authenticated decryption process, the
buffer containing the tag is only partially verified (the first 4 bytes only
instead of the full 16 bytes). This allows for the possibility of unauthenticated
data to be recognized as authentic. An attacker could theoretically construct
a malicious payload to actively exploit such partial verification.


Impacted PSA Crypto API functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following PSA single part crypto operation function is impacted:

- ``psa_aead_decrypt``


Mitigation
----------

The verification of the authentication tag must happen on the full 16 bytes of
instead of just the first 4 bytes. This means that loop that currently performs
such verification in the ``cc3xx_decrypt_chacha20_poly1305()`` function must be
changed from this:

.. code-block:: c

    /* Check tag in "constant-time" */
    for (diff = 0, i = 0; i < sizeof(tag_length); i++)
        diff |= tag[i] ^ local_tag_buffer[i];

to this:

.. code-block:: c

    /* Check tag in "constant-time" */
    for (diff = 0, i = 0; i < tag_length; i++)
        diff |= tag[i] ^ local_tag_buffer[i];


References
----------

.. [1] Chacha20 and Poly1305 for IETF Protocols: \ https://datatracker.ietf.org/doc/html/rfc7539
.. [2] PSA Cryptographic API v1.1: \ https://armmbed.github.io/mbed-crypto/html/
.. [3] PSA Unified Driver interface: \ https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-interface.md

---------------------

*Copyright (c) 2023, Arm Limited. All rights reserved.*
