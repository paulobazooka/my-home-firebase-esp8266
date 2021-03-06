#ifndef GERENCIADORDHT
#define GERENCIADORDHT_H

#include "DHT.h"
#include "Log.h"


#define INTERVALO_LEITURA 3000

class GerenciadorDHT
{
private:
    Log *l;
    DHT *sensor;
    float *temp;
    float *umid;
    float *indice;
    unsigned long valor_ms;
    const String TAG = "DHT";

public:
    GerenciadorDHT(DHT *dht, Log *console, float *temperatura, float *umidade, float *indiceCalor);
    ~GerenciadorDHT();
    bool lerSensor();
};

#endif
