#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WiFiServer.h>
#endif


#include <DNSServer.h>
#include <WiFiManager.h>

const char* ssid = "AP *Eureka! Technology*";
const char* password = "eureka123";

WiFiServer server(80);

void setup ()
{
    Serial.begin(115200);
    Serial.println("Configurando AP.....");
    WiFi.softAP(ssid,password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("http://");
    Serial.println(myIP);

    server.begin();
    Serial.print("Servidor iniciado....");
    //WiFi.disconnect(true);
}
void loop(){
    WiFiClient client = server.available();
    if(!client){
        return;
    }

    Serial.print("Creando nuevo Cliente ....");
    Serial.println(client.remoteIP());

    while (!client.available())
    {
        delay(1);
    }
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head> <meta charset=utf-8> </head>");
    client.println("<body>");
    client.println("<hl> Eueka! Technology Servidor AP *ESP32*</h1>");
    client.println("</body>");
    client.println("</html>");

    client.flush();
    client.stop();

}

