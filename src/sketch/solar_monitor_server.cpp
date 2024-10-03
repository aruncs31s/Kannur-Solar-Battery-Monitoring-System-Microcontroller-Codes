#include "solar_monitor_server.h"

void SolarMonitorServer::present_website(WiFiClient &client, Data &new_data) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" "
                 "content=\"width=device-width,\"initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println(
      "<script src=\"https://code.highcharts.com/highcharts.js\" ></script>");
  client.println("<style>html { font-family: Helvetica; display:inline-block; "
                 "margin: 0px auto; text-align: center;} .row{ display: "
                 "flex;} .column {flex: 50%; padding: 10px;}");
  client.println(".button { background-color: #4CAF50; border: none; color: "
                 "white; padding: 16px 40px;");
  client.println(
      "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");

  client.println("<body><h1>Signal Power Monitoring</h1>");
  client.println("<h3> Station IP :  " + String(WiFi.localIP().toString()) +
                 "</h3>");
  client.println("<div class=\"row\">");
  client.println("<div class=\"column\">");
  client.println("<div id=\"chart-volt\" class=\"container \"></div>");
  client.println("</div> ");
  client.println("<div class=\"column\">");
  client.println("<div id=\"chart-temperature\" class=\"container \"></div>");
  client.println("</div> ");
  client.println("</div> ");
  client.println("<div class=\"row\">");
  client.println("<div class=\"column\">");
  client.println("<div id=\"chart-humidity\" class=\"container\"></div>");
  client.println("</div>");
  client.println("<div class=\"column\">");
  client.println("<div id=\"chart-light\" class=\"container\"></div>");
  client.println("</div>");
  client.println("</div> ");
  client.println("<div class=\"row\">");
  client.println("<div class=\"column\">");
  client.println("<div id=\"chart-rain_volume\" class=\"container \"></div>");
  client.println("</div> ");
  client.println("</div> ");
  client.println(
      "<p>Street Light - Currently <span id='led_relayButton_State'>" +
      String(new_data.led_relayState) + "</span></p>");

  client.println(String("<p><button class=\"button\" id='led_relayButton' "
                        "onclick=\"toggleLED()\">") +
                 (new_data.led_relayState ? "OFF" : "ON") + "</button></p>");

  client.println("<p id='temperature'>Temperature: " +
                 String(new_data.temperature) + " (degree) C</p>");
  client.println("<p id='humidity'>Humidity: " + String(new_data.humidity) +
                 " %</p>");
  client.println("<p id='light_intensity'>Light Intensity: " +
                 String(new_data.light_sensor_value) + "</p>");
  client.println("<p id='battery_voltage'>Battery Voltage: " +
                 String(new_data.battery_voltage) + "</p>");
  client.println("<p id='rain_volume'>Current Rain: " +
                 String(new_data.rain_volume) + " mm </p>");
  client.println("<script>");
  client.println("let led_relayState = \"" +
                 String(new_data.led_relayState ? "on" : "off") + "\";");
  client.println("function toggleLED() {");
  client.println("var xhr = new XMLHttpRequest();");
  client.println("xhr.open('GET', '/data?=' + (led_relayState === 'off' "
                 "?'on' : 'off'), true);");
  client.println("xhr.onreadystatechange = function() {");
  client.println("if (xhr.readyState == 4 && xhr.status == 200) {");
  client.println("led_relayState = led_relayState === 'off' ? 'on' : 'off';");
  client.println("document.getElementById('led_relayButton_State').innerHTML = "
                 "led_relayState;");
  client.println("document.getElementById('led_relayButton').innerHTML = "
                 "(led_relayState === 'on' ? 'OFF' : 'ON');");
  client.println("    }");
  client.println("  };");
  client.println("  xhr.send();");
  client.println("}");

  client.println("setInterval(function() {");
  client.println("var xhr = new XMLHttpRequest();");
  client.println("xhr.onreadystatechange = function() {");
  client.println("if(xhr.readyState == 4 && xhr.status == 200) {");
  client.println("var data = JSON.parse(xhr.responseText);");
  client.println("document.getElementById('temperature').innerHTML "
                 "='Temperature: ' + data.temperature + ' (degree)C';");
  client.println("document.getElementById('humidity').innerHTML = 'Humidity: "
                 "' + data.humidity + ' %';");
  client.println("document.getElementById('light_intensity').innerHTML= "
                 "'Light Intensity: ' + data.light_sensor_value;");
  client.println("document.getElementById('battery_voltage').innerHTML= "
                 "'Battery Voltage: ' + data.battery_voltage;");
  client.println("document.getElementById('led_relayState').innerHTML= 'LED "
                 "Relay State ' + data.led_relayState;");
  client.println("    }");
  client.println("  };");
  client.println("  xhr.open('GET', '/data', true);");
  client.println("  xhr.send();");
  client.println("}, 3000);");
  // Rain Volume
  client.println("var chartR = new Highcharts.Chart({");
  client.println(" chart: {");
  client.println(" renderTo:'chart-rain_volume'");

  client.println(" }");
  client.println("   , title : {text : 'Rain Volume'},");
  client.println("             series : [ {showInLegend : false, "
                 "data : []} ],");
  client.println("                      plotOptions");
  client.println("       : {");
  client.println("         line : {animation : false, dataLabels : "
                 "{enabled : true}},");
  client.println("         series : {color : '#ffff00'}");
  client.println("       },");
  client.println("         xAxis : {");
  client.println("           type : 'datetime',");
  client.println("           dateTimeLabelFormats : {second : '%H:%M:%S'}");
  client.println("         },");
  client.println("                 yAxis : {");
  client.println("                   title : {text : 'Volume "
                 "(mm)'}");
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
  client.println("           y = parseFloat(data.rain_volume);");
  client.println("       if (chartR.series[0].data.length > 4000) {");
  client.println(
      "         chartR.series[0].addPoint([ x, y ], true, true, true);");

  client.println("       } else {");
  client.println(
      "         chartR.series[0].addPoint([ x, y ], true, false, true);");
  client.println("       }");
  client.println("     }");
  client.println("   };");
  client.println("   xhttp.open(\"GET\", \"/data\", true);");
  client.println("   xhttp.send();");
  client.println(" }, 1000);");

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
  client.println("           dateTimeLabelFormats : {second : '%H:%M:%S'}");
  client.println("         },");
  client.println("                 yAxis : {");
  client.println("                   title : {text : 'Voltage "
                 "(V)'}");
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
  client.println(
      "         chartB.series[0].addPoint([ x, y ], true, true, true);");

  client.println("       } else {");
  client.println(
      "         chartB.series[0].addPoint([ x, y ], true, false, true);");
  client.println("       }");
  client.println("     }");
  client.println("   };");
  client.println("   xhttp.open(\"GET\", \"/data\", true);");
  client.println("   xhttp.send();");
  client.println(" }, 1000);");
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
  client.println("           dateTimeLabelFormats : {second : '%H:%M:%S'}");
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
  client.println(
      "         chartL.series[0].addPoint([ x, y ], true, true, true);");

  client.println("       } else {");
  client.println(
      "         chartL.series[0].addPoint([ x, y ], true, false, true);");
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
  client.println("           dateTimeLabelFormats : {second : '%H:%M:%S'}");
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
  client.println(
      "         chartT.series[0].addPoint([ x, y ], true, true, true);");
  client.println("       } else {");
  client.println(
      "         chartT.series[0].addPoint([ x, y ], true, false, true);");
  client.println("       }");
  client.println("     }");
  client.println("   };");
  client.println("   xhttp.open(\"GET\", \"/data\", true);");
  client.println("   xhttp.send();");
  client.println(" }, 1000);");

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
  client.println("           dateTimeLabelFormats : {second : '%H:%M:%S'}");
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
  client.println(
      "         chartH.series[0].addPoint([ x, y ], true, true, true);");

  client.println("       } else {");
  client.println(
      "         chartH.series[0].addPoint([ x, y ], true, false, true);");
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
}
void SolarMonitorServer::update_json_response(WiFiClient &client,
                                              Data &new_data) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.println();
  client.print("{");
  client.print("\"temperature\":");
  client.print(new_data.temperature);
  client.print(",");
  client.print("\"humidity\":");
  client.print(new_data.humidity);
  client.print(",");
  client.print("\"light_sensor_value\":");
  client.print(new_data.light_sensor_value);
  client.print(",");
  client.print("\"battery_voltage\":");
  client.print(new_data.battery_voltage);
  client.print(",");
  client.print("\"led_relayState\":");
  client.print("\"" + new_data.led_relayState + "\"");
  client.print(",");
  client.print("\"rain_volume\":");
  client.print(new_data.rain_volume);
  client.println("}");
}

void SolarMonitorServer::turn_on_off_relay(bool _state) {
  digitalWrite(_led_relayPin, _state);
}
void SolarMonitorServer::init_relay() { pinMode(_led_relayPin, OUTPUT); }
