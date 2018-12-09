//Sensor readings with offsets:   1       -2      16380   0       0       -2
//Your offsets:   -3931   2067    989     74      108     21

// 191     -355    1497    -144    155     12 hat with plastic  ball
#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <algorithm>
#include <Wire.h>
#include <FastLED.h>
#include "MPU6050.h"


// FASTLED settings 
#define NUM_LEDS 30
#define PIN D4
#define COLOR_ORDER RGB
uint8_t globalBrightness = 5;
int speed = 100;


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
    mpu.setXAccelOffset(191);
    mpu.setYAccelOffset(-355);
    mpu.setZAccelOffset(1497);
    mpu.setXGyroOffset(-144);
    mpu.setYGyroOffset(155);
    mpu.setZGyroOffset(12);
    // mpu.setXAccelOffset(-3931);
    // mpu.setYAccelOffset(2067);
    // mpu.setZAccelOffset(989);
    // mpu.setXGyroOffset(74);
    // mpu.setYGyroOffset(108);
    // mpu.setZGyroOffset(21);
    mpuStatus = mpu.testConnection();
    Serial.println(mpuStatus ? "Connected" : "Connection failed");
    
}

void setup() { 
  Serial.begin(115200);
  while (! Serial);
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

  if(normX > 4 || normY > 4 || normZ > 4){
    leds[NUM_LEDS/2] = CHSV(hue++, 255, 255);//CRGB(normY, normX, normZ);
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
  delay(speed);
 
 printMotion();


 if (Serial.available())
  {
    char ch = Serial.read();
    if (ch >= '0' && ch <= '9')
    {
      
      int x = ch - '0';
      Serial.print("Value red: ");
      Serial.println(x);

      switch(x) {
          case 0 : FastLED.setBrightness(0);break; 
          case 1 : FastLED.setBrightness(1);  break;
          case 2 : FastLED.setBrightness(2); ; break;
          case 3 : FastLED.setBrightness(3); break;
          case 4 : FastLED.setBrightness(4); break;
          case 5 : FastLED.setBrightness(5); ;break;
          case 6 : FastLED.setBrightness(10); break;
          case 7 : FastLED.setBrightness(20); break;
          case 8 : FastLED.setBrightness(127); break;
          case 9 : FastLED.setBrightness(255); 
      }


    }

    if (ch == '+')
      FastLED.setBrightness(globalBrightness++);

    if (ch == '-')
      FastLED.setBrightness(globalBrightness--);

    if (ch == '.')
      speed += 10;

    if (ch == ',')
      speed -= 10;


    if (ch == 'a')
      FastLED.setMaxPowerInVoltsAndMilliamps(3.3, 500);

    if (ch == 'b')
      FastLED.setMaxPowerInVoltsAndMilliamps(1.5, 800);

    if (ch == 'c')
      FastLED.setMaxPowerInVoltsAndMilliamps(1, 1000);
    

    Serial.print("Brightness set to: ");
      Serial.println(FastLED.getBrightness());
    Serial.print("Speed set to: ");
      Serial.println(speed);

  }




}


