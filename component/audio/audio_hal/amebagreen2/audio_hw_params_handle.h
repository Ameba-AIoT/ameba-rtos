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
#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBAGREEN2_AUDIO_HW_PARAMS_HANDLE_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBAGREEN2_AUDIO_HW_PARAMS_HANDLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct string_cell {
	char *key;
	char *value;
	struct string_cell *next;
};

struct string_cell *string_cells_create_from_str(const char *_string);
void string_cells_destroy(struct string_cell *cell_head);

int32_t string_cells_has_key(struct string_cell *cell_head, const char *key);
int32_t string_cells_get_str(struct string_cell *cell_head, const char *key, char *out_val, int32_t len);
int32_t string_cells_get_int(struct string_cell *cell_head, const char *key, int32_t *out_val);

#endif