/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _DELEGATED_PROV_H_
#define _DELEGATED_PROV_H_

#include "mesh_api.h"

BEGIN_DECLS

/**
 * @addtogroup DELEGATED_PROV
 * @brief Delegated Provisioning - let provisioned nodes provision devices in
 *        parallel on behalf of the Provisioning Manager.
 *
 * The Provisioning Manager tells a Provisioning Delegate "you may provision, here
 * is your address range". The Delegate runs the standard provisioning flow on its
 * own, then reports back what it provisioned so the Manager ends up holding every
 * device key.
 *
 * The provisioning handshake itself is unchanged. A Delegate already holds four of
 * the five Provisioning Data fields (NetKey, NetKey Index, Flags, IV Index) simply
 * by being on the network - only the unicast address has to be granted, which is why
 * GRANT carries nothing else. The stack builds the Provisioning Data PDU from
 * mesh_node.net_key_list / mesh_node.iv_index automatically.
 *
 * Roles
 * -----
 * Provisioning Manager    phone / gateway / PC tool. Runs the Client. Owns the
 *                         address pool and the device key database. Provisioned the
 *                         Delegate itself, so it already holds the Delegate's device
 *                         key.
 *
 * Provisioning Delegate   a normally-provisioned node (features.role stays
 *                         MESH_ROLE_DEVICE) that gains the ability to provision
 *                         other devices after the Manager grants it. Runs the
 *                         Server plus the stack's standard PB-ADV provisioner flow.
 *
 * Nodes that do not compile this model in are unaffected: zero footprint and no
 * way to reach them with provisioning authority. The unprovisioned devices being
 * provisioned are standard SIG Mesh devices - they never see any DPROV message,
 * only the standard PB-ADV handshake with the Delegate.
 *
 * Node reports are protected by the Delegate's own device key (akf = 0), the same
 * credential Configuration Server messages use. The Delegate has a device key of its
 * own because it is a normally-provisioned node; the Manager used that same key when
 * it provisioned the Delegate and can therefore always decrypt the reports. No report
 * key, no extra application-layer encryption, and no node other than the Manager and
 * this Delegate can ever read a report.
 * @{
 */

/**
 * @defgroup DPROV_ACCESS_OPCODE Access Opcode
 * @note 0xC0 ~ 0xCD and 0xE0 ~ 0xE4 are taken by ping / light_cwrgb / tp /
 *       datatrans / switch models.
 * @{
 */
#define MESH_MSG_DPROV_GRANT                    0xD05D00 //!< Client -> Server
#define MESH_MSG_DPROV_GRANT_STATUS             0xD15D00 //!< Server -> Client
#define MESH_MSG_DPROV_REVOKE                   0xD25D00 //!< Client -> Server
#define MESH_MSG_DPROV_NODE_REPORT              0xD35D00 //!< Server -> Client
#define MESH_MSG_DPROV_NODE_REPORT_CONFIRM      0xD45D00 //!< Client -> Server
#define MESH_MSG_DPROV_SERVER_STATUS            0xD55D00 //!< Server -> Client (spontaneous)
#define MESH_MSG_DPROV_CAP_GET                  0xD65D00 //!< Client -> Server (fission probe)
#define MESH_MSG_DPROV_CAP_STATUS               0xD75D00 //!< Server -> Client (fission probe)
/** @} */

/**
 * @defgroup DPROV_MODEL_ID Model ID
 * @{
 */
#define MESH_MODEL_DPROV_SERVER                 0x0006005D
#define MESH_MODEL_DPROV_CLIENT                 0x0007005D
/** @} */

/**
 * @defgroup DPROV_STATUS_CODE Status Code
 * @{
 */
enum
{
    DPROV_STAT_SUCCESS              = 0x00,
    DPROV_STAT_INVALID_RANGE        = 0x01, //!< GRANT address range illegal
    DPROV_STAT_NO_RESOURCE          = 0x02, //!< Manager dev_key_list full (report will retry)
    DPROV_STAT_WRONG_STATE          = 0x03, //!< new GRANT arrived while another is active
} _SHORT_ENUM_;
typedef uint8_t dprov_stat_t;

/**
 * SERVER_STATUS reason codes. Spontaneous notifications from the Delegate to the
 * Manager, describing why the Delegate changed its own state. More reasons can be
 * added without breaking wire compatibility - unknown values must be logged and
 * ignored by the Manager.
 */
enum
{
    DPROV_SERVER_REASON_INACTIVE        = 0x00, //!< inactive timer expired, auto-reverted to IDLE
    DPROV_SERVER_REASON_ADDR_EXHAUSTED  = 0x01, //!< address range used up, needs a new GRANT
} _SHORT_ENUM_;
typedef uint8_t dprov_server_reason_t;
/** @} */

/**
 * @defgroup DPROV_MESH_MSG Mesh Msg
 * @{
 */

/**
 * GRANT: "you may provision, here is your address range."
 *
 * Sent once per batch, per Delegate. Re-sending replaces the range and clears the
 * failed-device list; it does not disturb a link in progress.
 *
 * The Manager shall never hand out a range it has handed out before: a unicast
 * address that has been put into a Provisioning Data PDU cannot be reused while the
 * IV Index is unchanged (Mesh Protocol v1.1, 5.4.2.5).
 *
 * uuid_prefix is matched against the leading octets of the Device UUID in the
 * Unprovisioned Device Beacon, and is how the Manager keeps Delegates off each
 * other's devices without any inter-Delegate arbitration. Empty means provision
 * anything that beacons, which under No OOB means any device in radio range joins
 * the network - acceptable only on a controlled line.
 *
 * A prefix of 4 octets or less keeps the message unsegmented.
 *
 * inactive_period encodes the auto-revert window on the wire: actual timeout is
 * (period + 1) * 60s. 0 gives 60s (bench default), 255 gives ~4.27h.
 *
 * NetKey and subnet come from the message context (net_key_index), never from the
 * payload. The destination for reports defaults to the message src, but report_dst
 * overrides it: a Delegate that provisions further devices (fission) sends GRANT with
 * report_dst set to its own Manager so its children report straight to the root
 * (@ref delegated_prov_delegate.c). report_dst = 0 keeps the v1 "report to src".
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_GRANT)];
    uint16_t addr_base;             //!< first unicast address the Delegate may assign
    uint16_t addr_count;            //!< how many consecutive addresses it may assign
    uint8_t inactive_period;        //!< inactive window = (period + 1) * 60s
    uint16_t report_dst;            //!< where node reports go; 0 = use msg src (v1 behaviour)
    uint8_t uuid_prefix[0];         //!< optional, length derived from msg_len
} _PACKED4_ dprov_grant_t;

/**
 * GRANT_STATUS: response to GRANT and to unicast REVOKE.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_GRANT_STATUS)];
    dprov_stat_t status;
} _PACKED4_ dprov_grant_status_t;

/**
 * REVOKE: stop provisioning. No parameters, may be group addressed to stop a whole
 * line at once. Reports already queued keep retransmitting until confirmed.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_REVOKE)];
} _PACKED4_ dprov_revoke_t;

/**
 * NODE_REPORT: hand a freshly provisioned node's device key to the Manager.
 * One node per message - the unicast address identifies it end-to-end, so uuid
 * is left off the wire (the Manager keyed the archive by uuid at provisioning
 * time and re-keys it by addr on receipt; both directions are 1:1).
 *
 * element_count comes from the Provisioning Capabilities PDU, so the Manager does
 * not need a Composition Data read to know how many addresses the node consumed.
 *
 * The whole message is sent with the Delegate's own device key (akf = 0, the same
 * credential Configuration Server messages use), so the device key of the provisioned
 * node travels in plaintext protected only by that mesh-layer encryption. The Manager
 * holds every Delegate's device key from having provisioned it, so it can always
 * decrypt, and no node other than the Manager and this Delegate can read the report.
 * No extra application-layer encryption and no report key are needed.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_NODE_REPORT)];
    uint16_t unicast_addr;
    uint8_t element_count;
    uint8_t dev_key[16];            //!< plaintext over the dev-key-encrypted message
} _PACKED4_ dprov_node_report_t;

/**
 * NODE_REPORT_CONFIRM: the Manager has archived it, the Delegate may drop its local
 * copy of the device key and stop retransmitting. The report travels under the
 * Delegate's device key, which already authenticates the whole message, so the
 * unicast_addr alone identifies the entry being confirmed.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_NODE_REPORT_CONFIRM)];
    uint16_t unicast_addr;          //!< identifies the report being confirmed
    dprov_stat_t status;
} _PACKED4_ dprov_node_report_confirm_t;

/**
 * SERVER_STATUS: Delegate → Manager spontaneous state-change notification.
 *
 * Sent when the Delegate changes state without a triggering Manager request:
 * inactive-timer expiry or the granted address range being used up.
 *
 * addr_next / addr_end describe the current grant window - how far into the
 * range the Delegate consumed (addr_next) and its upper bound (addr_end). This
 * lets the Manager see whether the batch was partial, how much of the range
 * remains to reclaim, and size the next grant immediately.
 *
 * Note there is deliberately no "report lost" reason: a node report retries
 * forever until confirmed (the Delegate never deletes the dev_key on its own),
 * so the same broken path that would strand a report also drops any status
 * message - a wire notification for it would be futile. A stalled report is
 * surfaced locally on the Delegate instead (@ref DPROV_SERVER_REPORT_STALLED).
 *
 * The message is best-effort, one-shot: the mesh transport covers single-hop
 * loss, and the next GRANT the Manager sends is enough to repair state
 * divergence if this message is lost.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_SERVER_STATUS)];
    dprov_server_reason_t reason;
    uint16_t addr_next;             //!< next address the Delegate would assign
    uint16_t addr_end;              //!< one past the last granted address
} _PACKED4_ dprov_server_status_t;

/**
 * CAP_GET: "can you take a fission sub-grant?" A Delegate sends this to a child it just
 * provisioned, before carving any address, to learn whether it can provision further.
 * No parameters; protected by the child's device key (akf = 0), held by the parent.
 *
 * A device without the DPROV Server never decodes this opcode, so never replies; the
 * probe times out and the child stays a plain node. That is the point of probing before
 * splitting - a range is never fenced off for a node that cannot use it.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_CAP_GET)];
} _PACKED4_ dprov_cap_get_t;

/**
 * CAP_STATUS: reply to CAP_GET. willing = 1 only when the child can accept a grant now
 * (DPROV Server IDLE, free device-key slot); 0 when already delegated or out of
 * resources, letting the parent give up without waiting out the probe timeout.
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DPROV_CAP_STATUS)];
    uint8_t willing;                //!< 1 = able to take a sub-grant now, 0 = not
} _PACKED4_ dprov_cap_status_t;
/** @} */

/**
 * @defgroup DPROV_SERVER_DATA Server Data
 * @{
 */
#define DPROV_SERVER_GRANT              0 //!< @ref dprov_server_grant_t
#define DPROV_SERVER_REVOKE             1 //!< no args
#define DPROV_SERVER_IDLE               2 //!< no args
#define DPROV_SERVER_REPORT_STALLED     3 //!< @ref dprov_server_report_stalled_t
#define DPROV_SERVER_INACTIVE           4 //!< @ref dprov_server_inactive_t
#define DPROV_SERVER_REPORT_CONFIRMED   5 //!< @ref dprov_server_report_confirmed_t

/**
 * The grant has been applied before this callback runs. Set status to something
 * other than DPROV_STAT_SUCCESS to refuse, and the grant is rolled back.
 */
typedef struct
{
    /** from the message context */
    uint16_t manager_addr;          //!< where node reports go
    uint16_t net_key_index;         //!< subnet to provision devices into
    /** from the payload */
    uint16_t addr_base;
    uint16_t addr_count;
    uint8_t inactive_period;        //!< our inactive window; passed to children on a split
    uint8_t prefix_len;
    uint8_t *uuid_prefix;
    /** app may override, default DPROV_STAT_SUCCESS */
    dprov_stat_t status;
} dprov_server_grant_t;

/**
 * A node report has been retrying for a while with no CONFIRM - almost always
 * the Manager is unreachable. Fired once per report as a local heads-up so the
 * fixture can alarm / log the address; it is NOT a loss. The Delegate keeps the
 * dev_key and keeps retransmitting forever, so delivery resumes automatically
 * once the Manager returns. No wire message is sent (the same broken path would
 * drop it), which is why the Manager is not notified here.
 */
typedef struct
{
    uint16_t unicast_addr;
    uint8_t element_count;
} dprov_server_report_stalled_t;

/**
 * A node report was archived by the Manager. Fires from inside the CONFIRM handler
 * BEFORE the device key is deleted, so a fission delegate can keep the key
 * (defer_key_delete) while it probes the child and, if willing, sub-grants it.
 *
 * No address is split here: the delegate probes first (CAP_GET) and reserves a tail
 * block (dprov_server_reserve_tail) only once the child answers willing, so nothing is
 * fenced off for a node that cannot use it. The inputs below just let the delegate
 * decide whether the remaining range is worth probing.
 *
 * The only write-back is defer_key_delete (default false = v1 behaviour): true keeps
 * the dev_key, which the delegate needs to probe, sub-grant, and decrypt GRANT_STATUS,
 * and deletes once that exchange is retired. The record is freed and the report path
 * advances regardless.
 */
typedef struct
{
    uint16_t unicast_addr;
    /** current grant window, read-only inputs for the delegate */
    uint16_t addr_next;             //!< next address the delegate would assign
    uint16_t addr_end;              //!< one past the last address it may assign
    uint16_t manager_addr;          //!< this delegate's own Manager (report_dst for children)
    uint16_t net_key_index;         //!< subnet the grant travels on
    /** write-back, default no-op */
    bool defer_key_delete;          //!< true = keep dev_key, delegate deletes it later
} dprov_server_report_confirmed_t;

/**
 * The inactive timer expired: no provisioning activity for one full window. The
 * Delegate has already reverted itself to IDLE, closed UDB reporting, and sent
 * SERVER_STATUS(REASON_INACTIVE) to the Manager. Fired right before
 * DPROV_SERVER_IDLE so the app can distinguish "Manager revoked me" from "I gave
 * up on my own"; if the app only cares that the batch is done, listen on IDLE
 * and ignore this event.
 */
typedef struct
{
    uint16_t records_reported;      //!< nodes CONFIRMed under this grant
} dprov_server_inactive_t;
/** @} */

/**
 * @defgroup DPROV_CLIENT_DATA Client Data
 * @{
 */
#define DPROV_CLIENT_GRANT_STATUS       0 //!< @ref dprov_client_grant_status_t
#define DPROV_CLIENT_NODE_REPORT        1 //!< @ref dprov_client_node_report_t
#define DPROV_CLIENT_SERVER_STATUS      2 //!< @ref dprov_client_server_status_t
#define DPROV_CLIENT_CAP_STATUS         3 //!< @ref dprov_client_cap_status_t

typedef struct
{
    uint16_t src;
    dprov_stat_t status;
} dprov_client_grant_status_t;

/**
 * A probed child answered CAP_GET. willing tells the fission delegate whether to
 * carve off an address block and sub-grant it, or leave the child a plain node.
 */
typedef struct
{
    uint16_t src;
    uint8_t willing;
} dprov_client_cap_status_t;

/**
 * Fires only on a fresh archive: the model has already stored the device key into
 * mesh_node.dev_key_list (same slot layout the stack uses after a self-run
 * prov_assign, so the Configuration Client can address the node right away) and
 * sent the CONFIRM back to the Delegate. Duplicates and out-of-space cases are
 * handled inside the model and do not surface here.
 *
 * The callback is optional. Wire it up for logging, UI updates, or to kick off
 * template configuration of the freshly provisioned node.
 */
typedef struct
{
    uint16_t src;
    uint16_t unicast_addr;
    uint8_t element_count;
    uint8_t dev_key[16];
} dprov_client_node_report_t;

/**
 * A Delegate spontaneously changed state (typically inactive-timer expiry). The
 * Manager app should update its local Delegate table and decide whether to
 * re-grant, retire, or ignore this Delegate.
 */
typedef struct
{
    uint16_t src;
    dprov_server_reason_t reason;
    uint16_t addr_next;             //!< next address the Delegate would assign
    uint16_t addr_end;              //!< one past the last granted address
} dprov_client_server_status_t;
/** @} */

/**
 * @defgroup DPROV_SERVER_API Server API
 * @{
 */

/**
 * @brief register delegated provisioning server
 * @param[in] element_index: element index that model registered to
 * @param[in] pmodel_info: pointer to model context
 * @return register result
 */
bool dprov_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info);

/**
 * @brief feed an Unprovisioned Device Beacon to the automatic provisioning engine
 *
 * Call from the app's device_info callback when pinfo->type == DEVICE_INFO_UDB.
 * When a grant is held, this filters the beacon (uuid match + capacity) and opens
 * a PB-ADV link; the rest of the flow (Invite ... Complete, node report) is
 * driven by @ref dprov_server_prov_cb automatically. When no grant is held the
 * beacon is ignored, so a node without a live grant never provisions anything.
 *
 * @param[in] dev_uuid: device uuid from the Unprovisioned Device Beacon
 * @return TRUE when a link was opened or is already busy, FALSE when filtered out
 */
bool dprov_server_on_udb(const uint8_t dev_uuid[16]);

/**
 * @brief provisioning FSM filter for the Delegate side
 *
 * Call at the very start of the app's own prov callback. Returns TRUE when the
 * Delegate has fully handled the event and the app should short-circuit;
 * returns FALSE when the app should proceed as usual.
 *
 * Gating is by the Delegate's own capability, so no callback swap is needed:
 *
 *   - no grant held → FALSE, app runs unchanged
 *   - node is still being provisioned itself (node_state == UNPROV_DEVICE)
 *     → FALSE, app handles the device side
 *   - otherwise this node is currently acting as a Delegate:
 *       PATH_CHOOSE / PUBLIC_KEY / COMPLETE / PB_ADV_LINK_STATE handled here,
 *       return TRUE
 *       FAIL runs both sides (Delegate clean-up plus the app's debug print)
 *       every other event returns FALSE so cosmetic app handling stays
 *
 * PUBLIC_KEY must be caught here: the app's default handler installs a fixed
 * test key with prov_params_set, which would clobber the stack's per-session
 * ECC key that dprov relies on.
 *
 * PB_ADV_LINK_STATE is what kicks the handshake off: the stack never sends
 * PROV_INVITE by itself, so once the PB-ADV link is established the Delegate
 * calls prov_invite() to move the FSM past PROV_INVITE.
 *
 * @param[in] cb_type: provisioning event
 * @param[in] cb_data: event payload from the stack
 * @return TRUE = app should return early, FALSE = app continues
 */
bool dprov_server_prov_cb(prov_cb_type_t cb_type, prov_cb_data_t cb_data);

/**
 * @brief addresses still unassigned in the current grant (addr_end - addr_next)
 *
 * A fission delegate reads this when a child is confirmed to decide whether the
 * remaining range is worth splitting, and again when the child answers willing to
 * size the sub-grant share. Returns 0 when no grant is held.
 *
 * @return count of not-yet-assigned addresses in the live grant window
 */
uint16_t dprov_server_remaining(void);

/**
 * @brief provisioned nodes still awaiting the Manager's report CONFIRM
 *
 * These are siblings that are already provisioned but whose report has not yet been
 * confirmed, so their own fission probe has not started. A delegate counts them into
 * the sub-grant split denominator so an early-confirmed child does not grab a large
 * block that later siblings would then be starved of; the range ends up shared evenly.
 *
 * @return number of records queued for report CONFIRM
 */
uint16_t dprov_server_pending_report_count(void);

/**
 * @brief reserve a block from the tail of the grant range for a sub-grant
 *
 * Lowers addr_end by count and returns the new addr_end as the block base, so the
 * delegate provisions upward from addr_next while sub-granted blocks are carved
 * downward from addr_end - the two never overlap. Reserving from the tail (rather
 * than fencing at confirm time) means a probe that fails costs no address: the block
 * is only taken once the child is known willing.
 *
 * @param[in]  count: addresses to reserve; must not exceed dprov_server_remaining()
 * @param[out] pbase: first address of the reserved block on success
 * @return TRUE if reserved, FALSE if no grant held or count exceeds the remaining range
 */
bool dprov_server_reserve_tail(uint16_t count, uint16_t *pbase);
/** @} */

/**
 * @defgroup DPROV_CLIENT_API Client API
 * @{
 */

/**
 * @brief register delegated provisioning client
 * @param[in] element_index: element index that model registered to
 * @param[in] pmodel_info: pointer to model context
 * @return register result
 */
bool dprov_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info);

/**
 * @brief allow a node to provision, and give it an address range
 *
 * The message is protected by the target Delegate's own device key, which the Manager
 * holds from having provisioned it - no AppKey is involved.
 *
 * @param[in] pmodel_info: pointer to model context
 * @param[in] dst: target node
 * @param[in] net_key_index: subnet the grant travels on
 * @param[in] addr_base: first address it may assign
 * @param[in] addr_count: how many addresses, must not overlap a previous grant
 * @param[in] inactive_period: inactive window = (inactive_period + 1) * 60s;
 *                             0 = 60s minimum, 255 = ~4.27h maximum
 * @param[in] report_dst: where the target should send its node reports; 0 = report to
 *                        this message's src (the Manager). A fission delegate sets this
 *                        to its own Manager so grandchildren report to the root.
 * @param[in] prefix_len: uuid prefix length, 0 to accept any device, 4 or less to
 *                        keep the message unsegmented
 * @param[in] uuid_prefix: uuid prefix to match
 * @return send status
 */
mesh_msg_send_cause_t dprov_client_grant(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t net_key_index, uint16_t addr_base,
                                         uint16_t addr_count, uint8_t inactive_period,
                                         uint16_t report_dst, uint8_t prefix_len,
                                         const uint8_t *uuid_prefix);

/**
 * @brief probe whether a node can take a fission sub-grant
 *
 * Sent by a fission delegate to a child it just provisioned, before reserving any
 * address, to learn whether the child can provision further. The reply arrives as
 * @ref DPROV_CLIENT_CAP_STATUS; a node without the DPROV Server never replies, so the
 * delegate times the probe out and leaves it a plain node. Protected by the target's
 * device key (akf = 0), which the delegate holds from provisioning it.
 *
 * @param[in] pmodel_info: pointer to model context
 * @param[in] dst: target node
 * @param[in] net_key_index: subnet the probe travels on
 * @return send status
 */
mesh_msg_send_cause_t dprov_client_cap_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t net_key_index);

/**
 * @brief stop a node from provisioning
 * @param[in] pmodel_info: pointer to model context
 * @param[in] dst: target node, may be a group address
 * @param[in] net_key_index: subnet the revoke travels on
 * @return send status
 */
mesh_msg_send_cause_t dprov_client_revoke(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t net_key_index);

/**
 * @brief confirm an archived node report
 *
 * The model sends this automatically after archiving every incoming report, so a
 * fixture app normally does not call it. Exposed for special error paths where
 * the app needs to override the archival result (for instance an application-layer
 * validation that rejects the node after the model has already stored its key).
 *
 * @param[in] pmodel_info: pointer to model context
 * @param[in] dst: reporting Delegate
 * @param[in] net_key_index: subnet the confirm travels on
 * @param[in] unicast_addr: address from the report being confirmed
 * @param[in] status: archival result
 * @return send status
 */
mesh_msg_send_cause_t dprov_client_report_confirm(const mesh_model_info_p pmodel_info,
                                                  uint16_t dst, uint16_t net_key_index,
                                                  uint16_t unicast_addr, dprov_stat_t status);
/** @} */
/** @} */

END_DECLS

#endif /** _DELEGATED_PROV_H_ */
