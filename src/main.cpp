#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "Radio_Tower";
const char* password = "xmjxmj777";

WebServer server(80);

// The pin to which the soil moisture sensor is connected
const int soilMoisturePin = 36;
const int dryValue = 0; // Value when soil is dry
const int wetValue = 4095; // Value when soil is wet
// put function declarations here:
int myFunction(int, int);

void handleRoot() 
{
  // Read the value from the soil moisture sensor
  int soilMoistureValue = analogRead(soilMoisturePin);

  // Map the raw value to a percentage
  int moisturePercentage = map(soilMoistureValue, dryValue, wetValue, 0, 100);
  // Make sure the percentage is within the 0 - 100 range
  moisturePercentage = constrain(moisturePercentage, 0, 100);

  // Build the HTTP response with auto - refresh meta tag
  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='5'>"; // Refresh every 5 seconds
  html += "</head><body>";
  html += "<h1>Soil Moisture Sensor Reading</h1>";
  html += "<p>Soil Moisture Value: ";
  html += moisturePercentage;
  html += "%</p>";
  html += "</body></html>";

  // Send the HTTP response
  server.send(200, "text/html", html);
}

void handleNotFound() {
  digitalWrite(2, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  server.send(404, "text/plain", message);
  digitalWrite(2, 0);
}

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
  // attachInterrupt(4, PinRaiseIntEvent, RISING);
  analogReadResolution(12);

  //WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("WiFi connected success!!");
  Serial.printf("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.printf("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain","this work as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.printf("HTTP server started");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(2, HIGH);
  delay(900);
  digitalWrite(2, LOW);
  delay(100);
  int adcValue = analogRead(36);
  float voltage = adcValue * 3.3 / 4095;
  Serial.printf("ADC Value: %fV\r\n", voltage);
  if(digitalRead(4) == HIGH) {
    Serial.printf("Water stop.\r\n");
    digitalWrite(5, HIGH);
  }
  else {
    Serial.printf("Water start.\r\n");
    digitalWrite(5, LOW);
  }
  server.handleClient();

  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}