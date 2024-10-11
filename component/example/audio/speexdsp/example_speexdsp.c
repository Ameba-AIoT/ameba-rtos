/* Copyright (C) 2007 Jean-Marc Valin

   File: testresample.c
   Testing the resampling code

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os_wrapper.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"

#include "speex/speex_resampler.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NN 256

const short Sine12bit[256] = {
	2048, 2098, 2148, 2198, 2248, 2298, 2348, 2398, 2447, 2496,
	2545, 2594, 2642, 2690, 2737, 2785, 2831, 2877, 2923, 2968,
	3013, 3057, 3100, 3143, 3185, 3227, 3267, 3307, 3347, 3385,
	3423, 3460, 3496, 3531, 3565, 3598, 3631, 3662, 3692, 3722,
	3750, 3778, 3804, 3829, 3854, 3877, 3899, 3920, 3940, 3958,
	3976, 3992, 4007, 4021, 4034, 4046, 4056, 4065, 4073, 4080,
	4086, 4090, 4093, 4095, 4095, 4095, 4093, 4090, 4086, 4080,
	4073, 4065, 4056, 4046, 4034, 4021, 4007, 3992, 3976, 3958,
	3940, 3920, 3899, 3877, 3854, 3829, 3804, 3778, 3750, 3722,
	3692, 3662, 3631, 3598, 3565, 3531, 3496, 3460, 3423, 3385,
	3347, 3307, 3267, 3227, 3185, 3143, 3100, 3057, 3013, 2968,
	2923, 2877, 2831, 2785, 2737, 2690, 2642, 2594, 2545, 2496,
	2447, 2398, 2348, 2298, 2248, 2198, 2148, 2098, 2047, 1997,
	1947, 1897, 1847, 1797, 1747, 1697, 1648, 1599, 1550, 1501,
	1453, 1405, 1358, 1310, 1264, 1218, 1172, 1127, 1082, 1038,
	995, 952, 910, 868, 828, 788, 748, 710, 672, 635,
	599, 564, 530, 497, 464, 433, 403, 373, 345, 317,
	291, 266, 241, 218, 196, 175, 155, 137, 119, 103,
	88, 74, 61, 49, 39, 30, 22, 15, 9, 5,
	2, 0, 0, 0, 2, 5, 9, 15, 22, 30,
	39, 49, 61, 74, 88, 103, 119, 137, 155, 175,
	196, 218, 241, 266, 291, 317, 345, 373, 403, 433,
	464, 497, 530, 564, 599, 635, 672, 710, 748, 788,
	828, 868, 910, 952, 995, 1038, 1082, 1127, 1172, 1218,
	1264, 1310, 1358, 1405, 1453, 1501, 1550, 1599, 1648, 1697,
	1747, 1797, 1847, 1897, 1947, 1997
};

void testresample(void *param)
{
	(void) param;
	spx_uint32_t i;
	float *fin, *fout;
	int count = 0;

	SpeexResamplerState *st = speex_resampler_init(1, 8000, 12000, 10, NULL);
	speex_resampler_set_rate(st, 96000, 44100);
	speex_resampler_skip_zeros(st);

	fin = malloc(NN * sizeof(float));
	fout = malloc(2 * NN * sizeof(float));

	while (1) {
		spx_uint32_t in_len;
		spx_uint32_t out_len;
		printf("[IN] = %d\n", NN);
		for (i = 0; i < NN; i++) {
			fin[i] = Sine12bit[i];
			printf(" %f ", fin[i]);
		}
		printf("\n");

		in_len = NN;
		out_len = 2 * NN;
		/*if (count==2)
		    speex_resampler_set_quality(st, 10);*/
		speex_resampler_process_float(st, 0, fin, &in_len, fout, &out_len);

		printf("[OUT] = %d\n", (int)out_len);
		for (i = 0; i < out_len; i++) {
			//out[i]=floor(.5+fout[i]);
			printf(" %f ", fout[i]);
		}
		printf("\n");
		count++;
	}
	speex_resampler_destroy(st);
	free(fin);
	free(fout);
}
