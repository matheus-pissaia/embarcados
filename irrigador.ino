#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include "webpageCode.h"

#define pinoAnalog A0  // Define o pino A0 como "pinoAnalog"
// #define pinoRele 8     // Define o pino 8 como "pinoRele"

//------------------------------------------

const char* ssid = "Fudido";
const char* password = "123fudido";

int ValAnalogIn;  // Introduz o valor analógico ao código
ESP8266WebServer server(80);

//------------------------------------------

void webpage()
{
  const char webpageCode[] =
  R"=====(
  <!DOCTYPE html>
  <html>
  <!------------------------------C S S--------------------------------->
  <head>
      <meta charset="UTF-8" />
      <meta name="viewport" content="width=device-width, initial-scale=1.0" />

      <title>Molhadinho 💦</title>

      <style>
      </style>
  </head>
  <!----------------------------H T M L--------------------------------->
  <body>
      <p>Nível de hidratação:</p>
      <p id="percentage">-</p>
  <!---------------------------JavaScript------------------------------->
  <script>
      const percentageEl = document.getElementById("percentage")

      async function updatePercentage() {
        const response = await fetch('/hydration-level').then(
          (response) => response.text()
        );

        percentageEl.innerText = response + '%'
      }

      setInterval(updatePercentage, 1_000)
  </script>
  </body>
  </html>
  )=====";
  server.send(200,"text/html", webpageCode);
}

void hydrationLevel() {
  ValAnalogIn = analogRead(pinoAnalog);              // Relaciona o valor analógico com o recebido do sensor
  int porcentagem = map(ValAnalogIn, 1023, 0, 0, 100);  // Relaciona o valor analógico à porcentagem

  char porcentagemString[3];  // buffer para armazenar a string resultante

  // Convertendo o número para string
  sprintf(porcentagemString, "%d", porcentagem);

  server.send(200, "application/json", porcentagemString);
}
//=================================================================
void setup()
{
  // pinMode(pinoRele, OUTPUT);  // Declara o pinoRele como Saída
  // Inicia a comunicação serial para debug
  Serial.begin(115200);
  delay(10);
  
  // Conecta-se ao Wi-Fi
  Serial.println();
  Serial.println("Conectando-se à rede Wi-Fi...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println("IP do Access Point: ");
  Serial.println(myIP);

  // Definicao de rotas
  server.on("/", webpage);
  server.on("/hydration-level", hydrationLevel);

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor iniciado");

}
//=================================================================
void loop()
{
  server.handleClient();

  // ValAnalogIn = analogRead(pinoAnalog);              // Relaciona o valor analógico com o recebido do sensor
  // int Porcento = map(ValAnalogIn, 1023, 0, 0, 100);  // Relaciona o valor analógico à porcentagem

  // Serial.print(Porcento);                            // Imprime o valor em Porcento no monitor Serial
  // Serial.println("%");      

  // TODO realizar conexoes com o relé, bateria e bomba
  // if (Porcento <= 45) {                        // Se a porcentagem for menor ou igual à
  //   Serial.println("Irrigando a planta ...");  // Imprime a frase no monitor serial
  //     digitalWrite(pinoRele, HIGH);  // Altera o estado do pinoRele para nível Alto
  // } else {                                  // Se não ...
  //   Serial.println("Planta Irrigada ...");  // Imprime a frase no monitor serial
  //   digitalWrite(pinoRele, LOW);            // Altera o estado do pinoRele para nível Baixo
  // }

  // delay(1000);
}