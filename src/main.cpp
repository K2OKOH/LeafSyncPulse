#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Arduino.h> // 显式包含 Arduino.h 头文件

const char* ssid = "Radio_Tower";
const char* password = "xmjxmj777";

WebServer server(80);

// The pin to which the soil moisture sensor is connected
const int soilMoisturePin = 36;
const int dryValue = 0; // Value when soil is dry
const int wetValue = 4095; // Value when soil is wet

// Function to read soil moisture percentage
float readSoilMoisturePercentage() {
  int soilMoistureValue = analogRead(soilMoisturePin);
  // Map the raw value to a percentage with floating - point precision
  float moisturePercentage = ((float)(soilMoistureValue - dryValue) / (float)(wetValue - dryValue)) * 100.0;
  // Make sure the percentage is within the 0 - 100 range
  return constrain(moisturePercentage, 0, 100);
}

// Function to handle requests to the root path
void handleRoot();

// Function to handle requests for non - existent pages
void handleNotFound();

void handleRoot() 
{
  float moisturePercentage = readSoilMoisturePercentage();

  char moisturePercentageStr[8];
  dtostrf(moisturePercentage, 4, 2, moisturePercentageStr);

  // Build the HTTP response with auto - refresh meta tag
  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='5'>"; // Refresh every 5 seconds
  html += "</head><body>";
  html += "<h1>Soil Moisture Sensor Reading</h1>";
  html += "<p>Soil Moisture Value: ";
  html += moisturePercentageStr;
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
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Starting setup...");

  // Set pin modes
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, INPUT_PULLUP);

  // Set ADC resolution
  analogReadResolution(12);

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.printf("WiFi connected success!! IP: %s\n", WiFi.localIP().toString().c_str());

  // Start MDNS responder
  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  // Define server routes
  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain","this work as well");
  });
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

  delay(2000);
}

void loop() {
  float moisturePercentage = readSoilMoisturePercentage();
  int adcValue = analogRead(36);
  float voltage = adcValue * 3.3 / 4095;
  Serial.printf("ADC Value: %.2fV, Soil Moisture: %.2f%%\n", voltage, moisturePercentage);

  // Check input on pin 4 and control output on pin 5
  if(digitalRead(4) == HIGH) {
    Serial.println("Water stop.");
    digitalWrite(5, HIGH);
  }
  else {
    Serial.println("Water start.");
    digitalWrite(5, LOW);
  }

  // Handle incoming client requests
  server.handleClient();

  delay(1000);
}
