<?php

require_once 'conn.php';
header('Content-Type: application/json');

/**
 * Menyimpan log data sensor ke tabel 'data'.
 *
 * @param PDO $pdo Objek koneksi database.
 * @param int $idSensor ID sensor yang datanya akan dicatat.
 * @param float $nilaiData Nilai dari sensor.
 * @return bool True jika berhasil, false jika gagal.
 */
function logSensorData(PDO $pdo, int $idSensor, float $nilaiData): bool
{
    try {
        // tglData akan diisi otomatis oleh database (DEFAULT CURRENT_TIMESTAMP)
        $stmt = $pdo->prepare("INSERT INTO data (idSensor, tglData, nilaiData) VALUES (:id, CURRENT_TIMESTAMP,:nilai)");
        $stmt->bindParam(':id', $idSensor, PDO::PARAM_INT);
        $stmt->bindParam(':nilai', $nilaiData, PDO::PARAM_STR); // Gunakan PARAM_STR untuk float
        return $stmt->execute();
    } catch (PDOException $e) {
        error_log("Error logging sensor data: " . $e->getMessage());
        return false;
    }
}

/**
 * Memperbarui status sensor di database.
 *
 * @param PDO $pdo Objek koneksi database.
 * @param int $idSensor ID sensor yang akan diperbarui.
 * @param int $status Status baru (1 atau 0).
 * @return bool True jika berhasil, false jika gagal.
 */
function updateSensorStatus(PDO $pdo, int $idSensor, int $status): bool
{
    try {
        $stmt = $pdo->prepare("UPDATE sensors SET statusSensor = :status WHERE idSensor = :id");
        $stmt->bindParam(':status', $status, PDO::PARAM_INT);
        $stmt->bindParam(':id', $idSensor, PDO::PARAM_INT);
        return $stmt->execute();
    } catch (PDOException $e) {
        error_log("Error updating sensor status: " . $e->getMessage());
        return false;
    }
}

try {
    $response = [];
    // Ambil data Lampu (ID 1)
    $stmtLampu = $conn->query("SELECT idSensor, namaSensor, statusSensor FROM sensors WHERE idSensor = 1");
    $lampuData = $stmtLampu->fetch(PDO::FETCH_ASSOC);
    if ($lampuData) {
        $response['Lampu'] = [
            "idSensor" => (string) $lampuData['idSensor'],
            "namaSensor" => $lampuData['namaSensor'],
            "statusSensor" => (string) $lampuData['statusSensor']
        ];
    }

    // Ambil data Pompa (ID 2)
    $stmtPompa = $conn->query("SELECT idSensor, namaSensor, statusSensor FROM sensors WHERE idSensor = 2");
    $pompaData = $stmtPompa->fetch(PDO::FETCH_ASSOC);
    if ($pompaData) {
        $response['Pompa'] = [
            "idSensor" => (string) $pompaData['idSensor'],
            "namaSensor" => $pompaData['namaSensor'],
            "statusSensor" => (string) $pompaData['statusSensor']
        ];
    }

    // Ambil nilai sensor lingkungan TERKINI dari tabel 'data' untuk respons.
    // Query ini mengambil satu baris (yang paling baru berdasarkan tglData) untuk setiap sensor.
    $sql = "
        SELECT t1.idSensor, t1.nilaiData
        FROM data t1
        INNER JOIN (
            SELECT idSensor, MAX(tglData) AS max_tgl
            FROM data
            WHERE idSensor IN (3, 4, 5)
            GROUP BY idSensor
        ) t2 ON t1.idSensor = t2.idSensor AND t1.tglData = t2.max_tgl";
    $stmtEnv = $conn->query($sql);
    $envSensors = $stmtEnv->fetchAll(PDO::FETCH_KEY_PAIR);

    // Siapkan struktur respons untuk NilaiSensor
    $response['NilaiSensor'] = [
        // Berikan nilai default 0.0 jika data belum ada
        "suhu" => isset($envSensors[3]) ? (float)$envSensors[3] : 0.0,
        "kelembapan" => isset($envSensors[4]) ? (float)$envSensors[4] : 0.0,
        "cahaya" => isset($envSensors[5]) ? (int)$envSensors[5] : 0,
    ];

    // Handle incoming sensor data from ESP32 (POST request)
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $input = file_get_contents('php://input');
        $data = json_decode($input, true);

        if (json_last_error() === JSON_ERROR_NONE && is_array($data)) {
            // --- LOGIKA OTOMATISASI ---
            // Definisikan ambang batas (thresholds). Anda bisa mengubah nilai ini.
            $lightThreshold = 1000;   // Nilai LDR di atas ini dianggap gelap
            $tempThreshold = 34.0;   // Suhu di atas ini dianggap panas
            $humidityThreshold = 60.0; // Kelembapan di bawah ini dianggap kering

            $temp = isset($data['temperature']) ? (float)$data['temperature'] : null;
            $hum = isset($data['humidity']) ? (float)$data['humidity'] : null;
            $light = isset($data['light']) ? (int)$data['light'] : null;

            // 1. Tentukan status baru untuk Lampu berdasarkan sensor cahaya
            if ($light !== null) {
                $newLampuStatus = ($light > $lightThreshold) ? 1 : 0;
                updateSensorStatus($conn, 1, $newLampuStatus); // Update status Lampu (ID 1)
                $lampuData['statusSensor'] = $newLampuStatus; // Perbarui data lokal untuk respons
            }

            // 2. Tentukan status baru untuk Pompa berdasarkan suhu dan kelembapan
            if ($temp !== null && $hum !== null) {
                $newPompaStatus = ($temp > $tempThreshold && $hum < $humidityThreshold) ? 1 : 0;
                updateSensorStatus($conn, 2, $newPompaStatus); // Update status Pompa (ID 2)
                $pompaData['statusSensor'] = $newPompaStatus; // Perbarui data lokal untuk respons
            }

            // 3. Catat data sensor ke tabel 'data' (misalnya, kita tetap log jika lampu menyala)
            if (isset($lampuData['statusSensor']) && $lampuData['statusSensor'] == '1') {
                if ($temp !== null) logSensorData($conn, 3, $temp);
                if ($hum !== null) logSensorData($conn, 4, $hum);
                if ($light !== null) logSensorData($conn, 5, $light);
            }

            // Perbarui data di variabel $response agar ESP32 menerima status terbaru
            if (isset($response['Lampu'])) $response['Lampu']['statusSensor'] = (string)$lampuData['statusSensor'];
            if (isset($response['Pompa'])) $response['Pompa']['statusSensor'] = (string)$pompaData['statusSensor'];
            if (isset($response['NilaiSensor']['suhu'])) $response['NilaiSensor']['suhu'] = $temp;
            if (isset($response['NilaiSensor']['kelembapan'])) $response['NilaiSensor']['kelembapan'] = $hum;
            if (isset($response['NilaiSensor']['cahaya'])) $response['NilaiSensor']['cahaya'] = $light;
        }
    }
} catch (PDOException $e) {
    http_response_code(500);
    echo json_encode(['error' => "Database error: " . $e->getMessage()]);
    exit();
}

// Tutup koneksi
if ($conn) {
    $conn = null;
}

// Cetak response sebagai JSON
echo json_encode($response);
exit();
