#include <Arduino.h>

#include <WiFiManager.h>       // Importa a Biblioteca que gerencia a conexão wifi
#include <LiquidCrystal_I2C.h> // Importa a Biblioteca display cristal liquido

const char *apName = "MyHome[192.168.4.1]";
WiFiManager wifiManager; // Instancia da classe Gerenciadora de WiFi

void wifiSetup(LiquidCrystal_I2C lcd)
{
    lcd.setCursor(0, 0);
    lcd.print("    Meu Lar");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Automacao");
    lcd.setCursor(0, 1);
    lcd.print("Residencial");
    delay(1000);
    lcd.clear();
    lcd.print("Conectando");
    lcd.setCursor(0, 1);
    lcd.print("     no roteador");

    if (!wifiManager.autoConnect(apName))
    {
        Serial.println("*WS: A conexão WiFi falhou");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("*WS: Nao foi possivel");
        lcd.setCursor(0, 1);
        lcd.print("*WS: conectar ao WiFi");
        delay(3000);
        ESP.reset(); //reset and try again, or maybe put it to deep sleep
        delay(5000);
    }
}