/**
 * @file dns_proxy_parser.c
 * @brief DNS Message Parser implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_PARSER"
#endif

/* DNS Header structure (12 bytes) */
typedef struct __attribute__((packed)) dns_header {
	uint16_t txid;		/**< Transaction ID */
	uint16_t flags;		/**< Flags */
	uint16_t qdcount;	/**< Question count */
	uint16_t ancount;	/**< Answer count */
	uint16_t nscount;	/**< Authority record count */
	uint16_t arcount;	/**< Additional record count */
} dns_header_t;

/* DNS Question structure */
typedef struct __attribute__((packed)) dns_question {
	uint16_t qtype;		/**< Query type (A=1, AAAA=28, etc.) */
	uint16_t qclass;	/**< Query class (IN=1) */
} dns_question_t;

/**
 * @brief  Parse domain name from DNS message
 * @param  msg DNS message buffer
 * @param  msg_len Message length
 * @param  offset Current offset in message
 * @param  out_domain Output: Parsed domain name
 * @param  out_new_offset Output: New offset after parsing
 * @return 0 on success, negative on error
 * @note   Handles DNS name compression
 */
static int parse_domain_name(const uint8_t *msg, uint16_t msg_len,
							 uint16_t offset, char *out_domain,
							 uint16_t *out_new_offset)
{
	uint16_t pos = offset;
	uint16_t domain_pos = 0;
	uint8_t label_len;
	bool compressed = false;
	uint16_t jump_count = 0;

	if (!msg || !out_domain || !out_new_offset) {
		return DNS_PROXY_ERR_INVAL;
	}

	out_domain[0] = '\0';

	while (pos < msg_len) {
		label_len = msg[pos];

		/* Check for end of domain name */
		if (label_len == 0) {
			if (!compressed) {
				pos++; /* Skip the zero byte */
			}
			break;
		}

		/* Check for compression pointer (top 2 bits set) */
		if ((label_len & 0xC0) == 0xC0) {
			if (pos + 1 >= msg_len) {
				return DNS_PROXY_ERR_INVAL;
			}

			/* Jump to compressed location */
			uint16_t new_pos = ((label_len & 0x3F) << 8) | msg[pos + 1];
			if (new_pos >= msg_len) {
				return DNS_PROXY_ERR_INVAL;
			}

			if (!compressed) {
				*out_new_offset = pos + 2;
			}

			pos = new_pos;
			compressed = true;

			/* Prevent infinite loops */
			jump_count++;
			if (jump_count > 10) {
				return DNS_PROXY_ERR_INVAL;
			}
			continue;
		}

		/* Regular label */
		pos++;

		if (pos + label_len > msg_len) {
			return DNS_PROXY_ERR_INVAL;
		}

		/* Add dot separator (except for first label) */
		if (domain_pos > 0) {
			if (domain_pos < DNS_MAX_DOMAIN_LEN - 1) {
				out_domain[domain_pos++] = '.';
			}
		}

		/* Copy label */
		if (domain_pos + label_len < DNS_MAX_DOMAIN_LEN - 1) {
			memcpy(&out_domain[domain_pos], &msg[pos], label_len);
			domain_pos += label_len;
		}

		pos += label_len;
	}

	out_domain[domain_pos] = '\0';

	if (!compressed) {
		*out_new_offset = pos;
	}

	return DNS_PROXY_OK;
}

/**
 * @brief  Extract Transaction ID from DNS message header
 * @param  msg DNS message buffer
 * @param  msg_len Message length
 * @param  out_txid Output: Transaction ID
 * @return 0 on success, negative on error
 */
int dns_parser_get_txid(const uint8_t *msg, uint16_t msg_len, uint16_t *out_txid)
{
	dns_header_t *header;

	if (!msg || !out_txid || msg_len < sizeof(dns_header_t)) {
		return DNS_PROXY_ERR_INVAL;
	}

	header = (dns_header_t *)msg;
	*out_txid = ntohs(header->txid);
	return DNS_PROXY_OK;
}

/**
 * @brief  Extract question section from DNS message
 * @param  msg DNS message buffer
 * @param  msg_len Message length
 * @param  out_txid Output: Transaction ID
 * @param  out_domain Output: Queried domain name
 * @param  out_qtype Output: Query type
 * @return 0 on success, negative on error
 */
int dns_parser_extract_question(const uint8_t *msg, uint16_t msg_len,
								uint16_t *out_txid, char *out_domain,
								uint16_t *out_qtype)
{
	dns_header_t *header;
	uint16_t offset;
	dns_question_t *question;

	if (!msg || !out_txid || !out_domain || !out_qtype) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Validate minimum message length */
	if (msg_len < sizeof(dns_header_t)) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS message too short: %d\n", msg_len);
		return DNS_PROXY_ERR_INVAL;
	}

	/* Parse header */
	header = (dns_header_t *)msg;
	*out_txid = ntohs(header->txid);

	/* Check if it's a query (QR bit = 0) */
	if (ntohs(header->flags) & 0x8000) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Not a DNS query (QR bit set)\n");
		return DNS_PROXY_ERR_INVAL;
	}

	/* Check question count */
	if (ntohs(header->qdcount) != 1) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Invalid question count: %d\n", ntohs(header->qdcount));
		return DNS_PROXY_ERR_INVAL;
	}

	/* Parse question section */
	offset = sizeof(dns_header_t);

	if (parse_domain_name(msg, msg_len, offset, out_domain, &offset) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to parse domain name\n");
		return DNS_PROXY_ERR_INVAL;
	}

	/* Parse QTYPE and QCLASS */
	if (offset + sizeof(dns_question_t) > msg_len) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS message truncated at question section\n");
		return DNS_PROXY_ERR_INVAL;
	}

	question = (dns_question_t *)&msg[offset];
	*out_qtype = ntohs(question->qtype);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Question extracted: txid=0x%x, domain=%s, qtype=%d\n",
				  *out_txid, out_domain, *out_qtype);

	return DNS_PROXY_OK;
}

/**
 * @brief  Extract A record IP and TTL from DNS answer section
 * @param  msg DNS message buffer
 * @param  msg_len Message length
 * @param  out_ip Output: Resolved IP address
 * @param  out_ttl Output: TTL value in seconds
 * @return 0 on success, negative on error
 * @note   Only extracts first A record (QTYPE=1)
 */
int dns_parser_extract_a_record(const uint8_t *msg, uint16_t msg_len,
								ip_addr_t *out_ip, uint32_t *out_ttl)
{
	dns_header_t *header;
	uint16_t offset;
	uint16_t ancount;
	char domain[DNS_MAX_DOMAIN_LEN];
	uint16_t new_offset;
	uint16_t rtype;
	uint16_t rclass;
	uint32_t ttl;
	uint16_t rdlength;
	uint8_t rcode;

	if (!msg || !out_ip) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Validate minimum message length */
	if (msg_len < sizeof(dns_header_t)) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Parse header */
	header = (dns_header_t *)msg;

	/* Check RCODE — non-zero means DNS-level error (NXDOMAIN, SERVFAIL, etc.) */
	rcode = ntohs(header->flags) & 0x000F;
	if (rcode != 0) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS error response: RCODE=%d\n", rcode);
		return DNS_PROXY_ERR_DNS_RCODE;
	}

	ancount = ntohs(header->ancount);

	if (ancount == 0) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "No answer records in DNS response\n");
		return DNS_PROXY_ERR_INVAL;
	}

	/* Skip question section */
	offset = sizeof(dns_header_t);

	/* Skip question domain name */
	if (parse_domain_name(msg, msg_len, offset, domain, &offset) != DNS_PROXY_OK) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Skip QTYPE and QCLASS (4 bytes) */
	offset += 4;

	/* Parse answer records */
	for (uint16_t i = 0; i < ancount; i++) {
		if (offset >= msg_len) {
			return DNS_PROXY_ERR_INVAL;
		}

		/* Skip answer domain name (may use compression) */
		if (parse_domain_name(msg, msg_len, offset, domain, &new_offset) != DNS_PROXY_OK) {
			return DNS_PROXY_ERR_INVAL;
		}
		offset = new_offset;

		/* Check remaining length for fixed fields */
		if (offset + 10 > msg_len) {
			return DNS_PROXY_ERR_INVAL;
		}

		/* Parse TYPE, CLASS, TTL, RDLENGTH */
		rtype = (msg[offset] << 8) | msg[offset + 1];
		rclass = (msg[offset + 2] << 8) | msg[offset + 3];
		ttl = ((uint32_t)msg[offset + 4] << 24) | ((uint32_t)msg[offset + 5] << 16) |
			  ((uint32_t)msg[offset + 6] << 8)  | (uint32_t)msg[offset + 7];
		rdlength = (msg[offset + 8] << 8) | msg[offset + 9];

		offset += 10;

		/* Check if this is an A record */
		if (rtype == 1 && rclass == 1) { /* TYPE=A, CLASS=IN */
			if (rdlength != 4) {
				DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Invalid A record length: %d\n", rdlength);
				return DNS_PROXY_ERR_INVAL;
			}

			if (offset + 4 > msg_len) {
				return DNS_PROXY_ERR_INVAL;
			}

			/* Extract IP address */
			IP4_ADDR(out_ip, msg[offset], msg[offset + 1],
					 msg[offset + 2], msg[offset + 3]);

			/* Return TTL if requested */
			if (out_ttl) {
				*out_ttl = ttl;
			}

			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "A record extracted: %s (TTL: %d)\n",
						  ipaddr_ntoa(out_ip), (unsigned long)ttl);

			return DNS_PROXY_OK;
		}

		/* Skip RDATA */
		offset += rdlength;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "No A record found in DNS response\n");
	return DNS_PROXY_ERR_NOTFOUND;
}

/**
 * @brief  Construct DNS response from cache
 * @param  query_msg Original DNS query message
 * @param  query_len Query message length
 * @param  cached_ip Cached IP address
 * @param  ttl TTL value
 * @param  out_response Output: Response buffer (must be freed by caller)
 * @param  out_response_len Output: Response length
 * @return 0 on success, negative on error
 * @note   Allocates memory for response, caller must free
 */
int dns_parser_construct_response(const uint8_t *query_msg, uint16_t query_len,
								  const ip_addr_t *cached_ip, uint32_t ttl,
								  uint8_t **out_response, uint16_t *out_response_len)
{
	dns_header_t *resp_header;
	uint8_t *response;
	uint16_t resp_len;
	uint16_t question_end;
	uint8_t *answer_ptr;

	if (!query_msg || !cached_ip || !out_response || !out_response_len) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Validate query */
	if (query_len < sizeof(dns_header_t) + 4) { /* header + minimum question */
		return DNS_PROXY_ERR_INVAL;
	}

	/* Calculate response length: query + answer section */
	/* Answer section: domain pointer (2) + TYPE(2) + CLASS(2) + TTL(4) + RDLENGTH(2) + RDATA(4) = 16 */
	if (query_len > (uint16_t)(0xFFFF - 16)) {
		return DNS_PROXY_ERR_INVAL;
	}
	resp_len = query_len + 16;

	/* Allocate response buffer */
	response = (uint8_t *)rtos_mem_malloc(resp_len);
	if (!response) {
		return DNS_PROXY_ERR_NOMEM;
	}

	/* Copy query to response */
	memcpy(response, query_msg, query_len);

	/* Modify header for response */
	resp_header = (dns_header_t *)response;
	resp_header->flags = htons(0x8180); /* QR=1, OPCODE=0, AA=0, TC=0, RD=1, RA=1, Z=0, RCODE=0 */
	resp_header->qdcount = htons(1);
	resp_header->ancount = htons(1);
	resp_header->nscount = 0;
	resp_header->arcount = 0;

	/* Find end of question section */
	question_end = sizeof(dns_header_t);

	/* Skip domain name in question (may use compression or be full name) */
	uint16_t pos = question_end;
	while (pos < query_len) {
		uint8_t label_len = query_msg[pos];

		if (label_len == 0) {
			pos++;
			break;
		}

		if ((label_len & 0xC0) == 0xC0) {
			pos += 2;
			break;
		}

		pos += 1 + label_len;
	}

	/* Skip QTYPE and QCLASS */
	pos += 4;
	question_end = pos;

	/* Construct answer section */
	answer_ptr = &response[question_end];

	/* Use compression pointer to domain name in question */
	answer_ptr[0] = 0xC0;
	answer_ptr[1] = 0x0C; /* Pointer to offset 12 (start of question domain) */
	answer_ptr += 2;

	/* TYPE = A (1) */
	answer_ptr[0] = 0x00;
	answer_ptr[1] = 0x01;
	answer_ptr += 2;

	/* CLASS = IN (1) */
	answer_ptr[0] = 0x00;
	answer_ptr[1] = 0x01;
	answer_ptr += 2;

	/* TTL */
	answer_ptr[0] = (ttl >> 24) & 0xFF;
	answer_ptr[1] = (ttl >> 16) & 0xFF;
	answer_ptr[2] = (ttl >> 8) & 0xFF;
	answer_ptr[3] = ttl & 0xFF;
	answer_ptr += 4;

	/* RDLENGTH = 4 (IPv4 address) */
	answer_ptr[0] = 0x00;
	answer_ptr[1] = 0x04;
	answer_ptr += 2;

	/* RDATA = IP address */
	answer_ptr[0] = ip4_addr1(cached_ip);
	answer_ptr[1] = ip4_addr2(cached_ip);
	answer_ptr[2] = ip4_addr3(cached_ip);
	answer_ptr[3] = ip4_addr4(cached_ip);

	*out_response = response;
	*out_response_len = resp_len;

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Response constructed: %d bytes\n", resp_len);
	return DNS_PROXY_OK;
}

/**
 * @brief  Modify transaction ID in DNS message
 * @param  msg DNS message buffer
 * @param  msg_len Message length
 * @param  new_txid New transaction ID
 * @return 0 on success, negative on error
 */
int dns_parser_modify_txid(uint8_t *msg, uint16_t msg_len, uint16_t new_txid)
{
	dns_header_t *header;

	if (!msg || msg_len < sizeof(dns_header_t)) {
		return DNS_PROXY_ERR_INVAL;
	}

	header = (dns_header_t *)msg;
	header->txid = htons(new_txid);

	return DNS_PROXY_OK;
}
