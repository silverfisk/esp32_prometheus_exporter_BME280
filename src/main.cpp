#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

const char* ssid = "wifiname";
const char* password = "wifipassword";

const char* placename = "livingroom";

WebServer server(80);             
 
String SendHTML(float temperature,float humidity,float pressure,float altitude){
  String ptr = "# HELP temperature_celcius Current temperature in celcius\n";
  ptr +="temperature_celcius{place=\"";
  ptr +=placename;
  ptr +="\"} ";
  ptr +=temperature;
  ptr +="\n";
  ptr +="# HELP humidity_percentage Current humidity in Percent\n";
  ptr +="humidity_percentage{place=\"";
  ptr +=placename;
  ptr +="\"} ";
  ptr +=humidity;
  ptr +="\n";
  ptr +="# HELP pressure_hpa Current pressure in hPa\n";
  ptr +="pressure_hpa{place=\"";
  ptr +=placename;
  ptr +="\"} ";
  ptr +=pressure;
  ptr +="\n";
  ptr +="# HELP altitude_meter Current altitude in meter\n";
  ptr +="altitude_meter{place=\"";
  ptr +=placename;
  ptr +="\"} ";
  ptr +=altitude;
  ptr +="\n";
  return ptr;
}

void handle_OnConnect() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}


void initWifi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1024);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.println("");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   

  initWifi();

  

  server.on("/", handle_OnConnect);
  server.on("/metrics", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}

