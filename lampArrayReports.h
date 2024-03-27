// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"

#pragma once

#define LAMP_ARRAY_ATTRIBUTES_REPORT_ID        1
#define LAMP_ATTRIBUTES_REQUEST_REPORT_ID      2
#define LAMP_ATTRIBUTES_RESPONSE_REPORT_ID     3
#define LAMP_MULTI_UPDATE_REPORT_ID            4
#define LAMP_RANGE_UPDATE_REPORT_ID            5
#define LAMP_ARRAY_CONTROL_REPORT_ID           6

#define LAMP_UPDATE_FLAG_UPDATE_COMPLETE       1

#define LAMP_MULTI_UPDATE_LAMP_COUNT           8

typedef struct __attribute__ ((__packed__)) LampArrayColor_
{
    uint8_t RedChannel;
    uint8_t GreenChannel;
    uint8_t BlueChannel;
    uint8_t GainChannel;
} LampArrayColor;

typedef struct __attribute__ ((__packed__)) LampArrayAttributesReport_
{
    uint16_t LampCount;
    uint32_t BoundingBoxWidth;
    uint32_t BoundingBoxHeight;
    uint32_t BoundingBoxDepth;
    uint32_t LampArrayKind;
    uint32_t MinUpdateIntervalInMicroseconds;
} LampArrayAttributesReport;

typedef struct __attribute__ ((__packed__)) LampAttributesRequestReport_
{
    uint16_t LampId;
} LampAttributesRequestReport;

typedef struct __attribute__ ((__packed__)) LampAttributesResponseReport_
{
    uint16_t LampId;
    uint32_t PositionX;
    uint32_t PositionY;
    uint32_t PositionZ;
    uint32_t UpdateLatency;
    uint32_t LampPurpose;
    uint8_t  RedChannelsCount;
    uint8_t  GreenChannelsCount;
    uint8_t  BlueChannelsCount;
    uint8_t  GainChannelsCount;
    uint8_t  IsProgrammable;
    uint8_t  LampKey;
} LampAttributesResponseReport;

typedef struct __attribute__ ((__packed__)) LampMultiUpdateReport_
{
    uint8_t  LampCount;
    uint8_t  LampUpdateFlags;
    uint16_t LampIds[LAMP_MULTI_UPDATE_LAMP_COUNT];
    LampArrayColor UpdateColors[LAMP_MULTI_UPDATE_LAMP_COUNT];
} LampMultiUpdateReport;

typedef struct __attribute__ ((__packed__)) LampRangeUpdateReport_
{
    uint8_t  LampUpdateFlags;
    uint16_t LampIdStart;
    uint16_t LampIdEnd;
    LampArrayColor UpdateColor;
} LampRangeUpdateReport;

typedef struct __attribute__ ((__packed__)) LampArrayControlReport_
{
    uint8_t AutonomousMode;
} LampArrayControlReport;
