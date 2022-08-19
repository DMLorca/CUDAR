
unsigned long TiempoAnt;

void setup() {
Serial.begin(115200);
Serial.println("");

TiempoAnt = millis();
}

void loop() {
  if( (millis() - TiempoAnt) >= 30000UL){
    TiempoAnt = millis();
    Serial.println("Hola");
    Serial.print(millis());
  }
}
