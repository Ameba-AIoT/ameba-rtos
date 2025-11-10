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

#ifndef AMEBA_BASE_CUTILS_INTERFACES_CUTILS_PARCEL_H
#define AMEBA_BASE_CUTILS_INTERFACES_CUTILS_PARCEL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Parcel Parcel;
typedef void (*release_func)(Parcel* parcel, const uint8_t* data, size_t dataSize);

Parcel *Parcel_Create(void);
void Parcel_IpcSetData(Parcel *parcel, uint8_t* data, size_t data_size, release_func rel_func);
void Parcel_Destroy(Parcel *parcel);
uint8_t* Parcel_IpcData(Parcel *parcel);
size_t Parcel_IpcDataSize(Parcel *parcel);

bool Parcel_WriteBool(Parcel *parcel, bool value);
bool Parcel_ReadBool(Parcel *parcel);
bool Parcel_WriteInt8(Parcel *parcel, int8_t value);
int8_t Parcel_ReadInt8(Parcel *parcel);
bool Parcel_WriteInt16(Parcel *parcel, int16_t value);
int16_t Parcel_ReadInt16(Parcel *parcel);
bool Parcel_WriteInt32(Parcel *parcel, int32_t value);
int32_t Parcel_ReadInt32(Parcel *parcel);
bool Parcel_WriteInt64(Parcel *parcel, int64_t value);
int64_t Parcel_ReadInt64(Parcel *parcel);
bool Parcel_WriteUint8(Parcel *parcel, uint8_t value);
uint8_t Parcel_ReadUint8(Parcel *parcel);
bool Parcel_WriteUint16(Parcel *parcel, uint16_t value);
uint16_t Parcel_ReadUint16(Parcel *parcel);
bool Parcel_WriteUint32(Parcel *parcel, uint32_t value);
uint32_t Parcel_ReadUint32(Parcel *parcel);
bool Parcel_WriteUint64(Parcel *parcel, uint64_t value);
uint64_t Parcel_ReadUint64(Parcel *parcel);
bool Parcel_WriteFloat(Parcel *parcel, float value);
float Parcel_ReadFloat(Parcel *parcel);
bool Parcel_WriteDouble(Parcel *parcel, double value);
double Parcel_ReadDouble(Parcel *parcel);
bool Parcel_WritePointer(Parcel *parcel, void *value);
void *Parcel_ReadPointer(Parcel *parcel);
bool Parcel_WriteBuffer(Parcel *parcel, void *data, size_t size);
void *Parcel_ReadBuffer(Parcel *parcel, size_t length);
bool Parcel_WriteCString(Parcel *parcel, char *value);
char *Parcel_ReadCString(Parcel *parcel);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_CUTILS_INTERFACES_CUTILS_PARCEL_H
