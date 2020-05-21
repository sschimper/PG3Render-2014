//
// Created by sebastian on 28.04.20.
//
# pragma once
#include <embree3/rtcore_device.h>
#include <embree3/rtcore.h>

// RTC device
static RTCDevice __embree_device = nullptr;

// convert ray to embree ray
RTCRayHit ConvertRayToRTCRayHit(Ray ray) {
    struct RTCRayHit rayhit;
    rayhit.ray.org_x = ray.org.x;
    rayhit.ray.org_y = ray.org.y;
    rayhit.ray.org_z = ray.org.z;
    rayhit.ray.dir_x = ray.dir.x;
    rayhit.ray.dir_y = ray.dir.y;
    rayhit.ray.dir_z = ray.dir.z;
    rayhit.ray.tnear = ray.tmin;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = 0;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    return rayhit;
}