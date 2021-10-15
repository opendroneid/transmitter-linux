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

#ifndef _WIFI_BEACON_H_
#define _WIFI_BEACON_H_

#include <opendroneid.h>

void send_beacon_message(const union ODID_Message_encoded *encoded, uint8_t msg_counter);
void send_beacon_message_pack(struct ODID_MessagePack_encoded *pack_enc, uint8_t msg_counter);
void send_quit();

#endif //_WIFI_BEACON_H_
