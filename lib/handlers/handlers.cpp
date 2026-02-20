#include "handlers.h"

// Constructor that creates and starts the server on default port 80
Handlers::Handlers() : _server(80) {
  _server.begin();
  Serial.println("Server started on port 80");
}

// Constructor that creates and starts the server on specified port
Handlers::Handlers(uint16_t port) : _server(port) {
  _server.begin();
  Serial.print("Server started on port ");
  Serial.println(port);
}

void Handlers::handleClient() {
  WiFiClient client = _server.accept();

  if (client) {
    _currentTime = millis();
    _previousTime = _currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && _currentTime - _previousTime <= TIMEOUT_MS) {
      _currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        _header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            Data current_data = readingsRepo.getAllReadings();
            // HTTP response
            if (_header.indexOf("GET /data") >= 0) {
              // Return JSON data
              Solar_monitor_server.update_json_response(client, current_data);
            } else if (_header.indexOf("GET /on") >= 0) {
              readingsRepo.updateLedState(LedState::ON);
              Solar_monitor_server.turn_on_off_relay(true);
              Solar_monitor_server.present_website(client, current_data);
            } else if (_header.indexOf("GET /off") >= 0) {
              readingsRepo.updateLedState(LedState::OFF);
              Solar_monitor_server.turn_on_off_relay(false);
              Solar_monitor_server.present_website(client, current_data);
            } else {
              // Default homepage
              Solar_monitor_server.present_website(client, current_data);
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    _header = "";
    client.stop();
    Serial.println("Client disconnected.\n");
  }
}
