#ifndef GERENCIADORWIFI_H
#define GERENCIADORWIFI_H

#include "Log.h"
#include <iostream>
#include <LittleFS.h>
#include <DNSServer.h>
#include "Estruturas.h"
#include "Utilidades.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "AsyncWebSocket.h"
#include "GerenciadorDados.h"
#include "ESPAsyncWebServer.h"
#include "GerenciadorArquivos.h"


class GerenciadorWifi
{
private:
    Log *l;
    Device *device;
    bool *restart;
    WifiData *wifiData;
    AsyncWebServer *server;
    const String TAG = "WIFI";
    ServerData *serverData;
    GerenciadorDados *gerenciadorDados;
    GerenciadorArquivos *gerenciadorArquivos;
    const int TEMPO_REINICIALIZACAO = 3000;
    DNSServer dnsServer;        // DNS para portal cativo
    IPAddress ipAP; // Endereço de IP
    const byte DNS_PORT = 53; // Porta para modo DNS em Access Point


    String processor(const String& var);

public:
    GerenciadorWifi(Log *console, AsyncWebServer *s, GerenciadorArquivos *ga);
    GerenciadorWifi(AsyncWebServer *s, GerenciadorArquivos *ga, Log *console, ServerData *ad, WifiData *wd, Device *d, bool *reiniciar);
    ~GerenciadorWifi();
    void capturarRequisicoes();
    bool conectado();
    IPAddress localIp();
    void modoStation();
    void conectarWiFi();
    void modoAccessPoint();
    void configurarServidor();
    void conectarWiFi(WifiData *wifiData);
    void conectarWiFi(String ssid, String pass);
};

#endif