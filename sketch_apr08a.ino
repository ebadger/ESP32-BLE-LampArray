#include "pch.h"


#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN       33
#define NUMPIXELS 512


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Always set the device to black when first powered-on.
LampArrayState displayState = {0};

BLEServer *_pServer;
LampArray_ *_pLampArray = NULL;
char buf[255];
bool _fReady = false;

class Callbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    Serial.println("Connected");
    
    pixels.show();

    _fReady = true;
  }

  void onDisconnect(BLEServer* pServer){
    Serial.println("Disconnected");

    _fReady = false;   
  }
};

void taskServer(void *)
{
  BLEDevice::init("BadgerLED");
  
  _pServer = BLEDevice::createServer();
  _pLampArray = new LampArray_(_pServer);

  _pServer->setCallbacks(new Callbacks());

  _pLampArray->Start();

  delay(portMAX_DELAY);
}

void setup() {
  pixels.begin();
  
  esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level

  Serial.begin(115200);
  Serial.println("setup()");
  Serial.setDebugOutput(true);

  for(int i = 0; i < LAMP_COUNT; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,16));
  }

  pixels.show();

  
  xTaskCreate(taskServer, "server", 20000, NULL, 5, NULL);

}

void loop() {
   if (_fReady)
   {
      LampArrayState currentState = {0};
      _pLampArray->GetCurrentState(&currentState);
  
      bool update = false;
  
      for (int i = 0; i < LAMP_COUNT; i++)
      {
          // Ignore the gain channel, only care about RGB.
          if (displayState.Colors[i].RedChannel != currentState.Colors[i].RedChannel || 
              displayState.Colors[i].GreenChannel != currentState.Colors[i].GreenChannel || 
              displayState.Colors[i].BlueChannel != currentState.Colors[i].BlueChannel)
          {
              // Update the current color (cache and set pixel state)
              memcpy(&(displayState.Colors[i]), &(currentState.Colors[i]), sizeof(LampArrayColor));
              pixels.setPixelColor(i, pixels.Color(
                  displayState.Colors[i].RedChannel, 
                  displayState.Colors[i].GreenChannel, 
                  displayState.Colors[i].BlueChannel));
  
              update = true;
          }
      }
  
      // Only call update on the NeoPixels when something has changed.  pixels.show() takes a long time to execute.
      if (update)
      {
          // Send the updated pixel color to hardware.
          pixels.show();
      }
    }
    ::vTaskDelay(10);
}
