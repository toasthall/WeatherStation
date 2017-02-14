/* DHT22 temperature and humidity data are uploaded to thingspeak using
 * WeMOS ESP8266 MCU.
 * 
 * Details: http://embedded-lab.com/blog/iot-temperature-and-humidity-logger/
 */
#include <ESP8266WiFi.h>
#include <DHT.h>

#include <Adafruit_BMP085.h>
#include "Math.h"
#define DHTTYPE DHT22
#define DHTPIN  D4   // DHT22 data pin connects to D4

// Time to sleep (in seconds):
const int sleepTimeS = 600;

DHT dht(DHTPIN, DHTTYPE, 22); // 11 works fine for ESP8266
Adafruit_BMP085 bmp;



float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey ="5LHFPCWDY3B9GG79";
const char* MY_SSID = "CYCAD"; 
const char* MY_PWD = "0439865684";

int sent = 0;
void setup() {
  Serial.begin(115200);
  dht.begin();
  bmp.begin();
  connectWifi();
  
}

void loop() {
  float temp,humidity,pressure;
  //char buffer[10];
  humidity = dht.readHumidity();          // Read humidity (percent)
  temp = dht.readTemperature();     // Read temperature as Fahrenheit
  pressure = (bmp.readPressure()/100.00);
    // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp) || isnan(pressure)) {
     Serial.println("Failed to read from DHT sensor!");
      return;
  }
  
  
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temp);
  Serial.print(String(sent)+" Humidity: ");
  Serial.println(humidity);
  Serial.print(String(sent)+" Presure: ");
  Serial.println(pressure);
  sendTeperatureTS(temp, humidity, pressure);
  ESP.deepSleep(sleepTimeS*1000000, WAKE_RF_DEFAULT); // Sleep for 60 seconds
  
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float temp1, float temp2, float temp3)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp1);
   postStr += "&field2=";
   postStr += String(temp2);
   postStr += "&field3=";
   postStr += String(temp3);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
