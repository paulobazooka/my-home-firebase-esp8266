#include "Log.h"

Log::Log(unsigned int monitor_speed)
{
    Serial.begin(monitor_speed); // configuração de monitor serial
    delay(800);
    Serial.println();
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.println("SERIAL Monitor Serial iniciado om sucesso!");    
}


void Log::print(String message)
{
    Serial.print(message);
}


void Log::println(String tag, const String &message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}

void Log::println(String tag, int message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}

void Log::println(String tag, long message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}


void Log::println(String tag, double message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}

void Log::println(String tag, float message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}

void Log::println(String tag, const char *message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.print("\t");
    Serial.println(message);
}

void Log::println(String tag, Device *message)
{    
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.print(tag);
    Serial.println("\tDispositivo ");
    Serial.printf("\t\t\t\t\tid: %s\n", message->id);
    Serial.printf("\t\t\t\t\tmodelo: %s\n", message->modelo);
    Serial.printf("\t\t\t\t\tprojeto: %s\n", message->projeto);
    Serial.printf("\t\t\t\t\tversão firmware: %s\n", message->versao_firmware);
    Serial.printf("\t\t\t\t\tversão interface: %s\n", message->versao_interface);
    Serial.printf("\t\t\t\t\tdevice: %s\n", message->device);
    Serial.printf("\t\t\t\t\turl_update: %s\n", message->url_update);
}

void Log::println(String tag, StaticJsonDocument<512> message)
{
    Serial.print(retornaTempoCorrenteFormatado());
    Serial.println(tag);
    serializeJsonPretty(message, Serial);
    Serial.println("");
}
