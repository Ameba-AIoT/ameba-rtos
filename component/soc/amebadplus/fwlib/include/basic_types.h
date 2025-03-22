/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> /* for size_t */
#include <limits.h> /* for INT_MAX */
#include "rtk_compiler.h"

#define PLATFORM_LITTLE_ENDIAN                  0
#define PLATFORM_BIG_ENDIAN                     1

#define SYSTEM_ENDIAN                           PLATFORM_LITTLE_ENDIAN

#define SUCCESS	0
#define FAIL	(-1)
#define RTK_SUCCESS	0
#define RTK_FAIL	(-1)

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef MIN
#define MIN(x, y)			(((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y)			(((x) > (y)) ? (x) : (y))
#endif

#define DIV_ROUND_UP(X, Y) (((X) + (Y) -1) / (Y))

#ifndef UNUSED
#define UNUSED(X)      (void)X
#endif

typedef unsigned int	        uint;
typedef	signed int		        sint;

#define s8                      int8_t
#define u8                      uint8_t
#define s16                     int16_t
#define u16                     uint16_t
#define s32                     int32_t
#define u32                     uint32_t
#define s64                     int64_t
#define u64                     uint64_t

#define IN
#define OUT
#define INOUT
#define NDIS_OID uint
#define NDIS_STATUS uint

typedef     u32 dma_addr_t;

#define     FIELD_OFFSET(s,field)	((size_t)&((s*)(0))->field)
#define 	SIZE_PTR 				size_t

#ifndef ON
#define   ON        1
#endif

#ifndef OFF
#define   OFF       0
#endif

#ifndef ENABLE
#define   ENABLE    1
#endif

#ifndef DISABLE
#define   DISABLE   0
#endif

#ifndef BIT0
#define BIT0	0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10	0x0400
#define BIT11	0x0800
#define BIT12	0x1000
#define BIT13	0x2000
#define BIT14	0x4000
#define BIT15	0x8000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000
#endif

#define BIT_(__n)       (1U<<(__n))

#ifndef BIT
#define BIT(__n)       (1U<<(__n))
#endif

#define SWAP32(x) ((u32)(                         \
 			(((u32)(x) & (u32)0x000000ff) << 24) |            \
 			(((u32)(x) & (u32)0x0000ff00) <<  8) |            \
 			(((u32)(x) & (u32)0x00ff0000) >>  8) |            \
 			(((u32)(x) & (u32)0xff000000) >> 24)))

#define WAP16(x) ((u16)(                         \
 			(((u16)(x) & (u16)0x00ff) <<  8) |            \
 			(((u16)(x) & (u16)0xff00) >>  8)))

#if SYSTEM_ENDIAN == PLATFORM_LITTLE_ENDIAN
#define CONCAT_TO_UINT32(b4, b3, b2, b1) (((u32)((b4) & 0xFF) << 24) | ((u32)((b3) & 0xFF) << 16) | ((u32)((b2) & 0xFF) << 8) | ((u32)((b1) & 0xFF)))
#define rtk_cpu_to_be32(x)		SWAP32((x))

#elif SYSTEM_ENDIAN == PLATFORM_BIG_ENDIAN
#define CONCAT_TO_UINT32(b4, b3, b2, b1) (((u32)((b1) & 0xFF) << 24) | ((u32)((b2) & 0xFF) << 16) | ((u32)((b3) & 0xFF) << 8) | ((u32)((b4) & 0xFF)))
#define rtk_cpu_to_be32(x)		((u32)(x))
#endif


/*
 *	Call endian free function when
 *		1. Read/write packet content.
 *		2. Before write integer to IO.
 *		3. After read integer from IO.
*/

//
// Byte Swapping routine.
//
#define EF1Byte	(u8)
#define EF2Byte (u16)
#define EF4Byte	(u32)

//
// Read LE format data from memory
//
#define ReadEF1Byte(_ptr)		EF1Byte(*((u8 *)(_ptr)))
#define ReadEF2Byte(_ptr)		__UNALIGNED_UINT16_READ(_ptr)
#define ReadEF4Byte(_ptr)		__UNALIGNED_UINT32_READ(_ptr)

//
// Write LE data to memory
//
#define WriteEF1Byte(_ptr, _val)	(*((u8 *)(_ptr)))=EF1Byte(_val)
#define WriteEF2Byte(_ptr, _val)	__UNALIGNED_UINT16_WRITE(_ptr, _val)
#define WriteEF4Byte(_ptr, _val)	__UNALIGNED_UINT32_WRITE(_ptr, _val)

//
//	Example:
//		BIT_LEN_MASK_32(0) => 0x00000000
//		BIT_LEN_MASK_32(1) => 0x00000001
//		BIT_LEN_MASK_32(2) => 0x00000003
//		BIT_LEN_MASK_32(32) => 0xFFFFFFFF
//
#define BIT_LEN_MASK_32(__BitLen) \
	(0xFFFFFFFF >> (32 - (__BitLen)))
//
//	Example:
//		BIT_OFFSET_LEN_MASK_32(0, 2) => 0x00000003
//		BIT_OFFSET_LEN_MASK_32(16, 2) => 0x00030000
//
#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_32(__BitLen) << (__BitOffset))

//
//	Description:
//		Return 4-byte value in host byte ordering from
//		4-byte pointer in litten-endian system.
//
#define LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
	ReadEF4Byte(__pStart)

//
//	Description:
//		Translate subfield (continuous bits in little-endian) of 4-byte value in litten byte to
//		4-byte value in host byte ordering.
//
#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_32(__BitLen) \
	)

//
//	Description:
//		Mask subfield (continuous bits in little-endian) of 4-byte value in litten byte oredering
//		and return the result in 4-byte value in host byte ordering.
//
#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
		& \
		( ~ BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) ) \
	)

//
//	Description:
//		Set subfield of little-endian 4-byte value to specified value.
//
#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	WriteEF4Byte(__pStart, \
		EF4Byte(LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
			| ( (((u32)(__Value)) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset) ) ) \
			)

#define BIT_LEN_MASK_16(__BitLen) \
		(0xFFFF >> (16 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_16(__BitLen) << (__BitOffset))

#define LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
	ReadEF2Byte(__pStart)

#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_16(__BitLen) \
	)

#define LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
		& \
		( ~ BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) ) \
	)

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	WriteEF2Byte(__pStart, \
		EF2Byte(LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
			| ( (((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset) )) \
		)


#define BIT_LEN_MASK_8(__BitLen) \
		(0xFF >> (8 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) \
	(BIT_LEN_MASK_8(__BitLen) << (__BitOffset))

#define LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
	(EF1Byte(*((u8 *)(__pStart))))

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		( LE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset) ) \
		& \
		BIT_LEN_MASK_8(__BitLen) \
	)

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
		& \
		( ~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) ) \
	)

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	*((u8 *)(__pStart)) = \
		EF1Byte( \
			LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
			| \
			( (((u8)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset) ) \
		);

//pclint
#define LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
	( \
		LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
	)

//pclint
#define SET_BITS_TO_LE_1BYTE_8BIT(__pStart, __BitOffset, __BitLen, __Value) \
{ \
	*((u8 *)(__pStart)) = \
		EF1Byte( \
			LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
			| \
			((u8)__Value) \
		); \
}

// Get the N-bytes aligment offset from the current length
#define N_BYTE_ALIGMENT(__Value, __Aligment) ((__Aligment == 1) ? (__Value) : (((__Value + __Aligment - 1) / __Aligment) * __Aligment))

#define TEST_FLAG(__Flag,__testFlag)		(((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag)			((__Flag) |= __setFlag)
#define CLEAR_FLAG(__Flag, __clearFlag)		((__Flag) &= ~(__clearFlag))
#define CLEAR_FLAGS(__Flag)					((__Flag) = 0)
#define TEST_FLAGS(__Flag, __testFlags)		(((__Flag) & (__testFlags)) == (__testFlags))

#endif// __BASIC_TYPES_H__
