#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WiFiServer.h>
#endif


#include <DNSServer.h>
#include <WiFiManager.h>

const int PIN_AP=2;

void configModeCallback(WiFiManager *myWifiManager)
{
    Serial.println("Modo de configuración....");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWifiManager->getConfigPortalSSID());

}

void saveConfigCallback(){
    Serial.println("Configuración Guardada");
    Serial.println(WiFi.softAPIP());
}

void setup ()
{
    Serial.begin(115200);
    pinMode(PIN_AP,INPUT);

    WiFiManager wifiManager; 
    wifiManager.setAPCallback(configModeCallback());
    wifiManager.setSaveConfigCallback(saveConfigCallback());

    wifiManager.autoConnect("Eureka! Technology RED Wifi","eureka123");

}
void loop(){
  91200372731

  WiFiManager wifiManager;
  if(digitalRead(PIN_AP)== HIGH){
      
      if(!wifiManager.startConfigPortal("Eureka! Technology RED Wifi","eureka123")){
          delay(2000);
          ESP.restart();
          delay(1000);
      }

  }

}

