#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 
#include <IRremoteESP8266.h>

ESP8266WebServer server(80);
IRsend irSender(0);

void setup() 
{
    Serial.begin(115200);
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP_IrBlaster");
    Serial.println("Succesfully connected to network.");

    server.on("/irdata",handleIrDataRequest);
    server.begin();

    irSender.begin();
}

void loop() 
{
    server.handleClient(); 
}

void handleIrDataRequest()
{
  server.send(200, "text/html", "Received IR code.");
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& jsonData = jsonBuffer.parseObject(server.arg("plain"));
  const char *protocol = jsonData["protocol"];
  const int     irData = jsonData["data"];
  const int   irLength = jsonData["length"];
  const int     repeat = jsonData["repeat"];  
  const int    delayms = jsonData["delay"];

  Serial.print("protocol: ");
  Serial.println(protocol);

  Serial.print("data: ");
  Serial.println(irData);

  Serial.print("length: ");
  Serial.println(irLength);

  Serial.print("repeat:" );
  Serial.println(repeat);

  if(strcmp(protocol,"sony") == 0)
  {
    Serial.println("Sending Sony ir code...");
    for(int i=0; i<repeat;i++)
    {
      irSender.sendSony(irData,irLength);
      delay(delayms);
    }
  }

  else if(strcmp(protocol,"somsung") == 0)
  {
    Serial.println("Sending Samsung ir code...");
    for(int i=0; i<repeat;i++)
    {
      irSender.sendSAMSUNG(irData,irLength);
      delay(delayms);
    }
  }

  else if(strcmp(protocol,"lg") == 0)
  {
    Serial.println("Sending LG ir code...");
    for(int i=0; i<repeat;i++)
    {
      irSender.sendLG(irData,irLength);
      delay(delayms);
    }
  }
  
  else if(strcmp(protocol,"nec") == 0)
  {
    Serial.println("Sending NEC ir code...");
    for(int i=0; i<repeat;i++)
    {
      irSender.sendNEC(irData,irLength);
      delay(delayms);
    }
  }
  // Still got to implement other protocols if needed.
}
