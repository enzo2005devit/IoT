#include <Arduino.h>
#include <PubSubClient.h>
#include "isrgrootx1.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

ESP8266WiFiMulti WiFiMulti;
//const char* SSID = "MovistarFibra-F71ED0";
//const char* PASSWORD = "ROCCO123";
const char* mqtt_server = "c23b07ba3743474c88359e37395922a4.s1.eu.hivemq.cloud";
WiFiClientSecure client;
PubSubClient mqttclient(client);


WiFiManager wm;

int LUZ_1 = 4;
int LUZ_2 = 3;
int SW_LUZ_2 = 2;
int SW_LUZ_1 = 1;



void setup(){

  Serial.begin(115200);
  pinMode(LUZ_1, OUTPUT);
  pinMode(LUZ_2, OUTPUT);
  pinMode(SW_LUZ_2, INPUT);
  pinMode(SW_LUZ_1, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  bool res;

    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.run();
  mqttclient.setServer(mqtt_server, 8883);
  mqttclient.setCallback(callback);

  
}

void loop() {
  // wait for WiFi connection
if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    client.setInsecure();
    if (!mqttclient.connected()) 
    {
      while(!mqttclient.connect("ESP8266", "ESP8266","Juve2020")) 

        {
        Serial.println("conectando");
        }

        Serial.println("conectadoooo");
        mqttclient.subscribe("PE_LUZ_1");
        mqttclient.subscribe("PE_LUZ_2");
    }
    else {

      mqttclient.loop();

    }
  }
}


void callback(char* topic, byte* payload, unsigned int length)  //logica para cada subscripcion
{
     if (strcmp(topic, "PE_LUZ_1") == 0){

      if(payload[0] == '1'){
          Serial.println("Led on");
          digitalWrite(LED_BUILTIN, HIGH);
        }

      if(payload[0] == '0'){
      Serial.println("Led off");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

    if (strcmp(topic, "PE_LUZ_2") == 0){

      if(payload[0] == '1'){
          Serial.println("Led on");
          digitalWrite(LED_BUILTIN, HIGH);
        }

      if(payload[0] == '0'){
      Serial.println("Led off");
      digitalWrite(LED_BUILTIN, LOW);
    }                                                                                                                                                                         
  }
}