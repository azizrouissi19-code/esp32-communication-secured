#include <WiFi.h>
#include <WebServer.h>

// Credentials live in secrets.h (gitignored). Copy secrets.h.example -> secrets.h
// and fill in your own values. Never commit secrets.h.
#include "secrets.h"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WebServer server(80);

// Simulated "sensitive" data to demonstrate cleartext exposure
String deviceToken = DEVICE_TOKEN;

// --- Security additions ---
const String API_KEY = API_KEY_STR;

// Whitelist of allowed commands (only these values are accepted)
const String ALLOWED_COMMANDS[] = {"forward", "backward", "left", "right", "stop", "led_on", "led_off"};
const int NUM_ALLOWED_COMMANDS = 7;

bool isAuthorized() {
  if (server.hasHeader("Authorization")) {
    String authHeader = server.header("Authorization");
    // Expecting format: "Bearer my-secret-api-key-9F3kD8"
    if (authHeader == "Bearer " + API_KEY) {
      return true;
    }
  }
  // Also allow api_key as a query parameter, for easier browser testing
  if (server.hasArg("api_key") && server.arg("api_key") == API_KEY) {
    return true;
  }
  return false;
}

bool isValidCommand(String cmd) {
  for (int i = 0; i < NUM_ALLOWED_COMMANDS; i++) {
    if (cmd == ALLOWED_COMMANDS[i]) {
      return true;
    }
  }
  return false;
}
// --- End security additions ---

void handleRoot() {
  server.send(200, "text/plain", "ESP32 is alive");
}

void handleStatus() {
  if (!isAuthorized()) {
    server.send(401, "text/plain", "Unauthorized: missing or invalid API key");
    Serial.println("Rejected /status request: bad or missing API key");
    return;
  }
  String json = "{\"status\":\"ok\",\"token\":\"" + deviceToken + "\",\"uptime\":" + String(millis()) + "}";
  server.send(200, "application/json", json);
}

void handleCommand() {
  if (!isAuthorized()) {
    server.send(401, "text/plain", "Unauthorized: missing or invalid API key");
    Serial.println("Rejected /command request: bad or missing API key");
    return;
  }

  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Missing cmd parameter");
    return;
  }

  String cmd = server.arg("cmd");

  if (!isValidCommand(cmd)) {
    server.send(400, "text/plain", "Invalid command: " + cmd);
    Serial.println("Rejected invalid command: " + cmd);
    return;
  }

  Serial.println("Executing authorized command: " + cmd);
  server.send(200, "text/plain", "Command executed: " + cmd);
  // TODO: call your actual motor control function here, e.g. executeMotorCommand(cmd);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/command", handleCommand);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
