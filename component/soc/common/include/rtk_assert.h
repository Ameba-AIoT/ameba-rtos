/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __RTK_ASSERT_
#define __RTK_ASSERT_

void vAssertCalled(const char *pcFile, uint32_t ulLine);

#if defined(CONFIG_ASSERTION_DISABLE)
#define rtk_assert(__e) ((void)(__e))
#elif defined(CONFIG_ASSERTION_SILENT) && CONFIG_ASSERTION_SILENT
#define rtk_assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(NULL, 0))
#else
#define rtk_assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(__FILE__, __LINE__))
#endif

#endif
