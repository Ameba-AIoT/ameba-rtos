/**
 * @file example_rnat.h
 * @brief R-NAT Example Entry Point
 *
 * Demonstrates how to use R-NAT for multi-interface routing management.
 */

#ifndef __EXAMPLE_RNAT_H
#define __EXAMPLE_RNAT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/*                    Route Priority Configuration                          */
/* ======================================================================== */

/**
 * @brief R-NAT route priority definition
 * @note  Higher value = higher priority for default gateway selection
 * @note  LAN interfaces typically use lower priority
 * @note  WAN interfaces use higher priority to be preferred as default gateway
 */
#define RNAT_ROUTE_PRIO_WAN_ETH      103  /* ETH as WAN */
#define RNAT_ROUTE_PRIO_WAN_STA      102  /* STA as WAN */
#define RNAT_ROUTE_PRIO_WAN_USB      101  /* USB as WAN */
#define RNAT_ROUTE_PRIO_LAN            0  /* LAN interfaces - lowest priority */

void example_rnat(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLE_RNAT_H */
