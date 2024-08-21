#include "ameba_soc.h"
#include <string.h>
#include <stdlib.h>

#define USHRT_MAX_INTERNAL       ((u16)(~0U))
#define BUF_MAX        			((s16)(USHRT_MAX_INTERNAL>>1))
#define STR_STORE_MAX_LEN		24
#define KSTRTOX_OVERFLOW        (1U << 31)

#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == ',')


/**
 * div_u64_rem - unsigned 64bit divide with 32bit divisor with remainder
 *
 * This is commonly provided by 32bit archs to provide an optimized 64bit
 * divide.
 */
static u64 div_u64_rem(u64 dividend, u32 divisor, u32 *remainder)
{
	*remainder = (u32)dividend % divisor;
	return (u32)dividend / divisor;
}

/**
 * div_s64_rem - signed 64bit divide with 32bit divisor with remainder
 */
static s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder)
{
	*remainder = (s32)dividend % divisor;
	return (s32)dividend / divisor;
}

/**
 * div_u64 - unsigned 64bit divide with 32bit divisor
 *
 * This is the most common 64bit divide and should be used if possible,
 * as many 32bit archs can optimize this variant better than a full 64bit
 * divide.
 */

static u64 div_u64(u64 dividend, u32 divisor)
{
	u32 remainder;
	return div_u64_rem(dividend, divisor, &remainder);
}

/**
 * div_s64 - signed 64bit divide with 32bit divisor
 */

static s64 div_s64(s64 dividend, s32 divisor)
{
	s32 remainder;
	return div_s64_rem(dividend, divisor, &remainder);
}


static const char *_parse_integer_fixup_radix(const char *s, unsigned int *base)
{
	if (*base == 0) {
		if (s[0] == '0') {
			if (_tolower(s[1]) == 'x' && isxdigit(s[2])) {
				*base = 16;
			} else {
				*base = 8;
			}
		} else {
			*base = 10;
		}
	}
	if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x') {
		s += 2;
	}
	return s;
}

static char *skip_spaces(const char *str)
{
	while (isspace(*str)) {
		++str;
	}

	return (char *)str;
}
static int skip_atoi(const char **s)
{
	int i = 0;

	while (isdigit(**s)) {
		i = i * 10 + *((*s)++) - '0';
	}

	return i;
}

static int judge_digit_width(const char *str, unsigned int base)
{

	int width = 0;

	if ((base == 16 || base == 0) && str[0] == '0' && _tolower(str[1]) == 'x') {
		str += 2;
		width += 2;

		while (isxdigit(*str)) {
			width++;
			str++;
		}
	} else if (base == 16) {
		while (isxdigit(*str)) {
			width++;
			str++;
		}
	} else if (base == 8 || (base == 0 && str[0] == '0')) {
		while (isdigit(*str) && (*str) < '8') {
			width++;
			str++;
		}
	} else {
		while (isdigit(*str)) {
			width++;
			str++;
		}
	}

	return width;
}

//_OPTIMIZE_NONE_
int _vsscanf_minimal(const char *buf, const char *fmt, va_list args)
{
	const char *str = buf;
	char *next;
	char digit;
	int num = 0;
	int i = 0;
	u8 qualifier;
	unsigned int base;
	union {
		long long s;
		unsigned long long u;
	} val;
	s16 field_width;
	bool is_sign;

	char str_store[STR_STORE_MAX_LEN] = {0};

	while (*fmt) {
		/* skip any white space in format */
		/* white space in format matchs any amount of
		    * white space, including none, in the input.
		    */
		if (isspace(*fmt)) {
			fmt = skip_spaces(++fmt);
			str = skip_spaces(str);
		}

		/* anything that is not a conversion must match exactly */
		if (*fmt != '%' && *fmt) {
			if (*fmt++ != *str++) {
				break;
			}

			continue;
		}

		if (!*fmt) {
			break;
		}

		++fmt;

		/* skip this conversion.
		    * advance both strings to next white space
		    */
		if (*fmt == '*') {
			if (!*str) {
				break;
			}

			while (!isspace(*fmt) && *fmt != '%' && *fmt) {
				fmt++;
			}
			while (!isspace(*str) && *str) {
				str++;
			}

			continue;
		}

		/* get field width */
		field_width = -1;

		if (isdigit(*fmt)) {

			field_width = skip_atoi(&fmt);

			if (field_width <= 0) {
				break;
			}
		}

		/* get conversion qualifier */
		qualifier = -1;

		if (*fmt == 'h' || _tolower(*fmt) == 'l' || _tolower(*fmt) == 'z') {
			qualifier = *fmt++;

			if (qualifier == *fmt) {
				if (qualifier == 'h') {
					qualifier = 'H';
					fmt++;
				} else if (qualifier == 'l') {
					qualifier = 'L';
					fmt++;
				}
			}
		}

		if (!*fmt) {
			break;
		}

		if (*fmt == 'n') {
			/* return number of characters read so far */
			*va_arg(args, int *) = str - buf;
			++fmt;
			continue;
		}

		if (!*str) {
			break;
		}

		base = 10;
		is_sign = 0;

		switch (*fmt++) {
		case 'c': {
			char *s = (char *)va_arg(args, char *);

			if (field_width == -1) {
				field_width = 1;
			}

			do {
				*s++ = *str++;
			} while (--field_width > 0 && *str);

			num++;
		}

		continue;

		case 's': {
			char *s = (char *)va_arg(args, char *);
			if (field_width == -1) {
				field_width = SHRT_MAX;
			}
			/* first, skip leading white space in buffer */
			str = skip_spaces(str);

			/* now copy until next white space */
			while (*str && !isspace(*str) && field_width--) {
				*s++ = *str++;
			}

			*s = '\0';
			num++;
		}
		continue;

		case 'o':
			base = 8;
			break;

		case 'x':
		case 'X':
			base = 16;
			break;

		case 'i':
			base = 0;
			is_sign = 1;
			break;

		case 'd':
			is_sign = 1;
			break;

		case 'u':
			break;

		case '%':
			/* looking for '%' in str */
			if (*str++ != '%') {
				return num;
			}
			continue;

		default:
			/* invalid format; stop here */
			return num;
		}

		/* have some sort of integer conversion.
		    * first, skip white space in buffer.
		*/
		str = skip_spaces(str);

		digit = *str;

		if (is_sign && digit == '-') {
			if (field_width == -1) {
				field_width = judge_digit_width(str + 1, base);
				field_width++;
			} else {
				field_width = (judge_digit_width(str + 1, base) + 1) < field_width ?
							  (judge_digit_width(str + 1, base) + 1) :
							  field_width;
			}
			digit = *(str + 1);
		} else if (field_width == -1) {
			field_width = judge_digit_width(str, base);
		} else {
			field_width = judge_digit_width(str, base) < field_width ?
						  judge_digit_width(str, base) :
						  field_width;
		}

		if (!digit
			|| (base == 16 && !isxdigit(digit))
			|| (base == 10 && !isdigit(digit))
			|| (base == 8 && (!isdigit(digit) || digit > '7'))
			|| (base == 0 && !isdigit(digit))) {
			break;
		}

		//assert_param (field_width <= STR_STORE_MAX_LEN);
		for (i = 0; i < field_width ; i++) {
			str_store[i] = str[i];
		}

		if (is_sign) {
			val.s = qualifier != 'L' ?
					strtol(str_store, &next, base) :
					strtoll(str_store, &next, base);
		} else {
			val.u = qualifier != 'L' ?
					_strtoul(str_store, &next, base) :
					strtoull(str_store, &next, base);
		}

		/* standard strtoul need this */
		next = (char *)str + field_width;

		for (i = 0; i < STR_STORE_MAX_LEN ; i++) {
			str_store[i] = 0;
		}


		//判断转换的字符串的宽度是否大于 %2d
		if (field_width > 0 && next - str > field_width) {
			if (base == 0) {
				_parse_integer_fixup_radix(str, &base);
			}

			while (next - str > field_width) {
				if (is_sign) {
					val.s = div_s64(val.s, base);
				} else {
					val.u = div_u64(val.u, base);
				}

				--next;
			}
		}

		switch (qualifier) {
		case 'H':       /* that's 'hh' in format */
			if (is_sign) {
				*va_arg(args, signed char *) = val.s;
			} else {
				*va_arg(args, unsigned char *) = val.u;
			}
			break;

		case 'h':
			if (is_sign) {
				memcpy(va_arg(args, short *), &(val.s), sizeof(short));
			} else {
				memcpy(va_arg(args, unsigned short *), &(val.u), sizeof(unsigned short));
			}
			break;

		case 'l':
			if (is_sign) {
				memcpy(va_arg(args, long *), &(val.s), sizeof(long));
			} else {
				memcpy(va_arg(args, unsigned long *), &(val.u), sizeof(unsigned long));
			}
			break;

		case 'L':
			if (is_sign) {
				memcpy(va_arg(args, long long *), &(val.s), sizeof(long long));
			} else {
				memcpy(va_arg(args, unsigned long long *), &(val.u), sizeof(unsigned long long));
			}
			break;

		case 'Z':
		case 'z':
			memcpy(va_arg(args, size_t *), &(val.u), sizeof(size_t));
			break;

		default:
			if (is_sign) {
				memcpy(va_arg(args, int *), &(val.s), sizeof(int));
			} else {
				memcpy(va_arg(args, unsigned int *), &(val.u), sizeof(unsigned int));
			}
			break;
		}

		num++;

		if (!next) {
			break;
		}

		str = next;
	}

	return num;
}

/**
* sscanf - Unformat a buffer into a list of arguments
* @buf:        input buffer
* @fmt:        formatting of buffer
* @...:        resulting arguments
*/
/* support %* %x %c %s %o %x %i %d %u %h %z and %l(within len 24) */
/* for internal use, not support [] and ^ */
int _sscanf_ss(const char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsscanf_minimal(buf, fmt, args);
	va_end(args);

	return i;
}

