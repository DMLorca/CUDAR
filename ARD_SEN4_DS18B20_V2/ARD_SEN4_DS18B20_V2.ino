// * Incluimos las siguientes librerías
#include <Ethernet.h>
#include <SPI.h>
#include <UbidotsEthernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Parámetros para el servidor de Ubidots.*/
char const * TOKEN = "BBFF-DjBThly88LoO672PMWeceXT0dC6X8W"; // Token asignado por Ubidots
char const * DEVICE_LABEL = "arduino-ethernet";
char const * VARIABLE_LABEL_1 = "Temp"; // Asigna una etiqueta de variable única para enviar los datos
char const * VARIABLE_LABEL_2 = "Temp2"; // Asigna una etiqueta de variable única para enviar los datos de la humedad
char const * VARIABLE_LABEL_3 = "Temp3";
char const * VARIABLE_LABEL_4 = "Temp4";
char const * VARIABLE_LABEL_5 = "Rele";


// Data wire is plugged TO GPIO 4
#define ONE_WIRE_BUS 9
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

/* Dirección MAC para el módulo ethernet */
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Ubidots client(TOKEN);

unsigned long TiempoAnt;

void setup() {
  Serial.begin(9600);

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
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



  /* Inicializa la conexión Ethernet */
  Serial.print(F("Inicializando conexión ethernet..."));
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Fallo"));
  } else {
    Serial.println(Ethernet.localIP());
  }
  // Esperamos un tiempo para el módulo arranque
  delay(2000);
  Serial.println(F("PLaca ethernet lista!!!"));

  TiempoAnt = millis();
}

void loop() {

  Ethernet.maintain();
  sensors.requestTemperatures(); // Send the command to get temperatures

  if( (millis() - TiempoAnt) >= 300000UL){
    TiempoAnt = millis();
  
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
          client.add(VARIABLE_LABEL_1, tempC);
          Serial.print("\n");
          break;
        case 1:
          client.add(VARIABLE_LABEL_2, tempC);
          Serial.print("\n");
          break;
        case 2:
          client.add(VARIABLE_LABEL_3, tempC);
          Serial.print("\n");
          break;
        default:
          client.add(VARIABLE_LABEL_4, tempC);
          Serial.print("\n");
      }    
    }
  }
  client.sendAll();
  }
  /* Enviando datos a Ubidots*/ 
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
