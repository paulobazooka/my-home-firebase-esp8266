#ifndef GERENCIADORUPDATE_H
#define GERENCIADORUPDATE_H

#include "Log.h"
#include "Estruturas.h"
#include "GerenciadorLog.h"
#include "ESP8266httpUpdate.h"
#include <ArduinoJson.h>

class GerenciadorUpdate
{
private:
    Log *l;
    Device *d;
    HTTPClient http;
    GerenciadorLog *gL;
    WiFiClientSecure client;
    const char *TAG = "UPDT";
    void updateStarted();
    void updateFinished();
    void updateError(int err);
    void updateProgress(int progresso, int total);

public:
    GerenciadorUpdate(Log *console, GerenciadorLog *geL, Device *device);
    ~GerenciadorUpdate();
    void verificarAtualizacao();
};

/**
 * @param console &Log
 * @param geL   &GerenciadorLog
 * @param device &Device
 */
GerenciadorUpdate::GerenciadorUpdate(Log *console, GerenciadorLog *geL, Device *device)
{
    l = console;
    gL = geL;
    d = device;
    client.setInsecure();
}

GerenciadorUpdate::~GerenciadorUpdate()
{
}

void GerenciadorUpdate::verificarAtualizacao()
{

    l->println(TAG, "Verificando atualização");

    http.begin(client, d->url_update);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String s = http.getString();

        s.trim();
        StaticJsonDocument<512> json;
        if (deserializeJson(json, s))
        {
            l->println(TAG, "Erro ao deserializar json de versão");
        }
        else
        {
            l->println(TAG, json);
            
            ESPhttpUpdate.rebootOnUpdate(false);
            ESPhttpUpdate.onEnd([this]() -> void { return updateFinished(); } );
            ESPhttpUpdate.onStart([this]() -> void { return updateStarted(); } );
            ESPhttpUpdate.onError([this] (int error) -> void { return updateError(error); } );
            ESPhttpUpdate.onProgress([this](int progresso, int total) -> void { return updateProgress(progresso, total); } );

            //t_httpUpdate_return r = ESPhttpUpdate.update(client, d->versao_firmware);
            //t_httpUpdate_return r = ESPhttpUpdate.updateSpiffs(client, d->versao_interface);
        }
    }
    else
    {
        l->println(TAG, "Erro de http cod.:" + httpCode);
    }

    http.end();
}

void GerenciadorUpdate::updateStarted()
{
    l->println(TAG, "Atualização iniciada");
    gL->inserirRegistro(TAG, "Atualização iniciada");
}

void GerenciadorUpdate::updateFinished()
{
    l->println(TAG, "Atualização finalizada");
    gL->inserirRegistro(TAG, "Atualização finalizada");
}

void GerenciadorUpdate::updateError(int err)
{
    String erro = "Erro durante a atualização: ";
    erro += ESPhttpUpdate.getLastError() + String(" - ") + ESPhttpUpdate.getLastErrorString().c_str();
    l->println(TAG, erro);
    gL->inserirRegistro(TAG, erro.c_str());
}

void GerenciadorUpdate::updateProgress(int progresso, int total)
{
    int avanco = progresso *100/ total;
    l->print(String(avanco) + String("% "));
}

#endif