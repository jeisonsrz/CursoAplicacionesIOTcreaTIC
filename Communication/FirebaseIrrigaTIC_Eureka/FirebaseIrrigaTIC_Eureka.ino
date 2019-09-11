#include <DHT11.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <IOXhop_FirebaseESP32.h>

#define FIREBASE_HOST "irrigatic-react.firebaseio.com"                         // the project name address from firebase id

#define FIREBASE_AUTH "PmqZg81rptA5GkHIe148bGikeHzgSr8Ht1qLxQ5D"                    // the secret key generated from firebase

String fireStatus = "";                                                     // led status received from firebase
// LED Pin
const int ledPin = 2;

const char* ssid = "FAMILIARUIZ";
const char* password = "3215323276";

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
}


void setup()
{
    Serial.begin(115200);
    delay(1000);
    //WiFi.begin(ssid,password) ;  Procedimiento de conexión
    connectToNetwork();  
     attachInterrupt(digitalPinToInterrupt(sensorPin), ISRCountPulse, RISING);
   t0 = millis();
    pinMode(ledPin, OUTPUT);

                                                   //print local IP address

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase

  Firebase.setString("LED_STATUS", "OFF");                                          //send initial string of led status
}

void loop()
{
    float temp,hum;
    int err;
    
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin("https://irrigatic-react.firebaseio.com//object.json");
        http.addHeader("Content-Type", "text/plain");

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

///RELE LED

fireStatus = Firebase.getString("LED_STATUS");                     // get led status input from firebase

  if (fireStatus == "ON") {                         // compare the input of led status received from firebase

    Serial.println("Led Turned ON");                 

    digitalWrite(ledPin, HIGH);                                                         // make output led ON

  }

  else if (fireStatus == "OFF") {              // compare the input of led status received from firebase

    Serial.println("Led Turned OFF");

    digitalWrite(ledPin, LOW);                                                         // make output led OFF

  }

  else {

    Serial.println("Wrong Credential! Please send ON/OFF");

  }
    
}
