#include <ESP8266WiFi.h>

#define PIN_NUM 2

enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE };

WiFiServer server(80);

template<typename KEY, typename VAL>
struct MapNode {
  KEY key;
  VAL val;
  MapNode<KEY, VAL> *next;
};

template<typename KEY, typename VAL>
class Map {
public:
  Map() {
    _head = NULL;
  }
  ~Map() {
    deleteNode(_head);
  }

  typedef MapNode<KEY, VAL> Node;
  
  void insert(KEY key, VAL val) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->key;
    node->val = val;
    node->next = NULL;

    Node **last = &_head;
    while (*last) {
      last = &(*last)->next;
    }
    *last = node;
  }

  VAL* valueForKey(KEY key) {
    Node *curr = _head;
    while (curr) {
      if (key == curr->key) {
        return &curr->val;
      }
      curr = curr->next;
    }
    return NULL;
  }

protected:
  void deleteNode(Node *node) {
    if (node == NULL) {
      return;  
    }

    if (node->next) {
      deleteNode(node->next);
    }

    free(node);
  }
  
  Node *_head;
};


//typedef Map<String, String> JSON;

class JSON : public Map<String, String> {
public:
  String printData() {
    String data = "{\n";
    Node *curr = _head;
    while (curr) {
      data += String(curr->key);
      data += " : ";
      data += String(curr->val);
      data += "\n";
    }
    data += "}\n";
    return data;
  }
};


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

  Serial.println("Waiting for network info");

  while (!connectToNework()) {
    delay(1);
  }
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
  while (req.length()) {
    Serial.println(req);
    req = client.readStringUntil('\r');
  }
//  char c = client.read();
//  while (c != 255) {
//    Serial.print(c);
//    c = client.read();
//  }
//  Serial.println("");
  client.flush();
  
  // Match the request
  byte val = 1;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else if (req.indexOf("/networks") != -1) {
    //connectToNework("temp", "temp");
    Serial.println("invalid request");
    client.stop();
    return;
  }
  else if (req.indexOf("/networks") != -1) {
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

String createHttpResponse(const char *message) {
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/html\r\n\r\n";
  response += "<!DOCTYPE HTML>\r\n";
  response += "<html>\r\n";
  if (message) {
    response += message;
  }
  response += "\n</html>\n";

  return response;
}

String createJsonResponse(const char *json, int length) {
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: application/json\r\n";
  //response += "<!DOCTYPE HTML>\r\n<html>\r\nG";
  if (json) {
    response += "Content-Length: " + String(length) + "\r\n\r\n";
    response += json;
  }
  else {
    response += "Content-Length: 0\r\n\r\n";
  }
  //response += "</html>\n";

  return response;
}

boolean connectToNework() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return false;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  
  // Parse the request
  String req = client.readStringUntil('\r');
  HTTPMethod httpMethod;
  if (req.indexOf("GET") == -1) {
    httpMethod = HTTP_GET;
  }
  else if (req.indexOf("POST") == -1) {
    httpMethod = HTTP_POST;
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return false;
  }

  // Get the resource
  String resourceName = strtok((char*)req.c_str(), " ");
  resourceName = strtok(0, " ");

  Serial.print("Resource: ");
  Serial.println(resourceName);
  
  while (req.length()) {
    Serial.println(req);
    req = client.readStringUntil('\r');
  }
  client.flush();

  //String response = createHttpResponse("Testing");
  String json = "{\r\n \"Testing\" : \"Hell yeahhhh\" \r\n}\r\n";
  String response = createJsonResponse(json.c_str(), json.length());
  client.print(response);
  
  return false;
}

