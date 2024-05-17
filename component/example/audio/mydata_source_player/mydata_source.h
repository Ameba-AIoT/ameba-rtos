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

#ifndef _MYDATA_SOURCE_H_
#define _MYDATA_SOURCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "media/rtdata_source.h"

typedef struct MyDataSource MyDataSource;
struct MyDataSource {
	RTDataSource base;

	char *data;
	int data_length; // current total source length; for unknown_data_length source, data_length will change until last_data_gained equals 1
	bool unknown_data_length;
	bool last_data_gained;

	bool alive;
};

RTDataSource *MyDataSource_Create(char *source, int length);
void MyDataSource_Destroy(MyDataSource *source);

int32_t MyDataSource_CheckPrepared(const RTDataSource *source);
ssize_t MyDataSource_ReadAt(const RTDataSource *source, off_t offset, void *data, size_t size);
int32_t MyDataSource_GetLength(const RTDataSource *source, off_t *size);

#ifdef __cplusplus
}
#endif

#endif  // _MYDATA_SOURCE_H_