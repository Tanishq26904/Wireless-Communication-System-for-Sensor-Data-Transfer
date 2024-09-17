#include <SPI.h>
#include <RF24.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>

#define TCA9548A_ADDRESS 0x70 // Default address of TCA9548A multiplexer
#define MLX90614_ADDRESS 0x5A // Default address of MLX90614 sensor (same for all sensors)
// Define the number of sensors and channels used
#define NUM_SENSORS 3
uint8_t sensorChannels[NUM_SENSORS] = {0, 1, 2}; // Channels connected to sensors

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

RF24 radio(7, 8); 
const byte address[6] = "00001"; 

void setup(void) {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial)
    delay(10); 
  while (!Serial);

  Serial.println("Adafruit MLX90614 test");

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu_temp = mpu.getTemperatureSensor();
  mpu_temp->printSensorDetails();

  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();

  mpu_gyro = mpu.getGyroSensor();
  mpu_gyro->printSensorDetails();
    
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN); 
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76); 
}

void loop() {
  float data[9]={};
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);

  data[7]=mlx.readObjectTempC();
  data[0]=accel.acceleration.x;
  data[1]=accel.acceleration.y;
  data[2]=accel.acceleration.z;
  data[3]=gyro.gyro.x;
  data[4]=gyro.gyro.y;
  data[5]=gyro.gyro.z;
  TCA9548A(1);
  data[6]=mlx.readObjectTempC();
  TCA9548A(2);
  data[7]=mlx.readObjectTempC();
  TCA9548A(3);
  data[8]=mlx.readObjectTempC();
  data[9]=444444;
  radio.write(&data, sizeof(data));
  delay(500); 
}


void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);
  Wire.write(1 <<bus);
  Wire.endTransmission();
}

