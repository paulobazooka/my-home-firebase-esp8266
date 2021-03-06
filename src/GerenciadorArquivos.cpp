#include "GerenciadorArquivos.h"

GerenciadorArquivos::GerenciadorArquivos(Log *console)
{
  l = console;
}

GerenciadorArquivos::~GerenciadorArquivos()
{
}

bool GerenciadorArquivos::iniciarParticao(){
  bool ok = LittleFS.begin();
  if(ok)
    l->println(TAG, "Partição iniciado com sucesso!");
  else  
    l->println(TAG, "Erro: Partição não iniciada!");

  return ok;
}

void GerenciadorArquivos::encerrarParticao(){
  l->println(TAG, "Partição encerrada!");
  LittleFS.end();
}

boolean GerenciadorArquivos::particaoIntegra(){
  return ok;
}

String GerenciadorArquivos::abrirArquivo(const char* path){
  File file = LittleFS.open(path, MODO_LEITURA);
  if(!file){
    file.close();
    l->println(TAG, String("Arquivo ") + path + String(" não aberto ou não encontrado!"));
    return "{}";
  }
  String content = file.readString();
  file.close();
  return content;
}

File GerenciadorArquivos::_abrirArquivo(const char* path){
  File file = LittleFS.open(path, MODO_LEITURA);
  if(!file)
    l->println(TAG, String("Arquivo ") + path + String(" não aberto ou não encontrado!"));
  return file;
}

StaticJsonDocument<512> GerenciadorArquivos::obterJson(const char* path){
  StaticJsonDocument<512> doc;
  String content = abrirArquivo(path);
  deserializeJson(doc, content);
  return doc;
}

StaticJsonDocument<512> GerenciadorArquivos::obterJson(File file){
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    l->println(TAG, String("deserializeJson() falhou: ") + error.f_str());
  }
  file.close();
  return doc;
}

StaticJsonDocument<512> GerenciadorArquivos::obterJsonString(String conteudo){
  StaticJsonDocument<512> doc;
  conteudo.trim();
  if(deserializeJson(doc, conteudo)){
    l->println(TAG, "Erro ao deserializar string em Json: " + conteudo);
  }
  return doc;
}

boolean GerenciadorArquivos::escreverArquivo(StaticJsonDocument<512> doc, const char* path){
  File file = LittleFS.open(path, MODO_ESCRITA);
  if(!file){
    file.close();
    return false;
  }
  serializeJson(doc, file);
  file.close();
  return true;
}

void GerenciadorArquivos::atualizarNumeroBoot(Device *dispositivo){
  File file = LittleFS.open(ARQUIVO_DISPOSITIVO, MODO_LEITURA);
  StaticJsonDocument<512> json;
  if(file){
    deserializeJson(json, file);
    json["boot"] = ((int)json["boot"]) + 1;
    dispositivo->boot = json["boot"];      
  }
  file.close();
  file = LittleFS.open(ARQUIVO_DISPOSITIVO, MODO_ESCRITA);
  if(file)
    serializeJson(json, file);
  file.close();
  //l->println(TAG, json);
}

void GerenciadorArquivos::obterDevice(Device *dispositivo){
    StaticJsonDocument<512> doc = obterJson(ARQUIVO_DISPOSITIVO);
    if(doc.size() == 0)
      l->println(TAG, "Erro ao aobter arquivo Json" + String(ARQUIVO_DISPOSITIVO));

    strlcpy(dispositivo->id, deviceID().c_str(), sizeof(dispositivo->id));  
    strlcpy(dispositivo->device, doc["device"] | "", sizeof(dispositivo->device));  
    strlcpy(dispositivo->modelo, doc["modelo"] | "", sizeof(dispositivo->modelo));
    strlcpy(dispositivo->projeto, doc["projeto"] | "", sizeof(dispositivo->projeto));
    strlcpy(dispositivo->url_update, doc["url_update"] | "", sizeof(dispositivo->url_update));
    strlcpy(dispositivo->versao_firmware, doc["versao_firmware"] | "", sizeof(dispositivo->versao_firmware));
    strlcpy(dispositivo->versao_interface, doc["versao_interface"] | "", sizeof(dispositivo->versao_interface));

   // l->println(TAG, doc);
}

void GerenciadorArquivos::obterWifiData(WifiData *wifiData){
    StaticJsonDocument<512> doc = obterJson(ARQUIVO_CONFIG_WIFI);
    if(doc.size() == 0)
      l->println(TAG, "Erro ao aobter arquivo Json" + String(ARQUIVO_CONFIG_WIFI));

    String ssid = doc["ssid"];
    String password = doc["password"];
    wifiData->ssid = ssid;
    wifiData->password = password;

    //l->println(TAG, doc);
}

void GerenciadorArquivos::obterServerData(ServerData *serverData){
   StaticJsonDocument<512> doc = obterJson(ARQUIVO_CONFIG_ADAFRUIT);
    if(doc.size() == 0)
      l->println(TAG, "Erro ao aobter arquivo Json" + String(ARQUIVO_CONFIG_ADAFRUIT));

      serverData->port = doc["port"];
      serverData->ativo = doc["ativo"];
      serverData->qos = doc["qos"];
      strlcpy(serverData->key, doc["key"] | "", sizeof(serverData->key));
      strlcpy(serverData->user, doc["user"] | "", sizeof(serverData->user)); 
      strlcpy(serverData->server, doc["server"] | "", sizeof(serverData->server));
      strlcpy(serverData->feed_umidade, doc["feed_umidade"] | "", sizeof(serverData->feed_umidade));
      strlcpy(serverData->feed_temperatura, doc["feed_temperatura"] | "", sizeof(serverData->feed_temperatura));
      strlcpy(serverData->feed_indice_calor, doc["feed_indice_calor"] | "", sizeof(serverData->feed_indice_calor));
     // l->println(TAG, doc);
}

String GerenciadorArquivos::obterStringArquivo(const char *path){
   File file = LittleFS.open(path, MODO_LEITURA);
   String content = file.readString();
   file.close();
   return content; 
}

void GerenciadorArquivos::adicionarConteudoArquivo(const char *path, const char *content){
    File f = LittleFS.open(path, MODO_LEITURA);
    
    if(f &&f.size() > 300000){ // maior que 300kb
      l->println(TAG, String("Arquivo maior que o permitido! ") + String(f.size()) + String("kb - deletando..."));
      f.close();
      File file = LittleFS.open(path, MODO_ESCRITA);
      if(file)
        file.print(content);
      
      file.close();
    }
    else{
      File file = LittleFS.open(path, MODO_APENDICE);
      if(file)
        file.print(content);
      file.close();
      }
}

String GerenciadorArquivos::hexStr(const unsigned long &h, const byte &l = 8) {
  // Retorna valor em formato hexadecimal
  String s;
  s= String(h, HEX);
  s.toUpperCase();
  s = ("00000000" + s).substring(s.length() + 8 - l);
  return s;
}

String GerenciadorArquivos::deviceID() {
  // Retorna ID padrão
  #ifdef ESP8266
    // ESP8266 utiliza função getChipId()
    return "ESP8266-" + hexStr(ESP.getChipId(), 8);
  #else
    // ESP32 utiliza função getEfuseMac()
    return "ESP32-" + hexStr(ESP.getEfuseMac());
  #endif
}

