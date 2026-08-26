/*
 * P2P - generic helper functions
 * Copyright (c) 2009, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/defs.h"
#include "common/ieee802_11_common.h"
#include "p2p_i.h"


/**
 * p2p_random - Generate random string for SSID and passphrase
 * @buf: Buffer for returning the result
 * @len: Number of octets to write to the buffer
 * Returns: 0 on success, -1 on failure
 *
 * This function generates a random string using the following character set:
 * 'A'-'Z', 'a'-'z', '0'-'9'.
 */
int p2p_random(char *buf, size_t len)
{
	u8 val;
	size_t i;
	u8 letters = 'Z' - 'A' + 1;
	u8 numbers = 10;

	if (os_get_random((unsigned char *) buf, len)) {
		return -1;
	}
	/* Character set: 'A'-'Z', 'a'-'z', '0'-'9' */
	for (i = 0; i < len; i++) {
		val = buf[i];
		val %= 2 * letters + numbers;
		if (val < letters) {
			buf[i] = 'A' + val;
		} else if (val < 2 * letters) {
			buf[i] = 'a' + (val - letters);
		} else {
			buf[i] = '0' + (val - 2 * letters);
		}
	}

	return 0;
}


/**
 * p2p_channel_to_freq - Convert channel info to frequency
 * @op_class: Operating class
 * @channel: Channel number
 * Returns: Frequency in MHz or -1 if the specified channel is unknown
 */
int p2p_channel_to_freq(int op_class, int chan)
{
	/* Table E-4 in IEEE Std 802.11-2012 - Global operating classes */
	switch (op_class) {
	case 81:
		/* channels 1..13 */
		if (chan < 1 || chan > 13) {
			return -1;
		}
		return 2407 + 5 * chan;
	case 82:
		/* channel 14 */
		if (chan != 14) {
			return -1;
		}
		return 2414 + 5 * chan;
	case 83: /* channels 1..9; 40 MHz */
	case 84: /* channels 5..13; 40 MHz */
		if (chan < 1 || chan > 13) {
			return -1;
		}
		return 2407 + 5 * chan;
	case 115: /* channels 36,40,44,48; indoor only */
	case 116: /* channels 36,44; 40 MHz; indoor only */
	case 117: /* channels 40,48; 40 MHz; indoor only */
	case 118: /* channels 52,56,60,64; dfs */
	case 119: /* channels 52,60; 40 MHz; dfs */
	case 120: /* channels 56,64; 40 MHz; dfs */
		if (chan < 36 || chan > 64) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 121: /* channels 100-140 */
	case 122: /* channels 100-142; 40 MHz */
	case 123: /* channels 104-136; 40 MHz */
		if (chan < 100 || chan > 140) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 124: /* channels 149,153,157,161 */
		if (chan < 149 || chan > 161) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 125: /* channels 149,153,157,161,165,169,173,177 */
	case 126: /* channels 149,157,165,173; 40 MHz */
	case 127: /* channels 153,161,169,177; 40 MHz */
		if (chan < 149 || chan > 177) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 128: /* center freqs 42, 58, 106, 122, 138, 155, 171; 80 MHz */
	case 130: /* center freqs 42, 58, 106, 122, 138, 155, 171; 80 MHz */
		if (chan < 36 || chan > 177) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 129: /* center freqs 50, 114, 163; 160 MHz */
		if (chan < 36 || chan > 177) {
			return -1;
		}
		return 5000 + 5 * chan;
	case 131: /* UHB channels, 20 MHz: 1, 5, 9.. */
	case 132: /* UHB channels, 40 MHz: 3, 11, 19.. */
	case 133: /* UHB channels, 80 MHz: 7, 23, 39.. */
	case 134: /* UHB channels, 160 MHz: 15, 47, 79.. */
	case 135: /* UHB channels, 80+80 MHz: 7, 23, 39.. */
		if (chan < 1 || chan > 233) {
			return -1;
		}
		return 5950 + chan * 5;
	case 136: /* UHB channels, 20 MHz: 2 */
		if (chan == 2) {
			return 5935;
		}
		return -1;
	case 180: /* 60 GHz band, channels 1..8 */
		if (chan < 1 || chan > 8) {
			return -1;
		}
		return 56160 + 2160 * chan;
	case 181: /* 60 GHz band, EDMG CB2, channels 9..15 */
		if (chan < 9 || chan > 15) {
			return -1;
		}
		return 56160 + 2160 * (chan - 8);
	case 182: /* 60 GHz band, EDMG CB3, channels 17..22 */
		if (chan < 17 || chan > 22) {
			return -1;
		}
		return 56160 + 2160 * (chan - 16);
	case 183: /* 60 GHz band, EDMG CB4, channel 25..29 */
		if (chan < 25 || chan > 29) {
			return -1;
		}
		return 56160 + 2160 * (chan - 24);
	}
	return -1;
}


/**
 * p2p_chan_to_reg_class - Convert channel number to operating class
 * @chan: Channel number
 * Returns: Operating class or -1 if the specified channel is unknown
 *
 * Use this when only the channel a frame was received on is known, i.e. when
 * there is no operating class to pair it with. cfg->reg_class must not be used
 * for that, as it always describes the 2.4G listen channel while frames can be
 * received on any operating channel (e.g. Auto GO on 5G).
 *
 * The returned class is only meant to be fed back into p2p_channel_to_freq()
 * (which maps it back to the same frequency); it is not necessarily the class
 * negotiated in the P2P negotiation (e.g. 40/80/160 MHz classes).
 */
int p2p_chan_to_reg_class(u8 chan)
{
	if (chan >= 1 && chan <= 13) {
		return 81;
	}

	if (chan == 14) {
		return 82;
	}

	if (chan >= 36 && chan <= 177) {
		return 128;
	}

	return -1;
}


/**
 * p2p_freq_to_channel - Convert frequency into channel info
 * @op_class: Buffer for returning operating class
 * @channel: Buffer for returning channel number
 * Returns: 0 on success, -1 if the specified frequency is unknown
 */
int p2p_freq_to_channel(unsigned int freq, u8 *op_class, u8 *channel)
{
	/* TODO: more operating classes */
	if (freq >= 2412 && freq <= 2472) {
		*op_class = 81; /* 2.407 GHz, channels 1..13 */
		*channel = (freq - 2407) / 5;
		return 0;
	}

	if (freq == 2484) {
		*op_class = 82; /* channel 14 */
		*channel = 14;
		return 0;
	}

	if (freq >= 5180 && freq <= 5240) {
		*op_class = 115; /* 5 GHz, channels 36..48 */
		*channel = (freq - 5000) / 5;
		return 0;
	}

	if (freq >= 5260 && freq <= 5320) {
		*op_class = 118; /* 5 GHz, channels 52..64 */
		*channel = (freq - 5000) / 5;
		return 0;
	}

	if (freq >= 5500 && freq <= 5720) {
		*op_class = 121; /* 5 GHz, channels 100..144 */
		*channel = (freq - 5000) / 5;
		return 0;
	}

	if (freq >= 5745 && freq <= 5885) {
		if (freq <= 5805) { /* 5 GHz, channels 149..177 */
			*op_class = 124;
		} else {
			*op_class = 125;
		}

		*channel = (freq - 5000) / 5;
		return 0;
	}

	return -1;
}


static void p2p_reg_class_intersect(const struct p2p_reg_class *a,
									const struct p2p_reg_class *b,
									struct p2p_reg_class *res)
{
	size_t i, j;

	res->reg_class = a->reg_class;

	for (i = 0; i < a->channels; i++) {
		for (j = 0; j < b->channels; j++) {
			if (a->channel[i] != b->channel[j]) {
				continue;
			}
			res->channel[res->channels] = a->channel[i];
			res->channels++;
			if (res->channels == P2P_MAX_REG_CLASS_CHANNELS) {
				return;
			}
		}
	}
}


/**
 * p2p_channels_intersect - Intersection of supported channel lists
 * @a: First set of supported channels
 * @b: Second set of supported channels
 * @res: Data structure for returning the intersection of support channels
 *
 * This function can be used to find a common set of supported channels. Both
 * input channels sets are assumed to use the same country code. If different
 * country codes are used, the regulatory class numbers may not be matched
 * correctly and results are undefined.
 */
void p2p_channels_intersect(const struct p2p_channels *a,
							const struct p2p_channels *b,
							struct p2p_channels *res)
{
	size_t i, j;

	os_memset(res, 0, sizeof(*res));

	for (i = 0; i < a->reg_classes; i++) {
		const struct p2p_reg_class *a_reg = &a->reg_class[i];
		for (j = 0; j < b->reg_classes; j++) {
			const struct p2p_reg_class *b_reg = &b->reg_class[j];
			if (a_reg->reg_class != b_reg->reg_class) {
				continue;
			}
			p2p_reg_class_intersect(
				a_reg, b_reg,
				&res->reg_class[res->reg_classes]);
			if (res->reg_class[res->reg_classes].channels) {
				res->reg_classes++;
				if (res->reg_classes == P2P_MAX_REG_CLASSES) {
					return;
				}
			}
		}
	}
}


static void p2p_op_class_union(struct p2p_reg_class *cl,
							   const struct p2p_reg_class *b_cl)
{
	size_t i, j;

	for (i = 0; i < b_cl->channels; i++) {
		for (j = 0; j < cl->channels; j++) {
			if (b_cl->channel[i] == cl->channel[j]) {
				break;
			}
		}
		if (j == cl->channels) {
			if (cl->channels == P2P_MAX_REG_CLASS_CHANNELS) {
				return;
			}
			cl->channel[cl->channels++] = b_cl->channel[i];
		}
	}
}


/**
 * p2p_channels_union_inplace - Inplace union of channel lists
 * @res: Input data and place for returning union of the channel sets
 * @b: Second set of channels
 */
void p2p_channels_union_inplace(struct p2p_channels *res,
								const struct p2p_channels *b)
{
	size_t i, j;

	for (i = 0; i < res->reg_classes; i++) {
		struct p2p_reg_class *cl = &res->reg_class[i];
		for (j = 0; j < b->reg_classes; j++) {
			const struct p2p_reg_class *b_cl = &b->reg_class[j];
			if (cl->reg_class != b_cl->reg_class) {
				continue;
			}
			p2p_op_class_union(cl, b_cl);
		}
	}

	for (j = 0; j < b->reg_classes; j++) {
		const struct p2p_reg_class *b_cl = &b->reg_class[j];

		for (i = 0; i < res->reg_classes; i++) {
			struct p2p_reg_class *cl = &res->reg_class[i];
			if (cl->reg_class == b_cl->reg_class) {
				break;
			}
		}

		if (i == res->reg_classes) {
			if (res->reg_classes == P2P_MAX_REG_CLASSES) {
				return;
			}
			os_memcpy(&res->reg_class[res->reg_classes++],
					  b_cl, sizeof(struct p2p_reg_class));
		}
	}
}


/**
 * p2p_channels_union - Union of channel lists
 * @a: First set of channels
 * @b: Second set of channels
 * @res: Data structure for returning the union of channels
 */
void p2p_channels_union(const struct p2p_channels *a,
						const struct p2p_channels *b,
						struct p2p_channels *res)
{
	os_memcpy(res, a, sizeof(*res));
	p2p_channels_union_inplace(res, b);
}


void p2p_channels_remove_freqs(struct p2p_channels *chan,
							   const struct wpa_freq_range_list *list)
{
	size_t o, c;

	if (list == NULL) {
		return;
	}

	o = 0;
	while (o < chan->reg_classes) {
		struct p2p_reg_class *op = &chan->reg_class[o];

		c = 0;
		while (c < op->channels) {
			int freq = p2p_channel_to_freq(op->reg_class,
										   op->channel[c]);
			if (freq > 0 && freq_range_list_includes(list, freq)) {
				op->channels--;
				os_memmove(&op->channel[c],
						   &op->channel[c + 1],
						   op->channels - c);
			} else {
				c++;
			}
		}

		if (op->channels == 0) {
			chan->reg_classes--;
			os_memmove(&chan->reg_class[o], &chan->reg_class[o + 1],
					   (chan->reg_classes - o) *
					   sizeof(struct p2p_reg_class));
		} else {
			o++;
		}
	}
}


/**
 * p2p_channels_includes - Check whether a channel is included in the list
 * @channels: List of supported channels
 * @reg_class: Regulatory class of the channel to search
 * @channel: Channel number of the channel to search
 * Returns: 1 if channel was found or 0 if not
 */
int p2p_channels_includes(const struct p2p_channels *channels, u8 reg_class,
						  u8 channel)
{
	size_t i, j;
	for (i = 0; i < channels->reg_classes; i++) {
		const struct p2p_reg_class *reg = &channels->reg_class[i];
		if (reg->reg_class != reg_class) {
			continue;
		}
		for (j = 0; j < reg->channels; j++) {
			if (reg->channel[j] == channel) {
				return 1;
			}
		}
	}
	return 0;
}


int p2p_channels_includes_freq(const struct p2p_channels *channels,
							   unsigned int freq)
{
	size_t i, j;
	for (i = 0; i < channels->reg_classes; i++) {
		const struct p2p_reg_class *reg = &channels->reg_class[i];
		for (j = 0; j < reg->channels; j++) {
			if (p2p_channel_to_freq(reg->reg_class,
									reg->channel[j]) == (int) freq) {
				return 1;
			}
		}
	}
	return 0;
}


int p2p_supported_freq(struct p2p_data *p2p, unsigned int freq)
{
	u8 op_reg_class, op_channel;
	if (p2p_freq_to_channel(freq, &op_reg_class, &op_channel) < 0) {
		return 0;
	}
	return p2p_channels_includes(&p2p->cfg->channels, op_reg_class,
								 op_channel);
}


int p2p_supported_freq_go(struct p2p_data *p2p, unsigned int freq)
{
	u8 op_reg_class, op_channel;
	if (p2p_freq_to_channel(freq, &op_reg_class, &op_channel) < 0) {
		return 0;
	}
	return p2p_channels_includes(&p2p->cfg->channels, op_reg_class,
								 op_channel) &&
		   !freq_range_list_includes(&p2p->no_go_freq, freq);
}


int p2p_supported_freq_cli(struct p2p_data *p2p, unsigned int freq)
{
	u8 op_reg_class, op_channel;
	if (p2p_freq_to_channel(freq, &op_reg_class, &op_channel) < 0) {
		return 0;
	}
	return p2p_channels_includes(&p2p->cfg->channels, op_reg_class,
								 op_channel) ||
		   p2p_channels_includes(&p2p->cfg->cli_channels, op_reg_class,
								 op_channel);
}


unsigned int p2p_get_pref_freq(struct p2p_data *p2p,
							   const struct p2p_channels *channels)
{
	unsigned int i;
	int freq = 0;
	const struct p2p_channels *tmpc = channels ?
										  channels : &p2p->cfg->channels;

	if (tmpc == NULL) {
		return 0;
	}

	for (i = 0; p2p->cfg->pref_chan && i < p2p->cfg->num_pref_chan; i++) {
		freq = p2p_channel_to_freq(p2p->cfg->pref_chan[i].op_class,
								   p2p->cfg->pref_chan[i].chan);
		if (p2p_channels_includes_freq(tmpc, freq)) {
			return freq;
		}
	}
	return 0;
}


void p2p_channels_dump(struct p2p_data *p2p, const char *title,
					   const struct p2p_channels *chan)
{
	char *buf, *pos, *end;
	size_t i, j;
	int ret;

	(void) p2p;
	(void) title;

	buf = rtos_mem_zmalloc(500);
	if (!buf) {
		return;
	}

	pos = buf;
	end = pos + 500;

	for (i = 0; i < chan->reg_classes; i++) {
		const struct p2p_reg_class *c;
		c = &chan->reg_class[i];
		ret = os_snprintf(pos, end - pos, " %u:", c->reg_class);
		if (os_snprintf_error(end - pos, ret)) {
			break;
		}
		pos += ret;

		for (j = 0; j < c->channels; j++) {
			ret = os_snprintf(pos, end - pos, "%s%u",
							  j == 0 ? "" : ",",
							  c->channel[j]);
			if (os_snprintf_error(end - pos, ret)) {
				break;
			}
			pos += ret;
		}
	}
	*pos = '\0';

	p2p_dbg(p2p, "%s:%s", title, buf);

	rtos_mem_free(buf);
}


static u8 p2p_channel_pick_random(const u8 *channels, unsigned int num_channels)
{
	unsigned int r;
	if (os_get_random((u8 *) &r, sizeof(r)) < 0) {
		r = 0;
	}
	r %= num_channels;
	return channels[r];
}


int p2p_channel_select(struct p2p_channels *chans, const int *classes,
					   u8 *op_class, u8 *op_channel)
{
	unsigned int i, j;

	for (j = 0; classes == NULL || classes[j]; j++) {
		for (i = 0; i < chans->reg_classes; i++) {
			struct p2p_reg_class *c = &chans->reg_class[i];

			if (c->channels == 0) {
				continue;
			}

			if (classes == NULL || c->reg_class == classes[j]) {
				/*
				 * Pick one of the available channels in the
				 * operating class at random.
				 */
				*op_class = c->reg_class;
				*op_channel = p2p_channel_pick_random(
								  c->channel, c->channels);
				return 0;
			}
		}
		if (classes == NULL) {
			break;
		}
	}

	return -1;
}


int p2p_channel_random_social(struct p2p_channels *chans, u8 *op_class,
							  u8 *op_channel,
							  struct wpa_freq_range_list *avoid_list,
							  struct wpa_freq_range_list *disallow_list)
{
	u8 chan[4];
	unsigned int num_channels = 0;

	/* Try to find available social channels from 2.4 GHz.
	 * If the avoid_list includes any of the 2.4 GHz social channels, that
	 * channel is not allowed by p2p_channels_includes() rules. However, it
	 * is assumed to allow minimal traffic for P2P negotiation, so allow it
	 * here for social channel selection unless explicitly disallowed in the
	 * disallow_list. */
	if (p2p_channels_includes(chans, 81, 1) ||
		(freq_range_list_includes(avoid_list, 2412) &&
		 !freq_range_list_includes(disallow_list, 2412))) {
		chan[num_channels++] = 1;
	}
	if (p2p_channels_includes(chans, 81, 6) ||
		(freq_range_list_includes(avoid_list, 2437) &&
		 !freq_range_list_includes(disallow_list, 2437))) {
		chan[num_channels++] = 6;
	}
	if (p2p_channels_includes(chans, 81, 11) ||
		(freq_range_list_includes(avoid_list, 2462) &&
		 !freq_range_list_includes(disallow_list, 2462))) {
		chan[num_channels++] = 11;
	}

	/* Try to find available social channels from 60 GHz */
	if (p2p_channels_includes(chans, 180, 2)) {
		chan[num_channels++] = 2;
	}

	if (num_channels == 0) {
		return -1;
	}

	*op_channel = p2p_channel_pick_random(chan, num_channels);
	if (*op_channel == 2) {
		*op_class = 180;
	} else {
		*op_class = 81;
	}

	return 0;
}


int p2p_channels_to_freqs(const struct p2p_channels *channels, int *freq_list,
						  unsigned int max_len)
{
	unsigned int i, idx;

	if (!channels || max_len == 0) {
		return 0;
	}

	for (i = 0, idx = 0; i < channels->reg_classes; i++) {
		const struct p2p_reg_class *c = &channels->reg_class[i];
		unsigned int j;

		if (idx + 1 == max_len) {
			break;
		}
		for (j = 0; j < c->channels; j++) {
			int freq;
			unsigned int k;

			if (idx + 1 == max_len) {
				break;
			}
			freq = p2p_channel_to_freq(c->reg_class,
									   c->channel[j]);
			if (freq < 0) {
				continue;
			}

			for (k = 0; k < idx; k++) {
				if (freq_list[k] == freq) {
					break;
				}
			}

			if (k < idx) {
				continue;
			}
			freq_list[idx++] = freq;
		}
	}

	freq_list[idx] = 0;

	return idx;
}


void p2p_copy_channels(struct p2p_channels *dst,
					   const struct p2p_channels *src)
{
	size_t i, j;

	for (i = 0, j = 0; i < src->reg_classes; i++) {
		os_memcpy(&dst->reg_class[j], &src->reg_class[i],
				  sizeof(struct p2p_reg_class));
		j++;
	}
	dst->reg_classes = j;
}
