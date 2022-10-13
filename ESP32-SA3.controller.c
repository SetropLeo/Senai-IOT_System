// INCLUDE LIBRARIES
#include "DHTesp.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "PubSubClient.h"
#include "Arduino_JSON.h"

// INSTANTIATE OBJECTS
HTTPClient http;
DHTesp dhtSensor;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// DEFINE USED CONSTANTS AND VARIABLES
const int outputOne = 14;
const int outputTwo = 27;
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* server = "broker.hivemq.com";
const char* clientId = "leolawpoclientId";
const String url = "https://api.thingspeak.com/update?api_key=1DXST6JX1HPVRW6O&";
const char* topic = "senai_tec_des-leo_law_po";

float humidity = 0.0;
float temperature = 0.0;
int port = 1883;
int httpCode = 0;
String path = "";
String jsonString = "";

JSONVar contextData;


// INITIAL SETUP
void setup() {
  Serial.begin(115200);
  Serial.println("Setup Starting");

  defineUsedSensors();
  initWifiConnection();
  initBrokerConnection();

  Serial.println("Setup Finished");
}

void loop() {
  // Get Data from the sensor DHT22
  getSensorData();

  // Print data from the sensor DHT22
  printSensorData();

  // Build the API URL that will be used in the Post method
  buildApiUrl();

  // Construct the JSON that will be sent via Mqtt
  buildJsonWithData();

  // Delay to turn the lights on
  delay(1000);

  // Turn on the lights - Relay On
  turnLightsOn();

  // Send data to the Cloud
  sendDataViaHttp();
  sendDataViaMqtt();

  // Delay to turn the lights off
  delay(1000);

  // Turn off the lights - Relay Off
  turnLightsOff();
}

void initWifiConnection() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wifi");
  }
  Serial.println("Wifi connected");
}

void initBrokerConnection() {
  mqttClient.setServer(server, port);
  mqttClient.connect(clientId);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to the Broker");
  }
  Serial.println("Broker connected");

  mqttClient.subscribe(topic);
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
  if (!temperature || !humidity) {
    Serial.println("Missing Data to build the API URL");
  } else {
  path = url + "field1=" + String(temperature) + "&field2=" + String(humidity);
  }
}

void buildJsonWithData() {
  if (!temperature || !humidity) {
    Serial.println("Missing Data to build the Json");
  } else {
    contextData["temperatura"] = temperature;
    contextData["umidade"] = humidity;
    jsonString = JSON.stringify(contextData);
    Serial.println(jsonString);
  }
}

void sendDataViaHttp() {
  http.begin(path);
  httpCode = http.GET();

  if (httpCode == 200) {
    Serial.println("Data sent correctly via HTTP");
  } else {
    Serial.println("Error sending data via HTTP");
  }
}

void sendDataViaMqtt() {
  int msg = mqttClient.publish(topic, jsonString.c_str());

  if (msg) {
    Serial.println("Data sent correctly via MQTT");
  } else {
    Serial.println("Error sending data via MQTT");
    mqttClient.connect(clientId);
    mqttClient.subscribe(topic);
  }
  delay(2000);
}

void turnLightsOn() {
    digitalWrite(outputOne, HIGH);
  digitalWrite(outputTwo, HIGH);
}

void turnLightsOff() {
    digitalWrite(outputOne, LOW);
  digitalWrite(outputTwo, LOW);
}
