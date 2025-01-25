#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/********** PLEASE CHANGE THIS *************************/
const char* ssid = "<WIFI NAMW>";
const char* password = "<PASSWORD>";

ESP8266WebServer server(80);

// Pins for LEDs
uint8_t LEDPins[] = {D0, D1, D2, D3, D4};
bool LEDStatus[] = {LOW, LOW, LOW, LOW, LOW};

void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(LEDPins[i], OUTPUT);
    digitalWrite(LEDPins[i], LEDStatus[i]);
  }

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/toggleLED", updateLED);
  server.on("/toggleAll", toggleAllLEDs);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// Handles the root page
void handleRoot() {
  server.send(200, "text/html", prepareHTML());
}

// Updates individual LED status
void updateLED() {
  uint8_t ledIndex = server.arg("led").toInt();
  if (ledIndex >= 0 && ledIndex < 5) {
    LEDStatus[ledIndex] = !LEDStatus[ledIndex];
    digitalWrite(LEDPins[ledIndex], LEDStatus[ledIndex]);
    Serial.printf("LED %d toggled to %s\n", ledIndex, LEDStatus[ledIndex] ? "ON" : "OFF");
  }
  server.send(200, "text/html", prepareHTML());
}

// Toggles all LEDs at once
void toggleAllLEDs() {
  bool allOn = true;

  // Check if all LEDs are ON
  for (uint8_t i = 0; i < 5; i++) {
    if (!LEDStatus[i]) {
      allOn = false;
      break;
    }
  }

  // Set all LEDs to the opposite state
  for (uint8_t i = 0; i < 5; i++) {
    LEDStatus[i] = !allOn;
    digitalWrite(LEDPins[i], LEDStatus[i]);
  }

  Serial.println(allOn ? "All LEDs turned OFF" : "All LEDs turned ON");
  server.send(200, "text/html", prepareHTML());
}

// Handles 404 errors
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// Prepares the HTML page
String prepareHTML() {
  String html = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                "<title>NodeMCU ESP8266 Web Server</title>"
                "<style>"
                "body {"
                "  font-family: 'Courier New', Courier, monospace;"
                "  background: #1c1c1c;"
                "  color: #00ff00;"
                "  text-shadow: 0 0 5px rgba(0, 255, 0, 0.7);"
                "}"
                "h1 {"
                "  text-align: center;"
                "  margin-bottom: 30px;"
                "}"
                ".container {"
                "  max-width: 800px;"
                "  margin: auto;"
                "  text-align: center;"
                "}"
                ".button {"
                "  display: inline-block;"
                "  margin: 10px;"
                "  padding: 10px 20px;"
                "  font-size: 20px;"
                "  color: #00ff00;"
                "  text-decoration: none;"
                "  background: #333;"
                "  border: 2px solid #00ff00;"
                "  border-radius: 5px;"
                "  transition: 0.3s;"
                "}"
                ".button:hover {"
                "  color: #ff007f;"
                "  border-color: #ff007f;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<h1>NodeMCU ESP8266 Web Server</h1>";

  // Add buttons for each LED
  for (uint8_t i = 0; i < 5; i++) {
    html += "<p>LED " + String(i + 1) + " ";
    html += (LEDStatus[i] ? "(ON)" : "(OFF)");
    html += "</p>";
    html += "<a class=\"button\" href=\"/toggleLED?led=" + String(i) + "\">Toggle LED " + String(i + 1) + "</a>";
  }

  // Add button to toggle all LEDs
  html += "<hr>"
          "<a class=\"button\" href=\"/toggleAll\">Toggle All LEDs</a>"
          "</div>"
          "</body>"
          "</html>";

  return html;
}
