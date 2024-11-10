#include <WiFiClient.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Define pin connections
#define SOIL_MOISTURE_PIN A0
#define RELAY_PIN D3

WiFiClient wifiClient;

// WiFi credentials
const char *ssid = "wifi-name";
const char *password = "wifi-password";

String BASE_URL = "http://192.168.0.167:3333/rega"; // TODO set correct URL

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int soilMoistureValue = 0;
int activationLevel = 20;

unsigned long lastPostTime = 0;
const unsigned long postInterval = 2000; // 2 seconds

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendMoistureLevel(int moistureLevel) {
  HTTPClient http;
  http.begin(wifiClient, BASE_URL + "/moisture");
  http.addHeader("Content-Type", "application/json"); // Set content type to JSON
  
  // Create JSON document
  JsonDocument doc;
  doc["moisture"] = moistureLevel;

  String json;
  serializeJson(doc, json);

  http.POST(json);
  http.end();
}

void getActivationLevel() {
  HTTPClient http;
  DynamicJsonDocument doc(1024);

  http.begin(wifiClient, BASE_URL + "/activation");
  http.GET();

  String payload = http.getString();

  http.end();

  deserializeJson(doc, payload);

  if (doc["activation"])
    activationLevel = doc["activation"].as<int>();
}

void setup() {
  // Set relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Ensure the relay is off initially

  // Start serial communication
  Serial.begin(115200);

  // Prevents reconnection issues (taking too long to connect)
  WiFi.mode(WIFI_OFF);

  delay(1000);

  // Hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);

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

  // Set activation level saved previously in the API
  getActivationLevel();

  // -------- ROUTES --------

  // Should be able to set activation level from API (Use polling instead?)
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
  unsigned long currentTime = millis();

  soilMoistureValue = map(analogRead(SOIL_MOISTURE_PIN), 1023, 490, 0, 100);

  if (soilMoistureValue < activationLevel) { // Adjust threshold as needed
    digitalWrite(RELAY_PIN, LOW);  // Start irrigation
  }
  else {
    digitalWrite(RELAY_PIN, HIGH); // Stop irrigation
  }

  // Send moisture value every 2 seconds
  if (currentTime - lastPostTime >= postInterval) {
    lastPostTime = currentTime;
    sendMoistureLevel(soilMoistureValue);
  }

  delay(500);
}
