<h1 align="center">Proyek IoT: Sistem Monitoring dan Kontrol Otomatis</h1>

## Deskripsi Proyek

Proyek ini adalah sistem Internet of Things (IoT) berbasis PHP dan MySQL yang dirancang untuk memantau data sensor lingkungan (suhu, kelembapan, cahaya) dan mengontrol aktuator (lampu, pompa) secara otomatis berdasarkan ambang batas yang ditentukan. Sistem ini menyediakan API JSON untuk komunikasi dengan perangkat mikrokontroler seperti ESP32, serta dashboard web sederhana untuk memantau status aktuator.

## Fitur Utama

-   **Penerimaan Data Sensor:** Menerima data suhu, kelembapan, dan cahaya dari perangkat IoT (misalnya ESP32) melalui permintaan HTTP POST.
-   **Pencatatan Data:** Menyimpan data sensor yang diterima ke database MySQL untuk keperluan historis.
-   **Kontrol Otomatis:** Mengontrol status lampu dan pompa berdasarkan logika ambang batas sensor yang telah dikonfigurasi (misalnya, lampu menyala jika kondisi gelap, pompa menyala jika suhu tinggi dan kelembapan rendah).
-   **API JSON:** Menyediakan endpoint API yang mudah digunakan untuk perangkat IoT, mengembalikan status aktuator terbaru dan nilai sensor terkini.
-   **Dashboard Web:** Tampilan web sederhana (`dashboard2.php`) untuk memantau status aktuator (lampu dan pompa) secara real-time.

## Prasyarat

Sebelum menjalankan proyek ini, pastikan Anda memiliki lingkungan berikut:

-   **Web Server:** Apache, Nginx, atau sejenisnya dengan dukungan PHP.
-   **PHP:** Versi 7.4 atau lebih tinggi (disarankan PHP 8.x).
-   **Database:** MySQL atau MariaDB.
-   **Perangkat IoT:** Mikrokontroler seperti ESP32 yang dapat mengirimkan data HTTP POST dan menerima respons JSON.

## Instalasi & Konfigurasi

1.  **Unduh Proyek:**
    Unduh atau kloning repositori proyek ini ke direktori web server Anda (misalnya `htdocs` untuk Apache, `www` untuk Laragon).

    ```bash
    # Jika menggunakan Git
    git clone <URL_REPOSITORI_ANDA> iot-json
    cd iot-json
    ```

2.  **Konfigurasi Database:**
    *   Buat database baru di MySQL/MariaDB Anda, misalnya `iot_json`.
    *   Impor skema database dari file `iot_json.sql` yang tersedia di proyek ini:
        ```bash
        mysql -u username -p iot_json < iot_json.sql
        ```
        Ganti `username` dengan username database Anda. Anda akan diminta memasukkan password.
    *   Pastikan tabel `sensors` dan `data` terisi dengan data awal seperti yang ada di `iot_json.sql`.

3.  **Konfigurasi Koneksi Database (`conn.php`):**
    *   Buka file `conn.php`.
    *   Sesuaikan detail koneksi database Anda sesuai dengan pengaturan server Anda:
        ```php
        // Konfigurasi database
        $host = 'localhost'; // atau IP database Anda
        $db_name = 'iot_json'; // Nama database yang Anda buat
        $username = 'root'; // Username database Anda
        $password = ''; // Password database Anda
        ```

## Penggunaan

### 1. Endpoint API untuk Perangkat IoT (`sensorMultiJsonEvaluasi.php`)

Perangkat ESP32 Anda harus mengirimkan permintaan HTTP POST ke `http://your_server_ip/iot-json/sensorMultiJsonEvaluasi.php`.

-   **Format Data POST (JSON):**
    Perangkat IoT harus mengirimkan data sensor dalam format JSON seperti contoh berikut:
    ```json
    {
        "temperature": 32.5,
        "humidity": 65.2,
        "light": 1500
    }
    ```
    *   `temperature`: Nilai suhu (float).
    *   `humidity`: Nilai kelembapan (float).
    *   `light`: Nilai sensor cahaya (integer).

-   **Respons API (JSON):**
    Sistem akan merespons dengan status aktuator terbaru dan nilai sensor terakhir setelah memproses data.
    ```json
    {
        "Lampu": {
            "idSensor": "1",
            "namaSensor": "LED 1",
            "statusSensor": "1" // 1 = HIDUP, 0 = MATI
        },
        "Pompa": {
            "idSensor": "2",
            "namaSensor": "LED 2",
            "statusSensor": "0" // 1 = HIDUP, 0 = MATI
        },
        "NilaiSensor": {
            "suhu": 32.5,
            "kelembapan": 65.2,
            "cahaya": 1500
        }
    }
    ```

### 2. Logika Otomatisasi

Logika kontrol otomatis diimplementasikan dalam `sensorMultiJsonEvaluasi.php`:

-   **Lampu (ID 1):**
    *   Menyala (`statusSensor = 1`) jika nilai sensor cahaya (`light`) lebih besar dari `lightThreshold` (default 1000, menandakan kondisi gelap).
    *   Mati (`statusSensor = 0`) jika nilai sensor cahaya kurang dari atau sama dengan `lightThreshold`.
-   **Pompa (ID 2):**
    *   Menyala (`statusSensor = 1`) jika suhu (`temperature`) lebih besar dari `tempThreshold` (default 34.0) DAN kelembapan (`humidity`) kurang dari `humidityThreshold` (default 60.0).
    *   Mati (`statusSensor = 0`) dalam kondisi lainnya.
-   **Pencatatan Data:** Data sensor (suhu, kelembapan, cahaya) hanya dicatat ke tabel `data` jika Lampu sedang menyala (`statusSensor` Lampu adalah 1).

### 3. Dashboard Web (`dashboard2.php`)

Akses dashboard melalui browser Anda: `http://your_server_ip/iot-json/dashboard2.php`. Halaman ini akan secara otomatis me-refresh setiap 5 detik untuk menampilkan status Lampu dan Pompa terbaru dari database.

## Struktur Proyek

-   `conn.php`: Berisi konfigurasi dan objek koneksi PDO ke database MySQL.
-   `iot_json.sql`: Skema database dan data awal untuk tabel `sensors` dan `data`.
-   `sensorMultiJsonEvaluasi.php`: Endpoint utama API. Menerima data sensor, menerapkan logika otomatisasi, memperbarui status aktuator di database, mencatat data sensor, dan mengembalikan respons JSON.
-   `sensorMultiJson.php`: (Opsional/Alternatif) Endpoint API yang lebih sederhana. Tidak digunakan dalam logika otomatisasi utama.
-   `dashboard2.php`: Halaman web sederhana untuk menampilkan status Lampu dan Pompa yang diambil dari database.

## Catatan Penting

-   Pastikan `idSensor` di database sesuai dengan ID yang digunakan dalam kode PHP. Berdasarkan `sensorMultiJsonEvaluasi.php`:
    *   `idSensor = 1`: Lampu
    *   `idSensor = 2`: Pompa
    *   `idSensor = 3`: Suhu
    *   `idSensor = 4`: Kelembapan
    *   `idSensor = 5`: Cahaya
    Periksa dan sesuaikan entri `namasensor` di tabel `sensors` pada `iot_json.sql` jika ada ketidaksesuaian (misalnya, `namasensor` untuk ID 3, 4, 5 agar lebih deskriptif seperti 'Suhu', 'Kelembapan', 'Cahaya').
-   Untuk keamanan, pertimbangkan untuk memindahkan kredensial database dari `conn.php` ke variabel lingkungan atau file konfigurasi yang tidak di-commit ke repositori publik.
-   Penanganan error bisa ditingkatkan, terutama untuk respons API yang lebih informatif kepada perangkat IoT.