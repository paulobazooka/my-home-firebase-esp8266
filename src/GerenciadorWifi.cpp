#include "GerenciadorWifi.h"

GerenciadorWifi::GerenciadorWifi(Log *console, AsyncWebServer *s, GerenciadorArquivos *ga)
{
  l = console;
  server = s;
  gerenciadorArquivos = ga;
}

GerenciadorWifi::GerenciadorWifi(AsyncWebServer *s, GerenciadorArquivos *ga, Log *console, ServerData *ad, WifiData *wd, Device *d, bool *reiniciar)
{
  l = console;
  server = s;
  serverData = ad;
  wifiData = wd;
  device = d;
  gerenciadorArquivos = ga;
  gerenciadorDados = new GerenciadorDados();
  restart = reiniciar;
  ipAP = IPAddress(192, 168, 4, 1);
}

GerenciadorWifi::~GerenciadorWifi() {}

bool GerenciadorWifi::conectado() { return WiFi.status() == WL_CONNECTED; }

void GerenciadorWifi::conectarWiFi(String ssid, String pass)
{
  l->println(TAG, "Conectando no roteador");
  WiFi.begin(ssid, pass);
  for (size_t i = 0; i < 5; i++)
  {
    if (!WiFi.isConnected())
    {
      delay(500);
    }
  }
}

void GerenciadorWifi::conectarWiFi()
{
  l->println(TAG, "Conectando no roteador");
  StaticJsonDocument<200> doc = gerenciadorArquivos->obterJson(ARQUIVO_CONFIG_WIFI);
  String ssid = doc["ssid"];
  String pass = doc["password"];
  WiFi.begin(ssid, pass);
  for (size_t i = 0; i < 5; i++)
  {
    if (!WiFi.isConnected())
    {
      delay(500);
    }
  }
}

void GerenciadorWifi::conectarWiFi(WifiData *wifiData)
{
  // l->println(TAG, "Conectando na rede " + wifiData->ssid + " " + wifiData->password);

  WiFi.begin(wifiData->ssid, wifiData->password);
  for (size_t i = 0; i < 5; i++)
  {
    if (!WiFi.isConnected())
    {
      delay(500);
    }
  }

  if (WiFi.isConnected())
    l->println(TAG, String("Conectado com sucesso na rede ") + wifiData->ssid);
}

void GerenciadorWifi::capturarRequisicoes()
{
  dnsServer.processNextRequest();
}

void GerenciadorWifi::modoStation()
{
  WiFi.mode(WIFI_STA);
  l->println(TAG, "Modo Station ativado");
}

void GerenciadorWifi::modoAccessPoint()
{
  l->println(TAG, "Modo Access Point ativado");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Termo-Higômetro");
  //WiFi.enableAP(true);
  WiFi.softAPConfig(*ipAP, *ipAP, IPAddress(255, 255, 255, 0));
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", *ipAP);
}

IPAddress GerenciadorWifi::localIp() { return WiFi.localIP(); }

void GerenciadorWifi::configurarServidor()
{
  // Rota GET em /
  server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/html/index.html", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/index.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/index.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });

  // ****************************************************************************


 

  // Rota GET em /wifi
  server->on("/wifi", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "GET em /wifi");
    request->send(LittleFS, "/html/wifi.html", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  // Rota POST em /wifi
  server->on("/wifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->hasArg("ssid") && request->hasArg("password"))
    {
      AsyncWebParameter *s = request->getParam("ssid", true);
      AsyncWebParameter *p = request->getParam("password", true);
      if (gerenciadorDados->dadosWifiValidos(s->value(), p->value()))
      {
        GerenciadorArquivos g = GerenciadorArquivos(l);
        StaticJsonDocument<512> json = g.obterJson(ARQUIVO_CONFIG_WIFI);
        json["ssid"] = s->value();
        json["password"] = p->value();
        if (g.escreverArquivo(json, ARQUIVO_CONFIG_WIFI))
        {
          //l->println(TAG, json);
          request->redirect("/update");
        }
        else
          request->send(LittleFS, "/html/index.html", String(), false, [this](const String &var) -> String { return processor(var); });
      }
    }

    request->send(LittleFS, "/html/index.html", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/wifi.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/wifi.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });

  // ****************************************************************************

  // Rota GET em /server
  server->on("/server", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "GET em /adafruit");
    request->send(LittleFS, "/html/server.html", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  server->on("/script/server.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script/server.js", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  // Rota POST em /server
  server->on("/server", HTTP_POST, [this](AsyncWebServerRequest *request) {
    GerenciadorArquivos g = GerenciadorArquivos(l);
    StaticJsonDocument<512> json = g.obterJson(ARQUIVO_CONFIG_ADAFRUIT);

    json["ativo"] = true;

    if (request->hasArg("is_adafruit"))
    {
      if (request->hasArg("user") && request->hasArg("key") && request->hasArg("feed_temp") && request->hasArg("feed_umi") && request->hasArg("feed_indice"))
      {
        String key = request->getParam("key", true)->value();
        String user = request->getParam("user", true)->value();
        String umidade = user + "/feeds/" + request->getParam("feed_umi", true)->value();
        String temperatura = user + "/feeds/" + request->getParam("feed_temp", true)->value();
        String indice = user + "/feeds/" + request->getParam("feed_indice", true)->value();

        if (!camposServerCorretosAdafruit(user, key, request->getParam("feed_umi", true)->value(), request->getParam("feed_temp", true)->value(), request->getParam("feed_indice", true)->value()))
        {
          request->redirect("/error");
        }
        else
        {
          json["key"] = key;
          json["port"] = 1883;
          json["user"] = user;
          json["is_adafruit"] = true;
          json["feed_umidade"] = umidade;
          json["feed_temp"] = temperatura;
          json["feed_indice_calor"] = indice;
          json["server"] = "io.adafruit.com";

          if (g.escreverArquivo(json, ARQUIVO_CONFIG_ADAFRUIT))
          {
            request->redirect("/update");
          }
          else
          {
            request->redirect("/error");
          }
        }
      }
      else
        request->redirect("/error");
    }
    else
    {
      if (request->hasArg("server") && request->hasArg("port") && request->hasArg("user") && request->hasArg("key") && request->hasArg("feed_temp") && request->hasArg("feed_umi") && request->hasArg("feed_indice"))
      {

        String key = request->getParam("key", true)->value();
        String port = request->getParam("port", true)->value();
        String user = request->getParam("user", true)->value();
        String server = request->getParam("server", true)->value();
        String feed_temp = request->getParam("feed_temp", true)->value();
        String feed_umidade = request->getParam("feed_umi", true)->value();
        String feed_indice_calor = request->getParam("feed_indice", true)->value();

        if (!camposServerCorretos(user, key, server, port, feed_umidade, feed_temp, feed_indice_calor))
        {
          request->redirect("/error");
        }
        else
        {
          json["is_adafruit"] = false;
          json["key"] = key;
          json["user"] = user;
          json["server"] = server;
          json["port"] = port;
          json["feed_temp"] = feed_temp;
          json["feed_umidade"] = feed_umidade;
          json["feed_indice_calor"] = feed_indice_calor;

          if (g.escreverArquivo(json, ARQUIVO_CONFIG_ADAFRUIT))
          {
            request->redirect("/update");
          }
          else
          {
            request->redirect("/error");
          }
        }
      }
      else
        request->redirect("/error");
    }
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/server.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/server.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });
  // ****************************************************************************

  // ****************************************************************************

  // Rota GET em /
  server->on("/log", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //request->send(LittleFS, "/html/log.html", String(), false, [this](const String &var) -> String { return processor(var); });
    /*File file = LittleFS.open("/log/registro.log", "r");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html lang='pt'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1, minimum-scale=1.0, shrink-to-fit=no'><link rel='icon' href='../image/favicon.ico' type='image/x-icon' /><link href='../style/log.css' rel='stylesheet'><title>REGISTROS</title></head><body><div class='wrapper'><div><h1>Registros de Eventos</h1></div><div class='table-style'><table><thead><tr><th>tempo funcionamento</th><th>TAG</th><th>mensagem</th></tr></thead><tbody>");
    if(file){
      while(file.available())
        response->print((char)file.read());
    }
    response->print("</tbody></table></div></div></body></html>");
    request->send(response);*/
    request->send(LittleFS, "/log/registro.log", String(), true);
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/log.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/log.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });

  // ****************************************************************************

  // ****************************************************************************
  // Rota GET em /atualizacao
  server->on("/update", HTTP_GET, [this](AsyncWebServerRequest *request) {
    *restart = true;
    request->send(LittleFS, "/html/update.html", String(), false, [this](const String &var) -> String { return processor(var); });
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/update.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/update.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });

  // ****************************************************************************


  // ****************************************************************************
  // Rota GET em /Erro
  server->on("/error", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/html/error.html");
  });

  // Rota para carregar o arquivo style.css
  server->on("/style/error.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "style.css");
    request->send(LittleFS, "/style/error.css", "text/css");
  });

  // Rota para carregar o arquivio favicon.css
  server->on("/image/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //l->println(TAG, "favicon.ico");
    request->send(LittleFS, "/image/favicon.ico", "image/x-icon");
  });

  // ****************************************************************************

  // Rota para request solicitações não encontradas
  server->onNotFound([this](AsyncWebServerRequest *request) {
    //l->println(TAG, "on not found!");
    request->send(LittleFS, "/html/index.html");
  });
  // ****************************************************************************

  // Arquivos estáticos
  //server->serveStatic("/", LittleFS, "/html/").setTemplateProcessor([this](const String &var) -> String { return processor(var); });
  server->serveStatic("/", LittleFS, "/image/").setCacheControl("max-age=86400");
  server->serveStatic("/", LittleFS, "/style/").setCacheControl("max-age=86400");
  server->serveStatic("/server", LittleFS, "/script/").setCacheControl("max-age=86400");

  // Iniciar servidor
  server->begin();
}

String GerenciadorWifi::processor(const String &var)
{

  // Se for requisição da tela index, com dados do dispositivo
  if (var == "modelo" || var == "versao_firmware" || var == "versao_interface")
  {
    if (var == "versao_firmware")
      return device->versao_firmware;
    if (var == "modelo")
      return device->modelo;
    if (var == "versao_interface")
      return device->versao_interface;
    ;
  }
  /*
  // Se a requisição for de configuração de wifi
  if (var == "ssid" || var == "password")
  {
    if (var == "ssid")
      return wifiData->ssid;
    else
      return wifiData->password;
  }

  // Se a requisição for de configuração do adafruit
  if (var == "user" || var == "key" || var == "feed_temp" || var == "feed_umi" || var == "server" || var == "port" || var == "feed_indice")
  {
    if (var == "user")
      return serverData->user;
    if (var == "key")
      return serverData->key;
    if (var == "server")
      return serverData->server;
    if (var == "port")
      return String(serverData->port);
    if (var == "feed_umi")
      return serverData->feed_umidade;
    if (var == "feed_temp")
      return serverData->feed_temperatura;
    if (var == "feed_indice")
      return serverData->feed_indice_calor;
  }*/

  if (var == "segundos")
    return String(TEMPO_REINICIALIZACAO / 1000);

  // default
  return String();
}