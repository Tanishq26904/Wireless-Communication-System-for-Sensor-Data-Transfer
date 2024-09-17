#include <SPI.h>
#include <RF24.h>
#include <Adafruit_MLX90614.h>

RF24 radio(7, 8); 
const byte address[6] = "00001";
void setup() {

  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN); 
  radio.setDataRate(RF24_250KBPS); 
  radio.setChannel(76); 
  radio.startListening();
  Serial.print("\t");
  Serial.print("acclx=");
  Serial.print("\t");
  Serial.print("accly=");
  Serial.print("\t");
  Serial.print("acclz=");
  Serial.print("\t");
  Serial.print("gyrox=");
  Serial.print("\t");
  Serial.print("gyroy=");
  Serial.print("\t");
  Serial.print("gyroz=");
  Serial.print("\t");
  Serial.print("temp=");
}

void loop() {
  if (radio.available()) {
  
  float data[7]; 
    radio.read(&data, sizeof(data));
    Serial.print("\t");
    Serial.print(data[0]);
    Serial.print("\t");
    Serial.print(data[1]);
    Serial.print("\t");
    Serial.print(data[2]);
    Serial.print("\t");
    Serial.print(data[3]);
    Serial.print("\t");
    Serial.print(data[4]);
    Serial.print("\t");
    Serial.print(data[5]);
    Serial.print("\t");
    Serial.print(data[6]);
    Serial.println();
    delay(1000);
  }
}
