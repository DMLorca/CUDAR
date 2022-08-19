#include <OneWire.h>
#include <DallasTemperature.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>       //Si se quiere usar el esp32
#include <PubSubClient.h>

#include <WiFiClient.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define RELE 2

char* ssid = "WiFi-TPLINK_EXT";       // wifi ssid
const char* password =  "l4p1c3r4.AZUL56";         // wifi password


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" // io.adafruit.com   52.70.203.194
#define AIO_SERVERPORT  1883                   // use 8883 for SSL // 1883
#define AIO_USERNAME  "CUDAR"
#define AIO_KEY       "aio_Nawz95YkV3THMi0nrrnei0jocu0g"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'onoff' for subscribing to changes to the button
Adafruit_MQTT_Subscribe salidaDigital = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Rele", MQTT_QOS_1);

/*************************** Sketch Code ************************************/

void digitalCallback(char *data, uint16_t len) {
  Serial.print("Hey we're in a onoff callback, the button value is: ");
  Serial.println(data);

     String message = String(data);
      message.trim();
      if (message == "ON") {digitalWrite(RELE, HIGH);}
      if (message == "OFF") {digitalWrite(RELE, LOW);} 
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


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
    // Inicia Serial
  pinMode(RELE, OUTPUT); 
  digitalWrite(RELE, LOW);
  Serial.begin(115200);
  Serial.println("");
  setup_wifi(); 

  salidaDigital.setCallback(digitalCallback);
  mqtt.subscribe(&salidaDigital);

}

void loop() {
  MQTT_connect();


  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(500);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  // if(! mqtt.ping()) {
  //   mqtt.disconnect();
  // }

}
