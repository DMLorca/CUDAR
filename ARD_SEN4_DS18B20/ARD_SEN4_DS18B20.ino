// * Incluimos las siguientes librerías
#include <Ethernet.h>
#include <SPI.h>
#include <UbidotsEthernet.h>
#include "DHT.h"
#include "U8glib.h"
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI // Se habilita esta linea según el display a a usar en este caso el driver SH1106 
#define DHTPIN 2     // Pin digital al caual vamos a conectar el sensor de temperatura

// Tipo de sensor que vamos a usar
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
// Parámetros para el servidor de Ubidots.*/
char const * TOKEN = "BBFF-k3bid88HLUIzAbEXYKIZqrgATC0qM9"; // Token asignado por Ubidots
char const * VARIABLE_LABEL_1 = "temperatura_dht"; // Asigna una etiqueta de variable única para enviar los datos
char const * VARIABLE_LABEL_2 = "humedad_dht"; // Asigna una etiqueta de variable única para enviar los datos de la humedad
int temperatura = 0;
int humedad = 0;
/* Dirección MAC para el módulo ethernet */
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Ubidots client(TOKEN);

void setup() {
  Serial.begin(9600);
  //Inicializa el sensor de temperatura
  dht.begin();
  /* Inicializa la conexión Ethernet */
  Serial.print(F("Inicializando conexión ethernet..."));
  if (!Ethernet.begin(mac)) {
    Serial.println(F("ops hubo un problema"));
  } else {
    Serial.println(Ethernet.localIP());
  }
  // Esperamos un tiempo para el módulo arranque
  delay(2000);
  Serial.println(F("PLaca ethernet lista!!!"));
}

void loop() {

  Ethernet.maintain();
    float valor_temperatura = dht.readTemperature();// Leemos la temperatura en grados celsius
  float valor_humedad = dht.readHumidity();//Leemos la humedad
  temperatura = valor_temperatura;
  humedad = valor_humedad;
  //Impimimos estos valores en el terminal serial
  Serial.print("Temperatura:");
  Serial.println(valor_temperatura);
  Serial.print("Humedad:");
  Serial.println(valor_humedad);
  /* Enviando datos a Ubidots*/
  client.add(VARIABLE_LABEL_1, valor_temperatura);
  client.add(VARIABLE_LABEL_2, valor_humedad);
  client.sendAll();
  //Llamamos a nuestro display para que muestre los valores obtenidos
      u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );

  //Esperemos 5 segunsdo antes de volver a subir otro datos a Ubidots
  delay(5000);
}

void draw(void) {
  u8g.setFont(u8g_font_unifont);
  //Veremos en pantalla los valores de Temperatura y humedad en la posición asignada
  u8g.setPrintPos(0, 20);
  u8g.print("Temperatura:");
  u8g.print(temperatura);
  u8g.setPrintPos(0, 40); 
  u8g.print("Humedad:");
  u8g.print(humedad);
}
