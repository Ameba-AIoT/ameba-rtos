/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_client_defs.h"
#include "val_service_defs.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

#define SERVER_HEAP_SIZE 0x100 /* The size is same as heap_size field in manifest */

#ifdef SP_HEAP_MEM_SUPP
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
#endif

int32_t server_test_dynamic_mem_alloc_fn(void);

const server_test_t test_i067_server_tests_list[] = {
    NULL,
    server_test_dynamic_mem_alloc_fn,
    NULL,
};

int32_t server_test_dynamic_mem_alloc_fn(void)
{
   /* Perform checks only if heap memory support available to secure partition */
#ifdef SP_HEAP_MEM_SUPP
   uint8_t              *buffer, *buffer1;
   uint8_t              cmpbuff[SERVER_HEAP_SIZE] = {0};

   memset((uint8_t *)cmpbuff, 0, SERVER_HEAP_SIZE);

   /* Allocate whole heap memory size */
   buffer = (uint8_t *)malloc(sizeof(uint8_t) * SERVER_HEAP_SIZE);
   if (buffer == NULL)
   {
       val->print(PRINT_ERROR, "\tmalloc failed for full memory allocation\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Check for zero init by malloc() */
   if (memcmp(buffer, cmpbuff, SERVER_HEAP_SIZE))
   {
       val->print(PRINT_ERROR, "\tUnequal data in compared buffers-1\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Check for heap memory over run */
   buffer1 = (uint8_t *)malloc(sizeof(uint8_t) * 8);
   if (buffer1 != NULL)
   {
       val->print(PRINT_ERROR, "\tmalloc failed for over mem alloc\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Set buffer to non-zero value */
   memset((uint8_t *)buffer, 1, SERVER_HEAP_SIZE);

   /* Free up the memory */
   free(buffer);

   /* Check for memory scrub by free() */
   if (*buffer == 1)
   {
       val->print(PRINT_ERROR, "\tUnequal data in compared buffers-2\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Allocate 32 byte memory to test relloac */
   buffer = (uint8_t *)malloc(sizeof(uint8_t) * 32);
   if (buffer == NULL)
   {
       val->print(PRINT_ERROR, "\tmalloc failed\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Set buffer to non-zero value */
   memset((uint8_t *)buffer, 1, 32);
   memset((uint8_t *)cmpbuff, 1, 32);

   /* Re-allocate the buffer, Size = 64 byte */
   buffer1 = (uint8_t *)realloc(buffer, (sizeof(uint8_t) * 64));

   /* Check older object is deallocated */
   if (memcmp(buffer, (cmpbuff + 64), 32))
   {
       val->print(PRINT_ERROR, "\tUnequal data in compared buffers-3\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Check new object has previous data of older object */
   if (memcmp(buffer1, cmpbuff, 32))
   {
       val->print(PRINT_ERROR, "\tUnequal data in compared buffers-4\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Check new allocated size is zero init */
   if (memcmp((buffer1 + 32), (cmpbuff + 32), 32))
   {
       val->print(PRINT_ERROR, "\tUnequal data in compared buffers-5\n", 0);
       return VAL_STATUS_SPM_FAILED;
   }

   free(buffer1);

#endif
   return VAL_STATUS_SUCCESS;
}
