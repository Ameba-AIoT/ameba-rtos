/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines
    4. Local function prototypes
    5. Functions
        - JpegDecGetByte
        - JpegDecGet2Bytes
        - JpegDecShowBits
        - JpegDecFlushBits

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "jpegdecutils.h"
#include "jpegdecmarkers.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    5. Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

		Function name: JpegDecGetByte

        Functional description:
          Reads one byte (8 bits) from stream and returns the bits

          Note! This function does not skip the 0x00 byte if the previous
          byte value was 0xFF!!!

        Inputs:
          StreamStorage *pStream   Pointer to structure

        Outputs:
          Returns 8 bit value if ok
          else returns STRM_ERROR (0xFFFFFFFF)

------------------------------------------------------------------------------*/
u32 JpegDecGetByte(StreamStorage *pStream)
{
	u32 tmp;

	if ((pStream->readBits + 8) > (8 * pStream->streamLength)) {
		return (STRM_ERROR);
	}

	tmp = *(pStream->pCurrPos)++;
	tmp = (tmp << 8) | *(pStream->pCurrPos);
	tmp = (tmp >> (8 - pStream->bitPosInByte)) & 0xFF;
	pStream->readBits += 8;

	if ((g1_addr_t)pStream->pCurrPos - (g1_addr_t)pStream->pStartOfStream > pStream->streamLength) {
		return (STRM_ERROR);
	}

	return (tmp);
}

/*------------------------------------------------------------------------------

		Function name: JpegDecGet2Bytes

        Functional description:
          Reads two bytes (16 bits) from stream and returns the bits

          Note! This function does not skip the 0x00 byte if the previous
          byte value was 0xFF!!!

        Inputs:
          StreamStorage *pStream   Pointer to structure

        Outputs:
          Returns 16 bit value

------------------------------------------------------------------------------*/
u32 JpegDecGet2Bytes(StreamStorage *pStream)
{
	u32 tmp;

	if ((pStream->readBits + 16) > (8 * pStream->streamLength)) {
		return (STRM_ERROR);
	}

	tmp = *(pStream->pCurrPos)++;
	tmp = (tmp << 8) | *(pStream->pCurrPos)++;
	tmp = (tmp << 8) | *(pStream->pCurrPos);
	tmp = (tmp >> (8 - pStream->bitPosInByte)) & 0xFFFF;
	pStream->readBits += 16;

	if ((g1_addr_t)pStream->pCurrPos - (g1_addr_t)pStream->pStartOfStream > pStream->streamLength) {
		return (STRM_ERROR);
	}

	return (tmp);
}

/*------------------------------------------------------------------------------

		Function name: JpegDecShowBits

        Functional description:
          Reads 32 bits from stream and returns the bits, does not update
          stream pointers. If there are not enough bits in data buffer it
          reads the rest of the data buffer bits and fills the lsb of return
          value with zero bits.

          Note! This function will skip the byte valued 0x00 if the previous
          byte value was 0xFF!!!

        Inputs:
          StreamStorage *pStream   Pointer to structure

        Outputs:
          Returns  32 bit value

------------------------------------------------------------------------------*/
u32 JpegDecShowBits(StreamStorage *pStream)
{
	i32 bits;
	u32 readBits;
	u32 out = 0;
	u8 *pData = pStream->pCurrPos;

	/* bits left in buffer */
	bits = (i32)(8 * pStream->streamLength - pStream->readBits);
	if (!bits) {
		return (0);
	}

	readBits = 0;
	do {
		if (pData > pStream->pStartOfStream) {
			/* FF 00 bytes in stream -> jump over 00 byte */
			if ((pData[-1] == 0xFF) && (pData[0] == 0x00)) {
				pData++;
				bits -= 8;
			}
		}
		if (readBits == 32 && pStream->bitPosInByte) {
			out <<= pStream->bitPosInByte;
			out |= *pData >> (8 - pStream->bitPosInByte);
			readBits = 0;
			break;
		}
		out = (out << 8) | *pData++;
		readBits += 8;
		bits -= 8;
	} while (readBits < (32 + pStream->bitPosInByte) && bits > 0);

	if (bits <= 0 &&
		((readBits + pStream->readBits) >= (pStream->streamLength * 8))) {
		/* not enough bits in stream, fill with zeros */
		out = (out << (32 - (readBits - pStream->bitPosInByte)));
	}

	return (out);
}

/*------------------------------------------------------------------------------

		Function name: JpegDecFlushBits

        Functional description:
          Updates stream pointers, flushes bits from stream

          Note! This function will skip the byte valued 0x00 if the previous
          byte value was 0xFF!!!

        Inputs:
          StreamStorage *pStream   Pointer to structure
          u32 bits                 Number of bits to be flushed

        Outputs:
          OK
          STRM_ERROR

------------------------------------------------------------------------------*/
u32 JpegDecFlushBits(StreamStorage *pStream, u32 bits)
{
	u32 tmp;
	u32 extraBits = 0;

	if ((pStream->readBits + bits) > (8 * pStream->streamLength)) {
		/* there are not so many bits left in buffer */
		/* stream pointers to the end of the stream  */
		/* and return value STRM_ERROR               */
		pStream->readBits = 8 * pStream->streamLength;
		pStream->bitPosInByte = 0;
		pStream->pCurrPos = pStream->pStartOfStream + pStream->streamLength;
		return (STRM_ERROR);
	} else {
		tmp = 0;
		while (tmp < bits) {
			if (bits - tmp < 8) {
				if ((8 - pStream->bitPosInByte) > (bits - tmp)) {
					/* inside one byte */
					pStream->bitPosInByte += bits - tmp;
					tmp = bits;
				} else {
					if (pStream->pCurrPos[0] == 0xFF &&
						pStream->pCurrPos[1] == 0x00) {
						extraBits += 8;
						pStream->pCurrPos += 2;
					} else {
						pStream->pCurrPos++;
					}
					tmp += 8 - pStream->bitPosInByte;
					pStream->bitPosInByte = 0;
					pStream->bitPosInByte = bits - tmp;
					tmp = bits;
				}
			} else {
				tmp += 8;
				if (pStream->appnFlag) {
					pStream->pCurrPos++;
				} else {
					if (pStream->pCurrPos[0] == 0xFF &&
						pStream->pCurrPos[1] == 0x00) {
						extraBits += 8;
						pStream->pCurrPos += 2;
					} else {
						pStream->pCurrPos++;
					}
				}
			}
		}
		/* update stream pointers */
		pStream->readBits += bits + extraBits;

		if ((g1_addr_t)pStream->pCurrPos - (g1_addr_t)pStream->pStartOfStream > pStream->streamLength) {
			return (STRM_ERROR);
		}

		return (OK);
	}
}
