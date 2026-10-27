/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/* Provisioning Delegate role. See delegated_prov.h for the design. */

/* Opt-in: dprov needs the provisioner stack, so the app must enable
 * MESH_DELEGATED_PROV and link a provisioner-enabled library. */
#ifndef MESH_DELEGATED_PROV
#define MESH_DELEGATED_PROV                 0
#endif

#if MESH_DELEGATED_PROV

#include <string.h>
#include "delegated_prov.h"
#include "mesh_node.h"
#include "provision_provisioner.h"
#include "provision_adv.h"
#include "mesh_beacon.h"

/* No OOB, no attention timer - nobody is watching the devices. */
#define DPROV_ATTN_DUR                          0

/* Abort a Link Open that got no Ack after this, instead of the stack's 30s. */
#define DPROV_LINK_ABORT_TIMEOUT                800

/* UNPROV_DEVICE: close an inbound link stuck before PROV_START (provisioner walked
 * off) - the one stack-unguarded window - instead of idling out the stack's 80s. */
#define DPROV_DEV_LINK_START_TIMEOUT            8000

/* Report retries forever (dev_key kept): after DPROV_REPORT_STALL_RETRY tries
 * (~2 min) it raises a one-shot local alarm, then keeps retrying. */
#define DPROV_REPORT_RETRY_INTERVAL             1000
#define DPROV_REPORT_RETRY_INTERVAL_MAX         8000
#define DPROV_REPORT_STALL_RETRY                5

/* Inactive window wire encoding: (period + 1) * 60s. Zero disables. */
#define DPROV_INACTIVE_UNIT_MS                  (60u * 1000u)
#define DPROV_INACTIVE_MS(period)               (((uint32_t)(period) + 1u) * DPROV_INACTIVE_UNIT_MS)

/* Skip a device after DPROV_FAIL_MAX failures so its beacon cannot starve the
 * Delegate's only link slot; track up to DPROV_FAIL_TRACK_MAX at a time. */
#define DPROV_FAIL_TRACK_MAX                    4
#define DPROV_FAIL_MAX                          6

/* Authorization state, not persisted (reset revokes it). Unrelated to
 * mesh_node.features.role - a Delegate stays MESH_ROLE_DEVICE. */
typedef enum
{
    DPROV_STATE_IDLE                = 0x00, /* no grant held */
    DPROV_STATE_GRANTED             = 0x01, /* may provision */
} dprov_state_t;

/* Full 16-octet UUID as key - a prefix would collide across vendors. */
typedef struct
{
    uint8_t uuid[16];
    uint8_t fails;
} dprov_fail_t;

/* pnext is first so plt_list_* can treat this as a list element */
typedef struct dprov_record
{
    struct dprov_record *pnext;
    uint16_t unicast_addr;
    uint8_t element_count;
    uint8_t dev_key[16];
} dprov_record_t;

typedef struct
{
    dprov_state_t state;
    uint16_t manager_addr;          /* report_dst: where node reports go (root under fission) */
    uint16_t granted_addr;          /* who sent us the GRANT; only they may re-grant/revoke us */
    uint16_t net_key_index;
    uint16_t addr_next;             /* forward-only within a grant; reset to base on a fresh grant */
    uint16_t addr_end;
    uint8_t prefix_len;
    uint8_t uuid_prefix[16];
    plt_list_t record_list;         /* pending records; count is authoritative */
    dprov_record_t *current_record; /* NULL = report path idle */
    uint8_t current_report_retry;
    bool current_stall_alarmed;     /* one-shot local alarm fired for current_record */
    uint8_t in_flight_element_count;
    dprov_fail_t fail[DPROV_FAIL_TRACK_MAX];
    plt_timer_t retry_timer;
    plt_timer_t inactive_timer;
    plt_timer_t link_open_timer;    /* aborts a Link Open that got no Ack early */
    plt_timer_t dev_link_timer;     /* UNPROV_DEVICE: aborts an inbound link stuck before START */
    uint32_t inactive_ms;           /* cached from most recent GRANT; 0 disables */
    uint16_t records_reported;      /* local batch count (not sent on the wire) */
    mesh_model_info_p pmodel_info;
    bool prov_engine_started;
    bool child_link_active;         /* our link until it resolves; vs a stale leftover one */
} dprov_server_ctx_t;

/** one Delegated Provisioning Server per node */
static dprov_server_ctx_t dprov_ctx;

/* -------------------------------------------------------------------- tx helper */

/* akf = 0: message is protected by this node's own device key, no AppKey */
static mesh_msg_send_cause_t dprov_server_send(const mesh_model_info_p pmodel_info,
                                               uint16_t dst, uint16_t net_key_index,
                                               uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.akf = 0;
    mesh_msg.net_key_index = net_key_index;
    return access_send(&mesh_msg);
}

static void dprov_send_grant_status(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t net_key_index, dprov_stat_t status)
{
    dprov_grant_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_GRANT_STATUS);
    msg.status = status;
    dprov_server_send(pmodel_info, dst, net_key_index, (uint8_t *)&msg,
                      sizeof(dprov_grant_status_t));
}

static void dprov_send_cap_status(const mesh_model_info_p pmodel_info, uint16_t dst,
                                  uint16_t net_key_index, uint8_t willing)
{
    dprov_cap_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_CAP_STATUS);
    msg.willing = willing;
    dprov_server_send(pmodel_info, dst, net_key_index, (uint8_t *)&msg,
                      sizeof(dprov_cap_status_t));
}

static void dprov_send_server_status(const mesh_model_info_p pmodel_info,
                                     dprov_server_reason_t reason)
{
    dprov_server_status_t msg;

    /* self-granted bench path: no real Manager to notify */
    if (!MESH_IS_UNICAST_ADDR(dprov_ctx.manager_addr))
    {
        return;
    }
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_SERVER_STATUS);
    msg.reason = reason;
    msg.addr_next = dprov_ctx.addr_next;
    msg.addr_end = dprov_ctx.addr_end;
    dprov_server_send(pmodel_info, dprov_ctx.manager_addr, dprov_ctx.net_key_index,
                      (uint8_t *)&msg, sizeof(dprov_server_status_t));
}

/* --------------------------------------------------------------- record list */

static dprov_record_t *dprov_record_find(uint16_t unicast_addr)
{
    dprov_record_t *r = (dprov_record_t *)dprov_ctx.record_list.pfirst;
    while (NULL != r)
    {
        if (r->unicast_addr == unicast_addr)
        {
            return r;
        }
        r = r->pnext;
    }
    return NULL;
}

/* -------------------------------------------------------- inactive watchdog */

/* change_period + start every time so a refreshed window applies immediately */
static void dprov_inactive_arm(void)
{
    if ((NULL == dprov_ctx.inactive_timer) || (0 == dprov_ctx.inactive_ms))
    {
        return;
    }
    plt_timer_change_period(dprov_ctx.inactive_timer, dprov_ctx.inactive_ms, 0);
    if (!plt_timer_is_active(dprov_ctx.inactive_timer))
    {
        plt_timer_start(dprov_ctx.inactive_timer, 0);
    }
}

static void dprov_inactive_disarm(void)
{
    if (NULL == dprov_ctx.inactive_timer)
    {
        return;
    }
    plt_timer_stop(dprov_ctx.inactive_timer, 0);
}

static void dprov_inactive_reset(void)
{
    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return;
    }
    dprov_inactive_arm();
}

/* ------------------------------------------------------- failing device list */

static dprov_fail_t *dprov_fail_find(const uint8_t uuid[16])
{
    for (uint8_t i = 0; i < DPROV_FAIL_TRACK_MAX; i++)
    {
        if (dprov_ctx.fail[i].fails &&
            (0 == memcmp(dprov_ctx.fail[i].uuid, uuid, 16)))
        {
            return &dprov_ctx.fail[i];
        }
    }
    return NULL;
}

/* find-or-allocate; on allocation, displace the least troublesome entry */
static dprov_fail_t *dprov_fail_entry(const uint8_t uuid[16])
{
    dprov_fail_t *pf = dprov_fail_find(uuid);

    if (NULL == pf)
    {
        pf = &dprov_ctx.fail[0];
        for (uint8_t i = 0; i < DPROV_FAIL_TRACK_MAX; i++)
        {
            if (0 == dprov_ctx.fail[i].fails)
            {
                pf = &dprov_ctx.fail[i];
                break;
            }
            if (dprov_ctx.fail[i].fails < pf->fails)
            {
                pf = &dprov_ctx.fail[i];
            }
        }
        memcpy(pf->uuid, uuid, 16);
        pf->fails = 0;
    }

    return pf;
}

static void dprov_server_fail_note(const uint8_t uuid[16])
{
    dprov_fail_t *pf = dprov_fail_entry(uuid);

    if (pf->fails < 0xff)
    {
        pf->fails++;
    }

    if (pf->fails >= DPROV_FAIL_MAX)
    {
        printw("dprov: device skipped after %d failures, uuid:", pf->fails);
        dprintw(pf->uuid, 16);
    }
}

/* ------------------------------------------------------------- node report tx */

static void dprov_send_report(dprov_record_t *prec)
{
    dprov_node_report_t msg;

    if (NULL == prec)
    {
        return;
    }

    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_NODE_REPORT);
    msg.unicast_addr = prec->unicast_addr;
    msg.element_count = prec->element_count;
    memcpy(msg.dev_key, prec->dev_key, 16);
    dprov_server_send(dprov_ctx.pmodel_info, dprov_ctx.manager_addr,
                      dprov_ctx.net_key_index, (uint8_t *)&msg,
                      sizeof(dprov_node_report_t));
}

static bool dprov_any_pending(void)
{
    return (dprov_ctx.record_list.count > 0);
}

static void dprov_check_idle(void)
{
    mesh_model_info_p pmodel_info = dprov_ctx.pmodel_info;

    if ((DPROV_STATE_IDLE != dprov_ctx.state) || dprov_any_pending())
    {
        return;
    }

    plt_timer_stop(dprov_ctx.retry_timer, 0);

    if ((NULL != pmodel_info) && (NULL != pmodel_info->model_data_cb))
    {
        pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_IDLE, NULL);
    }
}

/* retry=1 -> 1s, retry=2 -> 2s, ..., capped at DPROV_REPORT_RETRY_INTERVAL_MAX */
static uint32_t dprov_backoff_period(uint8_t retry)
{
    uint32_t period = DPROV_REPORT_RETRY_INTERVAL;
    for (uint8_t k = 0; k < retry && period < DPROV_REPORT_RETRY_INTERVAL_MAX; k++)
    {
        period <<= 1;
    }
    if (period > DPROV_REPORT_RETRY_INTERVAL_MAX)
    {
        period = DPROV_REPORT_RETRY_INTERVAL_MAX;
    }
    return period;
}

static void dprov_start_next_report(void)
{
    dprov_record_t *prec = (dprov_record_t *)dprov_ctx.record_list.pfirst;

    if (NULL != prec)
    {
        dprov_ctx.current_record = prec;
        dprov_ctx.current_report_retry = 0;
        dprov_ctx.current_stall_alarmed = FALSE;
        data_uart_debug("[DPROV] report START 0x%04x to mgr 0x%04x\r\n",
                        prec->unicast_addr, dprov_ctx.manager_addr);
        dprov_send_report(prec);
        plt_timer_change_period(dprov_ctx.retry_timer,
                                DPROV_REPORT_RETRY_INTERVAL, 0);
        if (!plt_timer_is_active(dprov_ctx.retry_timer))
        {
            plt_timer_start(dprov_ctx.retry_timer, 0);
        }
        return;
    }

    dprov_ctx.current_record = NULL;
    dprov_ctx.current_report_retry = 0;
    dprov_ctx.current_stall_alarmed = FALSE;
    plt_timer_stop(dprov_ctx.retry_timer, 0);
    dprov_check_idle();
}

/* Retransmit forever until CONFIRM; a full dev_key_list supplies back-pressure. */
static void dprov_retry_timeout(void *ptimer)
{
    mesh_model_info_p pmodel_info = dprov_ctx.pmodel_info;
    dprov_record_t *prec = dprov_ctx.current_record;

    if (NULL == prec)
    {
        plt_timer_stop(dprov_ctx.retry_timer, 0);
        return;
    }

    if (dprov_ctx.current_report_retry < 0xff)
    {
        dprov_ctx.current_report_retry++;
    }

    /* one-shot local alarm (no wire message - the broken path would drop it) */
    if (!dprov_ctx.current_stall_alarmed &&
        (dprov_ctx.current_report_retry >= DPROV_REPORT_STALL_RETRY))
    {
        dprov_ctx.current_stall_alarmed = TRUE;
        printw("dprov: report for 0x%04x stalled (manager unreachable?), still retrying",
               prec->unicast_addr);
        if ((NULL != pmodel_info) && (NULL != pmodel_info->model_data_cb))
        {
            dprov_server_report_stalled_t cb_data;
            cb_data.unicast_addr = prec->unicast_addr;
            cb_data.element_count = prec->element_count;
            pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_REPORT_STALLED, &cb_data);
        }
    }

    dprov_send_report(prec);
    plt_timer_change_period(dprov_ctx.retry_timer,
                            dprov_backoff_period(dprov_ctx.current_report_retry), 0);
}

static mesh_msg_send_cause_t dprov_server_node_report(uint16_t unicast_addr,
                                                      uint8_t element_count)
{
    dprov_record_t *prec;
    int dev_key_idx;

    /* accept even in IDLE - a revoke mid-provision must not orphan the report */
    if (!MESH_IS_UNICAST_ADDR(unicast_addr) || (0 == element_count))
    {
        return MESH_MSG_SEND_CAUSE_INVALID_ACCESS_PARAMETER;
    }

    dev_key_idx = dev_key_find(unicast_addr);
    if (dev_key_idx < 0)
    {
        return MESH_MSG_SEND_CAUSE_INVALID_ACCESS_PARAMETER;
    }

    /* same address twice: refresh in place instead of duplicating */
    prec = dprov_record_find(unicast_addr);
    if (NULL == prec)
    {
        prec = (dprov_record_t *)plt_malloc(sizeof(dprov_record_t), RAM_TYPE_DATA_ON);
        if (NULL == prec)
        {
            printe("dprov: record alloc failed, dropping 0x%04x", unicast_addr);
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }
        memset(prec, 0, sizeof(dprov_record_t));
        plt_list_push(&dprov_ctx.record_list, prec);
    }

    prec->unicast_addr = unicast_addr;
    prec->element_count = element_count;
    dev_key_get((uint16_t)dev_key_idx, prec->dev_key);

    /* idle report path -> take over as current; otherwise wait on the list */
    if (NULL == dprov_ctx.current_record)
    {
        dprov_ctx.current_record = prec;
        dprov_ctx.current_report_retry = 0;
        dprov_ctx.current_stall_alarmed = FALSE;
        data_uart_debug("[DPROV] report START 0x%04x to mgr 0x%04x\r\n",
                        prec->unicast_addr, dprov_ctx.manager_addr);
        dprov_send_report(prec);
        plt_timer_change_period(dprov_ctx.retry_timer,
                                DPROV_REPORT_RETRY_INTERVAL, 0);
        if (!plt_timer_is_active(dprov_ctx.retry_timer))
        {
            plt_timer_start(dprov_ctx.retry_timer, 0);
        }
    }
    else
    {
        data_uart_debug("[DPROV] report QUEUED 0x%04x (list=%d)\r\n",
                        prec->unicast_addr, dprov_ctx.record_list.count);
    }

    dprov_inactive_reset();

    return MESH_MSG_SEND_CAUSE_SUCCESS;
}

/* ---------------------------------------------------------------- link gating */

/* address range handed out by GRANT is fully used up */
static bool dprov_server_addr_exhausted(void)
{
    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return FALSE;
    }
    return (dprov_ctx.addr_next >= dprov_ctx.addr_end);
}

/* no PB-ADV session in progress: safe to stand down without stranding a device that
 * reserved its address but has not yet reached COMPLETE (report not queued yet) */
static bool dprov_server_link_idle(void)
{
    return (PB_ADV_LINK_STATE_CLOSED == pb_adv_ctx[0].fsm);
}

static bool dprov_server_can_accept(void)
{
    if (dprov_server_addr_exhausted())
    {
        return FALSE;
    }
    return (dev_key_get_available_idx() >= 0);
}

static bool dprov_server_uuid_match(const uint8_t uuid[16])
{
    dprov_fail_t *pf;

    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return FALSE;
    }

    pf = dprov_fail_find(uuid);
    if ((NULL != pf) && (pf->fails >= DPROV_FAIL_MAX))
    {
        return FALSE;
    }

    if (0 == dprov_ctx.prefix_len)
    {
        return TRUE;
    }
    return (0 == memcmp(uuid, dprov_ctx.uuid_prefix, dprov_ctx.prefix_len));
}

/* ------------------------------------------------------- automatic engine */

/* idempotent: grant refresh must not reinstall the FSM or double-enable UDB */
static void dprov_engine_start(void)
{
    if (dprov_ctx.prov_engine_started)
    {
        return;
    }

    prov_init_prov();
    bool on = TRUE;
    beacon_cfg(BEACON_CFG_TYPE_UDB_REPORT_IN_DEVICE_ROLE, (uint8_t *)&on, sizeof(on));
    dprov_ctx.prov_engine_started = TRUE;
    printi("dprov: automatic provisioning engine started");
}

static void dprov_engine_stop(void)
{
    if (!dprov_ctx.prov_engine_started)
    {
        return;
    }

    bool on = FALSE;
    beacon_cfg(BEACON_CFG_TYPE_UDB_REPORT_IN_DEVICE_ROLE, (uint8_t *)&on, sizeof(on));
    dprov_ctx.prov_engine_started = FALSE;
    printi("dprov: automatic provisioning engine stopped");
}

#if MESH_MODEL_ENABLE_DEINIT
static void dprov_server_deinit(mesh_model_info_p pmodel_info)
{
    dprov_record_t *prec;

    UNUSED(pmodel_info);
    dprov_ctx.state = DPROV_STATE_IDLE;
    dprov_engine_stop();

    if ((PB_ADV_LINK_STATE_CLOSED != pb_adv_ctx[0].fsm))
    {
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_PROVISIONING_FAIL);
    }
    dprov_ctx.child_link_active = FALSE;

    if (NULL != dprov_ctx.retry_timer)
    {
        plt_timer_stop(dprov_ctx.retry_timer, 0);
        plt_timer_delete(dprov_ctx.retry_timer, 0);
        dprov_ctx.retry_timer = NULL;
    }
    if (NULL != dprov_ctx.inactive_timer)
    {
        plt_timer_stop(dprov_ctx.inactive_timer, 0);
        plt_timer_delete(dprov_ctx.inactive_timer, 0);
        dprov_ctx.inactive_timer = NULL;
    }
    if (NULL != dprov_ctx.link_open_timer)
    {
        plt_timer_stop(dprov_ctx.link_open_timer, 0);
        plt_timer_delete(dprov_ctx.link_open_timer, 0);
        dprov_ctx.link_open_timer = NULL;
    }
    if (NULL != dprov_ctx.dev_link_timer)
    {
        plt_timer_stop(dprov_ctx.dev_link_timer, 0);
        plt_timer_delete(dprov_ctx.dev_link_timer, 0);
        dprov_ctx.dev_link_timer = NULL;
    }

    dprov_ctx.current_record = NULL;
    while (NULL != (prec = (dprov_record_t *)plt_list_pop(&dprov_ctx.record_list)))
    {
        plt_free(prec, RAM_TYPE_DATA_ON);
    }

    pmodel_info->model_receive = NULL;
}
#endif

/* auto-revert equivalent to over-the-air REVOKE, plus notify Manager + app */
static void dprov_inactive_timeout(void *ptimer)
{
    mesh_model_info_p pmodel_info = dprov_ctx.pmodel_info;

    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return;
    }

    printi("dprov: inactive window elapsed, auto-reverting to IDLE");

    dprov_ctx.state = DPROV_STATE_IDLE;
    dprov_engine_stop();
    plt_timer_stop(dprov_ctx.link_open_timer, 0);   /* idle - no link can be OPENING */

    /* a stalled child link can let the window expire mid-session; release it */
    if (PB_ADV_LINK_STATE_CLOSED != pb_adv_ctx[0].fsm)
    {
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_SUCCESS);
    }
    dprov_ctx.child_link_active = FALSE;

    /* report the true high-water, then forfeit the uncommitted tail */
    dprov_send_server_status(pmodel_info, DPROV_SERVER_REASON_INACTIVE);
    dprov_ctx.addr_next = dprov_ctx.addr_end;

    if ((NULL != pmodel_info) && (NULL != pmodel_info->model_data_cb))
    {
        dprov_server_inactive_t cb_data;
        cb_data.records_reported = dprov_ctx.records_reported;
        pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_INACTIVE, &cb_data);
    }

    dprov_check_idle();
}

bool dprov_server_on_udb(const uint8_t dev_uuid[16])
{
    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return FALSE;
    }

    /* diagnostic: every UDB we consider, with the slot state that decides the outcome */
    data_uart_debug("[DPROV] on_udb: fsm=%d child_link=%d next=0x%04x end=0x%04x\r\n",
                    pb_adv_ctx[0].fsm, dprov_ctx.child_link_active,
                    dprov_ctx.addr_next, dprov_ctx.addr_end);

    if (!dprov_server_uuid_match(dev_uuid))
    {
        data_uart_debug("[DPROV] on_udb: uuid no match, skip\r\n");
        return FALSE;
    }

    if (!dprov_server_can_accept())
    {
        /* exhaustion is notified at COMPLETE/FAIL (last consuming link), not
         * here - waiting for a UDB could miss "last device, no more beacons" */
        data_uart_debug("[DPROV] on_udb: cannot accept (exhausted/no key slot), skip\r\n");
        return FALSE;
    }

    /* stale half-open link (slot held but not a child session of ours) would block
     * every link_open as Busy ~80s; close it, the next UDB opens the link */
    if ((PB_ADV_LINK_STATE_CLOSED != pb_adv_ctx[0].fsm) && !dprov_ctx.child_link_active)
    {
        printw("dprov: closing stale half-open link to free the slot");
        data_uart_debug("[DPROV] on_udb: stale link (fsm=%d), close & wait next udb\r\n",
                        pb_adv_ctx[0].fsm);
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_SUCCESS);
        return FALSE;
    }

    if (!pb_adv_link_open(0, (uint8_t *)dev_uuid))
    {
        data_uart_debug("[DPROV] on_udb: link_open failed (fsm=%d), skip\r\n", pb_adv_ctx[0].fsm);
        return FALSE;
    }
    data_uart_debug("[DPROV] on_udb: link_open ok, provisioning child\r\n");
    dprov_ctx.child_link_active = TRUE;     /* our session owns the slot until it resolves */
    /* arm the early-abort window; fires if no Link Ack arrives in time */
    plt_timer_change_period(dprov_ctx.link_open_timer, DPROV_LINK_ABORT_TIMEOUT, 0);
    if (!plt_timer_is_active(dprov_ctx.link_open_timer))
    {
        plt_timer_start(dprov_ctx.link_open_timer, 0);
    }
    dprov_inactive_reset();
    return TRUE;
}

/* Link Open got no Ack in time - release the slot for the next UDB. */
static void dprov_link_abort_timeout(void *ptimer)
{
    UNUSED(ptimer);
    if (pb_adv_ctx[0].fsm == PB_ADV_LINK_STATE_OPENING)
    {
        printw("dprov: link open aborted (no ack), freeing slot");
        data_uart_debug("[DPROV] child link open no-ack abort, slot freed\r\n");
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_TRANSACTION_TIMEOUT);
        dprov_ctx.child_link_active = FALSE;    /* abort may not raise LINK_CLOSED - clear here */
    }
}

static bool dprov_server_reserve(uint8_t element_count, uint16_t *paddr, int16_t *pidx)
{
    int16_t idx;

    if ((NULL == paddr) || (NULL == pidx))
    {
        return FALSE;
    }

    *paddr = MESH_UNASSIGNED_ADDR;
    *pidx = -1;

    if ((DPROV_STATE_GRANTED != dprov_ctx.state) || (0 == element_count))
    {
        return FALSE;
    }

    if ((dprov_ctx.addr_next + element_count) > dprov_ctx.addr_end)
    {
        printw("dprov: address range used up");
        return FALSE;
    }

    /* explicit idx to prov_assign() avoids its "evict lowest address" fallback */
    idx = dev_key_get_available_idx();
    if (idx < 0)
    {
        printw("dprov: no free device key slot, raise dev_key_num");
        return FALSE;
    }

    *paddr = dprov_ctx.addr_next;
    *pidx = idx;
    dprov_ctx.addr_next += element_count;

    return TRUE;
}

/* ------------------------------------------------------- fission range carve */

uint16_t dprov_server_remaining(void)
{
    if ((DPROV_STATE_GRANTED != dprov_ctx.state) ||
        (dprov_ctx.addr_next >= dprov_ctx.addr_end))
    {
        return 0;
    }
    return (uint16_t)(dprov_ctx.addr_end - dprov_ctx.addr_next);
}

uint16_t dprov_server_pending_report_count(void)
{
    return dprov_ctx.record_list.count;
}

bool dprov_server_reserve_tail(uint16_t count, uint16_t *pbase)
{
    if (NULL == pbase)
    {
        return FALSE;
    }
    *pbase = MESH_UNASSIGNED_ADDR;

    /* forward cursor grows up from addr_next, sub-grants carve down from addr_end;
     * safe iff the block does not cross addr_next (checked live, not at confirm time) */
    if ((0 == count) || (count > dprov_server_remaining()))
    {
        return FALSE;
    }

    dprov_ctx.addr_end -= count;
    *pbase = dprov_ctx.addr_end;
    return TRUE;
}

/* ------------------------------------------------ inbound link guard (device) */

/* Inbound link opened but PROV_START never came: close it so UDB resumes. node_state
 * guards a stale fire after we are provisioned - the slot may hold a child of ours. */
static void dprov_dev_link_timeout(void *ptimer)
{
    UNUSED(ptimer);
    if ((UNPROV_DEVICE == mesh_node.node_state) &&
        (PB_ADV_LINK_STATE_CLOSED != pb_adv_ctx[0].fsm))
    {
        printw("dprov: inbound link stuck before START, closing to resume UDB");
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_SUCCESS);
    }
}

static void dprov_dev_link_guard_stop(void)
{
    if (NULL != dprov_ctx.dev_link_timer)
    {
        plt_timer_stop(dprov_ctx.dev_link_timer, 0);
    }
}

/* Guard only the open->START gap: arm on LINK_OPENED, stop on START/FAIL/link
 * resolved. Never refresh on later steps - that false-closed slow live handshakes. */
static void dprov_dev_link_guard(prov_cb_type_t cb_type, prov_cb_data_t cb_data)
{
    if ((UNPROV_DEVICE != mesh_node.node_state) || (NULL == dprov_ctx.dev_link_timer))
    {
        return;
    }

    switch (cb_type)
    {
    case PROV_CB_TYPE_PB_ADV_LINK_STATE:
        if (PB_GENERIC_CB_LINK_OPENED == cb_data.pb_generic_cb_type)
        {
            plt_timer_start(dprov_ctx.dev_link_timer, 0);
        }
        else    /* OPEN_FAILED / LINK_CLOSED: session over */
        {
            dprov_dev_link_guard_stop();
        }
        break;
    case PROV_CB_TYPE_START:     /* provisioner is driving - hand off to the stack timeout */
    case PROV_CB_TYPE_FAIL:
        dprov_dev_link_guard_stop();
        break;
    default:
        break;
    }
}

/* --------------------------------------------------------- provisioning FSM */

bool dprov_server_prov_cb(prov_cb_type_t cb_type, prov_cb_data_t cb_data)
{
    /* device-side inbound link guard, ahead of the grant/role gates; does not swallow */
    dprov_dev_link_guard(cb_type, cb_data);

    if (DPROV_STATE_GRANTED != dprov_ctx.state)
    {
        return FALSE;
    }

    /* delegate acts only after being provisioned itself */
    if (UNPROV_DEVICE == mesh_node.node_state)
    {
        return FALSE;
    }

    switch (cb_type)
    {
    case PROV_CB_TYPE_PATH_CHOOSE:
        {
            /* no OOB throughout: FIPS-P256 curve, zero-init picks defaults */
            prov_start_t prov_start = {0};
            prov_path_choose(&prov_start);

            uint8_t element_count = cb_data.pprov_capabilities ?
                                    cb_data.pprov_capabilities->element_num : 1;
            uint16_t addr = MESH_UNASSIGNED_ADDR;
            int16_t idx = -1;

            if (!dprov_server_reserve(element_count, &addr, &idx))
            {
                /* do not ban - our resource shortage is not the device's fault,
                 * and a smaller-element device may still fit */
                data_uart_debug("[DPROV] child reserve failed (elem=%d), disconnect\r\n",
                                element_count);
                prov_disconnect(PB_ADV_LINK_CLOSE_PROVISIONING_FAIL);
                dprov_ctx.child_link_active = FALSE;
                return TRUE;
            }

            dprov_ctx.in_flight_element_count = element_count;
            prov_assign(idx, addr);
            prov_assign_net_key_index(0);
            data_uart_debug("[DPROV] child assigned addr 0x%04x (elem=%d)\r\n", addr, element_count);
        }
        return TRUE;
    case PROV_CB_TYPE_PUBLIC_KEY:
        /* swallow: app's default handler would clobber the per-session ECC key */
        return TRUE;
    case PROV_CB_TYPE_COMPLETE:
        {
            if (NULL == cb_data.pprov_data)
            {
                return TRUE;
            }

            if (MESH_MSG_SEND_CAUSE_SUCCESS !=
                dprov_server_node_report(cb_data.pprov_data->unicast_address,
                                         dprov_ctx.in_flight_element_count))
            {
                printe("dprov: node_report at COMPLETE failed for 0x%04x, dropping",
                       cb_data.pprov_data->unicast_address);
                int dev_key_idx = dev_key_find(cb_data.pprov_data->unicast_address);
                if (dev_key_idx >= 0)
                {
                    dev_key_delete((uint16_t)dev_key_idx);
                    mesh_flash_store(MESH_FLASH_PARAMS_DEV_KEY, &dev_key_idx);
                }
            }

            data_uart_debug("[DPROV] child 0x%04x COMPLETE, disconnect & report\r\n",
                            cb_data.pprov_data->unicast_address);
            /* stack does not close the bearer for a provisioner - do it here */
            prov_disconnect(PB_ADV_LINK_CLOSE_SUCCESS);
            dprov_ctx.child_link_active = FALSE;

            /* segment committed once Provisioning Data was sent; if this consumed
             * the last address, notify the Manager now (no future UDB may come) */
            if (dprov_server_addr_exhausted())
            {
                dprov_send_server_status(dprov_ctx.pmodel_info, DPROV_SERVER_REASON_ADDR_EXHAUSTED);
            }
        }
        return TRUE;
    case PROV_CB_TYPE_FAIL:
        {
            /* stack runs prov_assign_free() itself; nothing to release here */
            uint8_t uuid[16] = {0};
            memcpy(uuid, pb_adv_ctx[0].rmt_dev_uuid, 16);
            dprov_server_fail_note(uuid);
            data_uart_debug("[DPROV] child FAIL (type=%d), release addr & slot\r\n",
                            cb_data.prov_fail.fail_type);

            /* give back the reserved addresses - no Provisioning Data was sent,
             * so they were never "used" (Mesh Profile 5.4.2.5) */
            if ((dprov_ctx.in_flight_element_count > 0) &&
                (dprov_ctx.addr_next >= dprov_ctx.in_flight_element_count))
            {
                dprov_ctx.addr_next -= dprov_ctx.in_flight_element_count;
            }
            dprov_ctx.in_flight_element_count = 0;
            dprov_ctx.child_link_active = FALSE;    /* child session ended */
        }
        return FALSE;
    case PROV_CB_TYPE_PB_ADV_LINK_STATE:
        switch (cb_data.pb_generic_cb_type)
        {
        case PB_GENERIC_CB_LINK_OPENED:
            /* link established - early-abort window is done */
            plt_timer_stop(dprov_ctx.link_open_timer, 0);
            data_uart_debug("[DPROV] child link OPENED, send invite\r\n");
            /* the stack never sends PROV_INVITE on its own; kick the handshake */
            if (DPROV_STATE_GRANTED == dprov_ctx.state)
            {
                if (!prov_invite(DPROV_ATTN_DUR))
                {
                    printe("dprov: prov_invite after link open failed, fsm busy?");
                }
            }
            break;
        case PB_GENERIC_CB_LINK_OPEN_FAILED:
        case PB_GENERIC_CB_LINK_CLOSED:
            /* stack resolved the link (gave up or closed) - stop the abort timer and
             * release our ownership of the slot */
            plt_timer_stop(dprov_ctx.link_open_timer, 0);
            dprov_ctx.child_link_active = FALSE;
            data_uart_debug("[DPROV] child link %s, slot released\r\n",
                            (PB_GENERIC_CB_LINK_OPEN_FAILED == cb_data.pb_generic_cb_type) ?
                            "OPEN_FAILED" : "CLOSED");
            break;
        default:
            break;
        }
        return TRUE;
    default:
        return FALSE;
    }
}

/* -------------------------------------------------------------------- receive */

static void dprov_handle_grant(mesh_msg_p pmesh_msg)
{
    dprov_grant_t *pmsg = (dprov_grant_t *)(pmesh_msg->pbuffer + pmesh_msg->msg_offset);
    dprov_server_grant_t cb_data;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    uint8_t prefix_len;
    uint32_t old_inactive_ms = dprov_ctx.inactive_ms;
    /* snapshot of everything the grant overwrites, for rollback on refusal */
    dprov_state_t old_state = dprov_ctx.state;
    uint16_t old_manager_addr = dprov_ctx.manager_addr;
    uint16_t old_granted_addr = dprov_ctx.granted_addr;
    uint16_t old_net_key_index = dprov_ctx.net_key_index;
    uint16_t old_addr_next = dprov_ctx.addr_next;
    uint16_t old_addr_end = dprov_ctx.addr_end;
    uint8_t old_prefix_len = dprov_ctx.prefix_len;
    uint8_t old_uuid_prefix[16];

    if (pmesh_msg->msg_len < sizeof(dprov_grant_t))
    {
        return;
    }
    prefix_len = pmesh_msg->msg_len - sizeof(dprov_grant_t);

    /* re-grant by the same granter = extend; any other granter is rejected.
     * Compared against granted_addr, not manager_addr, which may point at the root. */
    if ((DPROV_STATE_GRANTED == dprov_ctx.state) &&
        (pmesh_msg->src != dprov_ctx.granted_addr))
    {
        printw("dprov: grant rejected, already delegated by another manager");
        dprov_send_grant_status(pmodel_info, pmesh_msg->src, pmesh_msg->net_key_index,
                                DPROV_STAT_WRONG_STATE);
        return;
    }

    /* overflow-safe range check: count-1 must fit in [addr_base, 0x7FFF] */
    if ((0 == pmsg->addr_count) || (prefix_len > 16) ||
        !MESH_IS_UNICAST_ADDR(pmsg->addr_base) ||
        ((uint16_t)(pmsg->addr_count - 1) > (uint16_t)(0x7FFF - pmsg->addr_base)))
    {
        dprov_send_grant_status(pmodel_info, pmesh_msg->src, pmesh_msg->net_key_index,
                                DPROV_STAT_INVALID_RANGE);
        return;
    }

    memcpy(old_uuid_prefix, dprov_ctx.uuid_prefix, 16);

    /* commit before the callback so the app may act from inside it. report_dst
     * routes our reports to the root (fission) or to the granter when 0 (v1). */
    dprov_ctx.state = DPROV_STATE_GRANTED;
    dprov_ctx.manager_addr = pmsg->report_dst ? pmsg->report_dst : pmesh_msg->src;
    dprov_ctx.granted_addr = pmesh_msg->src;
    dprov_ctx.net_key_index = pmesh_msg->net_key_index;
    if (DPROV_STATE_GRANTED == old_state)
    {
        /* extend a live grant: cursor only moves forward, never reusing a
         * committed address (Mesh Profile 5.4.2.5) */
        dprov_ctx.addr_next = (dprov_ctx.addr_next > pmsg->addr_base) ?
                              dprov_ctx.addr_next : pmsg->addr_base;
    }
    else
    {
        /* fresh grant: honor the assigned base, drop any stale high-water */
        dprov_ctx.addr_next = pmsg->addr_base;
    }
    dprov_ctx.addr_end = pmsg->addr_base + pmsg->addr_count;
    dprov_ctx.prefix_len = prefix_len;
    if (prefix_len)
    {
        memcpy(dprov_ctx.uuid_prefix, pmsg->uuid_prefix, prefix_len);
    }
    dprov_ctx.inactive_ms = DPROV_INACTIVE_MS(pmsg->inactive_period);
    memset(dprov_ctx.fail, 0, sizeof(dprov_ctx.fail));
    dprov_ctx.records_reported = 0;
    cb_data.manager_addr = dprov_ctx.manager_addr;
    cb_data.net_key_index = dprov_ctx.net_key_index;
    cb_data.addr_base = pmsg->addr_base;
    cb_data.addr_count = pmsg->addr_count;
    cb_data.inactive_period = pmsg->inactive_period;
    cb_data.prefix_len = prefix_len;
    cb_data.uuid_prefix = pmsg->uuid_prefix;
    cb_data.status = DPROV_STAT_SUCCESS;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_GRANT, &cb_data);
    }

    if (DPROV_STAT_SUCCESS == cb_data.status)
    {
        printi("dprov: granted, addr 0x%04x..0x%04x, inactive window %dms",
               dprov_ctx.addr_next, dprov_ctx.addr_end - 1, (int)dprov_ctx.inactive_ms);
        dprov_engine_start();
        dprov_inactive_arm();
    }
    else
    {
        dprov_ctx.state = old_state;
        dprov_ctx.manager_addr = old_manager_addr;
        dprov_ctx.granted_addr = old_granted_addr;
        dprov_ctx.net_key_index = old_net_key_index;
        dprov_ctx.addr_next = old_addr_next;
        dprov_ctx.addr_end = old_addr_end;
        dprov_ctx.prefix_len = old_prefix_len;
        dprov_ctx.inactive_ms = old_inactive_ms;
        memcpy(dprov_ctx.uuid_prefix, old_uuid_prefix, 16);
        printw("dprov: grant refused by app, status 0x%02x", cb_data.status);
    }

    dprov_send_grant_status(pmodel_info, pmesh_msg->src, pmesh_msg->net_key_index, cb_data.status);
}

static void dprov_handle_revoke(mesh_msg_p pmesh_msg)
{
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    /* queued reports keep retransmitting until confirmed */
    dprov_ctx.state = DPROV_STATE_IDLE;
    dprov_ctx.addr_next = dprov_ctx.addr_end;
    dprov_engine_stop();
    dprov_inactive_disarm();
    plt_timer_stop(dprov_ctx.link_open_timer, 0);

    /* revoke may land mid-child-session; release the slot or it blocks the next GRANT */
    if (PB_ADV_LINK_STATE_CLOSED != pb_adv_ctx[0].fsm)
    {
        pb_adv_link_close(0, PB_ADV_LINK_CLOSE_SUCCESS);
    }
    dprov_ctx.child_link_active = FALSE;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_REVOKE, NULL);
    }

    printi("dprov: revoked");

    /* group-addressed revoke has no reply */
    if (MESH_IS_UNICAST_ADDR(pmesh_msg->dst))
    {
        dprov_send_grant_status(pmodel_info, pmesh_msg->src, pmesh_msg->net_key_index,
                                DPROV_STAT_SUCCESS);
    }

    dprov_check_idle();
}

static void dprov_handle_report_confirm(mesh_msg_p pmesh_msg)
{
    dprov_node_report_confirm_t *pmsg =
        (dprov_node_report_confirm_t *)(pmesh_msg->pbuffer + pmesh_msg->msg_offset);
    dprov_record_t *prec = dprov_ctx.current_record;
    int dev_key_idx;

    /* only the Manager we hold a grant from may retire our reports */
    if (pmesh_msg->src != dprov_ctx.manager_addr)
    {
        return;
    }

    /* serial reports: confirm must match the record currently retransmitting */
    if ((NULL == prec) || (prec->unicast_addr != pmsg->unicast_addr))
    {
        data_uart_debug("[DPROV] report CONFIRM 0x%04x stray (current=0x%04x), ignore\r\n",
                        pmsg->unicast_addr, (NULL != prec) ? prec->unicast_addr : 0);
        return;
    }

    if (DPROV_STAT_SUCCESS != pmsg->status)
    {
        /* Manager refused to archive (likely its dev_key_list is full) - keep
         * retrying on the backoff schedule until a slot frees up */
        printw("dprov: report for 0x%04x rejected (status 0x%02x), will retry",
               pmsg->unicast_addr, pmsg->status);
        data_uart_debug("[DPROV] report CONFIRM 0x%04x rejected (status 0x%02x), retry\r\n",
                        pmsg->unicast_addr, pmsg->status);
        return;
    }

    data_uart_debug("[DPROV] report CONFIRM 0x%04x ok\r\n", pmsg->unicast_addr);

    uint16_t addr = pmsg->unicast_addr;
    mesh_model_info_p pmodel_info = dprov_ctx.pmodel_info;

    /* fire REPORT_CONFIRMED before deleting the key: the delegate may keep it
     * (defer_key_delete) to probe/sub-grant; no address is carved here yet */
    dprov_server_report_confirmed_t cb_data = {0};
    cb_data.unicast_addr = addr;
    cb_data.addr_next = dprov_ctx.addr_next;
    cb_data.addr_end = dprov_ctx.addr_end;
    cb_data.manager_addr = dprov_ctx.manager_addr;
    cb_data.net_key_index = dprov_ctx.net_key_index;
    if ((NULL != pmodel_info) && (NULL != pmodel_info->model_data_cb))
    {
        pmodel_info->model_data_cb(pmodel_info, DPROV_SERVER_REPORT_CONFIRMED, &cb_data);
    }

    /* delegate deletes the key later (on the child's GRANT_STATUS) when deferred */
    if (!cb_data.defer_key_delete)
    {
        dev_key_idx = dev_key_find(addr);
        if (dev_key_idx >= 0)
        {
            dev_key_delete((uint16_t)dev_key_idx);
            mesh_flash_store(MESH_FLASH_PARAMS_DEV_KEY, &dev_key_idx);
        }
    }

    /* record freed and report path advanced regardless of the deferral */
    plt_list_remove(&dprov_ctx.record_list, prec);
    plt_free(prec, RAM_TYPE_DATA_ON);
    if (dprov_ctx.records_reported < UINT16_MAX)
    {
        dprov_ctx.records_reported++;
    }
    printi("dprov: report for 0x%04x archived", addr);

    dprov_ctx.current_record = NULL;
    dprov_ctx.current_report_retry = 0;
    dprov_start_next_report();

    if ((DPROV_STATE_GRANTED == dprov_ctx.state) && !dprov_any_pending() &&
        dprov_server_addr_exhausted() && dprov_server_link_idle())
    {
        /* done: exhausted + all reports in + link idle. link-idle matters - the last
         * device may still be provisioning with its report not yet queued */
        dprov_ctx.state = DPROV_STATE_IDLE;
        dprov_engine_stop();
        dprov_inactive_disarm();
        dprov_check_idle();
    }
    else
    {
        dprov_inactive_reset();
    }
}

/* Probe: willing only when IDLE with a free key slot; a node already delegated says 0. */
static void dprov_handle_cap_get(mesh_msg_p pmesh_msg)
{
    uint8_t willing = ((DPROV_STATE_IDLE == dprov_ctx.state) &&
                       (dev_key_get_available_idx() >= 0)) ? 1 : 0;

    dprov_send_cap_status(pmesh_msg->pmodel_info, pmesh_msg->src,
                          pmesh_msg->net_key_index, willing);
}

static bool dprov_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_DPROV_GRANT:
        if (pmesh_msg->msg_len >= sizeof(dprov_grant_t))
        {
            dprov_handle_grant(pmesh_msg);
        }
        break;
    case MESH_MSG_DPROV_REVOKE:
        if (pmesh_msg->msg_len == sizeof(dprov_revoke_t))
        {
            dprov_handle_revoke(pmesh_msg);
        }
        break;
    case MESH_MSG_DPROV_NODE_REPORT_CONFIRM:
        if (pmesh_msg->msg_len == sizeof(dprov_node_report_confirm_t))
        {
            dprov_handle_report_confirm(pmesh_msg);
        }
        break;
    case MESH_MSG_DPROV_CAP_GET:
        if (pmesh_msg->msg_len == sizeof(dprov_cap_get_t))
        {
            dprov_handle_cap_get(pmesh_msg);
        }
        break;
    default:
        ret = FALSE;
        break;
    }

    return ret;
}

bool dprov_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    /* need at least own entry + one for a not-yet-confirmed node */
    if (mesh_node.dev_key_num < 2)
    {
        printe("dprov server reg: dev_key_num=%d, need >= 2", mesh_node.dev_key_num);
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_DPROV_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = dprov_server_receive;
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = dprov_server_deinit;
#endif
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("dprov server reg: missing model data process callback!");
        }
    }

    memset(&dprov_ctx, 0, sizeof(dprov_server_ctx_t));
    dprov_ctx.state = DPROV_STATE_IDLE;
    dprov_ctx.pmodel_info = pmodel_info;

    dprov_ctx.retry_timer = plt_timer_create("dprov_rpt", DPROV_REPORT_RETRY_INTERVAL, true, 0,
                                             dprov_retry_timeout);
    if (NULL == dprov_ctx.retry_timer)
    {
        printe("dprov server reg: create retry timer failed");
        return FALSE;
    }

    /* one-shot; every arm site sets the real period via change_period */
    dprov_ctx.inactive_timer = plt_timer_create("dprov_inact", DPROV_INACTIVE_UNIT_MS, false, 0,
                                                dprov_inactive_timeout);
    if (NULL == dprov_ctx.inactive_timer)
    {
        printe("dprov server reg: create inactive timer failed");
        plt_timer_delete(dprov_ctx.retry_timer, 0);
        return FALSE;
    }

    /* one-shot; armed at every successful link open, stopped on link resolution */
    dprov_ctx.link_open_timer = plt_timer_create("dprov_link", DPROV_LINK_ABORT_TIMEOUT, false, 0,
                                                 dprov_link_abort_timeout);
    if (NULL == dprov_ctx.link_open_timer)
    {
        printe("dprov server reg: create link_open timer failed");
        plt_timer_delete(dprov_ctx.inactive_timer, 0);
        plt_timer_delete(dprov_ctx.retry_timer, 0);
        return FALSE;
    }

    /* one-shot; UNPROV_DEVICE inbound-link open->START guard */
    dprov_ctx.dev_link_timer = plt_timer_create("dprov_dev_link", DPROV_DEV_LINK_START_TIMEOUT,
                                                false, 0, dprov_dev_link_timeout);
    if (NULL == dprov_ctx.dev_link_timer)
    {
        printe("dprov server reg: create dev_link timer failed");
        plt_timer_delete(dprov_ctx.link_open_timer, 0);
        plt_timer_delete(dprov_ctx.inactive_timer, 0);
        plt_timer_delete(dprov_ctx.retry_timer, 0);
        return FALSE;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

#endif /* MESH_DELEGATED_PROV */
