<?php
// Set header untuk output JSON
header('Content-Type: application/json');

// Include file koneksi database
require_once 'conn.php';

$response = new stdClass();

try {
    // Saya berasumsi nama tabelnya adalah 'sensors' dan Anda ingin mengambil data pertama.
    // Potongan kode asli tidak begitu jelas mengenai query-nya.
    $stmt = $conn->query("SELECT idSensor, namaSensor, statusSensor FROM sensors LIMIT 1");
    $sensor_data = $stmt->fetch(PDO::FETCH_ASSOC);

    if ($sensor_data) {
        $response->idSensor = $sensor_data['idSensor'];
        $response->namaSensor = $sensor_data['namaSensor'];
        $response->statusSensor = $sensor_data['statusSensor'];
    } else {
        // Untuk menghindari error, inisialisasi properti jika tidak ada hasil
        $response->idSensor = null;
        $response->namaSensor = null;
        $response->statusSensor = null;
    }

    // Baris ini ada di potongan kode asli
    $response->add = [36, '40']; // [suhu, lumen]

} catch (PDOException $e) {
    // Jika terjadi error, kirim response error
    http_response_code(500); // Internal Server Error
    $response = ['error' => "Database error: " . $e->getMessage()];
}

// Tutup koneksi
$conn = null;

// Cetak response sebagai JSON
// Kode asli menggunakan JSON_FORCE_OBJECT, yang merupakan praktik yang baik dengan stdClass
echo json_encode($response, JSON_FORCE_OBJECT);
