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

#ifndef _AP_INTERFACE_H_
#define _AP_INTERFACE_H_

struct wpa_ctrl;

void *ap_interface_init();
void wpa_request(struct wpa_ctrl *ctrl, int argc, char *argv[]);

#endif // _AP_INTERFACE_H_



