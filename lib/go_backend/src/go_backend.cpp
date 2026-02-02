#include "go_backend.h"

GoBackend::GoBackend(const char *serverHost, uint16_t serverPort)
    : _serverHost(serverHost),
      _serverPort(serverPort),
      _deviceID(0),
      _lastSendTime(0),
      _sendInterval(DEFAULT_SEND_INTERVAL)
{
}

GoBackend::GoBackend(const char *serverHost, uint16_t serverPort,const char* authToken)
    : _serverHost(serverHost),
      _serverPort(serverPort),
      _deviceToken(authToken),
      _deviceID(0),
      _lastSendTime(0),
      _sendInterval(DEFAULT_SEND_INTERVAL)
{
}



bool GoBackend::sendReading(float voltage, float current)
{

  if (_deviceToken.length() == 0)
  {
    Serial.println("[GoBackend] Device token not set! Call getDeviceAuthToken() first.");
    return false;
  }

  // Build JSON payload
  StaticJsonDocument<256> doc;
  doc["voltage"] = voltage;
  doc["current"] = current;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  Serial.println("[GoBackend] Sending reading...");
  Serial.print("[GoBackend] Payload: ");
  Serial.println(jsonPayload);

  HTTPClient http;
  String url = buildURL("/api/readings");

  http.begin(_wifiClient, url);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + _deviceToken);

  int responseCode = http.POST(jsonPayload);
  String response = http.getString();
  http.end();

  if (responseCode == 201 || responseCode == 200)
  {
    Serial.println("[GoBackend] Reading sent successfully!");
    _lastSendTime = millis();
    return true;
  }
  else
  {
    Serial.print("[GoBackend] Failed to send reading! HTTP Code: ");
    Serial.println(responseCode);
    Serial.print("[GoBackend] Response: ");
    Serial.println(response);
  }

  return false;
}

bool GoBackend::sendData(Data &data)
{
  float voltage = 0.0;
  float current = 0.0;

// Extract voltage from Data struct
#if defined(BATTERY_MONITORING)
  voltage = data.battery_voltage;
#endif

  // Add more fields as needed from your Data struct

  return sendReading(voltage, current);
}

bool GoBackend::shouldSendData()
{
  unsigned long currentTime = millis();

  // Handle millis() overflow
  if (currentTime < _lastSendTime)
  {
    _lastSendTime = 0;
  }

  return (currentTime - _lastSendTime >= _sendInterval);
}

void GoBackend::setSendInterval(unsigned long intervalMs)
{
  _sendInterval = intervalMs;
  Serial.print("[GoBackend] Send interval set to: ");
  Serial.print(_sendInterval / 1000);
  Serial.println(" seconds");
}

uint8_t GoBackend::getDeviceState()
{
  if (_deviceID == 0)
  {
    Serial.println("[GoBackend] Device not registered!");
    return 0;
  }

  String endpoint = "/api/devices/" + String(_deviceID);
  int responseCode;
  String response = makeGetRequest(endpoint.c_str(), responseCode);

  if (responseCode == 200)
  {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error)
    {
      return doc["device_state"] | 0;
    }
  }

  return 0;
}

bool GoBackend::updateDeviceState(uint8_t stateId)
{
  if (_deviceID == 0)
  {
    Serial.println("[GoBackend] Device not registered!");
    return false;
  }

  StaticJsonDocument<128> doc;
  doc["state_id"] = stateId;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  String endpoint = "/api/devices/" + String(_deviceID) + "/state";
  int responseCode;
  makePostRequest(endpoint.c_str(), jsonPayload, responseCode);

  return (responseCode == 200);
}

bool GoBackend::isConnected()
{
  return (WiFi.status() == WL_CONNECTED && _deviceID > 0);
}

uint32_t GoBackend::getDeviceID()
{
  return _deviceID;
}


String GoBackend::getDeviceToken()
{
  return _deviceToken;
}

bool GoBackend::getDeviceAuthToken(uint32_t userID)
{
  if (_deviceID == 0)
  {
    Serial.println("[GoBackend] Device not registered!");
    return false;
  }

  // Build request payload
  StaticJsonDocument<128> doc;
  doc["device_id"] = _deviceID;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  Serial.println("[GoBackend] Requesting device auth token...");

  int responseCode;
  String response = makePostRequest("/api/device-auth/token", jsonPayload, responseCode);

  if (responseCode == 200)
  {
    // Parse token from response
    StaticJsonDocument<512> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);

    if (!error && responseDoc["token"] != nullptr)
    {
      _deviceToken = responseDoc["token"].as<String>();
      Serial.println("[GoBackend] Device token obtained successfully!");
      Serial.print("[GoBackend] Token: ");
      Serial.println(_deviceToken);
      return true;
    }
  }
  else
  {
    Serial.print("[GoBackend] Failed to get device token! HTTP Code: ");
    Serial.println(responseCode);
  }

  return false;
}

String GoBackend::makePostRequest(const char *endpoint, const String &jsonPayload, int &responseCode)
{
  HTTPClient http;
  String url = buildURL(endpoint);

  Serial.print("[GoBackend] POST ");
  Serial.println(url);

  http.begin(_wifiClient, url);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("Content-Type", "application/json");

  if (_authToken.length() > 0)
  {
    http.addHeader("Authorization", "Bearer " + _authToken);
  }

  responseCode = http.POST(jsonPayload);
  String response = http.getString();

  http.end();

  return response;
}

String GoBackend::makeGetRequest(const char *endpoint, int &responseCode)
{
  HTTPClient http;
  String url = buildURL(endpoint);

  Serial.print("[GoBackend] GET ");
  Serial.println(url);

  http.begin(_wifiClient, url);
  http.setTimeout(HTTP_TIMEOUT);

  http.addHeader("Authorization", "Bearer " + _deviceToken);

  responseCode = http.GET();
  String response = http.getString();

  http.end();

  return response;
}

String GoBackend::buildURL(const char *endpoint)
{
  String url = "http://";
  url += _serverHost;
  url += ":";
  url += String(_serverPort);
  url += endpoint;
  return url;
}

uint32_t GoBackend::parseDeviceID(const String &response)
{
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, response);

  if (error)
  {
    Serial.print("[GoBackend] JSON parsing failed: ");
    Serial.println(error.c_str());
    return 0;
  }

  // Try different possible JSON structures
  if (doc.containsKey("id"))
  {
    return doc["id"];
  }
  else if (doc.containsKey("device_id"))
  {
    return doc["device_id"];
  }
  else if (doc.containsKey("device"))
  {
    return doc["device"]["id"];
  }

  return 0;
}
