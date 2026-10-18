#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <stdint.h>

#include <SPI.h>
#include <SD.h>

Adafruit_BME280 bme;

float callobration_X,callobration_Y,callobration_Z;
float x_Axis,y_Axis,z_Axis;
int16_t x_Output,y_Output,z_Output;

float tempreature, height, humid ,pres;
void setup() {
 Serial.begin(115200);
 Wire.setClock(400000);
 Wire.begin();
 delay(250);
 Wire.beginTransmission(0x68);
 Wire.write(0x6B);  // PWR_MGMT_1 register
 Wire.write(0x00);  // Wake up
 Wire.endTransmission();

 Wire.beginTransmission(0x68);
 Wire.write(0x1A);  //filter address
 Wire.write(0x05);  //filter setting
 Wire.endTransmission();


//FS_SEL 1 8bit  0x2 0x2 0x2 0x2 1x2^3 0x2 0x2 0x2 
// binary 8 to hex --> 0x08
 Wire.beginTransmission(0x68);
 Wire.write(0x1B);  // PWR_MGMT_1 register
 Wire.write(0x08);  // LSB sensativity to 65.5 lsb
 Wire.endTransmission();



 if (!bme.begin(0x76)) {
  Serial.println("Could not find BME280 at 0x76!");
  while (1);
}
SD.begin(15);



for(int i = 0; i<2000; i++){
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);

  x_Output = (Wire.read() << 8) | Wire.read();
  y_Output = (Wire.read() << 8) | Wire.read();
  z_Output = (Wire.read() << 8) | Wire.read();

   x_Axis = x_Output/65.5;
   y_Axis = y_Output/65.5;
   z_Axis = z_Output/65.5;

  callobration_X += x_Axis;
  callobration_Y += y_Axis;
  callobration_Z += z_Axis;
  delay(1);

}

callobration_X /= 2000;
callobration_Y /= 2000;
callobration_Z /= 2000;




}

void loop() {
  File dataFile = SD.open("data.txt", FILE_WRITE);
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);

  x_Output = (Wire.read() << 8) | Wire.read();
  y_Output = (Wire.read() << 8) | Wire.read();
  z_Output = (Wire.read() << 8) | Wire.read();

  x_Axis = (x_Output/65.5)-callobration_X;
  y_Axis = (y_Output/65.5)-callobration_Y;
  z_Axis = (z_Output/65.5)-callobration_Z;
  
  
  Serial.print("x degrees ");
  Serial.println(x_Axis);
  Serial.print("y degrees ");
  Serial.println(y_Axis);
  Serial.print("z degrees ");
  Serial.println(z_Axis);
  dataFile.print("x Axis ");
  dataFile.println(x_Axis);
  
  delay(100);
  dataFile.close();
  
  tempreature = bme.readTemperature();
  pres = bme.readPressure() / 100.0F;
  height = bme.readAltitude(1013.25);
  humid = bme.readHumidity();
 
  Serial.print("Temperature = ");
  Serial.print(tempreature);
  Serial.println(" *C");
  delay(1);
  // Convert temperature to Fahrenheit
  Serial.print("Temperature = ");
  Serial.print(1.8 * tempreature + 32);
  Serial.println(" *F");
  delay(1);
  Serial.print("Pressure = ");
  Serial.print(pres);
  Serial.println(" hPa");
  delay(1);
  Serial.print("Approx. Altitude = ");
  Serial.print(height);
  Serial.println(" m");
  delay(1);
  Serial.print("Humidity = ");
  Serial.print(humid);
  Serial.println(" %");
  
  Serial.println();

  delay(1000);


}

