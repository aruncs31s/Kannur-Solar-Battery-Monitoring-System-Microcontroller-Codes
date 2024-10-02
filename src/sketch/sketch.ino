/*
 * Time : 12:14 PM 2024-09-02
 */
/* Notes For Those who editing this 
  * modules.h Hould be always included first
*/

#include "modules.h"

#if defined(LCD_DISPLAY)
#include "LiquidCrystal_I2C.h"
#endif
#include "config.h"
#if defined(HUMID_TEMP_SENSING)
#include "humid_temp_sensor.h"
#endif
#if defined(LIGHT_SENSING)
#include "light_sensor.h"
#endif
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else 
#include <WiFi.h>
#endif

#if defined(ONEPLUS_AP)
const char *ssid = "802.11";
const char *password = "12345678p";
#endif
WiFiServer server(80);

String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
String currentLine = "";
char c ;

void check_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
#if defined(DEBUG)
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

#if defined(LIGHT_SENSING)
// Light sensor Object
lightSensor Light_Sensor;

#endif
// Data struct
Data new_data;

#if defined(LCD_DISPLAY)
// LCD Display object 
LiquidCrystal_I2C lcd(0x27, 16,2); 
#endif 
#if defined(HUMID_TEMP_SENSING)
// Humid Temp Sensor Object
humidTempSensor Humid_Temp_Sensor;
#endif

// Reconnect if lost connection
void reconnect() {
  if ((WiFi.status()) != WL_CONNECTED) {
#if defined(DEBUG)
    Serial.println("Reconnecting .");
#endif
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
#if defined(STATIC_IP)
#if defined(STATIC_IP) && defined(ONEPLUS_AP)
  // Get static IP
  IPAddress local_IP(192, 168, 67, 50);
  IPAddress gateway(192, 168, 67, 1);
  IPAddress subnet(255, 255, 255, 0);
#endif
#if defined(STATIC_IP) && defined(GCEK)
  IPAddress local_IP(172,168,32,8);
  IPAddress gateway(172,168,32,1);
  IPAddress subnet(255, 255, 252, 0);
#endif
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 8, 8);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
#endif
  check_wifi();
#if defined(LIGHT_SENSING)
  Light_Sensor.begin();
#endif
#if defined(HUMID_TEMP_SENSING)
  Humid_Temp_Sensor.begin();
#endif
  // Start Server
  server.begin();
#if defined(LCD_DISPLAY)
  // Iniitlize the lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();
#endif

}

void loop() {
  update_reading();
  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
#if defined(LCD_DISPLAY)
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
#endif

  WiFiClient client = server.available();

#if defined(DEBUG)
  lcd.setCursor(2, 1); // Set cursor to character 2 on line 0
  lcd.print("Volt = ");
  lcd.print(new_data.battery_voltage);
  lcd.print(" V");
#endif
  header = "";
  if (client) {
    currentTime = millis();
    previousTime = currentTime;
#if defined(DEBUG)
    Serial.println("New Client.");
#endif
    currentLine = "" ;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
#if defined(DEBUG)
Serial.println("Client connected.");
#endif
      currentTime = millis();
      if (client.available()) {
        c = '\0';
        c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
#if defined(DEBUG)
Serial.println("Hit new line");
#endif
          if (currentLine.length() == 0) {
#if defined(DEBUG)
Serial.println("Header received.");
#endif
            if (header.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.print("{");
#if defined(HUMID_TEMP_SENSING)
              client.print("\"temperature\":");
              client.print(new_data.temperature);
              client.print(",");
              client.print("\"humidity\":");
              client.print(new_data.humidity);
              client.print(",");
#endif
#if defined(LIGHT_SENSING)
              client.print("\"light_sensor_value\":");
              client.print(new_data.light_sensor_value);
              client.print(",");
#endif
              client.print("\"battery_voltage\":");
              client.print(new_data.battery_voltage);
              client.print(",");
              client.print("\"led_realayStatus\"");
              client.print(new_data.led_relayStatus);
              client.println("}");
              client.println();
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
              client.println("</style></head>");
              client.println("<body><h1>Solar Battery Monitor</h1>");
              client.println("<h3> Station IP :  " + String(WiFi.localIP().toString()) + "</h3>");
              client.println("<div class=\"row\">");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-volt\" class=\"container \"></div>");
              client.println("</div> ");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-temperature\" class=\"container \"></div>");
              client.println("</div> ");
              client.println("</div> ");
#if defined(HUMID_TEMP_SENSING)
              client.println("<div class=\"row\">");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-humidity\" class=\"container\"></div>");
              client.println("</div>");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-temperature\" class=\"container\"></div>");
              client.println("</div>");
              client.println("</div> ");
#endif 
#if defined(LIGHT_SENSING)
              client.println("<div class=\"row\">");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-light\" class=\"container \"></div>");
              client.println("</div> ");
              client.println("</div> ");
#endif 

#if defined(HUMID_TEMP_SENSING)
              client.println("<p id='temperature'>Temperature: " + String(new_data.temperature) +" (degree) C</p>");
              client.println("<p id='humidity'>Humidity: " +String(new_data.humidity) + " %</p>");
#endif
#if defined(LIGHT_SENSING)
              client.println("<p id='light_intensity'>Light Intensity: " +String(new_data.light_sensor_value) + "</p>");
#endif

              client.println("<p id='battery_voltage'>Battery Voltage: " +String(new_data.battery_voltage) + "</p>");
              client.println("<script>");
              client.println("setInterval(function() {");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.onreadystatechange = function() {");
              client.println("if(xhr.readyState == 4 && xhr.status == 200) {");
              client.println("var data = JSON.parse(xhr.responseText);");
#if defined(HUMID_TEMP_SENSING)
              client.println("document.getElementById('temperature').innerHTML ='Temperature: ' + data.temperature + ' (degree)C';");
              client.println("document.getElementById('humidity').innerHTML = 'Humidity: ' + data.humidity + ' %';");
#endif
#if defined(LIGHT_SENSING)
              client.println("document.getElementById('light_intensity').innerHTML= 'Light Intensity: ' + data.light_sensor_value;");
#endif
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

#if defined(HUMID_TEMP_SENSING)
              // Chart Temperature
              client.println("var chartT = new Highcharts.Chart({");
              client.println(" chart: {");
              client.println(" renderTo:");
              client.println("   'chart-temperature'");
              client.println(" }");
              client.println("   , title : {text : 'Temperature'},");
              client.println("             series : [ {showInLegend : false, "
                             "data : []} ],");
              client.println("                      plotOptions");
              client.println("       : {");
              client.println("         line : {animation : false, dataLabels : "
                             "{enabled : true}},");
              client.println("         series : {color : '#059e8a'}");
              client.println("       },");
              client.println("         xAxis : {");
              client.println("           type : 'datetime',");
              client.println(
                  "           dateTimeLabelFormats : {second : '%H:%M:%S'}");
              client.println("         },");
              client.println("                 yAxis : {");
              client.println("                   title : {text : 'Temperature "
                             "(Celsius)'}");
              client.println("                   // title: { text: "
                             "'Temperature (Fahrenheit)' }");
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
              client.println("       var data = JSON.parse(this.responseText);"); 
              client.println("       var x = (new Date()).getTime(),"); 
              client.println("           y = parseFloat(data.temperature);"); 
              client.println("       if (chartT.series[0].data.length > 4000) {");
              client.println("         chartT.series[0].addPoint([ x, y ], true, true, true);");
              client.println("       } else {");
              client.println("         chartT.series[0].addPoint([ x, y ], true, false, true);");
              client.println("       }");
              client.println("     }");
              client.println("   };");
              client.println("   xhttp.open(\"GET\", \"/data\", true);"); 
              client.println("   xhttp.send();");
              client.println(" }, 1000);");
#endif
#if defined(LIGHT_SENSING)
             // Light Sensor Value
              client.println("var chartL = new Highcharts.Chart({");
              client.println(" chart: {");
              client.println(" renderTo:'chart-light'");
              client.println(" }");
              client.println("   , title : {text : 'Light Sensor'},");
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
              client.println("           y = parseFloat(data.light_sensor_value);"); 
              client.println("       if (chartL.series[0].data.length > 4000) {");
              client.println("         chartL.series[0].addPoint([ x, y ], true, true, true);");

              client.println("       } else {");
              client.println("         chartL.series[0].addPoint([ x, y ], true, false, true);");
              client.println("       }");
              client.println("     }");
              client.println("   };");
              client.println("   xhttp.open(\"GET\", \"/data\", true);"); 
              client.println("   xhttp.send();");
              client.println(" }, 1000);"); 
#endif
#if defined(HUMID_TEMP_SENSING)
              // Humidity
              client.println("var chartH = new Highcharts.Chart({");
              client.println(" chart: {");
              client.println(" renderTo:'chart-humidity'");

              client.println(" }");
              client.println("   , title : {text : 'Humidity'},");
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
              client.println("                   title : {text : 'Humidity "
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
              // Get json response
              client.println("       var data = JSON.parse(this.responseText);"); 
              // Get current time
              client.println("       var x = (new Date()).getTime(),"); 
              // Get humidity value
              client.println("           y = parseFloat(data.humidity);"); 
              client.println("       if (chartH.series[0].data.length > 4000) {");
              client.println("         chartH.series[0].addPoint([ x, y ], true, true, true);");

              client.println("       } else {");
              client.println("         chartH.series[0].addPoint([ x, y ], true, false, true);");
              client.println("       }");
              client.println("     }");
              client.println("   };");
              client.println("   xhttp.open(\"GET\", \"/data\", true);"); 
              client.println("   xhttp.send();");
              client.println(" }, 1000);"); 
#endif
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
#if defined(LDC_DISPLAY)
  lcd.clear();
#endif
}

void update_reading(){
#if defined(HUMID_TEMP_SENSING)
  Humid_Temp_Sensor.get_readings();
  new_data.temperature = Humid_Temp_Sensor.temperature;
  new_data.humidity = Humid_Temp_Sensor.humidity;
#endif
#if defined(LIGHT_SENSING)
  new_data.light_sensor_value = Light_Sensor.get_value();
#endif
  new_data.battery_voltage = (((analogRead(A0)) * 11.0 * 3.3) / 1024.0);
}

