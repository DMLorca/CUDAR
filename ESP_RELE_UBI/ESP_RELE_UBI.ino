#include "UbidotsEsp32Mqtt.h"
#include <WiFi.h> 
/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-DjBThly88LoO672PMWeceXT0dC6X8W";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "WiFi_EXT";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "l4p1c3r4.AZUL56";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "arduino-ethernet";   // Replace with the device label to subscribe to
const char *VARIABLE_LABEL = "Rele"; // Replace with your variable label to subscribe to

#define Rele 2

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  //for (int i=0;i<length;i++) {
    Serial.print((char)payload[0]);
  //}
  if ((char)payload[0]=='1'){
    digitalWrite(Rele, HIGH);
  }
  else{
    digitalWrite(Rele, LOW);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(Rele, OUTPUT); 
  digitalWrite(Rele, LOW);
  ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL); // Insert the dataSource and Variable's Labels
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL); // Insert the dataSource and Variable's Labels
  }
  ubidots.loop();
}
