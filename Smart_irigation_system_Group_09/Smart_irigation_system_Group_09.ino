#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// network credentials
const char* ssid = "sachintha";
const char* password = "BSE01@123";

// Server URL (Python)
const String serverUrl = "http://192.168.1.100:5000/sensor-data";

// Pin definitions for sensors
int soilMoisturePin = 34;
int temperaturePin = 32;
int humidityPin = 33;

#define DHTPIN 5         // Pin connected to the DHT sensor
#define DHTTYPE DHT22    // DHT 22 sensor type

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// Pin definition for the relay module
const int relayPin = 27;  // Pin connected to the relay module

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Set sensor pins
  pinMode(soilMoisturePin, INPUT);
  pinMode(relayPin, OUTPUT);  // Set relay pin as output
  dht.begin();
}

void loop() {
  // Read the sensor values
  int soilMoistureValue = analogRead(soilMoisturePin);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if the readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Debugging: Print values to Serial Monitor
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Control the water pump based on soil moisture level
  if (soilMoistureValue < 500) {  // Example threshold value (adjust as needed)
    digitalWrite(relayPin, HIGH);  // Turn on the pump
    Serial.println("Pump ON");
  } else {
    digitalWrite(relayPin, LOW);   // Turn off the pump
    Serial.println("Pump OFF");
  }

  // Prepare the data for the server
  String payload = "{\"soilMoisture\": " + String(soilMoistureValue) + ", \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";

  // Send the data to the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Send the HTTP POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode > 0) {
      Serial.print("Data sent successfully. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data. Response code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  // Delay 10 seconds before sending the next reading
  delay(10000);
}
