#include <Arduino.h>
#include <SPI.h>
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  pinMode(4, INPUT_PULLUP);
  // attachInterrupt(4, PinRaiseIntEvent, RISING);
  analogReadResolution(12);

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

  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}