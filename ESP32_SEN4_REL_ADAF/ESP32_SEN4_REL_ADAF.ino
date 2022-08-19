#include <OneWire.h>
#include <DallasTemperature.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>       //Si se quiere usar el esp32
#include <PubSubClient.h>

#include <WiFiClient.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

char* ssid = "WiFi-Arnet-cg3e_2,4_EXT";       // wifi ssid
const char* password =  "l4p1c3r4.AZUL56";         // wifi password
//long int count = 0;

/************************* DS18B20 Setup *********************************/

// Data wire is plugged TO GPIO 4
#define ONE_WIRE_BUS 13
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Number of temperature devices found
int numberOfDevices;
// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 
char temp[15];
String strTemp = "";

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
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Temperatura_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sensor_1");
Adafruit_MQTT_Publish Temperatura_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sensor_2");
Adafruit_MQTT_Publish Temperatura_3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sensor_3");
Adafruit_MQTT_Publish Temperatura_4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Sensor_4");

// Setup a feed called 'onoff' for subscribing to changes to the button
//Adafruit_MQTT_Subscribe salidaDigital = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/salidaDigital", MQTT_QOS_1);

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

  uint8_t retries = 30;
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
  Serial.begin(115200);
  Serial.println("");
  setup_wifi();

  
  //##############  CONFIGURACION SENSOR DS18B20  #####################
  
  // Start up the library
  sensors.begin();  
  // Grab a count of devices on the wire
  delay(10000);
  numberOfDevices = sensors.getDeviceCount();   
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

}

void loop() {
  MQTT_connect();

    sensors.requestTemperatures(); // Send the command to get temperatures

  //if(count>=20000){
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Output the device ID
      Serial.print("Dato de sensor: ");
      Serial.println(i,DEC);
      // Print the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.print(tempC);
      strTemp=String(tempC,1);
      strTemp.toCharArray(temp, 15);
      
      switch(i){
        case 0:
          Temperatura_1.publish(temp);
          Serial.print("\n");
          break;
        case 1:
          Temperatura_2.publish(temp);
          Serial.print("\n");
          break;
        case 2:
          Temperatura_3.publish(temp);
          Serial.print("\n");
          break;
        default:
          Temperatura_4.publish(temp);
          Serial.print("\n");

      }
    
    }
   // count = 0;
  }
  //}
  //count++;
  //Serial.print(" valor  " + count);
  delay(10000);

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(500);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  // if(! mqtt.ping()) {
  //   mqtt.disconnect();
  // }

}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
