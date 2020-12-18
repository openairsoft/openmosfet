#include <Arduino.h>
#include <esp_now.h>
#include <openMosfetEspNow.h>

#include "utilities.h"
#include "config.h"
#include "inputsInterface.h"
#include "components.h"
#include "configuration.h"
#include "wifiServer.h"
#include "otaUploader.h"

void setup();
void loop();

// uint8_t peer[] {0x30, 0xAE, 0xA4, 0xF8, 0x0B, 0x1C}; //esp32
static uint8_t peer[] {0x84, 0x0D, 0x8E, 0x8C, 0xBB, 0x22};

void setup() {
  //important initialize the input interface first, beacause this the sets the outputs and it may be important depending on the fet input logic
  OMInputsInterface::begin();
  OMVirtualReplica::begin();

  Serial.begin(115200);

  
  if (!FILESYSTEM.begin()) {
    #ifdef DEBUGM
      Serial.println("Failed to mount file system, trying to format...");
    #endif
    if (!FILESYSTEM.format()) {
      #ifdef DEBUG
        Serial.println("Unable to format(), aborting...");
      #endif   
      return;
    }else{
      if (!FILESYSTEM.begin()) {
      #ifdef DEBUG
        Serial.println("Failed to mount file system, evn after formating...");
      #endif
      }
    }
  }
  
 
  OMConfiguration::load();
  #ifdef DEBUG
    Serial.println("config chargée :");
    OMConfiguration::printCfg();
  #endif

  #ifdef OM_ADDITIONAL_HIGH_PIN
    pinMode (OM_ADDITIONAL_HIGH_PIN, OUTPUT);
    digitalWrite(OM_ADDITIONAL_HIGH_PIN, HIGH);
  #endif

  #ifdef OM_ADDITIONAL_LOW_PIN
    pinMode (OM_ADDITIONAL_LOW_PIN, OUTPUT);
    digitalWrite(OM_ADDITIONAL_LOW_PIN, LOW);
  #endif




  OMwifiserver::begin();
  OMOtaUploader::begin();

  //todo: only if espnow enabled
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peer, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println(WiFi.macAddress());
  Serial.print("macAddress_s.status: ");
}

void loop() {
  OMwifiserver::update();
  OMOtaUploader::update();
  OMInputsInterface::update();
  OMVirtualReplica::update();
  // Send message via ESP-NOW
  Serial.println("---------------------");
  unsigned long beforeSend1 = micros(); 
  struct_status_MacAdress macAddress_s;
  WiFi.macAddress(macAddress_s.macAddress);
  unsigned long afterSend1 = micros();
  Serial.printf("delta 1 : %lu\n", afterSend1 - beforeSend1);
  unsigned long beforeSend2 = micros();
  esp_err_t result = esp_now_send(peer, (uint8_t *) &macAddress_s, sizeof(struct_status_MacAdress));
  unsigned long afterSend2 = micros();
  Serial.printf("delta 2 : %lu\n", afterSend2 - beforeSend2);
  Serial.println("---------------------");
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  delay(1000);
}
