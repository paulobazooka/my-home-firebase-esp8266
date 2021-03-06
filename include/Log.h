#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Estruturas.h"
#include "Utilidades.h"

class Log
{
private:

public:
    Log(unsigned int monitor_speed);
    void print(String message);
    void println(String tag, Device *message);
    void println(String tag, int message);
    void println(String tag, long message);
    void println(String tag, float message);
    void println(String tag, double message);
    void println(String tag, const char *message);
    void println(String tag, const String &message);
    void println(String tag, StaticJsonDocument<512> message);
};

#endif