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

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "utils.h"

void init_bluetooth(struct config_data *config);
void send_bluetooth_message(const union ODID_Message_encoded *encoded, uint8_t msg_counter, struct config_data *config);
void send_bluetooth_message_extended_api(const union ODID_Message_encoded *encoded, uint8_t msg_counter, struct config_data *config);
void send_bluetooth_message_pack(const struct ODID_MessagePack_encoded *pack_enc, uint8_t msg_counter, struct config_data *config);
void close_bluetooth(struct config_data *config);

#endif //_BLUETOOTH_H_
