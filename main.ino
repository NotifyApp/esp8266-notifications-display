// Neopixel
#include <Adafruit_NeoPixel.h>

// ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Webserver Config
const char *ssid = "";
const char *password = "";
ESP8266WebServer server(80);

// Neopixel Config
#define NeoPIN D4
#define NUM_LEDS 12
int brightness = 150;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NeoPIN, NEO_RGB + NEO_KHZ800);

const int led = 2;

void setup (void) {
  Serial.begin(115200);

  // ##############
  // NeoPixel start
  Serial.println();
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); 
  delay(50);
  Serial.println("Notifications display v0.1");

  // #########
  // Webserver
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("notifyapp")) {
    Serial.println("MDNS responder started");
  }

  // what to do with requests
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void loop (void) {
  // waiting fo a client
  server.handleClient();
}

void handleRoot () {
  Serial.println("Client connected");
  digitalWrite(led, 1);

  // data from the colorpicker (e.g. #FF00FF)
  String color = server.arg("c");
  // int brightness = int(server.arg("b"));
  Serial.println("Color: " + color);
  // setting the color to the strip 
  setNeoColor(color);
  // strip.setBrightness(brightness);

  server.send(200, "text/html");
  digitalWrite(led, 0);
}

void handleNotFound () {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message +=(server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for(uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setNeoColor (String value){
  Serial.print("Setting Neopixel...\n");
  // converting Hex to Int
  int number = (int) strtol(&value[1], NULL, 16);

  // splitting into three parts
  int r = ((number >> 16) & 0xFF);
  int g = ((number >> 8) & 0xFF);
  int b = (number & 0xFF);

  // DEBUG
  Serial.print("RGB: ");
  Serial.print(r, DEC);
  Serial.print(" ");
  Serial.print(g, DEC);
  Serial.print(" ");
  Serial.print(b, DEC);
  Serial.println(" ");

  // setting whole strip to the given color
  for (int i=0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  // init
  strip.show();

  Serial.println("on.");
}
