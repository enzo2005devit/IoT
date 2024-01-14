#include <PubSubClient.h>
#include "isrgrootx1.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

ESP8266WiFiMulti WiFiMulti;

const char* mqtt_server = "c23b07ba3743474c88359e37395922a4.s1.eu.hivemq.cloud";
WiFiClientSecure client;
PubSubClient mqttclient(client);


WiFiManager wm;

#define LUZ_1 D4
#define LUZ_2 D3
#define LUZ_3 D2
#define LUZ_4 D8
#define SW_LUZ_1 D5
#define SW_LUZ_2 D1 //NO SE DEBE USAR NUNCA EL PIN 1, 6, 7 y 8 COMO SALIDA NI ENTRADA PORQUE MUERE EL ESP
#define SW_LUZ_3 D6
#define SW_LUZ_4 D7

void ICACHE_RAM_ATTR Luz1();
void ICACHE_RAM_ATTR Luz2();
void ICACHE_RAM_ATTR Luz3();
void ICACHE_RAM_ATTR Luz4();

void setup(){
  
  Serial.begin(115200);
  pinMode(LUZ_1, OUTPUT);
  pinMode(LUZ_2, OUTPUT);
  pinMode(LUZ_3, OUTPUT);
  pinMode(LUZ_4, OUTPUT);
  pinMode(SW_LUZ_1, INPUT_PULLUP);
  pinMode(SW_LUZ_2, INPUT_PULLUP);
  pinMode(SW_LUZ_3, INPUT_PULLUP);
  pinMode(SW_LUZ_4, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SW_LUZ_1), Luz1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_2), Luz2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_3), Luz3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_4), Luz4, CHANGE);

  bool res;

      res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res)
      Serial.println("Failed to connect");

    else 
        Serial.println("WIFI connected");

  WiFi.mode(WIFI_STA);
  WiFiMulti.run();
  mqttclient.setServer(mqtt_server, 8883);
  mqttclient.setCallback(callback);
}


void loop() {
  // wait for WiFi connection

if ((WiFiMulti.run() == WL_CONNECTED)) {
    client.setInsecure();
    
    if (!mqttclient.connected()) {
        if(mqttclient.connect("ESP8266", "ESP8266","Juve2020"))
        Serial.println("BROKER connected");
        mqttclient.subscribe("DISP_1");
    }
    
    else
      mqttclient.loop();
  }
}


void callback(char* topic, byte* payload, unsigned int length){

    if (strcmp(topic, "DISP_1") == 0){
    if(payload[0] == '1')
      Luz1();
    }

    if (strcmp(topic, "DISP_1") == 0){
      if(payload[0] == '2')
        Luz2();                                                                                                                                                                       
    }

    if (strcmp(topic, "DISP_1") == 0){
      if(payload[0] == '3')
        Luz3();
        Serial.println("int3");                                                                                                                                                               
    }

    if (strcmp(topic, "DISP_1") == 0){
      if(payload[0] == '4')
        Luz4();
        Serial.println("int4");                                                                                                                                                                 
    }
}

void ICACHE_RAM_ATTR Luz1() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
      digitalWrite(LUZ_1, !digitalRead(LUZ_1));
      Serial.println("int1");
  }
}

void ICACHE_RAM_ATTR Luz2(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
      digitalWrite(LUZ_2, !digitalRead(LUZ_2));
      Serial.println("int2");
  }
}

void ICACHE_RAM_ATTR Luz3(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
      digitalWrite(LUZ_3, !digitalRead(LUZ_3));
      Serial.println("int3");
  }
}

void ICACHE_RAM_ATTR Luz4(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
      digitalWrite(LUZ_4, !digitalRead(LUZ_4));
      Serial.println("int4");
  }
}