/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/* Fission orchestration over the pure DPROV models: a confirmed child is probed and a
 * willing one gets a tail block (even split) tracked under one shared 1s timer. */

#ifndef MESH_DELEGATED_PROV
#define MESH_DELEGATED_PROV                 0
#endif

#if MESH_DELEGATED_PROV

#include <string.h>
#include "delegated_prov.h"
#include "delegated_prov_delegate.h"
#include "mesh_node.h"
#include "mesh_flash.h"
#include "data_uart.h"

/* min free addresses worth splitting: at 4, an even split still leaves each side >= 2
 * (child can provision one and fission again). Below this, fission stops. */
#define DPROV_FISSION_MIN_RANGE             4
/* max children concurrently in the probe/grant pipeline; each pins one dev_key slot */
#define DPROV_PENDING_GRANT_MAX             4

/* retransmit schedule: fixed 1s tick, per-step send caps. Probe gives up sooner - a
 * plain node simply never answers, so there is no point resending CAP_GET for long. */
#define DPROV_GRANT_RETRY_INTERVAL          1000
#define DPROV_PROBE_RETRY_MAX               3
#define DPROV_GRANT_RETRY_MAX               6

/* smallest block worth handing a willing child: below 2 it cannot provision even one
 * node and fission again, so keep it a plain node instead of wasting the addresses */
#define DPROV_SUBGRANT_SHARE_MIN            2

/* which leg of the exchange a pending entry is on */
typedef enum
{
    DPROV_PEND_PROBING = 0,     /* CAP_GET sent, awaiting CAP_STATUS */
    DPROV_PEND_GRANTING,        /* block reserved, GRANT sent, awaiting GRANT_STATUS */
} dprov_pend_state_t;

/* one child in the fission pipeline: pins its dev_key until retired; block fields
 * are filled once it answers willing */
typedef struct
{
    bool used;
    dprov_pend_state_t state;
    uint16_t unicast_addr;      /* child address = message dst, also the dev_key owner */
    uint16_t net_key_index;
    uint16_t addr_base;         /* sub-granted block (valid once GRANTING) */
    uint16_t addr_count;
    uint16_t report_dst;        /* where the child reports (the root, under fission) */
    uint8_t retry;              /* sends of the current step; cap depends on state */
} dprov_pending_grant_t;

typedef struct
{
    mesh_model_info_t server_model;
    mesh_model_info_t client_model;

    /* captured from the GRANT this delegate received, reused for sub-grants */
    uint8_t inactive_period;
    uint8_t prefix_len;
    uint8_t uuid_prefix[16];

    dprov_pending_grant_t pending[DPROV_PENDING_GRANT_MAX];
    plt_timer_t grant_timer;    /* shared: polls pending[] and resends due GRANTs */
} dprov_delegate_ctx_t;

static dprov_delegate_ctx_t dprov_dctx;

/* ------------------------------------------------------------- key deletion */

/* delete a child's dev_key straight from the stack and persist it */
static void dprov_delegate_delete_key(uint16_t addr)
{
    int dev_key_idx = dev_key_find(addr);

    if (dev_key_idx >= 0)
    {
        dev_key_delete((uint16_t)dev_key_idx);
        mesh_flash_store(MESH_FLASH_PARAMS_DEV_KEY, &dev_key_idx);
    }
}

/* ------------------------------------------------------- pending GRANT table */

static dprov_pending_grant_t *dprov_pending_find(uint16_t addr)
{
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        if (dprov_dctx.pending[i].used && (dprov_dctx.pending[i].unicast_addr == addr))
        {
            return &dprov_dctx.pending[i];
        }
    }
    return NULL;
}

static dprov_pending_grant_t *dprov_pending_alloc(void)
{
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        if (!dprov_dctx.pending[i].used)
        {
            return &dprov_dctx.pending[i];
        }
    }
    return NULL;
}

static bool dprov_pending_any(void)
{
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        if (dprov_dctx.pending[i].used)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/* retire an entry: drop the child's dev_key and free the slot */
static void dprov_pending_retire(dprov_pending_grant_t *pe)
{
    dprov_delegate_delete_key(pe->unicast_addr);
    memset(pe, 0, sizeof(*pe));

    if (!dprov_pending_any())
    {
        plt_timer_stop(dprov_dctx.grant_timer, 0);
    }
}

/* children currently probing (incl. pe): one half of the even-split denominator, the
 * other half is siblings awaiting CONFIRM - blocks stay equal across ordering */
static uint8_t dprov_pending_probing_count(void)
{
    uint8_t n = 0;
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        if (dprov_dctx.pending[i].used && (DPROV_PEND_PROBING == dprov_dctx.pending[i].state))
        {
            n++;
        }
    }
    return n;
}

/* (re)send the current step for one entry (CAP_GET while probing, GRANT once granting)
 * and bump its retry counter */
static mesh_msg_send_cause_t dprov_pending_send(dprov_pending_grant_t *pe)
{
    if (pe->retry < 0xff)
    {
        pe->retry++;
    }
    if (DPROV_PEND_PROBING == pe->state)
    {
        return dprov_client_cap_get(&dprov_dctx.client_model, pe->unicast_addr,
                                    pe->net_key_index);
    }
    return dprov_client_grant(&dprov_dctx.client_model, pe->unicast_addr, pe->net_key_index,
                              pe->addr_base, pe->addr_count, dprov_dctx.inactive_period,
                              pe->report_dst, dprov_dctx.prefix_len,
                              dprov_dctx.prefix_len ? dprov_dctx.uuid_prefix : NULL);
}

/* poll: resend every entry's current step, retiring those past their retry cap.
 * A PROBING entry forfeits nothing; a GRANTING one loses its reserved block. */
static void dprov_grant_timeout(void *ptimer)
{
    UNUSED(ptimer);
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        dprov_pending_grant_t *pe = &dprov_dctx.pending[i];

        if (!pe->used)
        {
            continue;
        }
        if (DPROV_PEND_PROBING == pe->state)
        {
            if (pe->retry >= DPROV_PROBE_RETRY_MAX)
            {
                data_uart_debug("[DPROV] 0x%04x no reply, plain node\r\n", pe->unicast_addr);
                dprov_pending_retire(pe);   /* no address was reserved; nothing forfeited */
                continue;
            }
            dprov_pending_send(pe);
            continue;
        }

        if (pe->retry >= DPROV_GRANT_RETRY_MAX)
        {
            printw("dprov: sub-grant to 0x%04x unconfirmed after %d sends, giving up",
                   pe->unicast_addr, pe->retry);
            data_uart_debug("[DPROV] 0x%04x sub-grant gave up, block 0x%04x-0x%04x lost\r\n",
                            pe->unicast_addr, pe->addr_base, pe->addr_base + pe->addr_count - 1);
            dprov_pending_retire(pe);       /* reserved block is forfeited; not reclaimed */
            continue;
        }
        dprov_pending_send(pe);
    }
}

/* arm the shared 1s poll timer if it is not already running */
static void dprov_grant_timer_arm(void)
{
    if ((NULL != dprov_dctx.grant_timer) && !plt_timer_is_active(dprov_dctx.grant_timer))
    {
        plt_timer_start(dprov_dctx.grant_timer, 0);
    }
}

/* ------------------------------------------------------------------- fission */

/* fresh grant to us resets the fission bookkeeping and caches grant parameters. Any
 * in-flight sub-grants from a prior grant are dropped: a re-grant redraws our range. */
static void dprov_delegate_on_granted(const dprov_server_grant_t *pg)
{
    for (uint8_t i = 0; i < DPROV_PENDING_GRANT_MAX; i++)
    {
        if (dprov_dctx.pending[i].used)
        {
            dprov_pending_retire(&dprov_dctx.pending[i]);
        }
    }
    /* children inherit our inactive window: same policy down the tree */
    dprov_dctx.inactive_period = pg->inactive_period;
    dprov_dctx.prefix_len = (pg->prefix_len > 16) ? 16 : pg->prefix_len;
    if (dprov_dctx.prefix_len)
    {
        memcpy(dprov_dctx.uuid_prefix, pg->uuid_prefix, dprov_dctx.prefix_len);
    }
}

/* child confirmed: if the range is worth splitting and a slot is free, start probing
 * it (CAP_GET); no address is reserved until it answers. Sets defer_key_delete. */
static void dprov_delegate_try_subgrant(dprov_server_report_confirmed_t *cb)
{
    uint16_t remaining = cb->addr_end - cb->addr_next;
    dprov_pending_grant_t *pe;

    /* stop once the range is too small to give a child a usable block after our own share */
    if (remaining < DPROV_FISSION_MIN_RANGE)
    {
        data_uart_debug("[DPROV] 0x%04x plain node, %d addr(s) left\r\n",
                        cb->unicast_addr, remaining);
        return;
    }

    /* no free slot to run the probe/grant pipeline: leave this child a plain node */
    pe = dprov_pending_alloc();
    if (NULL == pe)
    {
        printw("dprov: no pending slot, child 0x%04x stays a plain node", cb->unicast_addr);
        data_uart_debug("[DPROV] 0x%04x plain node, pending full\r\n", cb->unicast_addr);
        return;
    }

    pe->used = TRUE;
    pe->state = DPROV_PEND_PROBING;
    pe->unicast_addr = cb->unicast_addr;
    pe->net_key_index = cb->net_key_index;
    pe->report_dst = cb->manager_addr;              /* child reports straight to the root */
    pe->retry = 0;

    cb->defer_key_delete = true;        /* server keeps the key; we need it to probe + grant */

    /* first send may fail to queue; keep the entry, the timer retries it */
    if (MESH_MSG_SEND_CAUSE_SUCCESS != dprov_pending_send(pe))
    {
        printw("dprov: initial probe to 0x%04x not sent, will retry", cb->unicast_addr);
    }
    dprov_grant_timer_arm();

    data_uart_debug("[DPROV] 0x%04x probe fission, %d left\r\n", cb->unicast_addr, remaining);
}

/* a probed child answered. Willing + usable share -> carve a tail block and GRANT it;
 * otherwise retire the entry (nothing reserved, no address lost) as a plain node. */
static void dprov_delegate_on_cap_status(uint16_t src, uint8_t willing)
{
    dprov_pending_grant_t *pe = dprov_pending_find(src);
    uint16_t remaining;
    uint16_t share;
    uint16_t base;

    if ((NULL == pe) || (DPROV_PEND_PROBING != pe->state))
    {
        return;     /* stray or duplicate CAP_STATUS */
    }

    if (!willing)
    {
        data_uart_debug("[DPROV] 0x%04x not willing, plain node\r\n", src);
        dprov_pending_retire(pe);
        return;
    }

    /* even split over probing children + CONFIRM-awaited siblings + us (+1);
     * probing_count >= 1 (incl. pe) so pe is never over-granted */
    remaining = dprov_server_remaining();
    share = remaining / (dprov_server_pending_report_count() +
                         dprov_pending_probing_count() + 1u);

    /* fair share too thin but the range is not: give the front child the minimum block
     * (greedy). Later probers see less remaining and fall through to plain nodes. */
    if ((share < DPROV_SUBGRANT_SHARE_MIN) && (remaining >= DPROV_SUBGRANT_SHARE_MIN))
    {
        share = DPROV_SUBGRANT_SHARE_MIN;
    }

    if (share < DPROV_SUBGRANT_SHARE_MIN)
    {
        data_uart_debug("[DPROV] 0x%04x plain node, %d left\r\n", src, remaining);
        dprov_pending_retire(pe);
        return;
    }

    if (!dprov_server_reserve_tail(share, &base))
    {
        data_uart_debug("[DPROV] 0x%04x reserve %d failed, plain node\r\n", src, share);
        dprov_pending_retire(pe);
        return;
    }

    pe->state = DPROV_PEND_GRANTING;
    pe->addr_base = base;
    pe->addr_count = share;
    pe->retry = 0;

    if (MESH_MSG_SEND_CAUSE_SUCCESS != dprov_pending_send(pe))
    {
        printw("dprov: initial sub-grant to 0x%04x not sent, will retry", src);
    }
    dprov_grant_timer_arm();

    data_uart_debug("[DPROV] FISSION 0x%04x gets 0x%04x-0x%04x (%d), self keeps %d\r\n",
                    src, base, base + share - 1, share, dprov_server_remaining());
}

/* ------------------------------------------------------------- model callbacks */

static int32_t dprov_delegate_server_cb(const mesh_model_info_p pmodel_info,
                                        uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case DPROV_SERVER_GRANT:
        if (NULL != pargs)
        {
            dprov_server_grant_t *p = pargs;
            dprov_delegate_on_granted(p);
            data_uart_debug("[DPROV] GOT 0x%04x-0x%04x (%d) mgr 0x%04x inact=%d\r\n",
                            p->addr_base, p->addr_base + p->addr_count - 1, p->addr_count,
                            p->manager_addr, p->inactive_period);
        }
        break;
    case DPROV_SERVER_REPORT_CONFIRMED:
        if (NULL != pargs)
        {
            dprov_server_report_confirmed_t *p = pargs;
            dprov_delegate_try_subgrant(p);
            data_uart_debug("[DPROV] 0x%04x CONFIRMED%s\r\n", p->unicast_addr,
                            p->defer_key_delete ? " (key kept)" : "");
        }
        break;
    case DPROV_SERVER_REVOKE:
        data_uart_debug("[DPROV] grant REVOKED, no longer provisioning\r\n");
        break;
    case DPROV_SERVER_IDLE:
        data_uart_debug("[DPROV] idle - no grant, no pending report\r\n");
        break;
    case DPROV_SERVER_REPORT_STALLED:
        if (NULL != pargs)
        {
            dprov_server_report_stalled_t *p = pargs;
            /* abnormal: report unconfirmed for a while - warn in the log too */
            printw("dprov: report stalled addr=0x%04x elem=%d, still retrying",
                   p->unicast_addr, p->element_count);
            data_uart_debug("[DPROV] 0x%04x report STALLED, still retrying\r\n",
                            p->unicast_addr);
        }
        break;
    case DPROV_SERVER_INACTIVE:
        if (NULL != pargs)
        {
            dprov_server_inactive_t *p = pargs;
            data_uart_debug("[DPROV] inactive window expired, reported=%d\r\n",
                            p->records_reported);
        }
        break;
    default:
        break;
    }
    return 0;
}

static int32_t dprov_delegate_client_cb(const mesh_model_info_p pmodel_info,
                                        uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    if (NULL == pargs)
    {
        return 0;
    }

    switch (type)
    {
    case DPROV_CLIENT_CAP_STATUS:
        {
            dprov_client_cap_status_t *p = pargs;
            dprov_delegate_on_cap_status(p->src, p->willing);
        }
        break;
    case DPROV_CLIENT_GRANT_STATUS:
        {
            dprov_client_grant_status_t *p = pargs;
            /* status closes the exchange either way: stop retransmitting, drop the key.
             * a refused block stays forfeited (no address reclaim yet). */
            dprov_pending_grant_t *pe = dprov_pending_find(p->src);
            if ((NULL != pe) && (DPROV_PEND_GRANTING == pe->state))
            {
                if (DPROV_STAT_SUCCESS == p->status)
                {
                    data_uart_debug("[DPROV] 0x%04x is a delegate now, got 0x%04x-0x%04x\r\n",
                                    p->src, pe->addr_base, pe->addr_base + pe->addr_count - 1);
                }
                else
                {
                    /* abnormal: child refused the block - warn in the log too */
                    printw("dprov: child 0x%04x refused sub-grant, status=0x%02x, block forfeited",
                           p->src, p->status);
                    data_uart_debug("[DPROV] 0x%04x REFUSED, block 0x%04x-0x%04x lost\r\n",
                                    p->src, pe->addr_base, pe->addr_base + pe->addr_count - 1);
                }
                dprov_pending_retire(pe);
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

bool dprov_delegate_init(uint8_t element_index)
{
    memset(&dprov_dctx, 0, sizeof(dprov_dctx));

    dprov_dctx.server_model.model_data_cb = dprov_delegate_server_cb;
    dprov_dctx.client_model.model_data_cb = dprov_delegate_client_cb;

    /* reload timer at the fixed 1s tick; started/stopped by arm/retire */
    dprov_dctx.grant_timer = plt_timer_create("dprov_grant", DPROV_GRANT_RETRY_INTERVAL, true, 0,
                                              dprov_grant_timeout);
    if (NULL == dprov_dctx.grant_timer)
    {
        return FALSE;
    }

    if (!dprov_server_reg(element_index, &dprov_dctx.server_model))
    {
        plt_timer_delete(dprov_dctx.grant_timer, 0);
        dprov_dctx.grant_timer = NULL;
        return FALSE;
    }
    return dprov_client_reg(element_index, &dprov_dctx.client_model);
}

#endif /* MESH_DELEGATED_PROV */
