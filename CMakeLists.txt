# Copyright (C) 2021, Soren Friis
#
# SPDX-License-Identifier: Apache-2.0
#
# Open Drone ID Linux transmitter example.
#
# Maintainer: Soren Friis
# friissoren2@gmail.com

cmake_minimum_required (VERSION 3.10)

project (opendroneid-transmit)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX")

if(DEFINED ODID_AUTH_MAX_PAGES)
    message(STATUS "Using externally defined ODID_AUTH_MAX_PAGES value")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DODID_AUTH_MAX_PAGES=${ODID_AUTH_MAX_PAGES}")
endif()

if(DEFINED ODID_BASIC_ID_MAX_MESSAGES)
    message(STATUS "Using externally defined ODID_BASIC_ID_MAX_MESSAGES value")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DODID_BASIC_ID_MAX_MESSAGES=${ODID_BASIC_ID_MAX_MESSAGES}")
endif()

include_directories(
        hostapd/src
        hostapd/src/utils
        core-c/libopendroneid
        gpsd/gpsd-dev
        bluez
)

add_executable(transmit
        hostapd/src/utils/os_unix.c
        hostapd/src/common/cli.c
        hostapd/src/common/wpa_ctrl.c
        hostapd/src/utils/common.c
        hostapd/src/utils/edit.c
        hostapd/src/utils/eloop.c
        hostapd/src/utils/wpa_debug.c
        core-c/libopendroneid/opendroneid.c
        bluez/lib/hci.c
        bluez/lib/bluetooth.c
        ap_interface.c
        utils.c
        bluetooth.c
        wifi_beacon.c
        gpsmod.c
        transmit.c
        print_bt_features.c
)

target_link_libraries(transmit
        pthread
        m
        "${PROJECT_SOURCE_DIR}/gpsd/gpsd-dev/libgps.so"
)
