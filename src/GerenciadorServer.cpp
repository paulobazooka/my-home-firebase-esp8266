#include "GerenciadorServer.h"

GerenciadorServer::GerenciadorServer(Log *console, Adafruit_MQTT_Client *m, Adafruit_MQTT_Publish *fT, Adafruit_MQTT_Publish *fU, Adafruit_MQTT_Publish *fI, ServerData *aD)
{
  mqtt = m;
  l = console;
  serverData = aD;
  feedUmidade = fU;
  feedIndiceCalor = fI;
  feedTemperatura = fT;
}

GerenciadorServer::~GerenciadorServer()
{
}

bool GerenciadorServer::configurarAdafruit()
{
  mqtt = new Adafruit_MQTT_Client(&client, serverData->server, serverData->port, serverData->user, serverData->key);
  mqtt->processPackets(5000);

  feedUmidade     = new Adafruit_MQTT_Publish(mqtt, serverData->feed_umidade, serverData->qos);
  feedTemperatura = new Adafruit_MQTT_Publish(mqtt, serverData->feed_temperatura, serverData->qos);
  feedIndiceCalor = new Adafruit_MQTT_Publish(mqtt, serverData->feed_indice_calor, serverData->qos);

  if(mqtt && feedTemperatura && feedUmidade && feedIndiceCalor)
    return true;
    
  return false;  
}

bool GerenciadorServer::conectar()
{
  int8_t ret;

  if (mqtt->connected())
    return true;

  l->println(TAG, String("Conectando no broker: ") + serverData->server);
  uint8_t retries = 3;
  while ((ret = mqtt->connect()) != 0)
  {
    mqtt->disconnect();
    delay(3000);
    retries--;
    if (retries == 0)
    {
      l->println(TAG, String("Não foi possível se conectar ao broker: ") + serverData->server);
      l->println(TAG, String("Erro: ") + mqtt->connectErrorString(ret));
      return false;
    }
  }
  //l->println(TAG, String("Conectado com sucesso no broker ") + serverData->server);
  return true;
}

bool GerenciadorServer::conectado()
{
  //mqtt->ping(3);
  return mqtt->connected();
}

bool GerenciadorServer::enviarTemperatura(float temperatura)
{
  return feedTemperatura->publish(temperatura);
}

bool GerenciadorServer::enviarUmidade(float umidade)
{
  return feedUmidade->publish(umidade);
}

bool GerenciadorServer::enviarIndiceCalor(float indice)
{
  return feedIndiceCalor->publish(indice);
}