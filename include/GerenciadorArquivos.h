#ifndef GERENCIADORARQUIVOS_H
#define GERENCIADORARQUIVOS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Estruturas.h"
#include <LittleFS.h>
#include "Log.h" 

#define MODO_LEITURA "r"
#define MODO_ESCRITA "w"
#define MODO_APENDICE "a"

#define ARQUIVO_CONFIG_WIFI "/config/wifi.json"
#define ARQUIVO_DISPOSITIVO "/config/dispositivo.json"
#define ARQUIVO_CONFIG_ADAFRUIT "/config/server.json"
#define ARQUIVO_CONFIG_REGISTROS "/log/registro.log"

class GerenciadorArquivos
{
private:
  Log *l;
  bool ok = false;
  File _abrirArquivo(const char *path);
  String abrirArquivo(const char *path);
  const String TAG = "FILE";

public:
  GerenciadorArquivos();
  ~GerenciadorArquivos();
  GerenciadorArquivos(Log *console);

  String deviceID();
  bool iniciarParticao();
  boolean particaoIntegra();
  void encerrarParticao();
  void obterDevice(Device *dispositivo);
  void atualizarNumeroBoot(Device *dispositivo);
  void obterWifiData(WifiData *wifiData);
  String obterStringArquivo(const char *path); 
  StaticJsonDocument<512> obterJson(File file);
  void obterServerData(ServerData *serverData);
  StaticJsonDocument<512> obterJson(const char *path);
  String hexStr(const unsigned long &h, const byte &l);
  StaticJsonDocument<512> obterJsonString(String conteudo); 
  void adicionarConteudoArquivo(const char *path,const char *content); 
  boolean escreverArquivo(StaticJsonDocument<512> doc, const char *path);
};

#endif