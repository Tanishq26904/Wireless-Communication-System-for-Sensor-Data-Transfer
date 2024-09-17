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
  wire.begin();
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
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tca9548a_select(sensorChannels[i]); // Select channel

    float objectTemp = readObjectTemperature();
    
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" - Object Temperature: ");
    Serial.print(objectTemp);
    Serial.println(" *C");
    
    delay(100); // Short delay between readings
  }
  delay(500);
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);

  float data[7]={};
  data[0]=accel.acceleration.x;
  data[1]=accel.acceleration.y;
  data[2]=accel.acceleration.z;
  data[3]=gyro.gyro.x;
  data[4]=gyro.gyro.y;
  data[5]=gyro.gyro.z;
  data[6]=mlx.readAmbientTempC();
  data[7]=44444444;
  radio.write(&data, sizeof(data));
  delay(500); 
}


void tca9548a_select(uint8_t channel) {
  if (channel > 7) return; // Invalid channel
  Wire.beginTransmission(TCA9548A_ADDRESS);
  Wire.write(1 << channel); // Select the channel
  Wire.endTransmission();
}

float readObjectTemperature() {
  Wire.beginTransmission(MLX90614_ADDRESS);
  Wire.write(0x07); // Object temperature register
  Wire.endTransmission();
  Wire.requestFrom(MLX90614_ADDRESS, 3);

  if (Wire.available() == 3) {
    uint16_t raw = Wire.read() | (Wire.read() << 8);
    return (raw * 0.02) - 273.15; // Convert to Celsius
  } else {
    Serial.println("Failed to read object temperature.");
    return -1000; // Return an error value
  }
}
