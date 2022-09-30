// INCLUDE LIBRARIES
#include <DHTesp.h>
#include "WiFi.h"
#include "HTTPClient.h"

// INSTANTIATE OBJECTS
HTTPClient http;
DHTesp dhtSensor;

// DEFINE USED VARIABLES
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const String url = "https://api.thingspeak.com/update?api_key=1DXST6JX1HPVRW6O&";
const int outputOne = 14;
const int outputTwo = 27;
int httpCode = 0;
String path = "";
float temperature = 0.0;
float humidity = 0.0;


// BASIC CODE SETUP
void setup() {
  Serial.begin(115200);
  Serial.println("Setup Starting");

  initConnection();
  defineUsedSensors();

  Serial.println("Setup Finished");
}

void loop() {
  // Get Data from the sensor DHT22
  getSensorData();

  // Print data from the sensor DHT22
  printSensorData();

  // Build the API URL that will be used in the Post method
  buildApiUrl();

  // Delay to turn the lights on
  delay(1000);

  // Turn on the lights - Relay On
  digitalWrite(outputOne, HIGH);
  digitalWrite(outputTwo, HIGH);

  // Send data to the Cloud
  sendData();

  // Verify Request response and in case of success return a positive message
  verifyApiResponse();

  // Delay to turn the lights off
  delay(1000);

  // Turn off the lights - Relay Off
  digitalWrite(outputOne, LOW);
  digitalWrite(outputTwo, LOW);
}

void initConnection() {
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wifi");
  }
  Serial.println("Wifi connected");
}

void defineUsedSensors() {
  dhtSensor.setup(13, DHTesp::DHT22);
  pinMode(outputOne, OUTPUT);
  pinMode(outputTwo, OUTPUT);
}

void getSensorData() {
  temperature = dhtSensor.getTemperature();
  humidity = dhtSensor.getHumidity();
}

void printSensorData() {
  Serial.println("=============================================");
  Serial.println("Temperatura: " + String(temperature) + "°C");
  Serial.println("Humidade: " + String(humidity));
}

void buildApiUrl() {
  path = url + "field1=" + String(temperature) + "&field2=" + String(humidity);
}

void sendData() {
  http.begin(path);
  httpCode = http.GET();
}

void verifyApiResponse() {
  if (httpCode == 200) {
  Serial.println("Data sent correctly");
  }
}
