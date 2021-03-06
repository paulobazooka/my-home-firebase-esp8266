#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

typedef struct wifi_data
{
    String ssid;
    String password;
} WifiData;

typedef struct server_data
{
    bool ativo;
    uint16_t port;
    uint8_t qos;
    char user[30];
    char key[50];
    char server[30];
    char feed_umidade[80];
    char feed_temperatura[80];
    char feed_indice_calor[80];
} ServerData;

typedef struct device
{
    char id[30];
    uint32_t boot = 0;
    char projeto[30];
    char device[20];
    char modelo[20];
    char versao_firmware[20];
    char versao_interface[20];
    char url_update[150];
} Device;

#endif