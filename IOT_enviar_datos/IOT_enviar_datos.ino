#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

#include "DHT.h"
#define DHTPIN D2  
const int EchoPin = D3;
const int TriggerPin = D4;   
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

int contador = 0;

const char *ssid = "INFINITUM403B_2.4";
const char *password = "pLKcAWGP9t";

unsigned long previousMillis = 0;

char host[48];
String HOST = "192.168.1.81";  
String URL = "/agregar"; 
String chipid = "";

String enviardatos(String Informacion) {
  Serial.print(Informacion);
  String linea = "error";
  WiFiClient client;
  HOST.toCharArray(host, 49);
  if (!client.connect(host, 3000)) {
    Serial.println("No hay respuesta del servidor");
    return linea;
  }

  client.print(String("POST ") + URL + " HTTP/1.1" + "\r\n" + 
               "Host: " + HOST + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + Informacion.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + Informacion);           
  delay(10);             
  
  Serial.print("Enviando datos a la API.");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("No logró conectarse");
      client.stop();
      return linea;
    }
  }

  while(client.available()){
    linea = client.readStringUntil('\r');
  }  
  Serial.println(linea);
  return linea;
}


void setup() {
  Serial.begin(115200);
  Serial.println("");
  dht.begin();

  Serial.print("chipId: "); 
  chipid = String(ESP.getChipId());
  Serial.println(chipid); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contador <50) {
    ++contador;
    delay(200);
    pinMode(TriggerPin, OUTPUT);
    pinMode(EchoPin, INPUT);
    Serial.print(".");
  }
  if (contador <50) {
  
      
      Serial.println("");
      Serial.println("WiFi conectado con la IP ");
      Serial.println(WiFi.localIP());
      
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion al wifi");  
  }
}


void loop() {

  float h  = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  int cm = ping(TriggerPin, EchoPin);
  // client.add("ultrasonico",cm);
   //client.sendAll(true);
   String nombre = "Jesús";
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10000) { 
    previousMillis = currentMillis;

    Serial.println(t);
    enviardatos("Humedad=" + String(h) +"&Temperatura=" + String(t) +"&Distancia=" + String(cm)+"&Usuario=" + String(nombre));
  }
}
int ping(int TriggerPin, int EchoPin) {
   int duration, distanceCm;
   
   digitalWrite(TriggerPin, LOW);  
   delayMicroseconds(4);
   digitalWrite(TriggerPin, HIGH);  
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
   
   duration = pulseIn(EchoPin, HIGH);  
   
   distanceCm = duration * 10 / 292/ 2;   
   return distanceCm;
}
