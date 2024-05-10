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

#include "mhu.h"

#include <stddef.h>
#include <stdint.h>

#include "mhu_v2_x.h"

#define MHU_NOTIFY_VALUE    (1234u)

static enum mhu_error_t
error_mapping_to_mhu_error_t(enum mhu_v2_x_error_t err)
{
    switch (err) {
    case MHU_V_2_X_ERR_NONE:
        return MHU_ERR_NONE;
    case MHU_V_2_X_ERR_NOT_INIT:
        return MHU_ERR_NOT_INIT;
    case MHU_V_2_X_ERR_ALREADY_INIT:
        return MHU_ERR_ALREADY_INIT;
    case MHU_V_2_X_ERR_UNSUPPORTED_VERSION:
        return MHU_ERR_UNSUPPORTED_VERSION;
    case MHU_V_2_X_ERR_INVALID_ARG:
        return MHU_ERR_INVALID_ARG;
    case MHU_V_2_X_ERR_GENERAL:
        return MHU_ERR_GENERAL;
    default:
        return MHU_ERR_GENERAL;
    }
}

static enum mhu_v2_x_error_t
signal_and_wait_for_clear(struct mhu_v2_x_dev_t *dev)
{
    enum mhu_v2_x_error_t err;
    uint32_t val;
    /* Using the last channel for notifications */
    uint32_t channel_notify = mhu_v2_x_get_num_channel_implemented(dev) - 1;

    /* FIXME: Avoid wasting a whole channel for notifying */
    err = mhu_v2_x_channel_send(dev, channel_notify, MHU_NOTIFY_VALUE);
    if (err != MHU_V_2_X_ERR_NONE) {
        return err;
    }

    do {
        err = mhu_v2_x_channel_poll(dev, channel_notify, &val);
        if (err != MHU_V_2_X_ERR_NONE) {
            break;
        }
    } while (val != 0);

    return err;
}

static enum mhu_v2_x_error_t
clear_and_wait_for_signal(struct mhu_v2_x_dev_t *dev)
{
    enum mhu_v2_x_error_t err;
    uint32_t num_channels = mhu_v2_x_get_num_channel_implemented(dev);
    uint32_t val, i;

    /* Clear all channels */
    for (i = 0; i < num_channels; ++i) {
        err = mhu_v2_x_channel_clear(dev, i);
        if (err != MHU_V_2_X_ERR_NONE) {
            return err;
        }
    }

    do {
        /* Using the last channel for notifications */
        err = mhu_v2_x_channel_receive(dev, num_channels - 1, &val);
        if (err != MHU_V_2_X_ERR_NONE) {
            break;
        }
    } while (val != MHU_NOTIFY_VALUE);

    return err;
}

enum mhu_error_t mhu_init_sender(void *mhu_sender_dev)
{
    enum mhu_v2_x_error_t err;
    struct mhu_v2_x_dev_t *dev = mhu_sender_dev;

    if (dev == NULL) {
        return MHU_ERR_INVALID_ARG;
    }

    err = mhu_v2_x_driver_init(dev, MHU_REV_READ_FROM_HW);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }

    /* This wrapper requires at least two channels to be implemented */
    if (mhu_v2_x_get_num_channel_implemented(dev) < 2) {
        return MHU_ERR_UNSUPPORTED;
    }

    return MHU_ERR_NONE;
}

enum mhu_error_t mhu_init_receiver(void *mhu_receiver_dev)
{
    enum mhu_v2_x_error_t err;
    struct mhu_v2_x_dev_t *dev = mhu_receiver_dev;
    uint32_t num_channels, i;

    if (dev == NULL) {
        return MHU_ERR_INVALID_ARG;
    }

    err = mhu_v2_x_driver_init(dev, MHU_REV_READ_FROM_HW);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }

    num_channels = mhu_v2_x_get_num_channel_implemented(dev);

    /* This wrapper requires at least two channels to be implemented */
    if (num_channels < 2) {
        return MHU_ERR_UNSUPPORTED;
    }

    /* Mask all channels except the notifying channel */
    for (i = 0; i < (num_channels - 1); ++i) {
        err = mhu_v2_x_channel_mask_set(dev, i, UINT32_MAX);
        if (err != MHU_V_2_X_ERR_NONE) {
            return error_mapping_to_mhu_error_t(err);
        }
    }

    /* The last channel is used for notifications */
    err = mhu_v2_x_channel_mask_clear(dev, (num_channels - 1), UINT32_MAX);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }

    err = mhu_v2_x_interrupt_enable(dev, MHU_2_1_INTR_CHCOMB_MASK);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }

    return MHU_ERR_NONE;
}

enum mhu_error_t mhu_send_data(void *mhu_sender_dev,
                               const uint8_t *send_buffer,
                               size_t size)
{
    enum mhu_v2_x_error_t err;
    struct mhu_v2_x_dev_t *dev = mhu_sender_dev;
    uint32_t num_channels = mhu_v2_x_get_num_channel_implemented(dev);
    uint32_t chan = 0;
    uint32_t i;
    uint32_t *p;

    if (dev == NULL || send_buffer == NULL) {
        return MHU_ERR_INVALID_ARG;
    } else if (size == 0) {
        return MHU_ERR_NONE;
    }

    /* For simplicity, require the send_buffer to be 4-byte aligned. */
    if ((uintptr_t)send_buffer & 0x3u) {
        return MHU_ERR_INVALID_ARG;
    }

    err = mhu_v2_x_initiate_transfer(dev);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }

    /* First send over the size of the actual message. */
    err = mhu_v2_x_channel_send(dev, chan, (uint32_t)size);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }
    chan++;

    p = (uint32_t *)send_buffer;
    for (i = 0; i < size; i += 4) {
        err = mhu_v2_x_channel_send(dev, chan, *p++);
        if (err != MHU_V_2_X_ERR_NONE) {
            return error_mapping_to_mhu_error_t(err);
        }
        if (++chan == (num_channels - 1)) {
            err = signal_and_wait_for_clear(dev);
            if (err != MHU_V_2_X_ERR_NONE) {
                return error_mapping_to_mhu_error_t(err);
            }
            chan = 0;
        }
    }

    /* Signal the end of transfer.
     *   It's not required to send a signal when the message was
     *   perfectly-aligned ((num_channels - 1) channels were used in the last
     *   round) preventing it from signaling twice at the end of transfer.
     */
    if (chan != 0) {
        err = signal_and_wait_for_clear(dev);
        if (err != MHU_V_2_X_ERR_NONE) {
            return error_mapping_to_mhu_error_t(err);
        }
    }

    err = mhu_v2_x_close_transfer(dev);
    return error_mapping_to_mhu_error_t(err);
}

enum mhu_error_t mhu_receive_data(void *mhu_receiver_dev,
                                  uint8_t *receive_buffer,
                                  size_t *size)
{
    enum mhu_v2_x_error_t err;
    struct mhu_v2_x_dev_t *dev = mhu_receiver_dev;
    uint32_t num_channels = mhu_v2_x_get_num_channel_implemented(dev);
    uint32_t chan = 0;
    uint32_t message_len;
    uint32_t i;
    uint32_t *p;

    if (dev == NULL || receive_buffer == NULL) {
        return MHU_ERR_INVALID_ARG;
    }

    /* For simplicity, require:
     * - the receive_buffer to be 4-byte aligned,
     * - the buffer size to be a multiple of 4.
     */
    if (((uintptr_t)receive_buffer & 0x3u) || (*size & 0x3u)) {
        return MHU_ERR_INVALID_ARG;
    }

    /* The first word is the length of the actual message. */
    err = mhu_v2_x_channel_receive(dev, chan, &message_len);
    if (err != MHU_V_2_X_ERR_NONE) {
        return error_mapping_to_mhu_error_t(err);
    }
    chan++;

    if (message_len > *size) {
        /* Message buffer too small */
        *size = message_len;
        return MHU_ERR_BUFFER_TOO_SMALL;
    }

    p = (uint32_t *)receive_buffer;
    for (i = 0; i < message_len; i += 4) {
        err = mhu_v2_x_channel_receive(dev, chan, p++);
        if (err != MHU_V_2_X_ERR_NONE) {
            return error_mapping_to_mhu_error_t(err);
        }

        /* Only wait for next transfer if there is still missing data. */
        if (++chan == (num_channels - 1) && (message_len - i) > 4) {
            /* Busy wait for next transfer */
            err = clear_and_wait_for_signal(dev);
            if (err != MHU_V_2_X_ERR_NONE) {
                return error_mapping_to_mhu_error_t(err);
            }
            chan = 0;
        }
    }

    /* Clear all channels */
    for (i = 0; i < num_channels; ++i) {
        err = mhu_v2_x_channel_clear(dev, i);
        if (err != MHU_V_2_X_ERR_NONE) {
            return error_mapping_to_mhu_error_t(err);
        }
    }

    *size = message_len;

    return MHU_ERR_NONE;
}
