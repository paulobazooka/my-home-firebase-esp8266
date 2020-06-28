#include <Arduino.h>

#include <Wire.h> // Importa a Biblioteca comunicação I2C
#include <ESP8266WiFi.h>
#include <user_interface.h> // Importa a Biblioteca necessaria para acessar os Timer`s.
#include <FirebaseESP8266.h>
#include "Adafruit_MCP23017.h" // Importa a Biblioteca MCP23017
#include <LiquidCrystal_I2C.h> // Importa a Biblioteca display cristal liquido
#include <DHT.h>

#include "SerialSetup.h"
#include "FirebaseConst.h"
#include "MCP23017Setup.h"
#include "WifiManagerSetup.h"

// Definições -------------------
#define DHTPIN D5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Objetos ----------------------
FirebaseData firebaseData;
FirebaseData firebaseData2;
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MCP23017 mcp;              // Instancia da classe Adafruit_MCP23017
LiquidCrystal_I2C lcd(0x27, 16, 2); // Instancia da classe liquid

// Variáveis Globais ------------
os_timer_t tmr0;
float umidade = 0.0;
float temperatura = 0.0;
unsigned int segundos = 0;
byte grau[8] = {B00000110, B00001001, B00001001, B00000110, B00000000, B00000000, B00000000, B00000000}; // icone de temperatura

// Declaração Funções -----------
void customTimmer(void *z);
void setupTimmer();
void setupLCD();
void setupFirebase();
void printResult(StreamData &data);
void printResult(FirebaseData &data);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void readSensor();

void setup()
{
  serialSetup();
  setupLCD();
  setupMCP23017(mcp);
  setupTimmer();
  wifiSetup(lcd);
  setupFirebase();
  dht.begin();
}

void loop()
{
  if (segundos >= 5)
  {
    readSensor();
    segundos = 0;
  }
}

// Implementações Funções ----------
void customTimmer(void *z)
{
  segundos++;
}

void setupTimmer()
{
  Serial.println("*MA: Configurando o Timmer0");
  os_timer_setfn(&tmr0, customTimmer, NULL); //Indica ao Timer qual sera sua Sub rotina.
  os_timer_arm(&tmr0, 1000, true);           //Inidica ao Timer seu Tempo em mS e se sera repetido ou apenas uma vez (loop = true)
                                             //Neste caso, queremos que o processo seja repetido, entao usaremos TRUE.
}

void setupLCD()
{
  Serial.println("*MA: Configurando LCD.16x2");
  lcd.init();
  delay(100);
  lcd.noDisplay();
  delay(100);
  lcd.display();
  lcd.createChar(0, grau);
  delay(50);
  lcd.backlight();
  delay(50);
}

void printResult(StreamData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson *json = data.jsonObjectPtr();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json->toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json->iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json->iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json->iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray *arr = data.jsonArrayPtr();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr->toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();

    for (size_t i = 0; i < arr->size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData *jsonData = data.jsonDataPtr();
      //Get the result data from FirebaseJsonArray object
      arr->get(*jsonData, i);
      if (jsonData->typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData->boolValue ? "true" : "false");
      else if (jsonData->typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData->intValue);
      else if (jsonData->typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData->doubleValue);
      else if (jsonData->typeNum == FirebaseJson::JSON_STRING ||
               jsonData->typeNum == FirebaseJson::JSON_NULL ||
               jsonData->typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData->typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData->stringValue);
    }
  }
}

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
}

void streamCallback(StreamData data)
{
  if (data.dataPath() == "/rele1")
    mcp.digitalWrite(0, !mcp.digitalRead(0));

  if (data.dataPath() == "/rele2")
    mcp.digitalWrite(1, !mcp.digitalRead(1));

  if (data.dataPath() == "/rele3")
    mcp.digitalWrite(2, !mcp.digitalRead(2));

  if (data.dataPath() == "/rele4")
    mcp.digitalWrite(3, !mcp.digitalRead(3));

  if (data.dataPath() == "/rele5")
    mcp.digitalWrite(4, !mcp.digitalRead(4));

  if (data.dataPath() == "/rele6")
    mcp.digitalWrite(5, !mcp.digitalRead(5));

  if (data.dataPath() == "/rele7")
    mcp.digitalWrite(6, !mcp.digitalRead(6));

  if (data.dataPath() == "/rele8")
    mcp.digitalWrite(7, !mcp.digitalRead(7));
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println("Stream timeout, resume streaming...");
  }
}

void setupFirebase()
{

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData2.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData2.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  String path = USER_PATH;

  lcd.setCursor(0, 0);
  if (!Firebase.beginStream(firebaseData, (path + "/reles")))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
    lcd.print("Erro conexao");
  }
  else
  {
    lcd.print(" ");
    lcd.print(WiFi.localIP());
    lcd.setCursor(0,1);
    lcd.print("                ");
  }

  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
}

void readSensor()
{
  lcd.setCursor(0, 1);

  String path = USER_PATH;

  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();

  if (isnan(temperatura))
  {
    Serial.println(F("Erro ao ler a temperatura!"));
    lcd.print("                ");
  }
  else
  {
    Firebase.setDouble(firebaseData2, (path + "/sensores/temperatura"), temperatura);
    if (temperatura < 10 && temperatura > 0)
      lcd.print("0");
    lcd.print(temperatura);
    lcd.write((byte)0);
    lcd.print("C   ");
  }

  if (isnan(umidade))
  {
    Serial.println(F("Erro ao ler umidade!"));
    lcd.print("                ");
  }
  else
  {
    Firebase.setDouble(firebaseData2, (path + "/sensores/umidade"), umidade);
    if (umidade < 10 && umidade > 0)
      lcd.print("0");
    lcd.print(umidade);
    lcd.print("% ");
  }
}
