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
#define SW_LUZ_2 D0 //NO SE DEBE USAR NUNCA EL PIN 1, 6, 7 y 8 COMO SALIDA NI ENTRADA PORQUE MUERE EL ESP
#define SW_LUZ_3 A0
#define SW_LUZ_4 D7

void ICACHE_RAM_ATTR interrupcionD0();
void ICACHE_RAM_ATTR interrupcionD5();
void ICACHE_RAM_ATTR interrupcionA0();
void ICACHE_RAM_ATTR interrupcionD7();


void setup(){
  
  Serial.begin(115200);
  pinMode(LUZ_1, OUTPUT);
  pinMode(LUZ_2, OUTPUT);
  pinMode(LUZ_3, OUTPUT);
  pinMode(LUZ_4, OUTPUT);
  pinMode(SW_LUZ_1, INPUT);
  pinMode(SW_LUZ_2, INPUT);
  pinMode(SW_LUZ_3, INPUT);
  pinMode(SW_LUZ_4, INPUT);

  attachInterrupt(digitalPinToInterrupt(SW_LUZ_1), interrupcionD0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_2), interrupcionD5, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_3), interrupcionA0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_LUZ_4), interrupcionD7, CHANGE);

  bool res;

      res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res)
      Serial.println("Failed to connect");

    else 
        Serial.println("connected...yeey :)");

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
        Serial.println("conectado");
        mqttclient.subscribe("PE_LUZ_1");
        mqttclient.subscribe("PE_LUZ_2");
    }
    
    else
      mqttclient.loop();
  }
}


void callback(char* topic, byte* payload, unsigned int length){

     if (strcmp(topic, "PE_LUZ_1") == 0){
      if(payload[0] == '1'){
          Serial.println("Led");
          digitalWrite(LUZ_1, !digitalRead(LUZ_1));
      }
    }

    if (strcmp(topic, "PE_LUZ_2") == 0){
      if(payload[0] == '1'){
          Serial.println("Led");
          digitalWrite(LUZ_2, !digitalRead(LUZ_2));
      }                                                                                                                                                                       
    }

    if (strcmp(topic, "PE_LUZ_3") == 0){
      if(payload[0] == '1'){
          Serial.println("Led");
          digitalWrite(LUZ_3, !digitalRead(LUZ_3));
      }                                                                                                                                                                       
    }

    if (strcmp(topic, "PE_LUZ_4") == 0){
      if(payload[0] == '1'){
          Serial.println("Led");
          digitalWrite(LUZ_4, !digitalRead(LUZ_4));
      }                                                                                                                                                                       
    }
}

void ICACHE_RAM_ATTR interrupcionD0() {

delay(50);

if (digitalRead(SW_LUZ_1) != digitalRead(LUZ_1)) 
    digitalWrite(LUZ_1, !digitalRead(LUZ_1));
  //if(digitalRead(SW_LUZ_1) == HIGH)
    //digitalWrite(LUZ_1, HIGH);
if (digitalRead(SW_LUZ_1) == digitalRead(LUZ_1)) 
    digitalWrite(LUZ_1, !digitalRead(LUZ_1));
  //else
    //digitalWrite(LUZ_1, LOW);
}

void ICACHE_RAM_ATTR interrupcionD5(){
  if(digitalRead(SW_LUZ_2) == HIGH)
    digitalWrite(LUZ_2, HIGH);

  else
    digitalWrite(LUZ_2, LOW);
}

void ICACHE_RAM_ATTR interrupcionA0(){
  if(digitalRead(SW_LUZ_3) == HIGH)
    digitalWrite(LUZ_3, HIGH);

  else
    digitalWrite(LUZ_3, LOW);
}

void ICACHE_RAM_ATTR interrupcionD7(){
  if(digitalRead(SW_LUZ_4) == HIGH)
    digitalWrite(LUZ_4, HIGH);

  else
    digitalWrite(LUZ_4, LOW);
}