#ifndef __DHCPS_H__
#define __DHCPS_H__
#include "lwip_netconf.h"


#define DHCP_UNLIMITED_POOL_START	2
#define DHCP_UNLIMITED_POOL_END		254

#define DHCP_POOL_START				100
#define DHCP_POOL_END				200

#define DHCPS_MAX_CLIENT_NUM 	(DHCP_POOL_END-DHCP_POOL_START+1)

#define CONFIG_ENABLE_CAPTIVE_PORTAL  0

/* DHCPS log levels */
enum {
	DHCPS_DEBUG,
	DHCPS_INFO,
	DHCPS_WARNING,
	DHCPS_ERROR
};

#ifndef DHCPS_DEBUG_LEVEL
#define DHCPS_DEBUG_LEVEL   DHCPS_INFO
#endif

#define DHCPS_LOG(level, fmt, arg...)     \
	do {\
		if (level >= DHCPS_DEBUG_LEVEL) {\
			RTK_LOGS("DHCPS", RTK_LOG_ALWAYS, fmt, ##arg);\
		}\
	}while(0)

/* dhcp server states */
#define DHCP_SERVER_STATE_OFFER 			(1)
#define DHCP_SERVER_STATE_DECLINE 			(2)
#define DHCP_SERVER_STATE_ACK 				(3)
#define DHCP_SERVER_STATE_NAK 				(4)
#define DHCP_SERVER_STATE_IDLE 				(5)

#define DHCP_MESSAGE_OP_REQUEST        			(1)
#define DHCP_MESSAGE_OP_REPLY          			(2)

#define DHCP_MESSAGE_HTYPE 				(1)
#define DHCP_MESSAGE_HLEN  				(6)

#ifndef DNS_SERVER_PORT
#define DNS_SERVER_PORT 				(53)
#endif
#ifndef DHCP_SERVER_PORT
#define DHCP_SERVER_PORT  				(67)
#endif
#ifndef DHCP_CLIENT_PORT
#define DHCP_CLIENT_PORT  				(68)
#endif

#define DHCP_MESSAGE_TYPE_DISCOVER  			(1)
#define DHCP_MESSAGE_TYPE_OFFER     			(2)
#define DHCP_MESSAGE_TYPE_REQUEST   			(3)
#define DHCP_MESSAGE_TYPE_DECLINE   			(4)
#define DHCP_MESSAGE_TYPE_ACK       			(5)
#define DHCP_MESSAGE_TYPE_NAK       			(6)
#define DHCP_MESSAGE_TYPE_RELEASE   			(7)

#define DHCP_OPTION_LENGTH_ONE				(1)
#define DHCP_OPTION_LENGTH_TWO				(2)
#define DHCP_OPTION_LENGTH_THREE			(3)
#define DHCP_OPTION_LENGTH_FOUR				(4)
#ifndef DHCP_MSG_LEN
#define DHCP_MSG_LEN							236
#endif
#define DHCP_OPTION_TOTAL_LENGTH_MAX	312	//(51)= 4(magic)+3(type)+44(option code: 1,3,6,51,54,28,26,32,end)

#define DHCP_OPTION_CODE_SUBNET_MASK   			(1)
#define DHCP_OPTION_CODE_ROUTER        			(3)
#define DHCP_OPTION_CODE_DNS_SERVER    			(6)
#define DHCP_OPTION_CODE_INTERFACE_MTU 			(26)
#define DHCP_OPTION_CODE_BROADCAST_ADDRESS 		(28)
#define DHCP_OPTION_CODE_PERFORM_ROUTER_DISCOVERY 	(31)
#define DHCP_OPTION_CODE_REQUEST_IP_ADDRESS   		(50)
#define DHCP_OPTION_CODE_LEASE_TIME   			(51)
#define DHCP_OPTION_CODE_MSG_TYPE     			(53)
#define DHCP_OPTION_CODE_SERVER_ID    			(54)
#define DHCP_OPTION_CODE_REQ_LIST     			(55)
#define DHCP_OPTION_CODE_END         			(255)

/* Reference by RFC 2131 */
struct dhcp_msg {
	uint8_t op; 		/* Message op code/message type. 1 = BOOTREQUEST, 2 = BOOTREPLY */
	uint8_t	htype;		/* Hardware address type */
	uint8_t hlen;		/* Hardware address length */
	uint8_t hops;		/* Client sets to zero, optionally used by relay agents
				   when booting via a relay agent */
	uint8_t xid[4];		/* Transaction ID, a random number chosen by the client,
				   used by the client and server to associate messages and
				   responses between a client and a server */
	uint16_t secs;		/* Filled in by client, seconds elapsed since client began address
				   acquisition or renewal process.*/
	uint16_t flags;		/* bit 0: Broadcast flag, bit 1~15:MBZ must 0*/
	uint8_t ciaddr[4];	/* Client IP address; only filled in if client is in BOUND,
				   RENEW or REBINDING state and can respond to ARP requests. */
	uint8_t yiaddr[4];	/* 'your' (client) IP address */
	uint8_t siaddr[4];	/* IP address of next server to use in bootstrap;
				   returned in DHCPOFFER, DHCPACK by server. */
	uint8_t giaddr[4];	/* Relay agent IP address, used in booting via a relay agent.*/
	uint8_t chaddr[16];	/* Client hardware address */
	uint8_t sname[64];	/* Optional server host name, null terminated string.*/
	uint8_t file[128];	/* Boot file name, null terminated string; "generic" name or
			           null in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER.*/
	uint8_t options[312];   /* Optional parameters field. reference the RFC 2132 */
};

/* use this to check whether the message is dhcp related or not */
static const uint8_t dhcp_magic_cookie[4] = {99, 130, 83, 99};
static const uint8_t dhcp_option_lease_time[] = {0x00, 0x00, 0x1c, 0x20}; //1 day
//static const uint8_t dhcp_option_lease_time[] = {0x00, 0x00, 0x0e, 0x10}; // one hour
//static const uint8_t dhcp_option_interface_mtu_576[] = {0x02, 0x40};
static const uint8_t dhcp_option_interface_mtu[] = {0x05, 0xDC};

struct table {
	uint32_t ip_range_offer[8];
	uint32_t ip_range[8];
	uint8_t (*client_mac)[6];
	uint8_t *ip_addr4;
};

/* DHCP Server Instance Structure */
typedef struct dhcps_s {
	// Linked list pointer
	struct dhcps_s *next;               // Pointer to next DHCP server instance in global list

	// Network interface binding
	struct netif *dhcps_netif;          // Bound network interface

	// UDP control blocks
	struct udp_pcb *dhcps_pcb;          // DHCP server UDP control block
	struct udp_pcb *dns_pcb;            // DNS server UDP control block

	// IP address configuration
	ip_addr_t broadcast_address;
	ip_addr_t local_address;
	ip_addr_t local_mask;
	ip_addr_t local_gateway;
	ip_addr_t network_id;
	ip_addr_t subnet_broadcast;

	// IP address pool management
	ip_addr_t addr_pool_start;
	ip_addr_t addr_pool_end;
	int addr_pool_set;

	// Client allocation records
	struct eth_addr allocated_client_ethaddr;
	ip_addr_t allocated_client_address;

	// IP allocation table
	struct table ip_table;
	rtos_mutex_t ip_table_sema;

	// DHCP message handling
	struct dhcp_msg *message_repository;
	int message_options_max_len;

	// Temporary variables
	ip_addr_t client_request_ip;
	uint8_t client_addr[6];

	// State machine
	uint8_t state_machine;

	// Transaction ID
	uint8_t recorded_xid[4];

	// DNS related
	uint8_t *domain_name_buf;
	uint8_t domain_name_buf_len;
} dhcps_t;

PACK_STRUCT_BEGIN
/** DNS message header */
struct dns_hdr {
	PACK_STRUCT_FIELD(u16_t id);
	PACK_STRUCT_FIELD(u8_t flags1);
	PACK_STRUCT_FIELD(u8_t flags2);
	PACK_STRUCT_FIELD(u16_t numquestions);
	PACK_STRUCT_FIELD(u16_t numanswers);
	PACK_STRUCT_FIELD(u16_t numauthrr);
	PACK_STRUCT_FIELD(u16_t numextrarr);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

/* Helper macros for IP bit operations */
#define IP_TO_RANGE_IDX(ip)				(((ip) - 1) >> 5)
#define IP_TO_BIT_OFFSET(ip)			(((ip) - 1) & 0x1F)
#define IP_BIT_MASK(ip)					BIT(IP_TO_BIT_OFFSET(ip))

#define CHECK_IP_BIT_IN_TABLE(tbl, ip)	((tbl).ip_range[IP_TO_RANGE_IDX(ip)] & IP_BIT_MASK(ip))
#define CHECK_IP_OFFER_BIT(tbl, ip)		((tbl).ip_range_offer[IP_TO_RANGE_IDX(ip)] & IP_BIT_MASK(ip))
#define SET_IP_BIT_IN_TABLE(tbl, ip)	((tbl).ip_range[IP_TO_RANGE_IDX(ip)] |= IP_BIT_MASK(ip))
#define SET_IP_OFFER_BIT(tbl, ip)		((tbl).ip_range_offer[IP_TO_RANGE_IDX(ip)] |= IP_BIT_MASK(ip))
#define CLEAR_IP_BIT_IN_TABLE(tbl, ip)	((tbl).ip_range[IP_TO_RANGE_IDX(ip)] &= ~IP_BIT_MASK(ip))
#define CLEAR_IP_OFFER_BIT(tbl, ip)		((tbl).ip_range_offer[IP_TO_RANGE_IDX(ip)] &= ~IP_BIT_MASK(ip))

/* expose API */
void dhcps_set_addr_pool(struct netif *pnetif, int addr_pool_set, struct ip_addr *addr_pool_start, struct ip_addr *addr_pool_end);
dhcps_t *dhcps_init(struct netif *pnetif);
err_t dhcps_start(struct netif *pnetif);
void dhcps_stop(struct netif *pnetif);
void dhcps_deinit(struct netif *pnetif);
int dhcps_ip_in_table_check(struct netif *pnetif, uint8_t gate, uint8_t d);
uint8_t dhcps_search_client_ip(struct netif *pnetif, uint8_t *src_mac);

#endif
