/* HOMEKIT>>>1-Light, SWITCH, DHT22
 *****on ESP8266******  
 * Light<--->D2
 * Switch<--->D5 (interupt14)
 * DHT22<--->D1
 ********* homekit2mqtt on RaspberryPi************
 ***********example-homekit2mqtt.json***************
  
  "Light": {
    "service": "Lightbulb",
    "name": "Light",
    "topic": {
      "setOn": "set/Light/STATE",
      "statusOn": "status/Light/STATE"
    },
    "payload": {
      "onTrue": 1,
      "onFalse": 0
    },
    "manufacturer": "phawit-homekit",
    "model": "SWITCH"
  },

  "Temperature": {
    "service": "TemperatureSensor",
    "name": "Temperature",
    "topic": {
      "statusTemperature": "status/Temperature"
    },
    "manufacturer": "phawit-homekit",
    "model": "HMS100T"
  },
  
 * *************************************************
 * 
 */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN D1     // what digital pin we're connected to
#define DHTTYPE DHT22
#define BUFFER_SIZE 100
#define L1 D2  //Light_pin<-->GND
#define SW 14  //int14==pin_D5

const char *ssid =  "MyWifi";      //wifi username
const char *pass =  "abcd1234";    //password wifi
IPAddress server(192, 168, 1, 6);  //ip raspberrypi

DHT dht(DHTPIN, DHTTYPE);

void callback(const MQTT::Publish& pub) {
  
  Serial.println(pub.payload_string());
  Serial.println("ccccc");
  if (pub.payload_string() == "1") {  

    digitalWrite(L1,HIGH);
    Serial.println(pub.payload_string());
    
  } else {   

    digitalWrite(L1,LOW);
    Serial.println(pub.payload_string());   
  }
}

WiFiClient wclient;
PubSubClient client(wclient, server);

void setup() {
  dht.begin();
  attachInterrupt(SW, sw, RISING); 
  pinMode(L1, OUTPUT);
  Serial.begin(115200);
  delay(10); 
}


void sw(){
  Serial.println("Press__SWITCH"); 
  if(digitalRead(L1)==HIGH){ Serial.println("set L1>>OFF"); digitalWrite(L1,LOW); }
  else{  Serial.println("set L1>>ON");  digitalWrite(L1,HIGH); } 
   
}


void loop() {
  if(digitalRead(L1)== HIGH){      client.publish("status/Light/STATE","1"); }    //Status-MQTT Topic publish
  else{   client.publish("status/Light/STATE","0");  }    //MQTT Topic publish

  float t = dht.readTemperature();
  client.publish("status/Temperature",String(t));
 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {

    if (!client.connected()) {
      if (client.connect("arduinoClient")) {
        client.set_callback(callback);
        client.subscribe("set/Light/STATE"); //Set-MQTT Topic description
      }
    }

    if (client.connected())
      client.loop();
  }

}

