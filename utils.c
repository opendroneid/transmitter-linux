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

#include "utils.h"

// Convert a single uint8_t to two chars representing the value in ASCII format
// 0 - 9 => 0x30 - 0x39, A - F => 0x41 - 0x46
void uchar_to_ascii(char *out, uint8_t in) {
    if (!out)
        return;

    unsigned char high = (in & 0xF0) >> 4;
    if (high < 0xA)
        *out++ = (char) (0x30 + high);
    else
        *out++ = (char) (0x41 + high - 0xA);
    unsigned char low = in & 0x0F;
    if (low < 0xA)
        *out = (char) (0x30 + low);
    else
        *out = (char) (0x41 + low - 0xA);
}

