#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <Arduino.h>

String retornaTempoCorrente();
String retornaTempoCorrenteFormatado();
bool camposServerCorretosAdafruit(String user, String key, String umi, String temp, String indice);
bool camposServerCorretos(String user, String key, String server, String port, String umi, String temp, String indice);


#endif