#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

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

