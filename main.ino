#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN 2     // Датчик DHT11 до GPIO2
#define DHTTYPE DHT11

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* host = "api.thingspeak.com";
String apiKey = "your_API_KEY";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.begin();

  Serial.println();
  Serial.println();
  Serial.print("Підключення до ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi підключений");
  Serial.println("IP-адреса: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Не вдалося прочитати дані з датчика DHT11!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      return;
    }

    String url = "/update?api_key=";
    url += apiKey;
    url += "&field1=";
    url += String(t);
    url += "&field2=";
    url += String(h);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
                 
    delay(10);
    
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    
    Serial.println();
    Serial.println("Закриття з'єднання");
    
  } else {
    Serial.println("Помилка підключення WiFi");
  }

  delay(15000); // Затримка в мілісекундах між надсиланням даних
}
