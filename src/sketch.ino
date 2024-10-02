/*
 * Time : 12:14 PM 2024-09-02
 */
#include "LiquidCrystal_I2C.h"
#include "config.h"
#include <ESP8266WiFi.h>

// Define All The Features Here 

#define ONEPLUS 
#define STATIC_IP
#define LCD_DISPLAY
#define DEBUG
// Function definition will go here

void print_ip();
void chekc_wifi();
void debug();
void update_reading();
const char *ssid = "802.11";
const char *password = "12345678p";

// All Pin Definitions go here

uint8_t led_relayPin = D4;

WiFiServer server(80);

String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 5000;

void check_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 }

// Light sensor Object

// Data struct
Data new_data;

#if defined(LCD_DISPLAY)
// LCD Display object 
LiquidCrystal_I2C lcd(0x27, 16,2); 
#endif
// Humid Temp Sensor Object

// Reconnect if lost connection
void reconnect() {
  if ((WiFi.status()) != WL_CONNECTED) {
    Serial.println("Reconnecting .");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
  // Get static IP
  pinMode(led_relayPin, OUTPUT);

#if defined(STATIC_IP)
  IPAddress local_IP(192, 168, 67, 50);
  IPAddress gateway(192, 168, 67, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 8, 8);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
#endif
  check_wifi();
  // Start Server
  server.begin();
  // Iniitlize the lcd

#if defined(LCD_DISPLAY)
  lcd.init();
  lcd.clear();
  lcd.backlight();
#endif
}


void loop() {
  print_ip();
  update_reading();

#if defined(LCD_DISPLAY)
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
#endif
  WiFiClient client = server.available();

#if defined(LCD_DISPLAY)
  lcd.setCursor(2, 1); // Set cursor to character 2 on line 0
  lcd.print("Volt = ");
  lcd.print(new_data.battery_voltage);
  lcd.print(" V");
#endif

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
#if defined(DEBUG)
    Serial.println("New Client.");
#endif
    
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
#if defined(DEBUG)
    Serial.println("Inside While");
#endif
      currentTime = millis();
      if (client.available()) {
#if defined(DEBUG)
        Serial.println("Client Available");
#endif
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
#if defined(DEBUG)
        Serial.println("c == newline");

#endif
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.print("{");
              client.print("\"battery_voltage\":");
              client.print(new_data.battery_voltage);
              client.print(",");
              client.print("\"led_relayState\":");
              client.print(new_data.led_relayState);
              client.println("}");
              client.println();
              break;
            } else {
#if defined(DEBUG)
              Serial.println("Inside Break else");
#endif
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
              client.println("<body><h1>Solar Battery Monitoring</h1>");
              client.println("<h3> Station IP :  " + String(WiFi.localIP().toString()) + "</h3>");
              client.println("<div class=\"row\">");
              client.println("<div class=\"column\">");
              client.println("<div id=\"chart-volt\" class=\"container \"></div>");
              client.println("</div> ");
              client.println("<div class=\"column\">");
              client.println("");
              client.println("</div> ");
              client.println("</div> ");
              client.println("<p id='battery_voltage'>Battery Voltage: " +String(new_data.battery_voltage) + "</p>");
              client.println("<script>");
              client.println("setInterval(function() {");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.onreadystatechange = function() {");
              client.println("if(xhr.readyState == 4 && xhr.status == 200) {");
              client.println("var data = JSON.parse(xhr.responseText);");
              client.println("document.getElementById('temperature').innerHTML ='Temperature: ' + data.temperature + ' (degree)C';");
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
  header = "";
  client.stop();
  reconnect();
#if defined(DEBUG)
  delay(2000);
  debug();
#endif
#if defined(LCD_DISPLAY)
  lcd.clear();
#endif
}


void print_ip(){
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
void debug() {
  Serial.println( "Battery Voltage " + String(new_data.battery_voltage));
}

void update_reading(){
  new_data.battery_voltage = (((analogRead(A0)) * 11.0 * 3.3) / 1024.0);
}
