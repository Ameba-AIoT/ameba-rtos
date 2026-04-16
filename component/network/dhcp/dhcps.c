
#include "wifi_api.h"
#include "atcmd_service.h"
#include "dhcps.h"

/* Global linked list head for all DHCP server instances */
static dhcps_t *dhcps_list_head = NULL;

/* Default domain name */
static const char g_dhcps_domain_name[] = "amebaiot.com";

/**
  * @brief  Search DHCP server instance by network interface from linked list.
  * @param  pnetif: pointer to the network interface to search for.
  * @retval Pointer to DHCP server instance (dhcps_t *), or NULL if not found.
  */
static dhcps_t *dhcps_get_from_netif(struct netif *pnetif)
{
	dhcps_t *current;

	/* Traverse the linked list */
	current = dhcps_list_head;
	while (current != NULL) {
		if (current->dhcps_netif == pnetif) {
			return current;
		}
		current = current->next;
	}

	return NULL;
}

/**
  * @brief  Insert a new DHCP server instance into the global linked list.
  * @param  dhcps: pointer to the DHCP server instance to insert.
  * @retval None.
  */
static void dhcps_list_insert(dhcps_t *dhcps)
{
	/* Insert at head of list */
	dhcps->next = dhcps_list_head;
	dhcps_list_head = dhcps;

	DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance inserted into list\r\n");
}

/**
  * @brief  Remove a DHCP server instance from the global linked list.
  * @param  dhcps: pointer to the DHCP server instance to remove (must be valid and in list).
  * @retval None.
  */
static void dhcps_list_remove(dhcps_t *dhcps)
{
	dhcps_t *current;
	dhcps_t *previous;

	/* Head node check */
	if (dhcps_list_head == dhcps) {
		dhcps_list_head = dhcps_list_head->next;
		dhcps->next = NULL;
		DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance removed from list\r\n");
		return;
	}

	/* Traverse to find and remove */
	previous = dhcps_list_head;
	current = dhcps_list_head->next;

	while (current != NULL) {
		if (current == dhcps) {
			previous->next = current->next;
			current->next = NULL;
			DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance removed from list\r\n");
			return;
		}
		previous = current;
		current = current->next;
	}

	DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance not found in list\r\n");
}

/**
  * @brief  Check if an IP address is already in the IP allocation table.
  * @param  pnetif: pointer to the network interface.
  * @param  gate: the third octet of the IP address (network segment).
  * @param  d: the fourth octet of the IP address (host ID).
  * @retval 0: IP is available; 1: IP is marked or network segment mismatch.
  * @note   Added by Realtek for ARP IP Check.
  */
int dhcps_ip_in_table_check(struct netif *pnetif, uint8_t gate, uint8_t d)
{
	dhcps_t *dhcps;
	int ret = 0;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "DHCPS instance not found\r\n");
		return 1;
	}

	// 192.168.43.xx in realtek softap, check 43.xx
	if (gate != ip4_addr3(ip_2_ip4(&dhcps->network_id))) {
		ret = 1;
		return ret;
	}

	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);
	if (CHECK_IP_BIT_IN_TABLE(dhcps->ip_table, d)) {
		ret = 1;
	}
	rtos_mutex_give(dhcps->ip_table_sema);

	return ret;
}

/**
  * @brief  Search and return the IP address assigned to a specific client MAC.
  * @param  pnetif: pointer to the network interface.
  * @param  src_mac: pointer to the client MAC address (6 bytes).
  * @retval Assigned IP address, or 0 if not found.
  */
uint8_t dhcps_search_client_ip(struct netif *pnetif, uint8_t *src_mac)
{
	dhcps_t *dhcps;
	uint8_t sta_num = wifi_user_config.ap_sta_num;
	uint8_t ip;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "DHCPS instance not found\r\n");
		return 0;
	}

	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);
	for (uint8_t j = 0; j < sta_num; j++) {
		ip = dhcps->ip_table.ip_addr4[j];
		if ((memcmp(dhcps->ip_table.client_mac[j], src_mac, 6) == 0) &&
			CHECK_IP_BIT_IN_TABLE(dhcps->ip_table, ip)) {
			rtos_mutex_give(dhcps->ip_table_sema);
			return ip;
		}
	}
	rtos_mutex_give(dhcps->ip_table_sema);
	return 0;
}

/**
  * @brief  Mark an IP address as occupied in the IP allocation table.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  d: the fourth octet of the IP address to mark (1-255).
  * @retval None.
  */
static void mark_ip_in_table(dhcps_t *dhcps, uint8_t d)
{
	DHCPS_LOG(DHCPS_DEBUG, "mark ip %d\r\n", d);

	if (d == 0) {
		DHCPS_LOG(DHCPS_ERROR, "Request IP out of range(1-255)\r\n");
		return;
	}

	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);

	SET_IP_BIT_IN_TABLE(dhcps->ip_table, d);

	DHCPS_LOG(DHCPS_DEBUG, "ip_table.ip_range[%d] = 0x%x\r\n", IP_TO_RANGE_IDX(d), dhcps->ip_table.ip_range[IP_TO_RANGE_IDX(d)]);

	rtos_mutex_give(dhcps->ip_table_sema);
}

/**
  * @brief  Save or update client MAC and IP mapping in the allocation table.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  candidate_ip: the fourth octet of the IP address to assign.
  * @param  hwaddr: pointer to the client MAC address (6 bytes).
  * @retval None.
  */
static void save_client_addr(dhcps_t *dhcps, uint8_t candidate_ip, uint8_t *hwaddr)
{
	unsigned int k;
	uint8_t i, j;
	struct rtw_client_list client_info = {0};
	uint8_t sta_num = wifi_user_config.ap_sta_num;

	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);

	for (i = 0; i < sta_num; i++) {
		if ((dhcps->ip_table.ip_addr4[i] == 0) || (dhcps->ip_table.ip_addr4[i] == candidate_ip)) {
			dhcps->ip_table.ip_addr4[i] = candidate_ip;
			memcpy(dhcps->ip_table.client_mac[i], hwaddr, 6);
			DHCPS_LOG(DHCPS_DEBUG, "init ip %d\r\n", candidate_ip);
			break;
		}
	}
	if (i == sta_num) {
		wifi_ap_get_connected_clients(&client_info);
		unsigned int client_num = client_info.count;
		for (j = 0; j < sta_num; j++) {
			for (k = 0; k < client_num; k++) {
				if (memcmp(dhcps->ip_table.client_mac[j], client_info.mac_list[k].octet, 6) == 0) {
					break;
				}
			}
			/* find an invalid mac&ip,update it with a new record */
			if (k == client_num) {
				uint8_t invalid_ipaddr4 = dhcps->ip_table.ip_addr4[j];
				CLEAR_IP_OFFER_BIT(dhcps->ip_table, invalid_ipaddr4);
				CLEAR_IP_BIT_IN_TABLE(dhcps->ip_table, invalid_ipaddr4);
				dhcps->ip_table.ip_addr4[j] = candidate_ip;
				memcpy(dhcps->ip_table.client_mac[j], hwaddr, 6);
				DHCPS_LOG(DHCPS_DEBUG, "replace ip %d with %d\r\n", invalid_ipaddr4, candidate_ip);
				break;
			}
		}
	}
	rtos_mutex_give(dhcps->ip_table_sema);

}

/**
  * @brief  Check if client's directly requested IP is valid and available.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  client_req_ip: pointer to the requested IP address structure.
  * @param  hwaddr: pointer to the client MAC address (6 bytes).
  * @retval Fourth octet of approved IP, or 0 if request is invalid/unavailable.
  */
static uint8_t check_client_direct_request_ip(dhcps_t *dhcps, struct ip_addr *client_req_ip, uint8_t *hwaddr)
{
	uint8_t ip_addr4 = 0;
	uint8_t sta_num = wifi_user_config.ap_sta_num;

	DHCPS_LOG(DHCPS_DEBUG, "%s: ip %d.%d.%d.%d, hwaddr %02x:%02x:%02x:%02x:%02x:%02x\r\n", __func__,
			  ip4_addr1(ip_2_ip4(client_req_ip)), ip4_addr2(ip_2_ip4(client_req_ip)), ip4_addr3(ip_2_ip4(client_req_ip)), ip4_addr4(ip_2_ip4(client_req_ip)),
			  hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
	if ((ip4_addr1(ip_2_ip4(&dhcps->network_id)) != ip4_addr1(ip_2_ip4(client_req_ip))) ||
		(ip4_addr2(ip_2_ip4(&dhcps->network_id)) != ip4_addr2(ip_2_ip4(client_req_ip))) ||
		(ip4_addr3(ip_2_ip4(&dhcps->network_id)) != ip4_addr3(ip_2_ip4(client_req_ip)))) {
		ip_addr4 = 0;
		goto Exit;
	}

	// check if the requested ip is available
	ip_addr4 = ip4_addr4(ip_2_ip4(client_req_ip));

	uint8_t pool_start = (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->addr_pool_start));
	uint8_t pool_end = (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->addr_pool_end));

	if (ip_addr4 < pool_start || ip_addr4 > pool_end) {
		DHCPS_LOG(DHCPS_WARNING, "Requested IP %d out of pool range [%d-%d]\r\n", ip_addr4, pool_start, pool_end);
		ip_addr4 = 0;
		goto Exit;
	}
	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);

	for (uint8_t i = 0; i < sta_num; i++) {
		uint8_t ip = dhcps->ip_table.ip_addr4[i];
		if ((ip == ip_addr4) &&
			(memcmp(dhcps->ip_table.client_mac[i], hwaddr, 6) != 0) &&
			CHECK_IP_BIT_IN_TABLE(dhcps->ip_table, ip)) {
			ip_addr4 = 0; // the ip is used
			break;
		}
		// the ip is available or already allocated to this client
	}

	rtos_mutex_give(dhcps->ip_table_sema);

Exit:
	return ip_addr4;
}

/**
  * @brief  Print the current DHCP client allocation table for debugging.
  * @param  dhcps: pointer to the DHCP server instance.
  * @retval None.
  */
void dump_client_table(dhcps_t *dhcps)
{
#if 0
	int i;
	uint8_t *p = NULL;
	printf("\r\nip_range: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
		   dhcps->ip_table.ip_range[0], dhcps->ip_table.ip_range[1], dhcps->ip_table.ip_range[2], dhcps->ip_table.ip_range[3],
		   dhcps->ip_table.ip_range[4], dhcps->ip_table.ip_range[5], dhcps->ip_table.ip_range[6], dhcps->ip_table.ip_range[7]);
	for (i = 0; i < wifi_user_config.ap_sta_num; i++) {
		p = dhcps->ip_table.client_mac[i];
		printf("\r\nip_addr4 = %d; Client[%d]: 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
			   dhcps->ip_table.ip_addr4[i], i, p[0], p[1], p[2], p[3], p[4], p[5]);
	}
	printf("\r\n");
#else
	(void)dhcps;
#endif
}

/**
  * @brief  Search and allocate the next available IP address from the IP pool.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  hwaddr: pointer to the client MAC address (6 bytes).
  * @retval Fourth octet of allocated IP address (1-254), or 0 if no IP available.
  */
static uint8_t search_next_ip(dhcps_t *dhcps, uint8_t *hwaddr)
{
	uint8_t i;
	uint8_t start, end;
	uint8_t sta_num = wifi_user_config.ap_sta_num;

	start = (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->addr_pool_start));
	end = (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->addr_pool_end));

	rtos_mutex_take(dhcps->ip_table_sema, RTOS_MAX_DELAY);
	/* If this MAC address has been allocated an address before, return the previously assigned address */
	for (i = 0; i < sta_num; i++) {
		if (memcmp(dhcps->ip_table.client_mac[i], hwaddr, 6) == 0) {
			uint8_t ip = dhcps->ip_table.ip_addr4[i];
			if (CHECK_IP_OFFER_BIT(dhcps->ip_table, ip)) {
				DHCPS_LOG(DHCPS_DEBUG, "found allocated IP %d\r\n", ip);
				rtos_mutex_give(dhcps->ip_table_sema);
				return ip;
			}
		}
	}

	for (uint8_t range_count = 0; range_count < 8; range_count++) {
		for (uint8_t offset_count = 0; offset_count < 32; offset_count++) {
			uint8_t candidate_ip = range_count * 32 + offset_count + 1;
			if ((((dhcps->ip_table.ip_range_offer[range_count] >> offset_count) & 0x01) == 0)
				&& (((dhcps->ip_table.ip_range[range_count] >> offset_count) & 0x01) == 0)
				&& (candidate_ip >= start)
				&& (candidate_ip <= end)) {
				SET_IP_OFFER_BIT(dhcps->ip_table, candidate_ip);
				rtos_mutex_give(dhcps->ip_table_sema);
				save_client_addr(dhcps, candidate_ip, hwaddr);
				return candidate_ip;
			}
		}
	}
	rtos_mutex_give(dhcps->ip_table_sema);
	return 0;
}

/**
  * @brief  Append DHCP message type option to the options field.
  * @param  msg_option_base_addr: pointer to the current position in options field.
  * @param  message_type: DHCP message type code (e.g., OFFER, ACK, NAK).
  * @retval Pointer to the next available position after the added option.
  */
static uint8_t *add_msg_type(uint8_t *msg_option_base_addr, uint8_t message_type)
{
	uint8_t *option_start;
	msg_option_base_addr[0] = DHCP_OPTION_CODE_MSG_TYPE;
	msg_option_base_addr[1] = DHCP_OPTION_LENGTH_ONE;
	msg_option_base_addr[2] = message_type;
	option_start = msg_option_base_addr + 3;
	if (DHCP_MESSAGE_TYPE_NAK == message_type) {
		*option_start++ = DHCP_OPTION_CODE_END;
	}
	return option_start;
}

/**
  * @brief  Fill a single DHCP option field with specified code and data.
  * @param  option_base_addr: pointer to the buffer where option will be written.
  * @param  option_code: DHCP option code (RFC 2132).
  * @param  option_length: length of the option data in bytes (1-4).
  * @param  copy_info: pointer to the data to copy, or NULL for zero-filled option.
  * @retval Pointer to the next available position after the filled option.
  */
static uint8_t *fill_one_option_content(uint8_t *option_base_addr,
										uint8_t option_code, uint8_t option_length, void *copy_info)
{
	uint8_t *option_data_base_address;
	uint8_t *next_option_start_address = NULL;
	uint8_t copy_info_array[4] = {0};
	option_base_addr[0] = option_code;
	option_base_addr[1] = option_length;
	option_data_base_address = option_base_addr + 2;
	if (copy_info == NULL) {
		copy_info = copy_info_array;
	}
	switch (option_length) {
	case DHCP_OPTION_LENGTH_FOUR:
		memcpy(option_data_base_address, copy_info, DHCP_OPTION_LENGTH_FOUR);
		next_option_start_address = option_data_base_address + 4;
		break;
	case DHCP_OPTION_LENGTH_TWO:
		memcpy(option_data_base_address, copy_info, DHCP_OPTION_LENGTH_TWO);
		next_option_start_address = option_data_base_address + 2;
		break;
	case DHCP_OPTION_LENGTH_ONE:
		memcpy(option_data_base_address, copy_info, DHCP_OPTION_LENGTH_ONE);
		next_option_start_address = option_data_base_address + 1;
		break;
	}

	return next_option_start_address;
}

/**
  * @brief  Append all required DHCP options to OFFER/ACK messages.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  option_start_address: pointer to the start position in options field.
  * @retval 0: success; -1: failed.
  */
static int8_t add_offer_options(dhcps_t *dhcps, uint8_t *option_start_address)
{
	// Total minimum len = 6+6+6+6+6+6+4+3+1 = 44
	uint8_t *temp_option_addr = option_start_address;
	int max_addable_option_len = dhcps->message_options_max_len - 4 - 3;	// -magic-type

	if (option_start_address == NULL) {
		goto ERROR;
	}

	/* add DHCP options 1.
	The subnet mask option specifies the client's subnet mask */
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(option_start_address, DHCP_OPTION_CODE_SUBNET_MASK,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcps->local_mask);
	} else {
		goto ERROR;
	}

	/* add DHCP options 3 (i.e router(gateway)). The time server option
	specifies a list of RFC 868 [6] time servers available to the client. */
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_ROUTER,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcps->local_address);
	} else {
		goto ERROR;
	}

	/* add DHCP options 6 (i.e DNS).
	    The option specifies a list of DNS servers available to the client. */
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_DNS_SERVER,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcps->local_address);
	} else {
		goto ERROR;
	}

	/* add DHCP options 51.
	This option is used to request a lease time for the IP address. */
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_LEASE_TIME,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcp_option_lease_time);
	} else {
		goto ERROR;
	}

	/* add DHCP options 54.
	The identifier is the IP address of the selected server. */
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_SERVER_ID,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcps->local_address);
	} else {
		goto ERROR;
	}

	/* add DHCP options 28.
	This option specifies the broadcast address in use on client's subnet.*/
	if (temp_option_addr + 6 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_BROADCAST_ADDRESS,
						   DHCP_OPTION_LENGTH_FOUR, (void *)&dhcps->subnet_broadcast);
	} else {
		goto ERROR;
	}

	/* add DHCP options 26.
	This option specifies the Maximum transmission unit to use */
	if (temp_option_addr + 4 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_INTERFACE_MTU,
						   DHCP_OPTION_LENGTH_TWO, (void *) &dhcp_option_interface_mtu);//dhcp_option_interface_mtu_576);
	} else {
		goto ERROR;
	}

	/* add DHCP options 31.
	This option specifies whether or not the client should solicit routers */
	if (temp_option_addr + 3 - option_start_address <= max_addable_option_len) {
		temp_option_addr = fill_one_option_content(temp_option_addr, DHCP_OPTION_CODE_PERFORM_ROUTER_DISCOVERY,
						   DHCP_OPTION_LENGTH_ONE,	NULL);
	} else {
		goto ERROR;
	}

	// END
	if (temp_option_addr + 1 - option_start_address <= max_addable_option_len) {
		*temp_option_addr++ = DHCP_OPTION_CODE_END;
	} else {
		goto ERROR;
	}
	return 0;

ERROR:
	DHCPS_LOG(DHCPS_ERROR, "%s: add options failed\r\n", __func__);
	return -1;
}

/**
  * @brief  Initialize common fields in DHCP reply message structure.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  dhcp_message_repository: pointer to the DHCP message structure to initialize.
  * @retval None.
  */
static void dhcps_initialize_message(dhcps_t *dhcps, struct dhcp_msg *dhcp_message_repository)
{
	dhcp_message_repository->op = DHCP_MESSAGE_OP_REPLY;
	dhcp_message_repository->htype = DHCP_MESSAGE_HTYPE;
	dhcp_message_repository->hlen = DHCP_MESSAGE_HLEN;
	dhcp_message_repository->hops = 0;
	memcpy((char *)dhcps->recorded_xid, (char *) dhcp_message_repository->xid,
		   sizeof(dhcp_message_repository->xid));
	dhcp_message_repository->secs = 0;

	memcpy((char *)dhcp_message_repository->yiaddr,
		   (char *)&dhcps->allocated_client_address,
		   sizeof(dhcp_message_repository->yiaddr));

	memset((char *)dhcp_message_repository->ciaddr, 0,
		   sizeof(dhcp_message_repository->ciaddr));
	memset((char *)dhcp_message_repository->siaddr, 0,
		   sizeof(dhcp_message_repository->siaddr));
	memset((char *)dhcp_message_repository->giaddr, 0,
		   sizeof(dhcp_message_repository->giaddr));
	memset((char *)dhcp_message_repository->sname,  0,
		   sizeof(dhcp_message_repository->sname));
	memset((char *)dhcp_message_repository->file,   0,
		   sizeof(dhcp_message_repository->file));
	memset((char *)dhcp_message_repository->options, 0,
		   dhcps->message_options_max_len);
	memcpy((char *)dhcp_message_repository->options, (char *)dhcp_magic_cookie,
		   sizeof(dhcp_magic_cookie));
}

/**
 * @brief  Allocate and prepare DHCP response message buffer.
 * @param  dhcps: pointer to the DHCP server instance.
 * @param  original_packet: pointer to the received DHCP packet buffer.
 * @param  msg_type: DHCP message type (OFFER/ACK).
 * @retval Pointer to prepared pbuf, or NULL on failure.
 */
static struct pbuf *dhcps_prepare_message(dhcps_t *dhcps, struct pbuf *original_packet, uint8_t msg_type)
{
	struct pbuf *response_packet = NULL;

	response_packet = pbuf_alloc(PBUF_TRANSPORT, DHCP_MSG_LEN + DHCP_OPTION_TOTAL_LENGTH_MAX, PBUF_RAM);
	if (response_packet == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "%s: pbuf alloc failed\r\n", __func__);
		return NULL;
	}

	if (pbuf_copy(response_packet, original_packet) != ERR_OK) {
		DHCPS_LOG(DHCPS_ERROR, "%s: pbuf copy failed\r\n", __func__);
		pbuf_free(response_packet);
		return NULL;
	}

	dhcps->message_options_max_len = DHCP_OPTION_TOTAL_LENGTH_MAX;
	dhcps->message_repository = (struct dhcp_msg *)response_packet->payload;

	dhcps_initialize_message(dhcps, dhcps->message_repository);

	uint8_t *option_ptr = add_msg_type(&dhcps->message_repository->options[4], msg_type);
	if (add_offer_options(dhcps, option_ptr) != 0) {
		pbuf_free(response_packet);
		return NULL;
	}

	return response_packet;
}

/**
 * @brief  Send DHCP response message via unicast or broadcast.
 * @param  dhcps: pointer to the DHCP server instance.
 * @param  response_packet: pointer to the prepared response packet buffer.
 * @retval ERR_OK: success; other: send failure.
 */
static err_t dhcps_send_message(dhcps_t *dhcps, struct pbuf *response_packet)
{
	err_t send_result = ERR_OK;

	if (dhcps->message_repository->flags == 0x0) {
		// Unicast
		for (int i = 0; i < 6; i++) {
			dhcps->allocated_client_ethaddr.addr[i] = dhcps->message_repository->chaddr[i];
		}
#if ETHARP_SUPPORT_STATIC_ENTRIES
		etharp_add_static_entry(ip_2_ip4(&dhcps->allocated_client_address), &dhcps->allocated_client_ethaddr);
#endif
		send_result = udp_sendto_if(dhcps->dhcps_pcb, response_packet,
									&dhcps->allocated_client_address, DHCP_CLIENT_PORT, dhcps->dhcps_netif);
#if ETHARP_SUPPORT_STATIC_ENTRIES
		etharp_remove_static_entry(ip_2_ip4(&dhcps->allocated_client_address));
#endif
	} else {
		// Broadcast
		send_result = udp_sendto_if(dhcps->dhcps_pcb, response_packet,
									&dhcps->broadcast_address, DHCP_CLIENT_PORT, dhcps->dhcps_netif);
	}

	return send_result;
}

/**
  * @brief  Initialize and send DHCP OFFER message to client.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  packet_buffer: pointer to the received DHCP DISCOVER packet buffer.
  * @retval None.
  */
static void dhcps_send_offer(dhcps_t *dhcps, struct pbuf *packet_buffer)
{
	uint8_t temp_ip = 0;
	struct pbuf *response_packet = NULL;

	temp_ip = dhcps_search_client_ip(dhcps->dhcps_netif, dhcps->client_addr);
	if (temp_ip == 0) {
		temp_ip = check_client_direct_request_ip(dhcps, &dhcps->client_request_ip, dhcps->client_addr);
		/* create new client ip */
		if (temp_ip == 0) {
			temp_ip = search_next_ip(dhcps, dhcps->client_addr);
		}
	}
	DHCPS_LOG(DHCPS_DEBUG, "temp_ip = %d\r\n", temp_ip);
	if (temp_ip == 0) {
#if 0
		memset(&dhcps->ip_table, 0, sizeof(struct table));
		mark_ip_in_table(dhcps, (uint8_t)ip4_addr4(&dhcps->local_address));
		printf("\r\n reset ip table!!\r\n");
#endif
		DHCPS_LOG(DHCPS_WARNING, "No usable IP\r\n");
	}

	at_printf_indicate("assign client ip:\"%d.%d.%d.%d\",hwaddr:\""MAC_FMT"\"\r\n", \
					   ip4_addr1(ip_2_ip4(&dhcps->network_id)), \
					   ip4_addr2(ip_2_ip4(&dhcps->network_id)), \
					   ip4_addr3(ip_2_ip4(&dhcps->network_id)), temp_ip, \
					   MAC_ARG(dhcps->client_addr));

	IP4_ADDR(ip_2_ip4(&dhcps->allocated_client_address), (ip4_addr1(ip_2_ip4(&dhcps->network_id))),
			 ip4_addr2(ip_2_ip4(&dhcps->network_id)), ip4_addr3(ip_2_ip4(&dhcps->network_id)), temp_ip);

	response_packet = dhcps_prepare_message(dhcps, packet_buffer, DHCP_MESSAGE_TYPE_OFFER);
	if (response_packet != NULL) {
		dhcps_send_message(dhcps, response_packet);
		pbuf_free(response_packet);
	}
}

/**
  * @brief  Initialize and send DHCP NAK message to client indicating request rejection.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  packet_buffer: pointer to the received DHCP request packet buffer.
  * @retval None.
  */
static void dhcps_send_nak(dhcps_t *dhcps, struct pbuf *packet_buffer)
{
	dhcps->message_repository = (struct dhcp_msg *)packet_buffer->payload;
	dhcps_initialize_message(dhcps, dhcps->message_repository);
	add_msg_type(&dhcps->message_repository->options[4], DHCP_MESSAGE_TYPE_NAK);
	udp_sendto_if(dhcps->dhcps_pcb, packet_buffer,
				  &dhcps->broadcast_address, DHCP_CLIENT_PORT, dhcps->dhcps_netif);
}

/**
  * @brief  Initialize and send DHCP ACK message confirming IP address lease to client.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  packet_buffer: pointer to the received DHCP request packet buffer.
  * @retval ERR_OK: success; other: send failure.
  */
static err_t dhcps_send_ack(dhcps_t *dhcps, struct pbuf *packet_buffer)
{
	struct pbuf *response_packet = NULL;
	err_t send_result = ERR_OK;

	// newly malloc a longer pbuf for dhcp ack rather than using the short pbuf from dhcp request
	response_packet = dhcps_prepare_message(dhcps, packet_buffer, DHCP_MESSAGE_TYPE_ACK);
	if (response_packet == NULL) {
		return ERR_BUF;
	}

	send_result = dhcps_send_message(dhcps, response_packet);

	pbuf_free(response_packet);

	return send_result;
}

/**
  * @brief  Update DHCP server state machine based on received message type.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  option_message_type: DHCP message type from client request.
  * @retval Updated DHCP server state (DHCP_SERVER_STATE_*).
  */
uint8_t dhcps_handle_state_machine_change(dhcps_t *dhcps, uint8_t option_message_type)
{
	switch (option_message_type) {
	case DHCP_MESSAGE_TYPE_DECLINE:
		DHCPS_LOG(DHCPS_DEBUG, "get message DHCP_MESSAGE_TYPE_DECLINE\r\n");
		dhcps->state_machine = DHCP_SERVER_STATE_IDLE;
		break;
	case DHCP_MESSAGE_TYPE_DISCOVER:
		DHCPS_LOG(DHCPS_DEBUG, "get message DHCP_MESSAGE_TYPE_DISCOVER\r\n");
		if (dhcps->state_machine == DHCP_SERVER_STATE_IDLE) {
			dhcps->state_machine = DHCP_SERVER_STATE_OFFER;
		}
		break;
	case DHCP_MESSAGE_TYPE_REQUEST:
		DHCPS_LOG(DHCPS_DEBUG, "[%d]get message DHCP_MESSAGE_TYPE_REQUEST\r\n", rtos_time_get_current_system_time_ms());
		DHCPS_LOG(DHCPS_DEBUG, "state_machine=%d\r\n", dhcps->state_machine);
		DHCPS_LOG(DHCPS_DEBUG, "allocated_client_address=%d.%d.%d.%d\r\n",
				  ip4_addr1(ip_2_ip4(&dhcps->allocated_client_address)), ip4_addr2(ip_2_ip4(&dhcps->allocated_client_address)),
				  ip4_addr3(ip_2_ip4(&dhcps->allocated_client_address)), ip4_addr4(ip_2_ip4(&dhcps->allocated_client_address)));
		DHCPS_LOG(DHCPS_DEBUG, "client_request_ip=%d.%d.%d.%d\r\n",
				  ip4_addr1(ip_2_ip4(&dhcps->client_request_ip)), ip4_addr2(ip_2_ip4(&dhcps->client_request_ip)),
				  ip4_addr3(ip_2_ip4(&dhcps->client_request_ip)), ip4_addr4(ip_2_ip4(&dhcps->client_request_ip)));

		// for renew
		{
#if 0
			if (((*(uint32_t *)((void *)dhcps->message_repository->ciaddr)) != 0) && ((*(uint32_t *)((void *)&dhcps->client_request_ip)) == 0)) {
				memcpy(&dhcps->client_request_ip, dhcps->message_repository->ciaddr, sizeof(dhcps->client_request_ip));
			}
#endif
#if 1
			//void * temp_repo = dhcps->message_repository->ciaddr;
			uint32_t temp_repo;
			memcpy(&temp_repo, dhcps->message_repository->ciaddr, sizeof(uint32_t));
			void *temp_ip = &dhcps->client_request_ip;
			if ((temp_repo != 0) && ((*(uint32_t *)(temp_ip)) == 0)) {
				memcpy(&dhcps->client_request_ip, dhcps->message_repository->ciaddr, sizeof(dhcps->client_request_ip));
			}
#endif
		}

		if (dhcps->state_machine == DHCP_SERVER_STATE_OFFER) {
			uint8_t ip_addr4 = check_client_direct_request_ip(dhcps, &dhcps->client_request_ip, dhcps->client_addr);

			if (ip_addr4 > 0) {
				IP4_ADDR(ip_2_ip4(&dhcps->allocated_client_address), (ip4_addr1(ip_2_ip4(&dhcps->network_id))),
						 ip4_addr2(ip_2_ip4(&dhcps->network_id)), ip4_addr3(ip_2_ip4(&dhcps->network_id)), ip_addr4);
				dhcps->state_machine = DHCP_SERVER_STATE_ACK;
				break;
			}

			if (ip4_addr4(ip_2_ip4(&dhcps->allocated_client_address)) != 0) {
				if (memcmp((void *)&dhcps->allocated_client_address, (void *)&dhcps->client_request_ip, 4) == 0) {
					dhcps->state_machine = DHCP_SERVER_STATE_ACK;
				} else {
					dhcps->state_machine = DHCP_SERVER_STATE_NAK;
				}
			} else {
				dhcps->state_machine = DHCP_SERVER_STATE_NAK;
			}
		} else if (dhcps->state_machine == DHCP_SERVER_STATE_IDLE) {
			uint8_t ip_addr4 = check_client_direct_request_ip(dhcps, &dhcps->client_request_ip, dhcps->client_addr);

			if (ip_addr4 > 0) {
				IP4_ADDR(ip_2_ip4(&dhcps->allocated_client_address), (ip4_addr1(ip_2_ip4(&dhcps->network_id))),
						 ip4_addr2(ip_2_ip4(&dhcps->network_id)), ip4_addr3(ip_2_ip4(&dhcps->network_id)), ip_addr4);
				dhcps->state_machine = DHCP_SERVER_STATE_ACK;
			} else {
				dhcps->state_machine = DHCP_SERVER_STATE_NAK;
			}
		} else {
			dhcps->state_machine = DHCP_SERVER_STATE_NAK;
		}
		break;
	case DHCP_MESSAGE_TYPE_RELEASE:
		DHCPS_LOG(DHCPS_INFO, "get message DHCP_MESSAGE_TYPE_RELEASE\r\n");
		dhcps->state_machine = DHCP_SERVER_STATE_IDLE;
		break;
	}

	return dhcps->state_machine;
}

/**
  * @brief  Parse DHCP message options and extract message type and requested IP.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  option_start: pointer to the first option byte after magic cookie.
  * @param  total_option_length: total length of all option fields.
  * @retval Updated DHCP server state after processing options.
  */
static uint8_t dhcps_handle_msg_options(dhcps_t *dhcps, uint8_t *option_start, int16_t total_option_length)
{

	int16_t option_message_type = 0;
	uint8_t *option_end = option_start + total_option_length;
	//dhcps->state_machine = DHCP_SERVER_STATE_IDLE;

	/* begin process the dhcp option info */
	while (option_start < option_end) {
		switch ((uint8_t)*option_start) {
		case DHCP_OPTION_CODE_MSG_TYPE:
			option_message_type = *(option_start + 2); // 2 => code(1)+lenth(1)
			break;
		case DHCP_OPTION_CODE_REQUEST_IP_ADDRESS :
			memcpy((char *)&dhcps->client_request_ip, (char *)option_start + 2, 4);
			break;
		}
		// calculate the options offset to get next option's base addr
		option_start += option_start[1] + 2; // optptr[1]: length value + (code(1)+ Len(1))
	}
	return dhcps_handle_state_machine_change(dhcps, option_message_type);
}

/**
  * @brief  Validate incoming UDP packet as DHCP message and process options.
  * @param  dhcps: pointer to the DHCP server instance.
  * @param  packet_buffer: pointer to the received UDP packet buffer.
  * @retval DHCP server state if valid DHCP message; 0 otherwise.
  */
static uint8_t dhcps_check_msg_and_handle_options(dhcps_t *dhcps, struct pbuf *packet_buffer)
{
	int dhcp_message_option_offset;
	dhcps->message_repository = (struct dhcp_msg *)packet_buffer->payload;
	dhcp_message_option_offset = ((int)dhcps->message_repository->options
								  - (int)packet_buffer->payload);
	dhcps->message_options_max_len = (packet_buffer->len
									  - dhcp_message_option_offset);
	memcpy(dhcps->client_addr, dhcps->message_repository->chaddr, 6);
	/* check the magic number,if correct parse the content of options */
	if (memcmp((char *)dhcps->message_repository->options,
			   (char *)dhcp_magic_cookie, sizeof(dhcp_magic_cookie)) == 0) {
		return dhcps_handle_msg_options(dhcps, &dhcps->message_repository->options[4],
										(dhcps->message_options_max_len - 4));
	}

	return 0;
}

/**
  * @brief  Handle incoming DHCP/UDP packets and send appropriate responses.
  * @param  arg: pointer to the DHCP server instance (dhcps_t *).
  * @param  udp_pcb: UDP protocol control block.
  * @param  udp_packet_buffer: received UDP packet buffer.
  * @param  sender_addr: sender IP address (unused).
  * @param  sender_port: sender port (should be DHCP_CLIENT_PORT).
  * @retval None.
  */
static void dhcps_receive_udp_packet_handler(void *arg, struct udp_pcb *udp_pcb,
		struct pbuf *udp_packet_buffer, struct ip_addr *sender_addr, uint16_t sender_port)
{
	/* To avoid gcc warnings */
	(void) sender_addr;
	(void) arg;
	err_t send_result = ERR_OK;

	int16_t total_length_of_packet_buffer;
	struct pbuf *merged_packet_buffer = NULL;

	if (udp_packet_buffer == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "UDP buffer NULL\r\n");
		return;
	}

	dhcps_t *dhcps = (dhcps_t *)arg;  // Get dhcps instance from arg

	dhcps->message_repository = (struct dhcp_msg *)udp_packet_buffer->payload;

	if (sender_port == DHCP_CLIENT_PORT) {
		if (LwIP_netif_get_idx(ip_current_input_netif()) == 0) {
			pbuf_free(udp_packet_buffer);
			return;
		}
		total_length_of_packet_buffer = udp_packet_buffer->tot_len;
		if (udp_packet_buffer->next != NULL) {
			merged_packet_buffer = pbuf_coalesce(udp_packet_buffer,
												 PBUF_TRANSPORT);
			if ((merged_packet_buffer->tot_len != total_length_of_packet_buffer) || (merged_packet_buffer == udp_packet_buffer)) {
				pbuf_free(merged_packet_buffer);
				return;
			}
			udp_packet_buffer = merged_packet_buffer;
		}
		switch (dhcps_check_msg_and_handle_options(dhcps, udp_packet_buffer)) {
		case  DHCP_SERVER_STATE_OFFER:
			DHCPS_LOG(DHCPS_DEBUG, "%s DHCP_SERVER_STATE_OFFER\r\n", __func__);
			dhcps_send_offer(dhcps, udp_packet_buffer);
			break;
		case DHCP_SERVER_STATE_ACK:
			DHCPS_LOG(DHCPS_DEBUG, "%s DHCP_SERVER_STATE_ACK\r\n", __func__);
			/*retry 20 times for alloc skb while softap TX UDP packet*/
			for (int retry = 1; retry < 20; retry++) {
				send_result = dhcps_send_ack(dhcps, udp_packet_buffer);
				if (send_result != ERR_OK) {
					rtos_time_delay_ms(10);//ms
				} else {
					break;
				}
			}

			mark_ip_in_table(dhcps, (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->allocated_client_address)));
			save_client_addr(dhcps, (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->allocated_client_address)), dhcps->client_addr);
			memset(&dhcps->client_request_ip, 0, sizeof(dhcps->client_request_ip));
			memset(&dhcps->client_addr, 0, sizeof(dhcps->client_addr));
			memset(&dhcps->allocated_client_address, 0, sizeof(dhcps->allocated_client_address));
			dump_client_table(dhcps);
			dhcps->state_machine = DHCP_SERVER_STATE_IDLE;
			break;
		case DHCP_SERVER_STATE_NAK:
			DHCPS_LOG(DHCPS_DEBUG, "%s DHCP_SERVER_STATE_NAK\r\n", __func__);
			dhcps_send_nak(dhcps, udp_packet_buffer);
			dhcps->state_machine = DHCP_SERVER_STATE_IDLE;
			break;
		case DHCP_OPTION_CODE_END:
			DHCPS_LOG(DHCPS_DEBUG, "%s DHCP_OPTION_CODE_END\r\n", __func__);
			break;
		}
	}

	/* free the UDP connection, so we can accept new clients */
	udp_disconnect(udp_pcb);

	/* Free the packet buffer */
	if (merged_packet_buffer != NULL) {
		pbuf_free(merged_packet_buffer);
	} else {
		pbuf_free(udp_packet_buffer);
	}
}

/**
  * @brief  Configure the IP address pool for DHCP allocation.
  * @param  pnetif: pointer to the network interface.
  * @param  addr_pool_set: 1=enable custom pool; 0=use unlimited range [2, 254].
  * @param  addr_pool_start: pointer to the start IP address structure (used when enabled=1).
  * @param  addr_pool_end: pointer to the end IP address structure (used when enabled=1).
  * @retval None.
  */
void dhcps_set_addr_pool(struct netif *pnetif, int addr_pool_set, struct ip_addr *addr_pool_start, struct ip_addr *addr_pool_end)
{
	dhcps_t *dhcps;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "DHCPS instance not found\r\n");
		return;
	}

	dhcps->addr_pool_set = addr_pool_set;

	if (addr_pool_set && addr_pool_start && addr_pool_end) {
		// Enable custom pool with specified range
		memcpy(&dhcps->addr_pool_start, addr_pool_start, sizeof(struct ip_addr));
		memcpy(&dhcps->addr_pool_end, addr_pool_end, sizeof(struct ip_addr));
		DHCPS_LOG(DHCPS_DEBUG, "Custom pool range: [%d-%d]\r\n",
				  ip4_addr4(ip_2_ip4(&dhcps->addr_pool_start)), ip4_addr4(ip_2_ip4(&dhcps->addr_pool_end)));
	} else if (!addr_pool_set) {
		// Disable pool limit, use unlimited range [2, 254]
		memcpy(&dhcps->addr_pool_start, &dhcps->local_address, sizeof(struct ip_addr));
		((uint8_t *)&dhcps->addr_pool_start)[3] = DHCP_UNLIMITED_POOL_START;

		memcpy(&dhcps->addr_pool_end, &dhcps->local_address, sizeof(struct ip_addr));
		((uint8_t *)&dhcps->addr_pool_end)[3] = DHCP_UNLIMITED_POOL_END;

		DHCPS_LOG(DHCPS_DEBUG, "Pool limit disabled, using unlimited range [%d-%d]\r\n",
				  DHCP_UNLIMITED_POOL_START, DHCP_UNLIMITED_POOL_END);
	}
}

/**
  * @brief  Handle incoming DNS query packets and generate responses.
  * @param  arg: pointer to the DHCP server instance (dhcps_t *).
  * @param  udp_pcb: UDP protocol control block.
  * @param  udp_packet_buffer: received DNS query packet buffer.
  * @param  sender_addr: sender IP address.
  * @param  sender_port: sender port.
  * @retval None.
  */
static void dnss_receive_udp_packet_handler(
	void *arg,
	struct udp_pcb *udp_pcb,
	struct pbuf *udp_packet_buffer,
	struct ip_addr *sender_addr,
	uint16_t sender_port)
{
	/* To avoid gcc warnings */
	(void) arg;

	dhcps_t *dhcps = (dhcps_t *)arg;

	struct dns_hdr *hdr = (struct dns_hdr *) udp_packet_buffer->payload;

#if(defined(CONFIG_ENABLE_CAPTIVE_PORTAL) && CONFIG_ENABLE_CAPTIVE_PORTAL)
	if (1) {
		uint8_t len = strlen((const char *) hdr + sizeof(struct dns_hdr)) + 1;
		struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dns_hdr) + len + 20, PBUF_RAM);
#else
	if (memcmp((uint8_t *) hdr + sizeof(struct dns_hdr), dhcps->domain_name_buf, dhcps->domain_name_buf_len) == 0) {
		DHCPS_LOG(DHCPS_DEBUG, " query %s\r\n", g_dhcps_domain_name);

		struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 20, PBUF_RAM);
#endif
		if (p) {
			struct dns_hdr *rsp_hdr = (struct dns_hdr *) p->payload;
			rsp_hdr->id = hdr->id;
			rsp_hdr->flags1 = 0x85;
			rsp_hdr->flags2 = 0x80;
			rsp_hdr->numquestions = PP_HTONS(1);
			rsp_hdr->numanswers = PP_HTONS(1);
			rsp_hdr->numauthrr = PP_HTONS(0);
			rsp_hdr->numextrarr = PP_HTONS(0);
#if(defined(CONFIG_ENABLE_CAPTIVE_PORTAL) && CONFIG_ENABLE_CAPTIVE_PORTAL)
			memcpy((uint8_t *) rsp_hdr + sizeof(struct dns_hdr), (uint8_t *) hdr + sizeof(struct dns_hdr), len - 1);
			*(uint8_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len - 1) = 0;
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len) = PP_HTONS(1);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 2) = PP_HTONS(1);
			*((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 4) = 0xc0;
			*((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 5) = 0x0c;
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 6) = PP_HTONS(1);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 8) = PP_HTONS(1);
			*(uint32_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 10) = PP_HTONL(0);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 14) = PP_HTONS(4);
			memcpy((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + len + 16, (void *)&dhcps->dhcps_local_address, 4);
#else
			memcpy((uint8_t *) rsp_hdr + sizeof(struct dns_hdr), dhcps->domain_name_buf, dhcps->domain_name_buf_len);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len) = PP_HTONS(1);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 2) = PP_HTONS(1);
			*((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 4) = 0xc0;
			*((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 5) = 0x0c;
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 6) = PP_HTONS(1);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 8) = PP_HTONS(1);
			*(uint32_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 10) = PP_HTONL(0);
			*(uint16_t *)((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 14) = PP_HTONS(4);
			memcpy((uint8_t *) rsp_hdr + sizeof(struct dns_hdr) + dhcps->domain_name_buf_len + 16, (void *)&dhcps->local_address, 4);
#endif

			udp_sendto(udp_pcb, p, sender_addr, sender_port);
			pbuf_free(p);
		}
	} else {
		struct dns_hdr *dns_rsp;

		/*
			printf("\n%s: Receive DNS query,ip = %d.%d.%d.%d, port = %d(%x)\n",__func__,\
					sender_addr->addr & 0xff,\
					sender_addr->addr>>8 & 0xff,\
					sender_addr->addr>>16 & 0xff,\
					sender_addr->addr>>24 & 0xff,\
					sender_port,sender_port);
		*/

		dns_rsp = (struct dns_hdr *) udp_packet_buffer->payload;

		dns_rsp->flags1 |= 0x80; // 0x80 : Response;
		dns_rsp->flags2 = 0x05;  //0x05 : Reply code (Query Refused)

		udp_sendto(udp_pcb, udp_packet_buffer, sender_addr, sender_port);
	}

	/* free the UDP connection, so we can accept new clients */
	udp_disconnect(udp_pcb);

	/* Free the packet buffer */
	pbuf_free(udp_packet_buffer);
}


/**
  * @brief  Initialize DNS server and bind to DNS_SERVER_PORT.
  * @param  dhcps: pointer to the DHCP server instance.
  * @retval ERR_OK: init success; other: init failure.
  */
err_t dns_server_init(dhcps_t *dhcps)
{
	const char *domain_name = g_dhcps_domain_name;
	uint8_t name_len = strlen(domain_name);
	uint8_t encoded_len = name_len + 1;

	// Convert domain name
	dhcps->domain_name_buf = (uint8_t *)rtos_mem_zmalloc(encoded_len);
	if (dhcps->domain_name_buf == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "Failed to malloc domain_name_buf\r\n");
		return ERR_MEM;
	}
	dhcps->domain_name_buf_len = encoded_len;

	uint8_t *ptr = dhcps->domain_name_buf;
	uint8_t idx, len = 0;
	for (idx = 0; idx < encoded_len; idx++) {
		if ((domain_name[idx] != '.') && (domain_name[idx] != 0)) {
			ptr[1 + len] = domain_name[idx];
			len++;
		} else {
			*ptr = len;
			ptr += (1 + len);
			len = 0;
		}
	}

	// Create DNS UDP PCB
	if (dhcps->dns_pcb != NULL) {
		udp_remove(dhcps->dns_pcb);
	}

	dhcps->dns_pcb = udp_new();
	if (dhcps->dns_pcb == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "udp_new error for dns_pcb\r\n");
		goto cleanup_dns_buf;
	}

	udp_bind(dhcps->dns_pcb, IP_ADDR_ANY, DNS_SERVER_PORT);
	udp_recv(dhcps->dns_pcb, (udp_recv_fn)dnss_receive_udp_packet_handler, dhcps);

	return ERR_OK;

cleanup_dns_buf:
	if (dhcps->domain_name_buf != NULL) {
		rtos_mem_free(dhcps->domain_name_buf);
		dhcps->domain_name_buf = NULL;
	}
	dhcps->domain_name_buf_len = 0;
	return ERR_BUF;
}

/**
  * @brief  Stop and cleanup DNS server resources.
  * @param  dhcps: pointer to the DHCP server instance.
  * @retval None.
  */
void dns_server_deinit(dhcps_t *dhcps)
{
	if (dhcps->domain_name_buf != NULL) {
		rtos_mem_free(dhcps->domain_name_buf);
		dhcps->domain_name_buf = NULL;
	}
	dhcps->domain_name_buf_len = 0;

	if (dhcps->dns_pcb != NULL) {
		udp_disconnect(dhcps->dns_pcb);
		udp_remove(dhcps->dns_pcb);
		dhcps->dns_pcb = NULL;
	}
}

/**
  * @brief  Initialize, configure DHCP server instance on specified network interface.
  * @param  pnetif: pointer to the network interface to bind.
  * @retval Pointer to the created DHCP server instance, or NULL on failure.
  */
dhcps_t *dhcps_init(struct netif *pnetif)
{
	dhcps_t *dhcps = NULL;

	// Check if already initialized
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps != NULL) {
		DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance already initialized\r\n");
		return dhcps;
	}

	// malloc dhcps_t structure
	dhcps = (dhcps_t *)rtos_mem_zmalloc(sizeof(dhcps_t));
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "Failed to malloc dhcps_t\r\n");
		return NULL;
	}

	// Initialize basic members
	dhcps->next = NULL;
	dhcps->dhcps_netif = pnetif;
	dhcps->state_machine = DHCP_SERVER_STATE_IDLE;
	memcpy(dhcps->recorded_xid, "\xff\xff\xff\xff", 4);

	// Initialize IP table (embedded in structure)
	memset(&dhcps->ip_table, 0, sizeof(struct table));
	dhcps->ip_table.client_mac = rtos_mem_zmalloc(wifi_user_config.ap_sta_num * 6 * sizeof(uint8_t));
	dhcps->ip_table.ip_addr4 = rtos_mem_zmalloc(wifi_user_config.ap_sta_num * sizeof(uint8_t));

	if (dhcps->ip_table.client_mac == NULL || dhcps->ip_table.ip_addr4 == NULL) {
		if (dhcps->ip_table.client_mac) {
			rtos_mem_free(dhcps->ip_table.client_mac);
		}
		if (dhcps->ip_table.ip_addr4) {
			rtos_mem_free(dhcps->ip_table.ip_addr4);
		}
		rtos_mem_free(dhcps);
		return NULL;
	}

//	int i = 0;
//	for(i=0; i< DHCPS_MAX_CLIENT_NUM+2; i++)
//		memset(dhcps->ip_table.client_mac[i], 0, 6);
//	dump_client_table();

	IP4_ADDR(ip_2_ip4(&dhcps->broadcast_address), 255, 255, 255, 255);

	// Get network interface information
	memcpy(&dhcps->local_address, &pnetif->ip_addr, sizeof(struct ip_addr));
	memcpy(&dhcps->local_mask, &pnetif->netmask, sizeof(struct ip_addr));
	memcpy(&dhcps->local_gateway, &pnetif->gw, sizeof(struct ip_addr));

	// Calculate network ID and broadcast address
	ip4_addr_set_u32(ip_2_ip4(&dhcps->network_id),
					 (ip_addr_get_ip4_u32(netif_ip_addr4(pnetif)) &
					  ip_addr_get_ip4_u32(netif_ip_netmask4(pnetif))));
	ip4_addr_set_u32(ip_2_ip4(&dhcps->subnet_broadcast),
					 (ip4_addr_get_u32(ip_2_ip4(&dhcps->network_id)) |
					  ~ip_addr_get_ip4_u32(netif_ip_netmask4(pnetif))));

	// Create mutex
	rtos_mutex_create(&dhcps->ip_table_sema);

	// Mark server IP and gateway as occupied
	mark_ip_in_table(dhcps, (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->local_address)));
	mark_ip_in_table(dhcps, (uint8_t)ip4_addr4(ip_2_ip4(&dhcps->local_gateway)));
#if 0
	for (i = 1; i < ip4_addr4(&dhcps->local_address); i++) {
		mark_ip_in_table(dhcps, i);
	}
#endif

	// Insert into list
	dhcps_list_insert(dhcps);

	// Set default address pool if not already set
	if (ip4_addr_get_u32(ip_2_ip4(&dhcps->addr_pool_start)) == 0 &&
		ip4_addr_get_u32(ip_2_ip4(&dhcps->addr_pool_end)) == 0) {
		ip_addr_t default_start, default_end;

		// Set default pool range [100, 200]
		memcpy(&default_start, &dhcps->local_address, sizeof(struct ip_addr));
		((uint8_t *)&default_start)[3] = DHCP_POOL_START;

		memcpy(&default_end, &dhcps->local_address, sizeof(struct ip_addr));
		((uint8_t *)&default_end)[3] = DHCP_POOL_END;

		dhcps_set_addr_pool(pnetif, 1, &default_start, &default_end);
	}

	DHCPS_LOG(DHCPS_DEBUG, "netif %c dhcps_init success\r\n", pnetif->name[1]);
	return dhcps;
}

/**
  * @brief  Start DHCP server service and begin accepting client requests.
  * @param  pnetif: pointer to the network interface.
  * @retval ERR_OK: success; other: start failure.
  */
err_t dhcps_start(struct netif *pnetif)
{
	dhcps_t *dhcps;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "DHCPS instance not found\r\n");
		return ERR_ARG;
	}

	// Create DHCP UDP PCB
	if (dhcps->dhcps_pcb != NULL) {
		udp_remove(dhcps->dhcps_pcb);
	}

	dhcps->dhcps_pcb = udp_new();
	if (dhcps->dhcps_pcb == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "udp_new error for dhcps_pcb\r\n");
		return ERR_MEM;
	}

	// Bind port and register callback
	udp_bind(dhcps->dhcps_pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
	udp_recv(dhcps->dhcps_pcb, (udp_recv_fn)dhcps_receive_udp_packet_handler, dhcps);

#ifndef IP_NAT
	// DNS server init
	err_t result = dns_server_init(dhcps);
	if (result != ERR_OK) {
		DHCPS_LOG(DHCPS_ERROR, "DNS server init failed %d\r\n", result);
		udp_remove(dhcps->dhcps_pcb);
		dhcps->dhcps_pcb = NULL;
		return result;
	}
#endif

	DHCPS_LOG(DHCPS_DEBUG, "netif %c dhcps_start success\r\n", pnetif->name[1]);
	return ERR_OK;
}

/**
  * @brief  Stop DHCP server service and release all UDP resources.
  * @param  pnetif: pointer to the netif.
  * @retval None.
  */
void dhcps_stop(struct netif *pnetif)
{
	dhcps_t *dhcps;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_ERROR, "DHCPS instance not found\r\n");
		return;
	}

	// Close UDP control blocks
	if (dhcps->dhcps_pcb != NULL) {
		udp_disconnect(dhcps->dhcps_pcb);
		udp_remove(dhcps->dhcps_pcb);
		dhcps->dhcps_pcb = NULL;
	}

	// Close DNS server
#ifndef IP_NAT
	dns_server_deinit(dhcps);
#endif

	DHCPS_LOG(DHCPS_DEBUG, "netif %c dhcps_stop success\r\n", pnetif->name[1]);
}

/**
  * @brief  Stop DHCP server and release all allocated resources.
  * @param  pnetif: pointer to the netif to unbind.
  * @retval None.
 */
void dhcps_deinit(struct netif *pnetif)
{
	dhcps_t *dhcps;

	// Get DHCP server instance from netif
	dhcps = dhcps_get_from_netif(pnetif);
	if (dhcps == NULL) {
		DHCPS_LOG(DHCPS_DEBUG, "DHCPS instance not found\r\n");
		return;
	}

	// If still running, stop first
	if (dhcps->dhcps_pcb != NULL) {
		dhcps_stop(pnetif);
	}

	// Delete mutex
	if (dhcps->ip_table_sema != NULL) {
		rtos_mutex_delete(dhcps->ip_table_sema);
		dhcps->ip_table_sema = NULL;
	}

	// Free IP table memory
	if (dhcps->ip_table.client_mac != NULL) {
		rtos_mem_free(dhcps->ip_table.client_mac);
		dhcps->ip_table.client_mac = NULL;
	}
	if (dhcps->ip_table.ip_addr4 != NULL) {
		rtos_mem_free(dhcps->ip_table.ip_addr4);
		dhcps->ip_table.ip_addr4 = NULL;
	}

	// Remove from list
	dhcps_list_remove(dhcps);

	// Free dhcps_t structure itself
	rtos_mem_free(dhcps);

	DHCPS_LOG(DHCPS_DEBUG, "netif %c dhcps_deinit success\r\n", pnetif->name[1]);
}
