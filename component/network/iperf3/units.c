/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * stdio.c
 * by Mark Gates <mgates@nlanr.net>
 * and Ajay Tirumalla <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * input and output numbers, converting with kilo, mega, giga, tera
 * ------------------------------------------------------------------- */

#include <assert.h>
#include <ctype.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "float.h"
#include <sys/time.h>

#include "lwip_netconf.h" //realtek add

#include "iperf.h"

#ifdef __cplusplus
extern    "C"
{
#endif

const double KILO_UNIT = 1024.0;
const double MEGA_UNIT = 1024.0 * 1024.0;
const double GIGA_UNIT = 1024.0 * 1024.0 * 1024.0;
const double TERA_UNIT = 1024.0 * 1024.0 * 1024.0 * 1024.0;

const double KILO_RATE_UNIT = 1000.0;
const double MEGA_RATE_UNIT = 1000.0 * 1000.0;
const double GIGA_RATE_UNIT = 1000.0 * 1000.0 * 1000.0;
const double TERA_RATE_UNIT = 1000.0 * 1000.0 * 1000.0 * 1000.0;

#include "log.h"
double atof_ss(const char *str)
{
	double result = 0.0;
	int sign = 1;
	bool is_fraction = 0;
	int fraction_exponent = 1;
	char c;
	int seen_digit = 0;

	// 跳过空白字符
	while (isspace((unsigned char)*str)) {
		str++;
	}

	// 处理正负号
	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}

	// 转换数字
	while (*str) {
		c = *str;
		if (isdigit((unsigned char)c)) {
			seen_digit = 1;
			if (is_fraction) {
				fraction_exponent *= 10;
			}
			result = result * 10 + (c - '0');
		} else if (c == '.') {
			if (is_fraction) {
				return DBL_MAX;
			}
			is_fraction = 1;
		} else {
			return DBL_MAX;
		}
		str++;
	}

	if (!seen_digit) {
		return DBL_MAX;
	}

#if 0
	// 处理指数
	if (*str == 'e' || *str == 'E') {
		str++;
		int exponent_sign = 1;
		if (*str == '-') {
			exponent_sign = -1;
			str++;
		} else if (*str == '+') {
			str++;
		}
		int exponent = 0;
		while (isdigit(*str)) {
			exponent = exponent * 10 + (*str - '0');
			str++;
		}
		for (int i = 0; i < exponent; i++) {
			result *= (exponent_sign == 1) ? 10.0 : 0.1;
		}
	}

	return sign * result * pow(10, -fraction_exponent);
#else
	return sign * result / fraction_exponent;
#endif

}

static int atof_and_one_char(const char *str, double *float_val, char *one_char)
{
	double result = 0.0;
	int sign = 1;
	bool is_fraction = 0;
	int fraction_exponent = 1;
	char c;
	int seen_digit = 0;

	// 跳过空白字符
	while (isspace((unsigned char)*str)) {
		str++;
	}

	// 处理正负号
	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}

	// 转换数字
	while (*str) {
		c = *str;
		if (isdigit((unsigned char)c)) {
			seen_digit = 1;
			if (is_fraction) {
				fraction_exponent *= 10;
			}
			result = result * 10 + (c - '0');
		} else if (c == '.') {
			if (is_fraction) {
				return -1;
			}
			is_fraction = 1;
		} else if (isalpha((unsigned char)c) && *(str + 1) == '\0') {
			*one_char = c;
			break;
		} else {
			return -1;
		}
		str++;
	}
	if (!seen_digit) {
		return -1;
	}

	*float_val = sign * result / fraction_exponent;
	return 0;
}

/* -------------------------------------------------------------------
 * unit_atof
 *
 * Given a string of form #x where # is a number and x is a format
 * character listed below, this returns the interpreted integer.
 * Gg, Mm, Kk are giga, mega, kilo respectively
 * ------------------------------------------------------------------- */

double    unit_atof(const char *s)
{
	double    n;
	char      suffix = '\0';

	if (s == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s s == NULL\n", __FUNCTION__);
		return DBL_MAX;
	}

	/* scan the number and any suffices */
	if (atof_and_one_char(s, &n, &suffix) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s format err\n", __FUNCTION__);
		return DBL_MAX;
	}

	/* convert according to [Tt Gg Mm Kk] */
	switch (suffix) {
	case 't':
	case 'T':
		n *= TERA_UNIT;
		break;
	case 'g':
	case 'G':
		n *= GIGA_UNIT;
		break;
	case 'm':
	case 'M':
		n *= MEGA_UNIT;
		break;
	case 'k':
	case 'K':
		n *= KILO_UNIT;
		break;
	case '\0':
		break;
	default:
		return DBL_MAX;
	}
	return n;
}				/* end unit_atof */


/* -------------------------------------------------------------------
 * unit_atof_rate
 *
 * Similar to unit_atof, but uses 10-based rather than 2-based
 * suffixes.
 * ------------------------------------------------------------------- */

double    unit_atof_rate(const char *s)
{
	double    n;
	char      suffix = '\0';

	if (s == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s s == NULL\n", __FUNCTION__);
		return DBL_MAX;
	}

	/* scan the number and any suffices */
	if (atof_and_one_char(s, &n, &suffix) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s format err\n", __FUNCTION__);
		return DBL_MAX;
	}

	/* convert according to [Tt Gg Mm Kk] */
	switch (suffix) {
	case 't':
	case 'T':
		n *= TERA_RATE_UNIT;
		break;
	case 'g':
	case 'G':
		n *= GIGA_RATE_UNIT;
		break;
	case 'm':
	case 'M':
		n *= MEGA_RATE_UNIT;
		break;
	case 'k':
	case 'K':
		n *= KILO_RATE_UNIT;
		break;
	case '\0':
		break;
	default:
		return DBL_MAX;
	}
	return n;
}				/* end unit_atof_rate */



/* -------------------------------------------------------------------
 * unit_atoi
 *
 * Given a string of form #x where # is a number and x is a format
 * character listed below, this returns the interpreted integer.
 * Tt, Gg, Mm, Kk are tera, giga, mega, kilo respectively
 * ------------------------------------------------------------------- */

iperf_size_t unit_atoi(const char *s)
{
	double    n;
	char      suffix = '\0';

	if (s == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s s == NULL\n", __FUNCTION__);
		return UINT64_MAX;
	}

	/* scan the number and any suffices */
	if (atof_and_one_char(s, &n, &suffix) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s format err\n", __FUNCTION__);
		return UINT64_MAX;
	}

	/* convert according to [Tt Gg Mm Kk] */
	switch (suffix) {
	case 't':
	case 'T':
		n *= TERA_UNIT;
		break;
	case 'g':
	case 'G':
		n *= GIGA_UNIT;
		break;
	case 'm':
	case 'M':
		n *= MEGA_UNIT;
		break;
	case 'k':
	case 'K':
		n *= KILO_UNIT;
		break;
	case '\0':
		break;
	default:
		return UINT64_MAX;
	}
	return (iperf_size_t) n;
}				/* end unit_atof */

/* -------------------------------------------------------------------
 * constants for byte_printf
 * ------------------------------------------------------------------- */

/* used as indices into conversion_bytes[], label_byte[], and label_bit[] */
enum {
	UNIT_CONV,
	KILO_CONV,
	MEGA_CONV,
	GIGA_CONV,
	TERA_CONV
};

/* factor to multiply the number by */
const double conversion_bytes[] = {
	1.0,			/* unit */
	1.0 / 1024,		/* kilo */
	1.0 / 1024 / 1024,	/* mega */
	1.0 / 1024 / 1024 / 1024, /* giga */
	1.0 / 1024 / 1024 / 1024 / 1024 /* tera */
};

/* factor to multiply the number by for bits*/
const double conversion_bits[] = {
	1.0,			/* unit */
	1.0 / 1000,		/* kilo */
	1.0 / 1000 / 1000,	/* mega */
	1.0 / 1000 / 1000 / 1000, /* giga */
	1.0 / 1000 / 1000 / 1000 / 1000 /* tera */
};


/* labels for Byte formats [KMGT] */
const char *label_byte[] = {
	"Byte",
	"KByte",
	"MByte",
	"GByte",
	"TByte"
};

/* labels for bit formats [kmgt] */
const char *label_bit[] = {
	"bit",
	"Kbit",
	"Mbit",
	"Gbit",
	"Tbit"
};

/* -------------------------------------------------------------------
 * unit_snprintf
 *
 * Given a number in bytes and a format, converts the number and
 * prints it out with a bits or bytes label.
 *   B, K, M, G, A for Byte, Kbyte, Mbyte, Gbyte, adaptive byte
 *   b, k, m, g, a for bit,  Kbit,  Mbit,  Gbit,  adaptive bit
 * adaptive picks the "best" one based on the number.
 * s should be at least 11 chars long
 * (4 digits + space + 5 chars max + null)
 * ------------------------------------------------------------------- */

void      unit_snprintf(char *s, int inLen,
						double inNum, char inFormat)
{
	int       conv;
	const char *suffix;
	const char *format;

	/* convert to bits for [bkmga] */
	if (!isupper((int) inFormat)) {
		inNum *= 8;
	}
	switch (toupper((u_char)inFormat)) {
	case 'B':
		conv = UNIT_CONV;
		break;
	case 'K':
		conv = KILO_CONV;
		break;
	case 'M':
		conv = MEGA_CONV;
		break;
	case 'G':
		conv = GIGA_CONV;
		break;
	case 'T':
		conv = TERA_CONV;
		break;

	default:
	case 'A': {
		double    tmpNum = inNum;
		conv = UNIT_CONV;

		if (isupper((int) inFormat)) {
			while (tmpNum >= 1024.0 && conv < TERA_CONV) {
				tmpNum /= 1024.0;
				conv++;
			}
		} else {
			while (tmpNum >= 1000.0 && conv < TERA_CONV) {
				tmpNum /= 1000.0;
				conv++;
			}
		}
		break;
	}
	}

	if (!isupper((int) inFormat)) {
		inNum *= conversion_bits[conv];
		suffix = label_bit[conv];
	} else {
		inNum *= conversion_bytes[conv];
		suffix = label_byte[conv];
	}

	/* print such that we always fit in 4 places */
	if (inNum < 9.995) {
		/* 9.995 would be rounded to 10.0 */
		format = "%4.2f %s";/* #.## */
	} else if (inNum < 99.95) {
		/* 99.95 would be rounded to 100 */
		format = "%4.1f %s";/* ##.# */
	} else if (inNum < 999.5) {
		/* 999.5 would be rounded to 1000 */
		format = "%4.0f %s";/* ### */
	} else {
		/* 1000-1024 fits in 4 places If not using
		 * Adaptive sizes then this code will not
		 * control spaces */
		format = "%4.0f %s";/* #### */
	}
	snprintf(s, inLen, format, inNum, suffix);
}				/* end unit_snprintf */

#ifdef __cplusplus
}				/* end extern "C" */

#endif
