#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

WiFiClient wifiClient;

const char* ssid = "INCODE";
const char* password = "12345678";
int pinLED = 5;
int pinPompa = 4;
#define ON HIGH
#define OFF LOW

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Gunakan pin ADC yang valid, misal GPIO34
#define LDR_PIN 7

IPAddress local_IP(192, 168, 137, 160); // Sesuaikan jika perlu
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  // Konfigurasi Modul
  Serial.begin(115200);
  // Configure static IP
  WiFi.config(local_IP, gateway, subnet);
  connectToWiFi();

  // Deklarasi PIN GPIO
  pinMode(pinLED, OUTPUT);
  pinMode(pinPompa, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  dht.begin();
  // pinMode tidak diperlukan untuk pin ADC-only seperti GPIO34
}

void connectToWiFi() {
  Serial.print("\nMenyambungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void reportAndExecute();

unsigned long previousMillis = 0;
const long interval = 5000;

void loop() {
  // Cek koneksi WiFi. Jika terputus, coba sambungkan kembali.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nKoneksi WiFi terputus. Menyambungkan ulang...");
    connectToWiFi();
    return;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    reportAndExecute();
  }
}

void reportAndExecute() {
  HTTPClient http;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int ldrValue = analogRead(LDR_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Gagal membaca dari sensor DHT!"));
    // return;
  }

  // --- Siapkan Payload JSON untuk POST ---
  JsonDocument postDoc;
  if (!isnan(t)) {
    postDoc["temperature"] = t;
  }
  if (!isnan(h)) {
    postDoc["humidity"] = h;
  }
  postDoc["light"] = ldrValue;
  String requestBody;
  serializeJson(postDoc, requestBody);

  Serial.println("\n--------------------");
  Serial.print(">> Mengirim Laporan Sensor: ");
  Serial.println(requestBody);

  //IP menuju ke server web
  http.begin(wifiClient, "http://192.168.137.1/iot-json/sensorMultiJsonEvaluasi.php");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(requestBody); // Lakukan permintaan POST

  if (httpCode > 0) {
    Serial.printf("[HTTP] Kode respons: %d\n", httpCode);
    
    if (httpCode == HTTP_CODE_OK) {
      JsonDocument doc;

      // Deserialize JSON dari stream HTTP
      DeserializationError error = deserializeJson(doc, http.getStream());

      if (error) {
        Serial.print(F("deserializeJson() gagal: "));
        Serial.println(error.c_str());
        http.end();
        return;
      }

      Serial.println("<< Menerima Perintah dari Server:");
      serializeJsonPretty(doc, Serial);
      Serial.println();

      // Parse Lampu dengan aman menggunakan default value jika key tidak ada
      const char* lampu_status = doc["Lampu"]["statusSensor"] | "0";
      const char* lampu_nama = doc["Lampu"]["namaSensor"] | "N/A";

      // Parse Pompa dengan aman
      const char* pompa_status = doc["Pompa"]["statusSensor"] | "0";
      const char* pompa_nama = doc["Pompa"]["namaSensor"] | "N/A";

      // Nilai Sensor (baca sebagai tipe data yang benar: float/int)
      float nilai_suhu = doc["NilaiSensor"]["suhu"] | 0.0;
      float nilai_kelembapan = doc["NilaiSensor"]["kelembapan"] | 0.0;
      int nilai_cahaya = doc["NilaiSensor"]["cahaya"] | 0;

      // Display status
      bool isLampuOn = (strcmp(lampu_status, "1") == 0);
      bool isPompaOn = (strcmp(pompa_status, "1") == 0);

      Serial.printf("Perintah untuk Lampu (%s): %s\n", lampu_nama, isLampuOn ? "HIDUP" : "MATI");
      Serial.printf("Perintah untuk Pompa (%s): %s\n", pompa_nama, isPompaOn ? "HIDUP" : "MATI");


      // Control actuators based on status
      digitalWrite(pinLED, strcmp(lampu_status, "1") == 0 ? HIGH : LOW);
      digitalWrite(pinPompa, strcmp(pompa_status, "1") == 0 ? HIGH : LOW);

    } else {
      Serial.printf("[HTTP] Permintaan POST gagal, error: %s\n", http.errorToString(httpCode).c_str());
      String errorPayload = http.getString();
      Serial.println("Error payload: " + errorPayload);
    }
  } else {
    Serial.printf("[HTTP] Koneksi gagal: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
