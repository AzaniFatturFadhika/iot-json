#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// Konfigurasi NeoPixel (untuk LED RGB internal ESP32-S3)
#define NEOPIXEL_PIN 48 
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Konfigurasi WiFi
const char* ssid = "INCODE"; // Ganti dengan nama WiFi Anda
const char* password = "12345678"; // Ganti dengan password WiFi Anda

// --- KONFIGURASI IP STATIS ---
// Sesuaikan dengan konfigurasi jaringan Anda
IPAddress local_IP(192, 168, 137, 150); // Alamat IP statis yang diinginkan untuk ESP32
IPAddress gateway(192, 168, 137, 1);    // Alamat IP gateway (biasanya alamat IP router Anda)
IPAddress subnet(255, 255, 255, 0);     // Subnet mask jaringan Anda

// URL ke server PHP Anda
// GANTI 192.168.103.131 DENGAN ALAMAT IP LOKAL SERVER LARAGON ANDA
const char* server_url = "http://192.168.137.1/iot-json/sensorMultiJson.php"; 

void setup() {
  Serial.begin(115200);
  
  pixels.begin(); // Inisialisasi NeoPixel
  pixels.clear(); // Matikan LED
  pixels.show();

  connectToWiFi();
}

void loop() {
  // Cek koneksi WiFi sebelum melanjutkan
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Koneksi WiFi terputus. Mencoba menyambungkan kembali...");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // LED Merah: Error/Terputus
    pixels.show();
    connectToWiFi();
    return;
  }

  fetchAndProcessJson();

  // Tunggu 10 detik sebelum permintaan berikutnya
  Serial.println("\nMenunggu 10 detik sebelum refresh data...");
  delay(10000); 
}

void connectToWiFi() {
  Serial.print("Menyambungkan ke WiFi: ");
  Serial.println(ssid);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Gagal mengonfigurasi IP statis");
  }

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // LED Biru: Proses menyambung
    pixels.show();
    delay(250);
    pixels.clear();
    pixels.show();
    delay(250);
    Serial.print(".");
    if(attempts++ > 40) { // Timeout setelah 20 detik
        Serial.println("\nTidak dapat terhubung ke WiFi. Memulai ulang ESP...");
        ESP.restart();
    }
  }

  Serial.println("\n\nWiFi tersambung!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
  pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // LED Hijau: Tersambung
  pixels.show();
}

void fetchAndProcessJson() {
  HTTPClient http;

  Serial.print("Menghubungi server: ");
  Serial.println(server_url);
  
  pixels.setPixelColor(0, pixels.Color(255, 255, 0)); // LED Kuning: Proses HTTP
  pixels.show();

  if (http.begin(server_url)) {
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("[HTTP] Kode respons: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Menerima payload:");
        Serial.println(payload);

        // Alokasikan dokumen JSON.
        // Ukuran harus cukup besar untuk menampung semua data.
        // Sesuaikan ukuran jika data Anda lebih besar.
        JsonDocument doc;

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print(F("deserializeJson() gagal: "));
          Serial.println(error.f_str());
          pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // LED Merah: Error
          pixels.show();
          http.end();
          return;
        }

        // Proses array 'sensors'
        JsonArray sensors = doc["sensors"];
        if (sensors) {
            Serial.println("--- Data Sensor ---");
            for(JsonObject sensor : sensors) {
                const char* idSensor = sensor["idSensor"];
                const char* namaSensor = sensor["namaSensor"];
                const char* statusSensor = sensor["statusSensor"];

                Serial.printf("ID: %s, Nama: %s, Status: %s\n", idSensor, namaSensor, statusSensor);
            }
        }

        // Proses array 'add'
        JsonArray add_array = doc["add"];
        if (add_array) {
            int add_0 = add_array[0];
            int add_1 = add_array[1];
            Serial.println("--- Data Tambahan (add) ---");
            Serial.printf("Suhu: %d, Lumen: %d\n", add_0, add_1);
        }
        
        pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // LED Hijau: Sukses
        pixels.show();

      } else {
        Serial.printf("[HTTP] Gagal, kode error: %d\n", httpCode);
        pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // LED Merah: Error
        pixels.show();
      }
    } else {
      Serial.printf("[HTTP] Gagal, error: %s\n", http.errorToString(httpCode).c_str());
      pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // LED Merah: Error
      pixels.show();
    }

    http.end();
  } else {
    Serial.println("Koneksi HTTP gagal.");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // LED Merah: Error
    pixels.show();
  }
}
