/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
*****************************************************************************/
#include "rtw_wifi_constants.h"

void wifi_hal_speaker_txrpt_parse(u8 rpt_idx)
{
	UNUSED(rpt_idx);
}

void wifi_hal_speaker_txrpt_fetch(void)
{
}

void wifi_hal_speaker_setting(u8 mode, u8 nav_thresh, u8 relay_enable)
{
	UNUSED(mode);
	UNUSED(nav_thresh);
	UNUSED(relay_enable);
}

u32 wifi_hal_speaker_tsf_interrupt(void *param)
{
	UNUSED(param);
	return 0;
}

void wifi_hal_speaker_audio_latch_count(u8 latch_port, u8 latch_period)
{
	UNUSED(latch_port);
	UNUSED(latch_period);
}

u32 wifi_hal_speaker_dma_interrupt(void *param)
{
	UNUSED(param);
	return 0;
}

void wifi_hal_async_i2s_tsf_init(void)
{
}

void wifi_hal_async_init(u8 mode)
{
	UNUSED(mode);
}

void wifi_hal_async_set_tsf_period(u32 period)
{
	UNUSED(period);
}

u64 wifi_hal_async_get_wmac_tsf_ns(void)
{
	return 0;
}

u64 wifi_hal_async_get_i2s_tsf_counter(void)
{
	return 0;
}

void wifi_hal_async_set_drift(u32 drift)
{
	UNUSED(drift);
}

void wifi_hal_async_register_interrupt_irq(void (*callback)(uint))
{
	UNUSED(callback);
}

void wifi_hal_speaker_set_tsf_timer(u64 tsft, u8 port)
{
	UNUSED(tsft);
	UNUSED(port);
}