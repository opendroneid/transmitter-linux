/*
 * Copyright (C) 2021, Soren Friis
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Open Drone ID Linux transmitter example.
 *
 * Maintainer: Soren Friis
 * friissoren2@gmail.com
 */

#include <unistd.h>
#include <semaphore.h>
#include "ap_interface.h"
#include "utils.h"
#include "wifi_beacon.h"

extern struct wpa_ctrl *ctrl_conn;
extern sem_t semaphore;

static void send_update_beacon() {
    char *cmd[] = { "update_beacon" };
    wpa_request(ctrl_conn, sizeof(cmd)/sizeof(cmd[0]), cmd);
    sem_wait(&semaphore);
}

/*
 * The header for WiFi Beacons, when specifying the data for vendor specific information elements,
 * consists of the following parts:
 *     dd = Indicates to hostapd that the following data is hexadecimal
 *     1E = The length of the data (30 bytes)
 *     FA, 0B, BC = The OUI reserved for ASD-STAN
 *     0D = The indicator within the ASD-STAN OUI address space indicating Direct Remote ID
 *     xx = 8-bit message counter starting at 0x00 and wrapping around at 0xFF
 */
#define WIFI_BEACON_HEADER_SIZE 7
void send_beacon_message(const union ODID_Message_encoded *encoded, uint8_t msg_counter) {
    char *cmd[] = { "set", "vendor_elements", "dd1EFA0BBC0D00" };

    // The buffers cmd points to are in read-only memory. Create a writable buffer
    uint8_t data[2*(WIFI_BEACON_HEADER_SIZE + ODID_MESSAGE_SIZE) + 1] = { 0 };
    memcpy(data, cmd[2], 2*WIFI_BEACON_HEADER_SIZE);
    cmd[2] = (char *) data;

    // Insert the message counter
    uchar_to_ascii((char *) &data[12], msg_counter);

    // Insert the encoded message data
    for (int i = 0; i < ODID_MESSAGE_SIZE; i++)
        uchar_to_ascii((char *) &data[2*(WIFI_BEACON_HEADER_SIZE + i)], encoded->rawData[i]);

    wpa_request(ctrl_conn, sizeof(cmd)/sizeof(cmd[0]), cmd);
    sem_wait(&semaphore);

    send_update_beacon();
    sleep(1);
}

// See also description for send_beacon_message()
void send_beacon_message_pack(struct ODID_MessagePack_encoded *pack_enc, uint8_t msg_counter) {
    char *cmd[] = { "set", "vendor_elements", "dd1EFA0BBC0D00" };

    // The buffers cmd points to are in read-only memory. Create a writable buffer
    char data[2*(WIFI_BEACON_HEADER_SIZE + 3 + ODID_PACK_MAX_MESSAGES*ODID_MESSAGE_SIZE) + 1] = { 0 };
    memcpy(data, cmd[2], 2*WIFI_BEACON_HEADER_SIZE);
    cmd[2] = data;

    // Update the data length
    int amount = pack_enc->MsgPackSize;
    uchar_to_ascii(&data[2], (WIFI_BEACON_HEADER_SIZE - 2) + 3 + amount*ODID_MESSAGE_SIZE);

    // Insert the message counter
    uchar_to_ascii(&data[12], msg_counter);

    // Insert the encoded message data
    for (int i = 0; i < 3 + amount*ODID_MESSAGE_SIZE; i++)
        uchar_to_ascii(&data[2*(WIFI_BEACON_HEADER_SIZE + i)], ((char *) pack_enc)[i]);

    wpa_request(ctrl_conn, sizeof(cmd)/sizeof(cmd[0]), cmd);
    sem_wait(&semaphore);
    sleep(1);

    send_update_beacon();
    sleep(1);
}

void send_quit() {
    char *cmd[] = { "quit" };
    wpa_request(ctrl_conn, sizeof(cmd)/sizeof(cmd[0]), cmd);
    sem_wait(&semaphore);
}

#include "wifi_beacon.h"
