/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMEBA_CUTILS_BITOPS_H
#define AMEBA_CUTILS_BITOPS_H

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline int popcount(unsigned int x)
{
	return __builtin_popcount(x);
}

static inline int popcountl(unsigned long x)
{
	return __builtin_popcountl(x);
}

static inline int popcountll(unsigned long long x)
{
	return __builtin_popcountll(x);
}

__END_DECLS

#endif // AMEBA_CUTILS_BITOPS_H
