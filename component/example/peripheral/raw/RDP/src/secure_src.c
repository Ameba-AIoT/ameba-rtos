#include "ameba_soc.h"

#ifdef __ICCARM__
typedef __cmse_nonsecure_call u32 nsFunc(u32);
#else
typedef u32 __attribute__((cmse_nonsecure_call)) nsFunc(u32);
#endif

/*
 * Internal secure data that can't be accessed directly by Non-secure functions.
 */
u32 rdp_data[32] = {
	0x10101010, 0x01010101, 0x02020202, 0x03030303,
	0x04040404, 0x05050505, 0x06060606, 0x07070707,
	0x08080808, 0x09090909, 0x0a0a0a0a, 0x0b0b0b0b,
	0x0c0c0c0c, 0x0d0d0d0d, 0x0e0e0e0e, 0x0f0f0f0f,
	0x10101010, 0x11111111, 0x12121212, 0x13131313,
	0x14141414, 0x15151515, 0x16161616, 0x17171717,
	0x18181818, 0x19191919, 0x1a1a1a1a, 0x1b1b1b1b,
	0x1c1c1c1c, 0x1d1d1d1d, 0x1e1e1e1e, 0x1f1f1f1f
};

/*
 * Internal secure function that can't be called directly by Non-secure functions.
 */
u32 rdp_protection_func(u32 data)
{
	u32 result;

	result = data / 5 + 3;
	result *= 2;
	result += 8;

	return result;
}

/*
 * The following function is the secure entry function and can be called by Non-secure functions.
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY u32 rdp_protection_entry(u32 idx)
{
	return rdp_protection_func(rdp_data[idx]);
}

/*
 * The following example shows how to call  NS function from secure functions.
 *	(1) Users should pass a non-secure functioin pointer.
 *	(2) Then in secure code, users can call this to execute the non-secure function.
 */
IMAGE3_ENTRY_SECTION
u32 NS_ENTRY rdp_no_protection_call(nsFunc *fp, u32 idx)
{
	u32 result, data;
	nsFunc *pFunc = cmse_nsfptr_create(fp);  /* set to non-secure function pointer */

	data = rdp_data[idx];
	result = pFunc(data);

	return result;
}

