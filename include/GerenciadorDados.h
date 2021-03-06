#ifndef GERENCIADORDADOS_H
#define GERENCIADORDADOS_H

#include <Arduino.h>

class GerenciadorDados
{
private:
    
public:
    GerenciadorDados();
    ~GerenciadorDados();
    bool dadosWifiValidos(String ssid, String password);
};

#endif