#include <WiFi.h>
#include <LittleFS.h>
#include <esp_task_wdt.h>
#include "ConfigSettings.h"
#include "RTSNetwork.h"
#include "Web.h"
#include "Sockets.h"
#include "Utils.h"
#include "Somfy.h"
#include "MQTT.h"
#include "GitOTA.h"
#include "MatterHandler.h"

ConfigSettings settings;
Web webServer;
SocketEmitter sockEmit;
RTSNetwork net;
rebootDelay_t rebootDelay;
SomfyShadeController somfy;
MQTTClass mqtt;
GitUpdater git;

uint32_t oldheap = 0;
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Startup/Boot....");
  Serial.println("Mounting File System...");
  if(LittleFS.begin()) Serial.println("File system mounted successfully");
  else Serial.println("Error mounting file system");
  settings.begin();
  if(WiFi.status() == WL_CONNECTED) WiFi.disconnect(true);
  delay(10);
  Serial.println();
  webServer.startup();
    esp_task_wdt_add(NULL); // Add the current task to the WDT watch
    webServer.begin();
  delay(1000);
  net.setup();  
  somfy.begin();

#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  matterHandler.begin();
#endif
  //git.checkForUpdate();
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 15000, // Increased timeout to prevent crashes on large operations
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Bitmask of all cores
    .trigger_panic = true
  };
  if (esp_task_wdt_reconfigure(&wdt_config) != ESP_OK) {
    esp_task_wdt_init(&wdt_config); // enable panic so ESP32 restarts
  }
  
  // esp_task_wdt_add is intentionally omitted here as it's done earlier to protect all tasks
}

void loop() {
  // put your main code here, to run repeatedly:
  //uint32_t heap = ESP.getFreeHeap();
  if(rebootDelay.reboot && millis() > rebootDelay.rebootTime) {
    Serial.print("Rebooting after ");
    Serial.print(rebootDelay.rebootTime);
    Serial.println("ms");
    net.end();
    ESP.restart();
    return;
  }
  uint32_t timing = millis();
  
  net.loop();
  if(millis() - timing > 100) Serial.printf("Timing Net: %ldms\n", millis() - timing);
  timing = millis();
  esp_task_wdt_reset();
  somfy.loop();

#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  matterHandler.loop();
#endif
  if(millis() - timing > 100) Serial.printf("Timing Somfy: %ldms\n", millis() - timing);
  timing = millis();
  esp_task_wdt_reset();
  if(net.connected() || net.softAPOpened) {
    if(!rebootDelay.reboot && net.connected() && !net.softAPOpened) {
      git.loop();
      esp_task_wdt_reset();
    }
    webServer.loop();
    esp_task_wdt_reset();
    if(millis() - timing > 100) Serial.printf("Timing WebServer: %ldms\n", millis() - timing);
    esp_task_wdt_reset();
    timing = millis();
    sockEmit.loop();
    if(millis() - timing > 100) Serial.printf("Timing Socket: %ldms\n", millis() - timing);
    esp_task_wdt_reset();
    timing = millis();
  }
  if(rebootDelay.reboot && millis() > rebootDelay.rebootTime) {
    net.end();
    ESP.restart();
  }
  esp_task_wdt_reset();
}
