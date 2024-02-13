#include <ESP8266WiFi.h>   // Biblioteca necesaria para las conexiones Wifi de las placas con chip ESP8266
#include <PubSubClient.h>  // Biblioteca ecesaria para poder publicar y recibir mensajes MQTT
#include <ArduinoOTA.h>    // Biblioteca necesaria para poder programar por OTA
#include <WiFiManager.h>
///// PARAMETROS CONFIGURABLES /////
const char* mqtt_server = "192.168.1.36";  // I.P. de tu servidor MQTT
int mqttport = 1883;                       // Puerto para MQTT
const char* mqttusuario = "esp8266";          // Usuario MQTT en Home Assistant
const char* mqttpass = "Juve2020";             // Contraseña para el usuario MQTT en Home Assistant
const char* OTA_password = "PASS_OTA";         // Contraseña OTA
#define CLIENT_ID "LUZ_PRUEBA"               // ID del dispositivo, debe ser único en tu sistema
#define MQTT_TOPIC1_COMMAND "luces/1/comando"              // Topic luz Nº1
#define MQTT_TOPIC2_COMMAND "luces/2/comando"              // Topic luz Nº2
#define MQTT_TOPIC3_COMMAND "luces/3/comando"              // Topic luz Nº3
#define MQTT_TOPIC4_COMMAND "luces/4/comando"              // Topic luz Nº4
#define MQTT_TOPIC1_STATE "luces/1/estado"
#define MQTT_TOPIC2_STATE "luces/2/estado"
#define MQTT_TOPIC3_STATE "luces/3/estado"
#define MQTT_TOPIC4_STATE "luces/4/estado"
int RECONECT = 5;                         // Segundos hasta nuevo intento de conexión
WiFiManager wm;

#define LUZ_1 D0                   // Lo conectamos a D5
#define LUZ_2 D3                     // Lo conectamos a D6
#define LUZ_3 D2                   // Lo conectamos a D7
#define LUZ_4 D8                     // Lo conectamos a RX
#define SW_LUZ_1 D5                   // Boton1 al D1
#define SW_LUZ_2 D1                     // Boton2 al D2
#define SW_LUZ_3 D6                     // Boton3 al D3
#define SW_LUZ_4 D7                    // Boton4 al D4


void ICACHE_RAM_ATTR Luz1();
void ICACHE_RAM_ATTR Luz2();
void ICACHE_RAM_ATTR Luz3();
void ICACHE_RAM_ATTR Luz4();

void reconnect(void);
///// FIN PARAMETROS CONFIGURABLES /////


bool flancowifi = true;   // Flanco para activar la funcion de conectar a Wifi
bool flancomqtt = false;  // Flanco para activar la funcion de conectar a MQTT
bool contador = false;    // Flanco para activar la funcion de contador para intentar nueva conexión a Wifi
int cont = 0;             // Variable para contar los ciclos

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);  // Iniciamos Serial
  Serial.println("Iniciando");

  pinMode(LUZ_1, OUTPUT);  // Establecemos los relés como salida
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
  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res)
    Serial.println("Failed to connect");
  else
    Serial.println("WIFI connected");

  client.setServer(mqtt_server, mqttport);  // Iniciamos setServer con los datos porpocionados
  client.setCallback(callback);             // Iniciamos Callback para recibir mensajes de MQTT
  Serial.println("Finalizado");
}

void loop() {

  //// INTERRUPTOR 1 ////

  //// FIN CODIGO INTERRUPTORES ////

  //// CONECTAMOS A WIFI Y MQTT ////

  if ((WiFi.status() != WL_CONNECTED) && (flancowifi == true)) {  // Si no estamos conectados a Wifi y "flancowifi" está activado:
    setup_wifi();                                                 // Llamamos a la funcion "setup_wifi"
    reconnect();                                                  // Llamamos a la funcion "reconnect"
    client.setServer(mqtt_server, mqttport);                      // Iniciamos "setServer" con los datos porpocionados
    client.setCallback(callback);                                 // Iniciamos "Callback" para recibir mensajes de MQTT
  }
  if (!client.connected()) {  // Si seguimos sin estar conectados:
    contador = true;          // Activamos el "contador"
    cont++;                   // "cont" suma de 1 en 1 en cada ciclo
    delay(10);                // Esperamos 10 milisegundos
  }

  if ((contador == true) && (cont == 1000)) {  // Si el "contador" está activado y "cont" ha llegado a 1000 ciclos (10 segundos)
    flancomqtt = true;                         // Activamos "flancomqtt"
    flancowifi = true;                         // Activamos "flancowifi"
    setup_wifi();                              // Iniciamos la conexión a Wifi
    reconnect();                               // Iniciamos la función "reconnect"
  }
  client.loop();  // Esta función revisa en cada ciclo si hay que enviar o leer mensaje por MQTT
}

void setup_wifi() {  // Configuración para conectar a Wifi
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");  //  Mostramos por serial el Wifi que nos queremos conectar
  WiFi.mode(WIFI_STA);            // Configuración de tipo de conexión

  while ((WiFi.status() != WL_CONNECTED) && (flancowifi == true)) {  // Si no estamos conectados a Wifi y "flancowifi" está activado:
    for (int i = 0; i <= 5; i++) {                                   // Iniciamos una cuenta de 5 ciclos
      Serial.println(i);                                             // Imprimimos el conteo por serial
      delay(1000);                                                   // Espermos 1 segundo en cada ciclo
      if (i == 5) {                                                  // Si pasados los 5 segundos (5 ciclos) no hemos conectado:
        flancowifi = false;                                          // Desactivamos "flancowifi" para no volver a reconectar hasta el tiempo estipulado
        contador = true;                                             // Iniciamos el "contador"
        break;                                                       // Salimos de la funcion "for"
      }
    }
  }

  if (WiFi.status() != WL_CONNECTED) {                                    // Si no hemos conectado tras el intento:
    Serial.print("No se ha podido conectar al Wifi, nuevo intento en ");  // Esperamos para conectar al Wifi durante los segundos configurados.
    Serial.print(RECONECT);                                               // Mostramos los segundos configurados
    Serial.println(" segundos...");                                       //
    flancowifi = false;                                                   // Desactivamos "flancowifi" para no volver a conectar hasta pasados los segundos estipulados en RECONECT
  }

  else {                             // Si no ocurre lo anterior:
    Serial.println("");                //
    Serial.println("WiFi conectado");  // Confirmamos que hemos conectado a Wifi
    Serial.println("Dirección IP: ");  // Y mostramos la IP otorgada por el router
    Serial.println(WiFi.localIP());    // IP otorgada por el router
    cont = 0;                          // Establecemos en "cont" a cero
    flancowifi = false;                // Establecemos "flancowifi" a false porque ya estamos conectados
    contador = false;                  // Establecemos "contador" a false porque ya estamos conectados
  }
}

void callback(char* topic, byte* payload, unsigned int length){  // Función para poder recibir los mensajes por MQTT

    if (strcmp(topic, MQTT_TOPIC1_COMMAND) == 0){  // Si recibimos un mensaje del topic configurado en "topic_LUZ_1":
      if ((char)payload[0] == '1') {         // Si el mensaje es "1":
        Luz1();
    }
    else if((char)payload[0] == '0')
      Luz1();
  }


    if (strcmp(topic, MQTT_TOPIC2_COMMAND) == 0){
      if ((char)payload[0] == '1') {
        Luz2();
    }
    else if((char)payload[0] == '0')
      Luz2();
  }


    if (strcmp(topic, MQTT_TOPIC3_COMMAND) == 0){
      if ((char)payload[0] == '1') {
        Luz3();
    }
    else if((char)payload[0] == '0')
      Luz3();
  }


    if (strcmp(topic, MQTT_TOPIC4_COMMAND) == 0){
      if ((char)payload[0] == '1') {
        Luz4();
    }
    else if((char)payload[0] == '0')
      Luz4();

  }
}


void reconnect(){  // Función para reconectar

  while ((!client.connected()) && (flancomqtt == true)) {    // Si no estamos conectados a MQTT y "flancomqtt" lo tenemos activado:
    Serial.println("Conectando con MQTT");                   // Mostramos por serial que estamos intentando conectar
    if (client.connect(CLIENT_ID, mqttusuario, mqttpass)) {  // Si conectamos a Wifi, establecemos la configuración para conectarnos
      Serial.println("conectado");                           // Mostramos que hemos conectado
      client.subscribe(MQTT_TOPIC1_COMMAND);
      client.subscribe(MQTT_TOPIC2_COMMAND);
      client.subscribe(MQTT_TOPIC3_COMMAND);
      client.subscribe(MQTT_TOPIC4_COMMAND);
      client.subscribe(MQTT_TOPIC1_STATE);
      client.subscribe(MQTT_TOPIC2_STATE);
      client.subscribe(MQTT_TOPIC3_STATE);
      client.subscribe(MQTT_TOPIC4_STATE); 
  }
    
    else {                                                        // Si no hemos podido conectar:
      Serial.print("No se ha podido conectar, nuevo intento en ");  // Intenamos conectar al servidos MQTT durante los segundos configurados.
      Serial.print(RECONECT);                                       // Mostramos los segundos configurados en "RECONECT"
      Serial.println(" segundos...");
      cont = 0;            // Establecemos "cont" a cero
      flancowifi = false;  // Establecemos "flancowifi" en false para no volver a internar la reconexión hasta que pasen los segundos configurados
      flancomqtt = false;  // Establecemos "flancomqtt" en false para no volver a internar la reconexión hasta que pasen los segundos configurados
      contador = true;     // Establecemos "contador" en true para iniciar el contador de los segundos establecidos he intentar la reconexión pasado ese tiempo
    }
  }
}


void ICACHE_RAM_ATTR Luz1(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
    digitalWrite(LUZ_1, !digitalRead(LUZ_1));
    if(digitalRead(LUZ_1) == HIGH)
      client.publish(MQTT_TOPIC1_STATE, "1", true);

    else{client.publish(MQTT_TOPIC1_STATE, "0", true);}

    Serial.println("int1");
  }
}

void ICACHE_RAM_ATTR Luz2(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
    digitalWrite(LUZ_2, !digitalRead(LUZ_2));
    if(digitalRead(LUZ_2) == HIGH)
      client.publish(MQTT_TOPIC2_STATE, "1", true);

    else{client.publish(MQTT_TOPIC2_STATE, "0", true);}
    
    Serial.println("int2");
  }
}
void ICACHE_RAM_ATTR Luz3(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
    digitalWrite(LUZ_3, !digitalRead(LUZ_3));
    if(digitalRead(LUZ_3) == HIGH)
      client.publish(MQTT_TOPIC3_STATE, "1", true);

    else{client.publish(MQTT_TOPIC3_STATE, "0", true);}
    
    Serial.println("int3");
  }
}

void ICACHE_RAM_ATTR Luz4(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 50) {
    lastInterruptTime = interruptTime;
    digitalWrite(LUZ_4, !digitalRead(LUZ_4));
    if(digitalRead(LUZ_4) == HIGH)
      client.publish(MQTT_TOPIC4_STATE, "1", true);

    else{client.publish(MQTT_TOPIC4_STATE, "0", true);}
    Serial.println("int4");
  }
}