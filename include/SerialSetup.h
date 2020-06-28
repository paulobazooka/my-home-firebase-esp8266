#include <Arduino.h>

int MONITOR_SPEED = 115200;

void serialSetup(){
      Serial.begin(MONITOR_SPEED);
      delay(100);
      Serial.println();
      Serial.println();
      Serial.print("*SS: Módulo de comunicação serial iniciado com ");
      Serial.print(MONITOR_SPEED);
      Serial.println(" kbps");
}