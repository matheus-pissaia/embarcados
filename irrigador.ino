#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Define pin connections
#define SOIL_MOISTURE_PIN A0
#define RELAY_PIN D3

// WiFi credentials
const char *ssid = "ssid";
const char *password = "password";

String BASE_URL = "https://api-regador.onrender.com";

int soilMoistureValue = 0;
int activationLevel = 20;

unsigned long lastPostTime = 0;
const unsigned long postInterval = 2000; // 2 seconds

void sendMoistureLevel(int moistureLevel) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient https;

  https.begin(*client, BASE_URL + "/regas");
  https.addHeader("Content-Type", "application/json"); // Set content type to JSON
  
  // Create JSON document
  JsonDocument doc;
  doc["umidade"] = moistureLevel;

  String json;
  serializeJson(doc, json);

  int httpCode = https.POST(json);

  if (httpCode != HTTP_CODE_OK)
    Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());

  https.end();
}

void getActivationLevel() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient https;

  https.begin(*client, BASE_URL + "/activation-level");
  int httpCode = https.GET();

  if (httpCode != HTTP_CODE_OK)
    Serial.printf("[HTTP] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());

  String payload = https.getString();

  https.end();

  // Server sends a normal string instead of JSON object
  int level = payload.toInt();

  if (level && level != activationLevel)
    activationLevel = level;
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
    getActivationLevel();
  }

  delay(500);
}
