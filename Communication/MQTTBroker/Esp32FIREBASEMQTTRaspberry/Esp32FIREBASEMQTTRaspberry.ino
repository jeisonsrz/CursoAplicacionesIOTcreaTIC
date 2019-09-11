#include <DHT11.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

//conexiones wifi y mqtt


// Replace the next variables with your SSID/Password combination
const char* ssid = "Emprendedores";
const char* password = "AxtVvm13";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.1.138";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// LED Pin
const int ledPin = 2;

////////////////////////7

//Temperaturaaa y humedad DHT11
const int DHTPIN = 27;
DHT11 dht11(DHTPIN);

//Sensor de Flujo YF-S201
const int sensorPin = 2;
const int measureInterval = 2500;
volatile int pulseConter;
 
// YF-S201
const float factorK = 7.5;
// FS300A
//const float factorK = 5.5;
 
// FS400A
//const float factorK = 3.5;

float volume = 0;
long t0 = 0;
 
 
void ISRCountPulse()
{
   pulseConter++;
}
 
float GetFrequency()
{
   pulseConter = 0;
 
   interrupts();
   delay(measureInterval);
   noInterrupts();
 
   return (float)pulseConter * 1000 / measureInterval;
}
 
void SumVolume(float dV)
{
   volume += dV / 60 * (millis() - t0) / 1000.0;
   t0 = millis();
}
///////////sensor de glujo


void connectToNetwork() 
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Tratando de establecer conexión WIFI...");
    }
    Serial.println("Conexión establecida :D");
     Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup()
{
    Serial.begin(115200);
    delay(1000);
    //WiFi.begin(ssid,password) ;  Procedimiento de conexión
    connectToNetwork();  
     attachInterrupt(digitalPinToInterrupt(sensorPin), ISRCountPulse, RISING);
   t0 = millis();

   ///////////////MQTT SERVIDOR RASPBERRY
   
    client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}



void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32 Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("conexion fallida, rc=");
      Serial.print(client.state());
      Serial.println(" intentado conextar de nuevo en 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//////////////////////////////

void loop()
{
      
  
    float temp,hum;
    int err;
    
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        //http.begin("https://irrigatic-react.firebaseio.com//object.json");
        //http.addHeader("Content-Type", "text/plain");

    if ((err = dht11.read(hum, temp))==0)
    {
        Serial.print ("Temperatura: ");
        Serial.println (temp);
        Serial.print ("Humedad: ");
        Serial.println (hum);
        delay(2000);
    }
   else{
        Serial.println("Error del sensor :'(");
        temp=0.00000;
        hum=0.0000;
    }       

         double latitude = random(1,25);
         double temp1 = temp;
         double hum1 = hum;
         double fer = random(75,100);


 ///sensor de fllujoo
  
   // obtener frecuencia en Hz
   float frequency = GetFrequency();
 
   // calcular caudal L/min
   float flow_Lmin = frequency / factorK;
   SumVolume(flow_Lmin);
 
   Serial.print(" Caudal: ");
   Serial.print(flow_Lmin, 3);
   Serial.print(" (L/min)\tConsumo:");
   Serial.print(volume, 1);
   Serial.println(" (L)");
   ////////

           
String SerialData="";
String SerialData2="";
String SerialData3="";
String SerialData4="";
String SerialData5="";
String SerialData6="";

SerialData = String(latitude,2);
SerialData2 = String(temp1,2);
SerialData3 = String(hum1,2);
SerialData4 = String(fer,2);
SerialData5 = String(flow_Lmin,3);
SerialData6 = String(volume,2);

        
         String stringTwo = String("{ \"ubicacion\" :  \"");        // converting a constant string into a String object
String stringOne = String(stringTwo + SerialData + "\",");


 String numA = String(" \"temperatura\" :  \"");        // converting a constant string into a String object
String numB= String(numA + SerialData2 + "\",");

 String numH1 = String(" \"humedad\" :  \"");        // converting a constant string into a String object
String numH= String(numH1 + SerialData3 + "\",");

 String numF1 = String(" \"fertilizante\" :  \"");        // converting a constant string into a String object
String numF= String(numF1 + SerialData4 + "\",");

String numC1 = String(" \"Caudal\" :  \"");        // converting a constant string into a String object
String numC= String(numC1 + SerialData5  + "\",");

String numCA1 = String(" \"Litros Agua\" :  \"");        // converting a constant string into a String object
String numCA= String(numCA1 + SerialData6 + "\"}");

        int httpCode = http.PUT(stringOne + numB + numH + numF + numC + numCA );
        //int httpCode = http.GET();

        if (httpCode >=200 && httpCode <206)
        {
            String payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
        }
        else
        {
            Serial.println(httpCode);
            Serial.println("Error en la petición");
        }
        
        http.end();

  
    }
    delay(2000);


//////////////MQTT RASPBERRY


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    // Temperature in Celsius
   // int temperature = 36; 
    
    char tempString[8];
    dtostrf(temp, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);
    char humString[8];
    dtostrf(hum, 1, 2, humString);
    Serial.print("Humedad: ");
    Serial.println(humString);
    client.publish("esp32/temperature", humString);
  }

//////////  
}
