#ifndef GERENCIADORADAFRUIT_H
#define GERENCIADORADAFRUIT_H

#include "Log.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Estruturas.h"
#include "Adafruit_MQTT.h"
#include "GerenciadorArquivos.h"
#include "Adafruit_MQTT_Client.h"
#include "Estruturas.h"

class GerenciadorServer
{
private:
    Log *l;
    const String TAG = "MQTT";
    ServerData *serverData;
    WiFiClient client;
    Adafruit_MQTT_Client *mqtt;
    Adafruit_MQTT_Publish *feedUmidade;
    Adafruit_MQTT_Publish *feedIndiceCalor;
    Adafruit_MQTT_Publish *feedTemperatura;

public:
    GerenciadorServer(Log *console, Adafruit_MQTT_Client *m, Adafruit_MQTT_Publish *fT, Adafruit_MQTT_Publish *fU, Adafruit_MQTT_Publish *fI, ServerData *aD);
    ~GerenciadorServer();
    bool configurarAdafruit();
    bool conectar();
    bool conectado();
    bool enviarUmidade(float umidade);
    bool enviarIndiceCalor(float indice);
    bool enviarTemperatura(float temperatura);
};

#endif