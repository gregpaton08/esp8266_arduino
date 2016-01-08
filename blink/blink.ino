#include <ESP8266WiFi.h>

#define PIN_NUM 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  Serial.println("here");
  
  pinMode(PIN_NUM, OUTPUT);
  digitalWrite(PIN_NUM, 0);
}

void loop() {
  // put your main code here, to run repeatedly
  digitalWrite(PIN_NUM, HIGH);
  delay(1000);
  digitalWrite(PIN_NUM, LOW);
  Serial.println("Hello world");
  delay(1000);
}
