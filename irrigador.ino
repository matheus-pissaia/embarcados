#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Define pin connections
#define SOIL_MOISTURE_PIN A0
#define RELAY_PIN 14

// WiFi credentials
const char* ssid = "irrigador";
const char* password = "irrigador123";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variable to store soil moisture level
int soilMoistureValue = 0;
int activationLevel = 40;

const char indexHtml[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
    <head>
      <meta charset="UTF-8" />
      <meta name="viewport" content="width=device-width, initial-scale=1.0" />

      <title>Irrigador 🌱💦</title>

      <style>
        html,
        :host {
          line-height: 1.5;
          -webkit-text-size-adjust: 100%;
          -moz-tab-size: 4;
          tab-size: 4;
          font-family: ui-sans-serif, system-ui, sans-serif,
            "Apple Color Emoji", "Segoe UI Emoji", "Segoe UI Symbol",
            "Noto Color Emoji";
          font-feature-settings: normal;
          font-variation-settings: normal;
          -webkit-tap-highlight-color: transparent;
        }

        body {
          margin: 0;
          line-height: inherit;
        }

        p,
        input {
          margin: 0;
        }

        button {
          border: 0;
          color: inherit;
          font: inherit;
          cursor: pointer;
        }

        .col-span-2 {
          grid-column: span 2 / span 2;
        }

        .mx-auto {
          margin-left: auto;
          margin-right: auto;
        }

        .mt-4 {
          margin-top: 1rem;
        }

        .hidden {
          display: none;
        }

        .flex {
          display: flex;
        }

        .inline-flex {
          display: inline-flex;
        }

        .grid {
          display: grid;
        }

        .size-40 {
          width: 10rem;
          height: 10rem;
        }

        .h-screen {
          height: 100vh;
        }

        .w-fit {
          width: fit-content;
        }

        .w-full {
          width: 100%;
        }

        .grid-cols-2 {
          grid-template-columns: repeat(2, minmax(0, 1fr));
        }

        .flex-col {
          flex-direction: column;
        }

        .place-content-center {
          place-content: center;
        }

        .items-center {
          align-items: center;
        }

        .justify-between {
          justify-content: space-between;
        }

        .gap-2 {
          gap: 0.5rem;
        }

        .gap-6 {
          gap: 1.5rem;
        }

        .rounded-2xl {
          border-radius: 1rem;
        }

        .rounded-lg {
          border-radius: 0.5rem;
        }

        .bg-blue-500 {
          --tw-bg-opacity: 1;
          background-color: rgb(59 130 246 / var(--tw-bg-opacity));
        }

        .bg-gray-700 {
          --tw-bg-opacity: 1;
          background-color: rgb(55 65 81 / var(--tw-bg-opacity));
        }

        .bg-gray-800 {
          --tw-bg-opacity: 1;
          background-color: rgb(31 41 55 / var(--tw-bg-opacity));
        }

        .p-2 {
          padding: 0.5rem;
        }

        .p-4 {
          padding: 1rem;
        }

        .text-2xl {
          font-size: 1.5rem;
          line-height: 2rem;
        }

        .font-bold {
          font-weight: 700;
        }

        .text-white {
          --tw-text-opacity: 1;
          color: rgb(255 255 255 / var(--tw-text-opacity));
        }
      </style>

      <script>
        let isEditing = false;

        function getElement(id) {
          return document.getElementById(id);
        }

        function handleAction() {
          isEditing = !isEditing;

          isEditing ? handleEdit() : handleSave();
        }

        function handleEdit() {
          const inputEl = getElement("input");
          const buttonMessage = getElement("action-button-message");

          if (!buttonMessage || !inputEl) return;

          inputEl.classList.remove("hidden");
          buttonMessage.innerText = "Save";
        }

        function handleSave() {
          const inputEl = getElement("input");
          const buttonMessage = getElement("action-button-message");

          if (!buttonMessage || !inputEl) return;

          buttonMessage.innerText = "Edit";
          const rangeValue = inputEl.value;
          inputEl.classList.add("hidden");

          fetch("/activation-level", {
            method: "POST",
            body: JSON.stringify({ level: rangeValue }),
          });
        }

        function handleInput(event) {
          const activationLevel = getElement("activation-level");

          if (!isEditing || !activationLevel) return;

          activationLevel.innerText = event.target.value + "%";
        }

        async function updatePercentage() {
          const percentageEl = getElement("percentage");

          if (!percentageEl) return

          const response = await fetch("/hydration-level").then(
            (response) => response.text()
          );

          percentageEl.innerText = response + "%";
        }

        async function updateActivationLevel() {
          const activationLevel = getElement("activation-level");

          if (!activationLevel) return;

          const response = await fetch("/activation-level").then(
            (response) => response.text()
          );

          activationLevel.innerText = response + "%";
        }

        document.addEventListener(
          "DOMContentLoaded", 
          () => {
            updateActivationLevel()
            setInterval(updatePercentage, 2_000)
          },
        );
      </script>
    </head>

    <body>
      <div class="h-screen place-content-center bg-gray-800 text-white">
        <div class="mx-auto grid grid-cols-2 w-fit gap-6">
          <div class="flex flex-col justify-between rounded-2xl bg-gray-700 p-4">
            <svg
              xmlns="http://www.w3.org/2000/svg"
              width="32"
              height="32"
              viewBox="0 0 24 24"
              style="fill: currentColor"
            >
              <path
                d="M12 22c4.636 0 8-3.468 8-8.246C20 7.522 12.882 2.4 12.579 2.185a1 1 0 0 0-1.156-.001C11.12 2.397 4 7.503 4 13.75 4 18.53 7.364 22 12 22zm-.001-17.74C13.604 5.55 18 9.474 18 13.754 18 17.432 15.532 20 12 20s-6-2.57-6-6.25c0-4.29 4.394-8.203 5.999-9.49z"
              ></path>
            </svg>

            <div>
              <p>Humidity</p>
              <p id="percentage" class="text-2xl font-bold">-%</p>
            </div>
          </div>

          <div class="flex size-40 flex-col justify-between rounded-2xl bg-blue-500 p-4">
            <svg
              xmlns="http://www.w3.org/2000/svg"
              width="32"
              height="32"
              viewBox="0 0 24 24"
              style="fill: currentColor"
            >
              <path
                d="M5.996 9c1.413 0 2.16-.747 2.705-1.293.49-.49.731-.707 1.292-.707s.802.217 1.292.707C11.83 8.253 12.577 9 13.991 9c1.415 0 2.163-.747 2.71-1.293.491-.49.732-.707 1.295-.707s.804.217 1.295.707C19.837 8.253 20.585 9 22 9V7c-.563 0-.804-.217-1.295-.707C20.159 5.747 19.411 5 17.996 5s-2.162.747-2.709 1.292c-.491.491-.731.708-1.296.708-.562 0-.802-.217-1.292-.707C12.154 5.747 11.407 5 9.993 5s-2.161.747-2.706 1.293c-.49.49-.73.707-1.291.707s-.801-.217-1.291-.707C4.16 5.747 3.413 5 2 5v2c.561 0 .801.217 1.291.707C3.836 8.253 4.583 9 5.996 9zm0 5c1.413 0 2.16-.747 2.705-1.293.49-.49.731-.707 1.292-.707s.802.217 1.292.707c.545.546 1.292 1.293 2.706 1.293 1.415 0 2.163-.747 2.71-1.293.491-.49.732-.707 1.295-.707s.804.217 1.295.707C19.837 13.253 20.585 14 22 14v-2c-.563 0-.804-.217-1.295-.707-.546-.546-1.294-1.293-2.709-1.293s-2.162.747-2.709 1.292c-.491.491-.731.708-1.296.708-.562 0-.802-.217-1.292-.707C12.154 10.747 11.407 10 9.993 10s-2.161.747-2.706 1.293c-.49.49-.73.707-1.291.707s-.801-.217-1.291-.707C4.16 10.747 3.413 10 2 10v2c.561 0 .801.217 1.291.707C3.836 13.253 4.583 14 5.996 14zm0 5c1.413 0 2.16-.747 2.705-1.293.49-.49.731-.707 1.292-.707s.802.217 1.292.707c.545.546 1.292 1.293 2.706 1.293 1.415 0 2.163-.747 2.71-1.293.491-.49.732-.707 1.295-.707s.804.217 1.295.707C19.837 18.253 20.585 19 22 19v-2c-.563 0-.804-.217-1.295-.707-.546-.546-1.294-1.293-2.709-1.293s-2.162.747-2.709 1.292c-.491.491-.731.708-1.296.708-.562 0-.802-.217-1.292-.707C12.154 15.747 11.407 15 9.993 15s-2.161.747-2.706 1.293c-.49.49-.73.707-1.291.707s-.801-.217-1.291-.707C4.16 15.747 3.413 15 2 15v2c.561 0 .801.217 1.291.707C3.836 18.253 4.583 19 5.996 19z"
              ></path>
            </svg>

            <div>
              <p>Water usage</p>
              <p class="text-2xl font-bold">21.5L</p>
            </div>
          </div>

          <div class="col-span-2 rounded-2xl bg-gray-700 p-4">
            <div class="flex justify-between items-center">
              <span>
                Activation level:
                <span class="font-bold" id="activation-level">-%</span>
              </span>

              <button
                class="inline-flex w-fit items-center gap-2 rounded-lg bg-blue-500 p-2 font-bold"
                onclick="handleAction()"
              >
                <span id="action-button-message">Edit</span>

                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="24"
                  height="24"
                  viewBox="0 0 24 24"
                  style="fill: currentColor"
                >
                  <path
                    d="M19.045 7.401c.378-.378.586-.88.586-1.414s-.208-1.036-.586-1.414l-1.586-1.586c-.378-.378-.88-.586-1.414-.586s-1.036.208-1.413.585L4 13.585V18h4.413L19.045 7.401zm-3-3 1.587 1.585-1.59 1.584-1.586-1.585 1.589-1.584zM6 16v-1.585l7.04-7.018 1.586 1.586L7.587 16H6zm-2 4h16v2H4z"
                  ></path>
                </svg>
              </button>
            </div>

            <input
              type="range"
              class="hidden mt-4 w-full"
              id="input"
              oninput="handleInput(event)"
              value="40"
            />
          </div>
        </div>
      </div>
    </body>
  </html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Start WiFi
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  // Set relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Ensure the relay is off initially

  // -------- ROUTES --------

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", indexHtml);
  });

  server.on("/hydration-level", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", String(soilMoistureValue));
  });

  server.on("/activation-level", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", String(activationLevel));
  });
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
    });

  server.onNotFound(notFound);

  // -------- ROUTES --------

  // Start server
  server.begin();
}

void loop() {
  soilMoistureValue = map(analogRead(SOIL_MOISTURE_PIN), 1023, 0, 0, 100);

  // // Automatic irrigation control logic
  // if (soilMoistureValue < 500 && !irrigationActive) { // Adjust threshold as needed
  //   irrigationActive = true;
  //   digitalWrite(RELAY_PIN, LOW);  // Start irrigation
  //   Serial.println("Irrigation started automatically.");
  // }
  // else if (soilMoistureValue >= 700 && irrigationActive) { // Adjust threshold as needed
  //   irrigationActive = false;
  //   digitalWrite(RELAY_PIN, HIGH); // Stop irrigation
  //   Serial.println("Irrigation stopped automatically.");
  // }

  delay(1000);
}
