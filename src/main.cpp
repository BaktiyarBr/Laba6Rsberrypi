#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <microDS18B20.h>
#include <WiFiClient.h>

MicroDS18B20<14> sensor; // датчик на D2

// Replace with your network credentials
const char *ssid = "Beeline_61";
const char *password = "87024670827";

float temperatureSensorReadings = 0;

String createHttpMsg()
{
  // Create JSON data
  StaticJsonDocument<50> jsonDoc;
  jsonDoc["temperature_value"] = temperatureSensorReadings;
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;
}

void setup()
{
  Serial.begin(921600);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}
void loop()
{
  static uint32_t tmr;
  if (millis() - tmr >= 5000)
  {
    tmr = millis();
    // читаем прошлое значение
    if (sensor.readTemp()){
      temperatureSensorReadings = sensor.getTemp();
      Serial.println(sensor.getTemp());
    } else
      Serial.println("error");
    // запрашиваем новое измерение
    sensor.requestTemp();

    // Send HTTP request
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://c166-37-99-40-96.ngrok-free.app/temperature/");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(createHttpMsg());

    // Check response code
    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(response);
    }
    else
    {
      Serial.println("Error sending JSON data");
    }
    http.end();
  }
}