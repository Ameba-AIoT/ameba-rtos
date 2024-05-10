########################
RSS communication design
########################

The RSS communication protocol is designed to be a lightweight serialization of
the psa_call() API through a combination of in-band MHU (Message Handling Unit)
transport and parameter-passing through shared memory.

**************
Message format
**************

To call an RSS service, the client must send a message in-band over the MHU
sender link to RSS and wait for a reply message on the MHU receiver (either by
polling the MHU or waiting for interrupt). The messages are defined as packed C
structs, which are serialized in byte-order over the MHU links.

Messages encoding a psa_call() to RSS take the form::

    __PACKED_STRUCT serialized_psa_msg_t {
        struct serialized_rss_comms_header_t header;
        __PACKED_UNION {
            struct rss_embed_msg_t embed;
            struct rss_pointer_access_msg_t pointer_access;
        } msg;
    };

Replies from RSS take the form::

    __PACKED_STRUCT serialized_psa_reply_t {
        struct serialized_rss_comms_header_t header;
        __PACKED_UNION {
            struct rss_embed_reply_t embed;
            struct rss_pointer_access_reply_t pointer_access;
        } reply;
    };

All messages (calls and replies), in all protocols carry the following header::

    __PACKED_STRUCT serialized_rss_comms_header_t {
        uint8_t protocol_ver;
        uint8_t seq_num;
        uint16_t client_id;
    };

The ``client_id`` can be used by the caller to identify different clients at the
endpoint for access control purposes. It is combined with an RSS-internal
identifier for the endpoint to create the PSA Client ID for the call.

The sequence number, ``seq_num``, is returned in the reply message to allow the
client to identify which message is being responded to, since replies may be
out-of-order. The sender is free to assign sequence numbers using a scheme of
its choice, and it is the sender's responsibility to ensure that two messages
with the same sequence number are not in simultaneous flight.

The ``protocol_ver`` identifies which message protocol is being used. There are
two protocols currently supported, "embed" (``protocol_ver=0``) and "pointer
access" (``protocol_ver=1``).

Embed protocol
==============

The embed protocol embeds the psa_call iovecs into the message sent over the
MHU. It has the following format::

    __PACKED_STRUCT rss_embed_msg_t {
        psa_handle_t handle;
        uint32_t ctrl_param;
        uint16_t io_size[PSA_MAX_IOVEC];
        uint8_t payload[RSS_COMMS_PAYLOAD_MAX_SIZE];
    };

The ``handle`` is the psa_call handle parameter and the ``ctrl_param`` packs the
type, in_len and out_len parameters of psa_call into one parameter.

The ``io_size`` array contains the sizes of the up to ``PSA_MAX_IOVEC`` (4)
iovecs, in order, with the invec sizes before the outvec sizes.

The ``payload`` array then contains the invec data packed contiguously in order.
The length of this parameter is variable, equal to the sum of the invec lengths
in io_size. The caller does not need to pad the payload to the maximum size. The
maximum payload size for this protocol, ``RSS_COMMS_PAYLOAD_MAX_SIZE``, is a
build-time option.

Replies in the embed protocol take the form::

    __PACKED_STRUCT rss_embed_reply_t {
        int32_t return_val;
        uint16_t out_size[PSA_MAX_IOVEC];
        uint8_t payload[RSS_COMMS_PAYLOAD_MAX_SIZE];
    };

The ``return_val`` is the return value of the psa_call() invocation, the
``out_size`` is the (potentially updated) sizes of the outvecs and the
``payload`` buffer contains the outvec data serialized contiguously in outvec
order.

Pointer access protocol
=======================

The pointer access protocol passes the psa_call iovecs as pointers to shared
memory using the following MHU message format::

    __PACKED_STRUCT rss_pointer_access_msg_t {
        psa_handle_t handle;
        uint32_t ctrl_param;
        uint32_t io_sizes[PSA_MAX_IOVEC];
        uint64_t host_ptrs[PSA_MAX_IOVEC];
    };

The ``handle``, ``ctrl_param`` and ``io_sizes`` have the same definition as in
the embed protocol.

The ``host_ptrs`` point to each of the up to ``PSA_MAX_IOVEC`` iovec buffers in
host system memory. It is the caller's responsibility to write the invec data to
the invec pointers before sending the message, and ensure that the memory
pointed-to remains valid for the duration of the call.

The reply message has the form::

    __PACKED_STRUCT rss_pointer_access_reply_t {
        int32_t return_val;
        uint32_t out_size[PSA_MAX_IOVEC];
    };

The ``return_val`` and ``out_size`` have the same meaning as in the embed
protocol.

RSS writes the outvec data to the pointers supplied in the call message prior to
sending the MHU reply message, so no further payload is sent in the reply
message.

************************
Implementation structure
************************

The RSS side of the communication implementation is located in
``platform/ext/target/arm/rss/common/rss_comms``. The implementation is
structured as follows:

- ``rss_comms.c``: Implements the TF-M RPC layer using RSS comms implementation.
- ``rss_comms_hal.c``: Abstracts MHU message sending and receiving.

- ``rss_comms_protocol.c``: The common part of the RSS comms protocol.
- ``rss_comms_protocol_embed.c``: The embed RSS comms protocol.
- ``rss_comms_protocol_protocol_access.c``: The pointer access RSS comms protocol.

- ``rss_comms_atu.c``: Allocates and frees ATU regions for host pointer access.
- ``rss_comms_permissions_hal.c``: Checks service access permissions and pointer validity.

A reference implementation of the client side of the RSS comms is available in
the Trusted Firmware-A repository.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
