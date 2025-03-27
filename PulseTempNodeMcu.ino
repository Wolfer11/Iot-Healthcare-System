#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PulseSensorPlayground.h>

// DS18B20 setup
#define ONE_WIRE_BUS D4  // GPIO pin for DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pulse sensor setup
const int PulsePin = A0; // Analog pin for Pulse Sensor
int Threshold = 100;
PulseSensorPlayground pulseSensor;
DallasTemperature temperature;

// Global variables for sensor data
float calibratedTemp = 0.0;
int myBPM = 0;
const float tempThreshold = 37.0; // Temperature threshol
const int bpmLimit = 100;     
const int templimit = 37;
     // Maximum BPM limit

void setup() {
  // Initialize Serial
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize DS18B20
  sensors.begin();
  Serial.println("DS18B20 Temp Sensor initialized.");

  // Initialize Pulse Sensor
  pulseSensor.analogInput(PulsePin);
  pulseSensor.setThreshold(Threshold);
  


  if (pulseSensor.begin()) {
    Serial.println("Pulse Sensor initialized successfully.");
  } else {
    Serial.println("Failed to initialize Pulse Sensor.");
  }
}

// Function to read sensor data
void readSensorData() {
  // Read temperature from DS18B20
  float calibrationFactor = 4.2;  // Calibration adjustment
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  // Ensure BPM never goes above 110
  if (tempC > templimit) {
    tempC = templimit;
  }

  if (tempC != -127.00) {
    calibratedTemp = tempC + calibrationFactor;
    Serial.print("Body Temperature: ");
    Serial.print(calibratedTemp);
    Serial.println(" °C");

    // Check if temperature exceeds threshold
    if (calibratedTemp > tempThreshold) {
      Serial.println("Warning: High Body Temperature!");
    }
  } else {
    Serial.println("Failed to read temperature.");
    calibratedTemp = -1; // Set to -1 if reading fails
  } 

  // Read pulse data
  myBPM = pulseSensor.getBeatsPerMinute();
  
  // Ensure BPM never goes above 110
  if (myBPM > bpmLimit) {
    myBPM = bpmLimit;
  }

  if (pulseSensor.sawStartOfBeat()) {
    Serial.println("Beat detected!");
    Serial.print("BPM: ");
    Serial.println(myBPM);
  }
}

void loop() {
  readSensorData(); // Read temperature and pulse data
  delay(2000);      // Read data every 2 seconds
}
