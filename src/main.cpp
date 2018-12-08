//Sensor readings with offsets:   1       -2      16380   0       0       -2
//Your offsets:   -3931   2067    989     74      108     21

#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <Wire.h>
#include <FastLED.h>
#include "MPU6050.h"


// FASTLED settings 
#define NUM_LEDS 30
#define PIN D4
#define COLOR_ORDER RGB


// MPU settings 
MPU6050 mpu;
const uint8_t sclMPU = D2;
const uint8_t sdaMPU = D3;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t dax, day, daz;
int16_t dgx, dgy, dgz;

int val;
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;
bool mpuStatus;



CRGBArray<NUM_LEDS> leds;


void setupMPU(){
    Wire.begin(sdaMPU, sclMPU);
    Serial.println("Initialize MPU");
    mpu.initialize();
    mpu.setXAccelOffset(-3931);
    mpu.setYAccelOffset(2067);
    mpu.setZAccelOffset(989);
    mpu.setXGyroOffset(74);
    mpu.setYGyroOffset(108);
    mpu.setZGyroOffset(21);
    mpuStatus = mpu.testConnection();
    Serial.println(mpuStatus ? "Connected" : "Connection failed");
}

void setup() { 
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, PIN>(leds, NUM_LEDS); FastLED.setBrightness( 5 );
  setupMPU();
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
}

void printMotion(){
  if(mpuStatus){
    //mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print(ax/AccelScaleFactor);
    Serial.print(" ");
    Serial.print(ay/AccelScaleFactor);
    Serial.print(" ");
    Serial.print(az/AccelScaleFactor);
    Serial.print(" ");
    Serial.print(gx/GyroScaleFactor);
    Serial.print(" ");
    Serial.print(gy/GyroScaleFactor);
    Serial.print(" ");
    Serial.print(gz/GyroScaleFactor);
    Serial.println();
  }
}




void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  auto normY = abs(gy/GyroScaleFactor);
  auto normX = abs(gx/GyroScaleFactor);
  auto normZ = abs(gz/GyroScaleFactor);
  auto ledRatio = int(250/NUM_LEDS);

  static uint8_t hue;
  auto thresh = 2;

  if(normX || normY || normZ){
    leds[NUM_LEDS/2] = CRGB(normY, normX, normZ);
  } else {
    leds[NUM_LEDS/2] = CRGB(0, 0, 0);
  }
  
  
  for(int i = 0; i < NUM_LEDS/2; i++){
    leds[i] = leds[i + 1];
  }
    
  for(int i = NUM_LEDS; i--> NUM_LEDS/2;){
    leds[i] = leds[i - 1];
  }


  FastLED.show();
  delay(100);
 
 //printMotion();
}



