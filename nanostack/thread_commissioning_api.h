/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-PBL
 *
 * Licensed under the Permissive Binary License, Version 1.0 (the "License"); you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.mbed.com/licenses/PBL-1.0
 *
 * See the License for the specific language governing permissions and limitations under the License.
 *
 */

/**Public API for enabling inside thread commissioner.
 *
 *  * this is simplified version of commissioner that only requires petition and keep alive message sending.
 *  * handing of Joiner finalisation message
 * TODO This API must also enable the un-secure commissioner.
 * Before that can be used the application must start bootstrap and network scan to find un-secure thread network
 * that is allowing
 *
 */

#ifndef THREAD_COMMISSIONING_API_H_
#define THREAD_COMMISSIONING_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ns_types.h"

#define TRACE_GROUP_THREAD_COMMISSIONING_API "TCoA"

typedef enum {
    COMMISSIONING_STATE_ACCEPT,
    COMMISSIONING_STATE_PENDING,
    COMMISSIONING_STATE_REJECT,
    COMMISSIONING_STATE_NO_NETWORK
} commissioning_state_e;

/** Commissioning petition response callback
 *
 * \param interface_id Interface id where the request was made.
 * \param state State of the commissioning.
 *
 * \return 0 success other values failure
 */
typedef int (commissioning_status_cb)(int8_t interface_id, commissioning_state_e state);


/** register commissioner interface.
 *
 * if the interface is up we start inside thread commissioner behaviour.
 * if there is no interface we must do scanning of network where we can add un-secure commissioner
 * and then do un-secure attach to it and start using the different communication method to border router.
 *
 * \param interface_id Interface id where the request was made.
 *
 * \return 0 success other values failure
 */
int commissioning_register(int8_t interface_id);

/** unregister commissioner interface.
 *
 * This cleans up all the commissioner data from the device and de-attaches from the thread network if un-secure commissioner was used.
 *
 * \param interface_id Interface id where the request was made.
 *
 * \return 0 success other values failure
 */
int commissioning_unregister(int8_t interface_id);

/** Start the commissioning petition.
 *
 * If we are un-secure commissioner then we must scan the networks and select the Thread network where we want to become commissioner.
 *
 * \param interface_id Interface id where the request was made.
 * \param cb_ptr callback function to inform the result of the operation. Can be NULL if no result code needed.
 * \return 0 indicates success.
 *
 * \return -1 no network connected client needs to scan network to become un-secure commissioner.
 * \return Any other value indicates other failures.
 */
int commissioning_petition_start(int8_t interface_id, char *commissioner_id_ptr, commissioning_status_cb *status_cb_ptr);


/** send petition Keep alive.
 *
 * this function must be called in 40 seconds intervals. TODO rethink if this should be automatic
 *
 * \param interface_id Interface id where the request was made.
 * \param cb_ptr callback function to inform the result of the operation. Can be NULL if no result code needed.
 *
 * \return 0 success other values failure
 */
int commissioning_petition_keep_alive(int8_t interface_id, commissioning_state_e state);

/** callback received when new device is finalasing the joining process.
 *
 * message may includes following meshcop TLV fields:
 * * State TLV
 * * Vendor Name TLV
 * * Vendor Model TLV
 * * Vendor SW Version TLV
 * * Vendor Data TLV
 * * Vendor Stack
 * * Version TLV
 * * Provisioning URL TLV
 *
 * \param interface_id Interface id where the request was made.
 * \param EUI64 client identifier.
 * \param message_ptr message including the meshcop TLV set. this message can be parsed using thread_meshcop_lib.h.
 * \param message_len Length of message.
 *
 * \return 0 device is accepted.
 * \return any other value device is rejected.
 */
typedef int (commissioning_joiner_finalisation_cb)(int8_t interface_id, uint8_t EUI64[8], uint8_t *message_ptr, uint16_t message_len);

/** Add device to commission to thread network.
 *
 *
 * \param interface_id Interface id where the request was made.
 * \param short_eui64 boolean value indicating if we use short eui version for bloom filter generation
 * \param EUI64 pointer to EUI64 buffer.
 * \param PSKd_ptr pointer to PSKd buffer.
 * \param PSKd_len PSKd string length current validity check is 1-32 bytes.
 * \param cb_ptr callback function to inform the result of the operation. Can be NULL if no result code needed.
 *
 * \return 0 success other values failure
 */
int commissioning_device_add(int8_t interface_id, bool short_eui64, uint8_t EUI64[8], uint8_t *PSKd_ptr, uint8_t PSKd_len, commissioning_joiner_finalisation_cb *joining_device_cb_ptr);

/** Delete device to commission to thread network.
 *
 *
 * \param interface_id Interface id where the request was made.
 * \param EUI64 pointer to EUI64 buffer.
 *
 * \return 0 success other values failure
 */
int commissioning_device_delete(int8_t interface_id, uint8_t EUI64[8]);

/** Get next added device details
 * \param ptr, for internal looping. For beginning, use NULL pointer, after that use return pointer
 * \param interface_id Interface id where the request was made.
 * \param short_eui64 boolean value indicating if we use short eui version for bloom filter generation. Can be NULL when no result wanted.
 * \param EUI64 pointer to EUI64 buffer. Can be NULL when no result wanted.
 * \param PSKd_ptr pointer to PSKd buffer. Can be NULL when no result wanted.
 *
 * \return >NULL for next iteration
 * \return NULL  when end of list
 */
void *commission_device_get_next(void *ptr, int8_t interface_id, bool *short_eui64, uint8_t EUI64[8], uint8_t PSKd[32], uint8_t *PSKd_len);
#ifdef __cplusplus
}
#endif

#endif /* THREAD_COMMISSIONING_API_H_ */
