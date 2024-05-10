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
#ifndef AMEBA_BASE_CUTILS_INTERFACES_CUTILS_SYSTEM_PROPERTIES_H
#define AMEBA_BASE_CUTILS_INTERFACES_CUTILS_SYSTEM_PROPERTIES_H

#ifdef __cplusplus
extern "C" {
#endif

/* System properties are *small* name value pairs managed by the
** property service.  If your data doesn't fit in the provided
** space it is not appropriate for a system property.
*/
#define PROP_NAME_MAX   32
#define PROP_VALUE_MAX  92

void property_load_boot_defaults(const char *file);

void property_init(void);
int property_get(const char *key, char *value, const char *default_value);
int property_set(const char *key, const char *value);
int property_list(void (*propfn)(const char *key, const char *value, void *cookie), void *cookie);

#ifdef __cplusplus
}
#endif

#endif
