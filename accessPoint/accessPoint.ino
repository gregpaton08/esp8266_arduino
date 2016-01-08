#include <ESP8266WiFi.h>

#define PIN_NUM 2

WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(10);
  
  pinMode(PIN_NUM, OUTPUT);
  digitalWrite(PIN_NUM, 0);

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP("arduino_network");

  WiFi.printDiag(Serial);

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.flush();

  // Print the IP address
  Serial.println(WiFi.localIP());
  Serial.flush();
  
  Serial.println("setup ended");

  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

//  delay(1000);
//  digitalWrite(PIN_NUM, HIGH);
//  delay(1000);
//  digitalWrite(PIN_NUM, LOW);

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  byte val = 1;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else if (req.indexOf("/scan") != -1) {
    byte numNetworks = WiFi.scanNetworks();
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nNum networks: ";
    s += String(numNetworks) + "\n\n";
    for (byte i = 0; i < numNetworks; ++i) {
      s += WiFi.SSID(i);
      s += "\n";
    }
    s += "</html>\n";
    client.print(s);
    return;
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(PIN_NUM, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}
