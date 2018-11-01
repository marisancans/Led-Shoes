// WiFi includes
#include <ESP8266WiFi.h>

// OTA Includes
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include "SSD1306Wire.h"
#include "MPU6050.h"
#include <NeoPixelBus.h>



const char* ssid         = "kasteste";
const char* password     = "punkaripa";
const char* mqttServer   = "178.128.197.152";
const char* id           = "5";
const char* subscribeTo  = "nodetalk/update/";
const char* publishTo    = "mcu/data";


WiFiClient espClient;                   // WIFI
PubSubClient client(espClient);         // MQTT
SSD1306Wire  display(0x3c, D3, D2);     // OLED



// MPU settings 
MPU6050 mpu;
const uint8_t sclMPU = D1;
const uint8_t sdaMPU = D2;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t dax, day, daz;
int16_t dgx, dgy, dgz;

int val;
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// NEO pixel settings
#define FASTLED_ALLOW_INTERRUPTS 0
const uint16_t pixelCount = 60;
// make sure to set this to the correct pins
const uint8_t dotClockPin = 2;
const uint8_t dotDataPin = 3;  

#define colorSaturation 128
NeoPixelBus<DotStarBgrFeature, DotStarMethod> strip(pixelCount, dotClockPin, dotDataPin);
RgbColor red = RgbColor(colorSaturation, 0, 0);
RgbColor green = RgbColor(0, colorSaturation, 0);
RgbColor blue = RgbColor(0, 0, colorSaturation);
RgbColor white = RgbColor(colorSaturation);
RgbColor black = RgbColor(0);

const RgbColor colors[3] = {red, green, blue};

int pos = 0;

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    display.clear();
    display.drawString(1, display.getHeight()/2, "Got new image");
    display.display();


    Serial.println();
    Serial.println("-----------------------");
 
}

void setupWifi()
{
    WiFi.begin ( ssid, password );
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 10 );
        display.clear();
        Serial.println("Connecting to WiFi..");
        display.drawString(0, display.getHeight()/2, "Connecting to WiFi..");
        display.display();
    }
    display.clear();
    Serial.println("Connected to the WiFi network");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setupMQTT()
{
    client.setServer(mqttServer, 1883);
    client.setCallback(callback);

    while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    
    String client_name = String("ESP32Client_") + String(id);
    if (client.connect(client_name.c_str())) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    std::string buf(subscribeTo);
    buf.append(id);
    client.subscribe(buf.c_str());
}

void setupOTA()
{
    ArduinoOTA.begin();
    ArduinoOTA.onStart([]() {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(display.getWidth()/2, display.getHeight()/2 - 10, "OTA Update");
        display.display();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
        display.display();
    });

    ArduinoOTA.onEnd([]() {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(display.getWidth()/2, display.getHeight()/2, "Restart");
        display.display();
    });

    // Align text vertical/horizontal center
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_10);
    display.drawString(display.getWidth()/2, display.getHeight()/2, "Ready for OTA:\n" + WiFi.localIP().toString());
    display.display();
}

void setupMPU(){
    Wire.begin(sdaMPU, sclMPU);
    Serial.println("Initialize MPU");
    mpu.initialize();
    Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
}


void setup() {
    Serial.begin(115200);
    // display.init();
    // display.setContrast(255);
    // setupWifi();
    // setupMQTT();
    // setupMPU();

    strip.Begin();
    strip.Show();
}

void loopy()
{
delay(1000);

    for (int i = 0; i < pixelCount; i++) {
      strip.SetPixelColor(i, colors[pos++]);
      if (pos > 2)
        pos = 0;
    }
    
    strip.Show();
}

void loop() {
    ArduinoOTA.handle();
    client.loop();
    loopy();
    delay(100);

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
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

    dax = ax/AccelScaleFactor;
    day = ay/AccelScaleFactor;
    daz = az/AccelScaleFactor;

    dgx = gx/GyroScaleFactor;
    dgy = gy/GyroScaleFactor;
    dgz = gz/GyroScaleFactor;

    client.publish(publishTo, String(dgx).c_str());

}

