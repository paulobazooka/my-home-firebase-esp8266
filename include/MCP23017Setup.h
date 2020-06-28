#include <Arduino.h>
#include "Adafruit_MCP23017.h" // Importa a Biblioteca MCP23017 


void setupMCP23017(Adafruit_MCP23017 mcp){
  Serial.println("*MP: Configurando I/O MCP23017");
  mcp.begin(0);
  delay(300);
  
  for (byte i=0; i<8; i++){
    mcp.pinMode(i, OUTPUT);
    mcp.digitalWrite(i, HIGH);
  }
}