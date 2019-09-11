#include <DHT11.h>
#include <WiFi.h>
#include <PubSubClient.h>

//Temperaturaaa y humedad DHT11
const int DHTPIN = 27;
DHT11 dht11(DHTPIN);

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

void setup() {
  Serial.begin(115200);
  // default settings
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
void loop() {

    float temp,hum;
    int err;

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
}
