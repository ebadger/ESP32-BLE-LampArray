// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

#ifdef CORE_DEBUG_LEVEL
#undef CORE_DEBUG_LEVEL
#endif

#define CORE_DEBUG_LEVEL 6
#define CONFIG_LOG_DEFAULT_LEVEL 6

// NeoPixel shield always communicates over PIN6.  However, since we have a level-shifter
// between board-output to shield-input we must output over a different pin (PIN5)
//#define NEO_PIXEL_PIN 5

#define LAMP_COUNT 512

//#define PIXEL_TYPE NEO_GRBW + NEO_KHZ800

#define PIN       33
//#define NUMPIXELS 512

#include <Adafruit_NeoPixel.h>
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLECharacteristic.h"
#include "BLEHIDDevice.h"
#include "BLEValue.h"
#include "BLEDescriptor.h"
#include "BLE2904.h"

#include "Arduino.h"
#include <atomic>
#include "lampArrayReports.h"
#include "lampArrayHidDescriptor.h"
#include "lampArrayAttributes.h"
#include "lampArray.h"
