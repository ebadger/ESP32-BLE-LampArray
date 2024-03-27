// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"

#pragma once

#define LAMPARRAY_STATES_SIZE 2

typedef struct __attribute__ ((__packed__)) LampArrayState_
{
    LampArrayColor Colors[LAMP_COUNT];
} LampArrayState;

class LampArray_ : public BLEHIDDevice, public BLECharacteristicCallbacks
{
public:
    LampArray_(BLEServer *);

    void Start();
    
    void GetCurrentState(LampArrayState* currentState) noexcept;

    void ProcessReceivedGetFeatureReport(uint8_t reportId) noexcept;

    void ProcessReceivedSetFeatureReport(uint8_t reportId, uint8_t* data, uint16_t length) noexcept;

    
private:
    void SendLampArrayAttributesReport() noexcept;

    void SendLampAttributesReport() noexcept;

    void UpdateRequestLampFromLampAttributesRequestReport(uint8_t* data, uint16_t length) noexcept;

    void UpdateLampStateCacheFromMultiUpdateReport(uint8_t* data, uint16_t length) noexcept;

    void UpdateLampStateCacheFromRangeUpdateReport(uint8_t* data, uint16_t length) noexcept;

    void UpdateCachedUseDefaultEffect(uint8_t* data, uint16_t length) noexcept;

    void SetDefaultColor(LampArrayColor color) noexcept;

    //BLECharacteristicCallbacks
    void onRead(BLECharacteristic* pCharacteristic);
    void onWrite(BLECharacteristic* pCharacteristic);
    void onNotify(BLECharacteristic* pCharacteristic);

private:
    // Last LampId of the corresponding LampAttributes requested by the Host.
    uint16_t m_lastLampIdRequested;

    LampArrayState m_cachedStateWriteTo;

    LampArrayState m_cachedStateReadFrom;

    // Static LampArrayState with the default colors (when Host doesn't have exclusive lock)
    LampArrayState m_defaultEffectColors;

    // Bool to indicate whether the default effect should be used.
    // Default effect is only used when Host doesn't have exclusive lock
    bool m_useDefaultEffect;

    BLEServer *_pServer;


    BLECharacteristic *m_bleLampArrayAttributesReport;
    BLECharacteristic *m_bleLampAttributesRequestReport;
    BLECharacteristic *m_bleLampAttributesReponseReport;
    BLECharacteristic *m_bleLampMultiUpdateReport;
    BLECharacteristic *m_bleLampRangeUpdateReport;
    BLECharacteristic *m_bleLampArrayControlReport;
    
    
};
