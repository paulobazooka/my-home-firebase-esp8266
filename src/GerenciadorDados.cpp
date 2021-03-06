#include "GerenciadorDados.h"

GerenciadorDados::GerenciadorDados(){}
GerenciadorDados::~GerenciadorDados(){}

bool GerenciadorDados::dadosWifiValidos(String ssid, String password){
    if(ssid.isEmpty() || password.isEmpty())
        return false;
    
    if(!ssid || !password)
        return false;

    if(ssid.length() > 50 || password.length() > 50)
        return false;
    
    return true;
}