/**
 * @file example_rnapt.h
 * @brief R-NAPT Example Entry Point
 *
 * Demonstrates how to use R-NAPT for multi-interface routing management.
 */

#ifndef __EXAMPLE_RNAPT_H
#define __EXAMPLE_RNAPT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/*                    Route Priority Configuration                          */
/* ======================================================================== */

/**
 * @brief R-NAPT route priority definition
 * @note  Higher value = higher priority for default gateway selection
 * @note  LAN interfaces typically use lower priority
 * @note  WAN interfaces use higher priority to be preferred as default gateway
 */
#define RNAPT_ROUTE_PRIO_WAN_ETH      103  /* ETH as WAN */
#define RNAPT_ROUTE_PRIO_WAN_STA      102  /* STA as WAN */
#define RNAPT_ROUTE_PRIO_WAN_USB      101  /* USB as WAN */
#define RNAPT_ROUTE_PRIO_LAN            0  /* LAN interfaces - lowest priority */

void example_rnapt(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLE_RNAPT_H */
