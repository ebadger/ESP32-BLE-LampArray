// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "pch.h"

extern Adafruit_NeoPixel pixels;

LampArray_::LampArray_(BLEServer *pServer) : 
    m_lastLampIdRequested(0),
    BLEHIDDevice(pServer)
{
    //static HIDSubDescriptor node(s_hidReportDescriptor, sizeof(s_hidReportDescriptor));
    //HID().AppendDescriptor(&node);
    //HID().AddReceiver(this);

    memset(&m_cachedStateWriteTo, 0, sizeof(m_cachedStateWriteTo));
    memset(&m_cachedStateReadFrom, 0, sizeof(m_cachedStateReadFrom));

    // Set the default effect to display all Lamps Blue.
    LampArrayColor defaultColor = {0, 0, 1, 0};
    SetDefaultColor(defaultColor);

    // On device start, always display the default effect.
    m_useDefaultEffect = true;
    _pServer = pServer;
    Serial.println("LampArray_::LampArray()");
}

void LampArray_::Start()
{
    std::string name = "Eric Badger";
    manufacturer()->setValue(name);
    pnp(0x01, 0x02e5, 0xbad6, 0x0110 );
    hidInfo(0x00, 0x02);

    reportMap((uint8_t *)s_hidReportDescriptor, sizeof(s_hidReportDescriptor));   


    m_bleLampArrayAttributesReport = featureReport(LAMP_ARRAY_ATTRIBUTES_REPORT_ID);
    m_bleLampArrayAttributesReport->setCallbacks(this);
         
    m_bleLampAttributesRequestReport = featureReport(LAMP_ATTRIBUTES_REQUEST_REPORT_ID);
    m_bleLampAttributesRequestReport->setCallbacks(this);
    
    m_bleLampAttributesReponseReport = featureReport(LAMP_ATTRIBUTES_RESPONSE_REPORT_ID);
    m_bleLampAttributesReponseReport->setCallbacks(this);
    
    m_bleLampMultiUpdateReport = featureReport(LAMP_MULTI_UPDATE_REPORT_ID);
    m_bleLampMultiUpdateReport->setCallbacks(this);
    
    m_bleLampRangeUpdateReport = featureReport(LAMP_RANGE_UPDATE_REPORT_ID);
    m_bleLampRangeUpdateReport->setCallbacks(this);
    
    m_bleLampArrayControlReport = featureReport(LAMP_ARRAY_CONTROL_REPORT_ID);
    m_bleLampArrayControlReport->setCallbacks(this);
    
    
    startServices();

    BLEAdvertising *pAdvertising = _pServer->getAdvertising();
    pAdvertising->setAppearance(GENERIC_HID);
    pAdvertising->addServiceUUID(hidService()->getUUID());
    pAdvertising->start();

    setBatteryLevel(100);
    
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    Serial.println("LampArray_::Start()");
}
void LampArray_::SetDefaultColor(LampArrayColor color) noexcept
{
    for (int i = 0; i < LAMP_COUNT; i++)
    {
        m_defaultEffectColors.Colors[i] = color;
    }
}

void LampArray_::GetCurrentState(LampArrayState* currentState) noexcept
{
    if (m_useDefaultEffect)
    {
        *currentState = m_defaultEffectColors;
    }
    else
    {
        *currentState = m_cachedStateReadFrom;
    }
}


void LampArray_::UpdateRequestLampFromLampAttributesRequestReport(uint8_t* data, uint16_t length) noexcept
{
    LampAttributesRequestReport report = {0};
    memcpy(&report, data, sizeof(LampAttributesRequestReport));

    // Per HID spec, if not within bounds, always set LampId to 0.
    if (report.LampId < LAMP_COUNT)
    {
        m_lastLampIdRequested = report.LampId;
    }
    else
    {
      m_lastLampIdRequested = 0;
    }
}

void LampArray_::UpdateLampStateCacheFromMultiUpdateReport(uint8_t* data, uint16_t length) noexcept
{
    LampMultiUpdateReport report = {0};
    memcpy(&report, data, sizeof(LampMultiUpdateReport));

    for (uint8_t i = 0; i < report.LampCount; i++)
    {
        // Ignore update if not within bounds.
        if (report.LampIds[i] < LAMP_COUNT)
        {
            m_cachedStateWriteTo.Colors[report.LampIds[i]] = report.UpdateColors[i];
        }
    }

    // Don't want the consumer to update before the Host has completed the batch of updates.
    if (report.LampUpdateFlags & LAMP_UPDATE_FLAG_UPDATE_COMPLETE)
    {
        m_cachedStateReadFrom = m_cachedStateWriteTo;
    }
}

void LampArray_::UpdateLampStateCacheFromRangeUpdateReport(uint8_t* data, uint16_t length) noexcept
{
    LampRangeUpdateReport report = {0};
    memcpy(&report, data, sizeof(LampRangeUpdateReport));

    log_i("report.LampIdStart=%d, report.LampIdEnd=%d", report.LampIdStart, report.LampIdEnd);
    
    // Ignore update if not within bounds.
    if (report.LampIdStart >= 0 && report.LampIdStart < LAMP_COUNT && 
        report.LampIdEnd >= 0 && report.LampIdEnd < LAMP_COUNT && 
        report.LampIdStart <= report.LampIdEnd)
    {
        for (uint16_t i = report.LampIdStart; i <= report.LampIdEnd; i++)
        {
            m_cachedStateWriteTo.Colors[i] = report.UpdateColor;
        }

        // Don't want the consumer to update before the Host has completed the batch of updates.
        if (report.LampUpdateFlags & LAMP_UPDATE_FLAG_UPDATE_COMPLETE)
        {
            m_cachedStateReadFrom = m_cachedStateWriteTo;
        }
    }
}

void LampArray_::UpdateCachedUseDefaultEffect(uint8_t* data, uint16_t length) noexcept
{
    LampArrayControlReport report = {0};
    memcpy(&report, data, sizeof(LampArrayControlReport));

    bool prevUseDefaultEffect = m_useDefaultEffect;

    m_useDefaultEffect = !!report.AutonomousMode;

    if (m_useDefaultEffect)
    {
        memset(&m_cachedStateReadFrom, 0, sizeof(m_cachedStateReadFrom));
        memset(&m_cachedStateWriteTo, 0, sizeof(m_cachedStateWriteTo));
    }
}

void LampArray_::onRead(BLECharacteristic* p)
{
  if (p == m_bleLampArrayAttributesReport)
  {
      uint8_t *pReport = (uint8_t *)&s_lampArrayAttributesReport; 
      p->setValue(pReport, sizeof(LampArrayAttributesReport));
      //p->notify();
      log_i("LampArrayAttributesReport size=%d", sizeof(LampArrayAttributesReport));
  }

  else if (p == m_bleLampAttributesReponseReport)
  {
    p->setValue((uint8_t *)&s_lampAttributesReports[m_lastLampIdRequested], sizeof(LampAttributesResponseReport)); 
    //p->notify();
    log_i("LampAttributesResponseReport size=%d", sizeof(LampAttributesResponseReport));

    m_lastLampIdRequested++;

    if (m_lastLampIdRequested >= LAMP_COUNT)
    {
        // Reset to zero
        m_lastLampIdRequested = 0;
    }
  }


  else
  {    
      Serial.println("unexpected");
  }
}

void LampArray_::onWrite(BLECharacteristic* p)
{
  Serial.println("LampArray_::onWrite");
  if (p == m_bleLampMultiUpdateReport)
  {
      Serial.println("m_bleLampMultiUpdateReport");
      UpdateLampStateCacheFromMultiUpdateReport(p->getData(), p->getLength() );
  }
  else if (p == m_bleLampRangeUpdateReport)
  {
      Serial.println("m_bleLampRangeUpdateReport");
      UpdateLampStateCacheFromRangeUpdateReport(p->getData(), p->getLength() );
  }
  else if (p == m_bleLampArrayControlReport)
  {
      Serial.println("m_bleLampArrayControlReport");
      UpdateCachedUseDefaultEffect(p->getData(), p->getLength() );    
      
  }
  else if (p == m_bleLampAttributesRequestReport)
  {
      Serial.println("m_bleLampAttributesRequestReport");
      UpdateRequestLampFromLampAttributesRequestReport(p->getData(), p->getLength() );
  }
}

void LampArray_::onNotify(BLECharacteristic* pCharacteristic)
{
}
