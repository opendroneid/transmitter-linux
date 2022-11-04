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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <opendroneid.h>

struct config_data {
    bool use_beacon;

    bool use_btl; // Bluetooth Legacy Advertising
    bool use_bt4; // Bluetooth Legacy Advertising using Extended Advertising APIs
    bool use_bt5; // Bluetooth Long Range with Extended Advertising

    bool use_gps;
    
    uint8_t handle_bt4;
    uint8_t handle_bt5;

    bool use_packs; // Message packs

    uint8_t msg_counters[ODID_MSG_COUNTER_AMOUNT];
};

void uchar_to_ascii(char *out, uint8_t in);

#endif //_UTILS_H_
