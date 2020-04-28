//
// Created by sebastian on 28.04.20.
//
# pragma once


#include <embree3/rtcore_device.h>

// boolean variable to switch embree on/off
static bool embree_enabled;

static void set_embree_enabled(bool enabled) {
    embree_enabled = enabled;
}

static bool is_embree_enabled() {
    return embree_enabled;
}

// RTC device
static RTCDevice __embree_device = nullptr;