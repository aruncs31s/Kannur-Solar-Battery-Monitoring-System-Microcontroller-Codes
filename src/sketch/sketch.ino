/* Author : Arun CS
 * Time : 12:14 PM 2024-09-02
 */
#include "LiquidCrystal_I2C.h"
#include "config.h"
#include <ESP8266WiFi.h>

// function definitions
void print_ip(); // Prints ip 
bool check_wifi(); // Checks wifi
void debug(); // Used for only debugging 
void reconnect(); // Used to reconnect 
void update_reading(); // used to update the readings 

// This ssid is only for testing and needs to be changed 
const char *ssid = "802.11";
// If there is no password keep it empty
const char *password = "12345678p";

// create webserver object and define its port 
WiFiServer server(80);

// For webserver 
String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

// Data struct 
Data new_data = {0,0,1};

// LCD Display object 
LiquidCrystal_I2C lcd(0x27, 16,2); 


void setup() {
  Serial.begin(9600);
  // Get static IP
  IPAddress local_IP(192, 168, 67, 51);
  IPAddress gateway(192, 168, 67, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 8, 8);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

  if (check_wifi()){
  print_ip();
  }
  // Start Server
  server.begin();
  // Iniitlize the lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void update_reading(){
  new_data.battery_voltage = (((analogRead(A0)) * 11.0 * 3.3) / 1024.0);
}
void loop() {
  update_reading();
  print_ip(); 
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
  WiFiClient client = server.available();

  lcd.setCursor(2, 1); // Set cursor to character 2 on line 0
  lcd.print("Volt = ");
  lcd.print(new_data.battery_voltage);
  lcd.print(" V");

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.print("{");
              client.print("\"battery_voltage\":");
              client.print(new_data.battery_voltage);
              client.println("}");
              client.println();
            if (header.indexOf("GET /D4/on") >= 0) {
              Serial.println("GPIO 02 on");
              new_data.led_relayState = true;
              digitalWrite(LED_BUILTIN, HIGH);
            } else if(header.indexOf("GET /D4/off") >= 0) {
              Serial.println("GPIO D4 off");
              new_data.led_relayState = false;
              digitalWrite(LED_BUILTIN, LOW);
            }     
              break;
            } else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width,\"initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              client.println("<script src=\"https://code.highcharts.com/highcharts.js\" ></script>");
              client.println("<style>html { font-family: Helvetica; display:inline-block; margin: 0px auto; text-align: center;} .row{ display: flex;} .column {flex: 50%; padding: 10px;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #555555;}</style></head>");
              client.println("<body><h1>Signal Power Monitoring</h1>");
              client.println("<h3> Station IP :  " + String(WiFi.localIP().toString()) + "</h3>");
              client.println("<div class=\"row\">");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-volt\" class=\"container \"></div>");
              client.println("</div> ");
              client.println("<div class=\"column\">");

              client.println("<p>BUILTIN LED  - State " + String(new_data.led_relayState) + "</p>");
              // led status 
              if (new_data.led_relayState==false) {
              client.println("<p><a href=\"/D4/on\"><button class=\"button\">ON</button></a></p>");
              } else {
              client.println("<p><a href=\"/D4/off\"><button class=\"button button2\">OFF</button></a></p>");
              } 
              client.println("</div> ");
              client.println("</div> ");
              client.println("<p id='battery_voltage'>Battery Voltage: " +String(new_data.battery_voltage) + "</p>");
              client.println("<script>");
              client.println("setInterval(function() {");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.onreadystatechange = function() {");
              client.println("if(xhr.readyState == 4 && xhr.status == 200) {");
              client.println("var data = JSON.parse(xhr.responseText);");
              client.println("document.getElementById('battery_voltage').innerHTML= 'Battery Voltage: ' + data.battery_voltage;");
              client.println("    }");
              client.println("  };");
              client.println("  xhr.open('GET', '/data', true);");
              client.println("  xhr.send();");
              client.println("}, 3000);");
              // Battery Voltage
              client.println("var chartB = new Highcharts.Chart({");
              client.println(" chart: {");
              client.println(" renderTo:'chart-volt'");

              client.println(" }");
              client.println("   , title : {text : 'Battery Voltage'},");
              client.println("             series : [ {showInLegend : false, "
                             "data : []} ],");
              client.println("                      plotOptions");
              client.println("       : {");
              client.println("         line : {animation : false, dataLabels : "
                             "{enabled : true}},");
              client.println("         series : {color : '#00ffff'}");
              client.println("       },");
              client.println("         xAxis : {");
              client.println("           type : 'datetime',");
              client.println(
                  "           dateTimeLabelFormats : {second : '%H:%M:%S'}");
              client.println("         },");
              client.println("                 yAxis : {");
              client.println("                   title : {text : 'Voltage "
                             "(Celsius)'}");
              client.println("                 },");
              client.println("                         credits : {");
              client.println("   enabled:");
              client.println("     false");
              client.println("   }");
              client.println(" });");

              client.println(" setInterval(function() {");
              client.println("   var xhttp = new XMLHttpRequest();");
              client.println("   xhttp.onreadystatechange = function() {");
              client.println("     if (this.readyState == 4 && this.status == 200) {");
              // Get Json response
              client.println("       var data = JSON.parse(this.responseText);"); 
              // Get current time
              client.println("       var x = (new Date()).getTime(),"); 
              // Get battery_voltage
              client.println("           y = parseFloat(data.battery_voltage);"); 
              client.println("       if (chartB.series[0].data.length > 4000) {");
              client.println("         chartB.series[0].addPoint([ x, y ], true, true, true);");

              client.println("       } else {");
              client.println("         chartB.series[0].addPoint([ x, y ], true, false, true);");
              client.println("       }");
              client.println("     }");
              client.println("   };");
              client.println("   xhttp.open(\"GET\", \"/data\", true);"); 
              client.println("   xhttp.send();");
              client.println(" }, 1000);"); 
              client.println("</script>");
              client.println("</body></html>");
              client.println();
              break;
            }
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
  }
  client.stop();
  reconnect();
  debug();
  lcd.clear();
}
// Check if wifi is connected
bool check_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  return true;
}
void print_ip(){
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Just used for debugging
void debug() {
  // Serial.println("Humid " + String(new_data.humidity));
  // Serial.println("Temp " + String(new_data.temperature));
  // Serial.println("Light " + String(new_data.light_sensor_value));
}
// Reconnect if lost connection
void reconnect() {
  if ((WiFi.status()) != WL_CONNECTED) {
    Serial.println("Reconnecting .");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}
