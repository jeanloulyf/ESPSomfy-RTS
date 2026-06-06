#ifndef MATTER_HANDLER_H
#define MATTER_HANDLER_H

#include <Arduino.h>

#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

#include <Matter.h>
#include "Somfy.h"

class MatterHandler {
  private:
    bool initialized = false;
  public:
    void begin();
    void loop();
};

extern MatterHandler matterHandler;

#endif // CONFIG_IDF_TARGET_ESP32C6
#endif // MATTER_HANDLER_H
