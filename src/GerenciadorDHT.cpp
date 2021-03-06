#include "GerenciadorDHT.h"

GerenciadorDHT::GerenciadorDHT(DHT *dht, Log *console, float *temperatura, float *umidade, float *indiceCalor)
{
    l = console;
    sensor = dht;
    umid = umidade;
    temp = temperatura;
    indice = indiceCalor;

    sensor->begin();
    valor_ms = millis();
}



GerenciadorDHT::~GerenciadorDHT()
{
}



bool GerenciadorDHT::lerSensor()
{
    if(millis() - valor_ms > INTERVALO_LEITURA){
        valor_ms = millis();
        float t = sensor->readTemperature();
        float h = sensor->readHumidity();
        float i = sensor->computeHeatIndex(t, h, false);

        if (isnan(h) || isnan(t) || isnan(i))
        {
            l->println(TAG, "Erro ao ler dados do sensor!");
            return false;
        }
        else
        {
            *temp = t;
            *umid = h;
            *indice = i;
            return true;
        }
    }
}
