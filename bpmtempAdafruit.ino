//https://io.adafruit.com/Pujankumar/dashboards/health-monitoring
#include <ESP8266WiFi.h>
#include "AdafruitIO_WiFi.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PulseSensorPlayground.h>

// WiFi credentials
#define WIFI_SSID "vivo 1904"
#define WIFI_PASS "PUJAN9647"

// Adafruit IO credentials
#define AIO_USERNAME "Pujankumar"
#define AIO_KEY "aio_DsSD81Pr1TmPH5iEUxdbgIAInxkU"

// Adafruit IO WiFi client
AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS);

// Adafruit IO feeds
AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *heartRateFeed = io.feed("heart_rate");

// DS18B20 setup
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pulse Sensor setup
const int PulsePin = A0; // Analog pin for Pulse Sensor
int Threshold = 550;
PulseSensorPlayground pulseSensor;

float calibratedTemp = 0.0;
int myBPM = 0;

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Connect to Adafruit IO
  Serial.print("Connecting to Adafruit IO...");
  io.connect();

  // Wait for connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" connected!");

  // Initialize sensors
  sensors.begin();
  pulseSensor.analogInput(PulsePin);
  pulseSensor.setThreshold(Threshold);

  if (!pulseSensor.begin()) {
    Serial.println("Failed to initialize Pulse Sensor.");
  }
}

void loop() {
  // Ensure Adafruit IO connection
  io.run();

  // Read temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC != -127.00) {
    float calibrationFactor = 6.2;
    calibratedTemp = tempC + calibrationFactor;
    Serial.print("Temperature: ");
    Serial.print(calibratedTemp);
    Serial.println(" °C");
  } else {
    Serial.println("Failed to read temperature.");
  }

  // Read BPM
  myBPM = pulseSensor.getBeatsPerMinute();
  if (pulseSensor.sawStartOfBeat()) {
    Serial.println("Beat detected!");
    Serial.print("BPM: ");
    Serial.println(myBPM);
  }

  // Send data to Adafruit IO
  temperatureFeed->save(calibratedTemp);
  heartRateFeed->save(myBPM);

  delay(6000); // Send data every 10 seconds
}
