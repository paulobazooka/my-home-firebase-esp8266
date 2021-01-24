#include <Arduino.h>

#include <DHT.h>
#include <Wire.h>     // Importa a Biblioteca comunicação I2C
#include <LittleFS.h> // Manipular arquivos
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>    // Importa a Biblioteca que gerencia a conexão wifi
#include <user_interface.h> // Importa a Biblioteca necessaria para acessar os Timer`s.
#include <FirebaseESP8266.h>
#include <Adafruit_MCP23017.h> // Importa a Biblioteca MCP23017
#include <LiquidCrystal_I2C.h> // Importa a Biblioteca display cristal liquido
#include <ESP8266httpUpdate.h>

#include "FirebaseConst.h"

// Definições -------------------
#define DHTPIN D5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Objetos ----------------------
Adafruit_MCP23017 mcp; // Instancia da classe Adafruit_MCP23017
WiFiClientSecure client;
WiFiManager wifiManager; // Instancia da classe Gerenciadora de WiFi
DHT dht(DHTPIN, DHTTYPE);
FirebaseData firebaseData;
FirebaseData firebaseData2;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Instancia da classe liquid

// Variáveis Globais ------------
os_timer_t tmr0;
char PROJETO[20];
float umidade = 0.0;
char VERSAO_LOCAL[10];
char URL_FIRMWARE[150];
char VERSAO_REMOTA[10];
float temperatura = 0.0;
const unsigned int MONITOR_SPEED = 115200;
const char *apName = "MyHome[192.168.4.1]";
unsigned long TEMPO_VERIFICAR_SENSOR;
unsigned long TEMPO_VERIFICAR_ATUALIZACAO;
const unsigned long INTERVALO_VERIFICAR_SENSOR = 1000 * 5; // 5 segundos
const unsigned long INTERVALO_VERIFICAR_ATUALIZACAO = 1000 * 60 * 30; // 30 minutos
byte grau[8] = {B00000110, B00001001, B00001001, B00000110, B00000000, B00000000, B00000000, B00000000}; // icone de temperatura
const char URL_UPDATE[150] = "https://raw.githubusercontent.com/paulobazooka/my_home_esp8266_vc/main/versao.json";

// Declaração Funções -----------
void error(int error);
void update();
void started();
void setupFS();
void finished();
void setupLCD();
void wifiSetup();
void readSensor();
void checkUpdate();
void serialSetup();
void setupFirebase();
void setupMCP23017();
void printResult(StreamData &data);
void printResult(FirebaseData &data);
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void progress(size_t progresso, size_t total);

void setup()
{
  client.setInsecure();

  serialSetup();
  setupFS();
  setupLCD();
  setupMCP23017();
  wifiSetup();
  checkUpdate();
  setupFirebase();
  dht.begin();
  TEMPO_VERIFICAR_ATUALIZACAO = millis();
  TEMPO_VERIFICAR_SENSOR = millis();
}

void loop()
{
  if (millis() - TEMPO_VERIFICAR_ATUALIZACAO > INTERVALO_VERIFICAR_ATUALIZACAO){
    TEMPO_VERIFICAR_ATUALIZACAO = millis();
    checkUpdate();
  }
  
  if (millis() - TEMPO_VERIFICAR_SENSOR > INTERVALO_VERIFICAR_SENSOR){
    TEMPO_VERIFICAR_SENSOR = millis();
    readSensor();
  }
  yield();
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
    lcd.setCursor(0, 1);
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

void serialSetup()
{
  Serial.begin(MONITOR_SPEED);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("*SS: Módulo de comunicação serial iniciado com ");
  Serial.print(MONITOR_SPEED);
  Serial.println(" kbps");
}

void setupMCP23017()
{
  Serial.println("*MP: Configurando I/O MCP23017");
  mcp.begin();
  delay(100);

  for (byte i = 0; i < 8; i++)
  {
    mcp.pinMode(i, OUTPUT);
    mcp.digitalWrite(i, HIGH);
  }
}

void wifiSetup()
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
    delay(1000);
    ESP.reset(); //reset and try again, or maybe put it to deep sleep
    delay(1000);
  }
}

void setupFS()
{
  if (LittleFS.begin())
  {
    Serial.println("*SF: Partição de arquivos iniciado com sucesso!");
    Dir dir = LittleFS.openDir("/");
    File versao = LittleFS.open("versao.json", "r");

    if (!versao)
    {
      Serial.println("*SF: Erro ao abrir arquivo de configuração!");
    }
    else
    {
      String s = versao.readString();
      s.trim();
      StaticJsonDocument<100> doc;
      deserializeJson(doc, s);
      strlcpy(VERSAO_LOCAL, doc["versao"], sizeof(VERSAO_LOCAL));
      strlcpy(PROJETO, doc["projeto"], sizeof(PROJETO));
      //Serial.print("projeto "); Serial.println(PROJETO);
      //Serial.print("versão "); Serial.println(VERSAO_LOCAL);
    }
    versao.close();

    LittleFS.end();
  }
}

void checkUpdate()
{
  Serial.println("*CH: Verificando versão do firmware.");
  HTTPClient http;
  http.begin(client, URL_UPDATE);
  int httpCode = http.GET();
  String s = http.getString();
  http.end();
  s.trim();

  if (httpCode != HTTP_CODE_OK)
  {
    Serial.print("*CH: Não foi possível acessar o serviço de versionamento: ");
    Serial.println(URL_UPDATE);
  }
  else
  {
    StaticJsonDocument<300> jsonVC;
    if (deserializeJson(jsonVC, s))
    {
      Serial.println("*CH: Arquivo de versão inválido");
    }
    else
    {
      strlcpy(VERSAO_REMOTA, jsonVC["versao"] | "", sizeof(VERSAO_REMOTA));
      strlcpy(URL_FIRMWARE, jsonVC["url"] | "", sizeof(URL_FIRMWARE));
      //Serial.print("*CH: Versão local  "); Serial.println(VERSAO_LOCAL);
      //Serial.print("*CH: Versão remota "); Serial.println(VERSAO_REMOTA);

      if (strcmp(VERSAO_LOCAL, VERSAO_REMOTA) != 0)
      {
        Serial.println("*CH: Nova versão disponível para download!");
        if (strcmp(URL_FIRMWARE, "") == 0 || URL_FIRMWARE == NULL)
        {
          Serial.println("*CH: Url do firmware mal formatado!");
        }
        else
        {
          Serial.println("*CH: Atualizando...");
          yield();
          update();
        }
      }
      else
      {
        Serial.println("*CH: Nenhuma atualização disponível!");
      }
    }
  }
}

void progress(size_t progresso, size_t total)
{
  Serial.print(progresso * 100 / total);
  Serial.print("% ");
};

void started()
{
  Serial.println("\n*UP: Atualização inicializada!");
}

void finished()
{
  Serial.println("\n*UP: Atualização completada com sucesso!");
  if (LittleFS.begin())
  {
      StaticJsonDocument<250> doc;
      
      File file = LittleFS.open("versao.json", "r");
      deserializeJson(doc, file);
      file.close();      
      
      doc["versao"] = VERSAO_REMOTA;
      
      File newFile = LittleFS.open("versao.json", "w");
      Serial.println("*UP: Atualizando arquivo local de versão...");
      serializeJsonPretty(doc, newFile);
      newFile.close();

      LittleFS.end();
  }
  Serial.println("*UP: Reinicializando...");
  ESP.restart();
}

void error(int err)
{
  String s = ESPhttpUpdate.getLastErrorString();
  Serial.println("\n*UP: Erro ao fazer atualização => " + s);
}

void update()
{
  ESPhttpUpdate.onError(error);
  ESPhttpUpdate.onEnd(finished);
  ESPhttpUpdate.onStart(started);
  ESPhttpUpdate.onProgress(progress);

  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.closeConnectionsOnUpdate(true);

  ESPhttpUpdate.update(client, URL_FIRMWARE); 
}
