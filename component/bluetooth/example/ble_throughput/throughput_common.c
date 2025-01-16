/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <throughput_include.h>

void ble_throughput_inc_sequence_generate(uint8_t *data, uint8_t data_bytes)
{
	uint8_t i = 0;
	uint8_t *pointer = data;
	while (data_bytes) {
		*pointer++ = i++;
		data_bytes--;
	}
}

void ble_throughput_check_calcu(throughput_result_t *p_result,
								uint16_t valid_len,
								uint16_t write_len,
								uint8_t *p_data,
								uint8_t count)
{
	uint8_t i = 0;

	if (valid_len != write_len) {
		p_result->rx_error_pkts++;
		return;
	}

	while (i < count) {
		if (p_data[i] != i) {
			p_result->rx_error_pkts++;
			return;
		}
		i++;
	}

	if (i == count) {
		p_result->rx_period_pkts++;
		p_result->rx_all_pkts++;
	}
}

uint32_t ble_throughput_get_elapsed_time(uint32_t start_time, uint32_t end_time)
{
#if 1
	if (end_time > start_time) {
		return end_time - start_time;
	} else {
		return (0x7FFFFFFF - (start_time - end_time));
	}
#else
	uint32_t expire_count;

	if (start_time <= end_time) {
		expire_count = end_time - start_time;
	} else {
		expire_count = 0xFFFFFFFF - (start_time - end_time);
	}
	return expire_count;
#endif
}

throughput_tx_end_t ble_throughput_get_tx_end(throughput_test_mode_t mode)
{
	return (mode & 0x0F) ? ((mode & 0XF0) ? THROUGHPUT_TX_END_SERVER_CLIENT : THROUGHPUT_TX_END_SERVER) : THROUGHPUT_TX_END_CLIENT;
}