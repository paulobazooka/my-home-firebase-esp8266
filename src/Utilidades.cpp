#include "Utilidades.h"

String retornaTempoCorrente()
{
  unsigned long tempo = millis();
  uint8_t segundos = (int)(tempo / 1000) % 60;
  uint8_t minutos = (int)(tempo / (1000 * 60)) % 60;
  uint8_t horas = (int)((tempo / (1000 * 60 * 60)) % 24);
  String h = horas < 10 ? String("0") + horas : horas;
  String m = minutos < 10 ? String("0") + minutos : minutos;
  String s = segundos < 10 ? String("0") + segundos : segundos;
  return (h + String(":") + m + String(":") + s);
}

String retornaTempoCorrenteFormatado()
{
  unsigned long tempo = millis();
  uint8_t segundos = (int)(tempo / 1000) % 60;
  uint8_t minutos = (int)(tempo / (1000 * 60)) % 60;
  uint8_t horas = (int)((tempo / (1000 * 60 * 60)) % 24);
  String h = horas < 10 ? String("0") + horas : horas;
  String m = minutos < 10 ? String("0") + minutos : minutos;
  String s = segundos < 10 ? String("0") + segundos : segundos;
  return (String("[") + h + String(":") + m + String(":") + s + String("] "));
}

byte returnYPos(float value)
{
  if (value <= -10) // se for menor ou igual a -10
    return (byte)3;
  if (value > -10 && value < 0) // Se for maior que -10 e menor que 0
    return (byte)12;
  if (value >= 0 && value < 10) // Se for maior ou igual a 0 e menor que 10
    return (byte)18;
  if (value >= 10) // Se for maior ou igual a 10
    return (byte)9;

  return (byte)0;
}

bool camposServerCorretos(String user, String key, String server, String port, String umi, String temp, String indice)
{
  if (user.isEmpty() || key.isEmpty() || server.isEmpty() || port.isEmpty() || umi.isEmpty() || temp.isEmpty() || indice.isEmpty())
    return false;
  
  if(temp.equals(umi) || temp.equals(indice) || umi.equals(indice))
    return false;

  if(isnan(port.toInt()))
    return false;

  return true;
}


bool camposServerCorretosAdafruit(String user, String key, String umi, String temp, String indice)
{
  if (user.isEmpty() || key.isEmpty() || umi.isEmpty() || temp.isEmpty() || indice.isEmpty())
    return false;
  
  if(temp.equals(umi) || temp.equals(indice) || umi.equals(indice))
    return false;

  return true;
}