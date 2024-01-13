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



bool estadoAnterior = LOW;


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
  int estadoActual = digitalRead(SW_LUZ_1);

if ((WiFiMulti.run() == WL_CONNECTED)) {
    client.setInsecure();
    
    if (!mqttclient.connected()) {
        if(mqttclient.connect("ESP8266", "ESP8266","Juve2020"))
        Serial.println("conectadoooo");
        mqttclient.subscribe("PE_LUZ_1");
        mqttclient.subscribe("PE_LUZ_2");
    }
    
    else 
      mqttclient.loop();
  }

  if (estadoActual != estadoAnterior) {
    // Cambia el estado del pin a cambiar (pinACambiar) al estado contrario
    digitalWrite(LUZ_1, !estadoActual);

    // Puedes incluir acciones adicionales aquí si es necesario

    // Imprime información en el puerto serie
    Serial.print("Cambio en el pin ");
    Serial.print(SW_LUZ_1);
    Serial.print(". Nuevo estado: ");
    Serial.println(estadoActual);

    // Actualiza el estado anterior
    estadoAnterior = estadoActual;
  }
}


void callback(char* topic, byte* payload, unsigned int length){

     if (strcmp(topic, "PE_LUZ_1") == 0){
      if(payload[0] == '1'){
          Serial.println("Led on");
          digitalWrite(LUZ_2, HIGH);
      }

      if(payload[0] == '0'){
      Serial.println("Led off");
      digitalWrite(LUZ_2, LOW);
      }
    }

    if (strcmp(topic, "PE_LUZ_2") == 0){

      if(payload[0] == '1'){
          Serial.println("Led on");
          digitalWrite(LUZ_2, HIGH);
        }

      if(payload[0] == '0'){
      Serial.println("Led off");
      digitalWrite(LUZ_2, LOW);
    }                                                                                                                                                                         
  }
}