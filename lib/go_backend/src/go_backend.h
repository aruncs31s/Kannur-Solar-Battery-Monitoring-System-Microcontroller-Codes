#ifndef __GO_BACKEND_H__
#define __GO_BACKEND_H__

#if defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#include <ArduinoJson.h>
#include <WiFiClient.h>
#include "config.h"

// Configuration
#define DEFAULT_SEND_INTERVAL 60000  // 60 seconds
#define HTTP_TIMEOUT 10000           // 10 seconds
#define MAX_RETRY_ATTEMPTS 3

class GoBackend {
public:
  /**
   * @brief Constructor for GoBackend
   * @param serverHost Server hostname or IP (e.g., "192.168.1.100")
   * @param serverPort Server port (default: 8080)
   */
  GoBackend(const char* serverHost, uint16_t serverPort = 8080);

  // Currently used constructor
  GoBackend(const char* serverHost, uint16_t serverPort,const char* authToken);

  // /**
  //  * @brief Initialize the connection
  //  * @return true if successful, false otherwise
  //  */
  // bool begin();

  /**
   * @brief Register this device with the backend server
   * @param deviceName Name of the device
   * @param deviceType Type ID (1 for battery monitor, check backend)
   * @param macAddress MAC address of the device
   * @param firmwareVersionID Firmware version ID
   * @return true if successful, false otherwise
   */
  bool registerDevice(const char* deviceName, uint8_t deviceType, 
                     const char* macAddress, uint8_t firmwareVersionID = 1);

  /**
   * @brief Send sensor reading to the backend
   * @param voltage Battery voltage reading
   * @param current Current reading (optional, can be 0.0)
   * @return true if successful, false otherwise
   */
  bool sendReading(float voltage, float current = 0.0);

  /**
   * @brief Send complete data structure to the backend
   * @param data Reference to Data struct from config.h
   * @return true if successful, false otherwise
   */
  bool sendData(Data& data);

  /**
   * @brief Check if it's time to send data based on interval
   * @return true if should send, false otherwise
   */
  bool shouldSendData();

  /**
   * @brief Set the interval for automatic data sending
   * @param intervalMs Interval in milliseconds
   */
  void setSendInterval(unsigned long intervalMs);

  /**
   * @brief Get current device state from server
   * @return Device state ID (0 if error)
   */
  uint8_t getDeviceState();

  /**
   * @brief Update device state on server
   * @param stateId New state ID
   * @return true if successful, false otherwise
   */
  bool updateDeviceState(uint8_t stateId);

  /**
   * @brief Check connection status
   * @return true if connected, false otherwise
   */
  bool isConnected();

  /**
   * @brief Get the assigned device ID
   * @return Device ID (0 if not registered)
   */
  uint32_t getDeviceID();

  /**
   * @brief Set authentication token (if using JWT authentication)
   * @param token JWT token
   */
  void setAuthToken(const char* token);

  /**
   * @brief Get device auth token from backend
   * @param userID User ID (not used in request, for tracking)
   * @return true if successful, false otherwise
   */
  bool getDeviceAuthToken(uint32_t userID = 0);

  /**
   * @brief Get the device JWT token
   * @return Device JWT token
   */
  String getDeviceToken();

private:
  const char* _serverHost;
  uint16_t _serverPort;
  uint32_t _deviceID;
  String _authToken;
  String _deviceToken;
  
  unsigned long _lastSendTime;
  unsigned long _sendInterval;
  
  WiFiClient _wifiClient;
  
  /**
   * @brief Make HTTP POST request
   * @param endpoint API endpoint (e.g., "/api/devices")
   * @param jsonPayload JSON payload as String
   * @param responseCode Reference to store HTTP response code
   * @return Response body as String
   */
  String makePostRequest(const char* endpoint, const String& jsonPayload, int& responseCode);

  /**
   * @brief Make HTTP GET request
   * @param endpoint API endpoint
   * @param responseCode Reference to store HTTP response code
   * @return Response body as String
   */
  String makeGetRequest(const char* endpoint, int& responseCode);

  /**
   * @brief Build full URL
   * @param endpoint API endpoint
   * @return Full URL as String
   */
  String buildURL(const char* endpoint);

  /**
   * @brief Parse device ID from registration response
   * @param response JSON response
   * @return Device ID
   */
  uint32_t parseDeviceID(const String& response);
};

#endif // __GO_BACKEND_H__
