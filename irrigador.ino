#include <WiFiClient.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Define pin connections
#define SOIL_MOISTURE_PIN A0
#define RELAY_PIN D3

// WiFi credentials
const char *ssid = "wifi-name";
const char *password = "wifi-password";
const char *API_URL = 'https://test.com'; // TODO set correct URL

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variable to store soil moisture level
int soilMoistureValue = 0;
int activationLevel = 20; // TODO load activation level from server

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendMoistureLevel(int *moistureLevel) {
  HTTPClient http;
  JsonDocument doc;
  String json;

  doc["umidade"] = moistureLevel;
  serializeJson(doc, json);

  http.begin(API_URL + "/regas");
  http.POST(json);

  http.end();
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Prevents reconnection issues (taking too long to connect)
  Wifi.mode(WIFI_OFF)

  // Set relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Ensure the relay is off initially

  delay(1000)

  // Hides the viewing of ESP as wifi hotspot
  Wifi.mode(WIFI_STA) 

  // Start WiFi
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // -------- ROUTES --------

  // Should be able to set activation level
  server.on("/activation-level", HTTP_POST, [](AsyncWebServerRequest *request){},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // Allocate a temporary JSON document
      // Use `StaticJsonDocument` if the JSON size is known at compile time
      DynamicJsonDocument doc(1024);

      // Parse the JSON data
      DeserializationError error = deserializeJson(doc, data);

      // Check for errors in parsing
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "text/plain", "Invalid JSON");
        return;
      }

      if (doc["level"])
        activationLevel = doc["level"].as<int>();

      request->send(200);
    }
  );

  server.onNotFound(notFound);

  // -------- ROUTES --------

  // Start server
  server.begin();
}

void loop() {
  soilMoistureValue = map(analogRead(SOIL_MOISTURE_PIN), 1023, 490, 0, 100);

  // Automatic irrigation control logic
  if (soilMoistureValue < activationLevel) { // Adjust threshold as needed
    digitalWrite(RELAY_PIN, LOW);  // Start irrigation
  }
  else {
    digitalWrite(RELAY_PIN, HIGH); // Stop irrigation
  }

  sendMoistureLevel(soilMoistureLevel);

  delay(1000); // Check humidity level every 1 seconds
}
