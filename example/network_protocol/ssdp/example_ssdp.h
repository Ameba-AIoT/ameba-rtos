/* example/network_protocol/ssdp/example_ssdp.h */
#ifndef _EXAMPLE_SSDP_H_
#define _EXAMPLE_SSDP_H_

/* Enable Device mode: broadcast ssdp:alive on startup and respond to M-SEARCH.
 * Requires httpd to serve description.xml (port EXAMPLE_SSDP_HTTP_PORT). */
#define EXAMPLE_SSDP_DEVICE         1

/* Enable Control Point mode: actively search and listen for UPnP devices on the LAN. */
#define EXAMPLE_SSDP_CONTROL_POINT  1

/* httpd listen port for Device mode. */
#define EXAMPLE_SSDP_HTTP_PORT      80

#if !EXAMPLE_SSDP_DEVICE && !EXAMPLE_SSDP_CONTROL_POINT
#error "At least one of EXAMPLE_SSDP_DEVICE or EXAMPLE_SSDP_CONTROL_POINT must be enabled"
#endif

void example_ssdp(void);

#endif /* _EXAMPLE_SSDP_H_ */
