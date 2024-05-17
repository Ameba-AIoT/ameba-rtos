/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MHU_H__
#define __MHU_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generic MHU error enumeration types.
 */
enum mhu_error_t {
    MHU_ERR_NONE                =  0,
    MHU_ERR_NOT_INIT            = -1,
    MHU_ERR_ALREADY_INIT        = -2,
    MHU_ERR_UNSUPPORTED_VERSION = -3,
    MHU_ERR_UNSUPPORTED         = -4,
    MHU_ERR_INVALID_ARG         = -5,
    MHU_ERR_BUFFER_TOO_SMALL    = -6,
    MHU_ERR_GENERAL             = -7,
};

/**
 * \brief Initializes sender MHU.
 *
 * \param[in] mhu_sender_dev        Pointer to the sender MHU.
 *
 * \return Returns mhu_error_t error code.
 *
 * \note This function must be called before mhu_send_data().
 */
enum mhu_error_t mhu_init_sender(void *mhu_sender_dev);

/**
 * \brief Initializes receiver MHU.
 *
 * \param[in] mhu_receiver_dev      Pointer to the receiver MHU.
 *
 * \return Returns mhu_error_t error code.
 *
 * \note This function must be called before mhu_receive_data().
 */
enum mhu_error_t mhu_init_receiver(void *mhu_receiver_dev);

/**
 * \brief Sends data over MHU.
 *
 * \param[in] mhu_sender_dev  Pointer to the sender MHU.
 * \param[in] send_buffer     Pointer to buffer containing the data to be
 *                            transmitted.
 * \param[in] size            Size of the data to be transmitted in bytes.
 *
 * \return Returns mhu_error_t error code.
 *
 * \note The send_buffer must be 4-byte aligned and its length must be at least
 *       (4 - (size % 4)) bytes bigger than the data size to prevent buffer
 *       over-reading.
 */
enum mhu_error_t mhu_send_data(void *mhu_sender_dev,
                               const uint8_t *send_buffer,
                               size_t size);

/**
 * \brief Receives data from MHU.
 *
 * \param[in]     mhu_receiver_dev  Pointer to the receiver MHU.
 * \param[out]    receive_buffer    Pointer the buffer where to store the
 *                                  received data.
 * \param[in,out] size              As input the size of the receive_buffer,
 *                                  as output the number of bytes received.
 *                                  As a limitation, the size of the buffer
 *                                  must be a multiple of 4.
 *
 * \return Returns mhu_error_t error code.
 *
 * \note The receive_buffer must be 4-byte aligned and its length must be a
 *       multiple of 4.
 */
enum mhu_error_t mhu_receive_data(void *mhu_receiver_dev,
                                  uint8_t *receive_buffer,
                                  size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* __MHU_H__ */
