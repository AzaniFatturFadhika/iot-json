<?php
// Include file koneksi database
require_once 'conn.php';

/**
 * Memperbarui status sensor di database berdasarkan ID.
 *
 * @param PDO $pdo Objek koneksi database PDO.
 * @param int $id ID sensor yang akan diperbarui.
 * @param int $status Status baru untuk sensor (misal: 1 untuk ON, 0 untuk OFF).
 * @return bool Mengembalikan true jika berhasil, false jika gagal.
 */
function updateSensorStatus(PDO $pdo, int $id, int $status): bool
{
    try {
        $stmt = $pdo->prepare("UPDATE sensors SET statusSensor = :status WHERE idSensor = :id");
        $stmt->bindParam(':status', $status, PDO::PARAM_INT);
        $stmt->bindParam(':id', $id, PDO::PARAM_INT);
        return $stmt->execute();
    } catch (PDOException $e) {
        // Dalam aplikasi nyata, Anda mungkin ingin mencatat error ini daripada menghentikan skrip.
        error_log("Error updating record: " . $e->getMessage());
        return false;
    }
}

// Cek jika ada permintaan untuk mengubah status sensor
if (isset($_GET['id']) && isset($_GET['status'])) {
    $id = intval($_GET['id']);
    $status = intval($_GET['status']);

    if (!updateSensorStatus($conn, $id, $status)) {
        die("Gagal memperbarui status sensor.");
    }
    // Redirect kembali ke halaman utama untuk menghindari re-submit form
    header("Location: " . $_SERVER['PHP_SELF']);
    exit();
}

// // Cek jika permintaan datang dari ESP (misal dengan parameter khusus)
// if (isset($_GET['from']) && $_GET['from'] == 'esp') {
//     // Set header untuk output JSON
//     header('Content-Type: application/json');

//     $response = [
//         "Lampu" => new stdClass(),
//         "Pompa" => new stdClass(),
//         "NilaiSensor" => [
//             "suhu" => 36,
//             "kelembapan" => 40,
//             "cahaya" => 120
//         ]
//     ];

//     try {
//         // Ambil data Lampu (ID 1)
//         $stmtLampu = $conn->query("SELECT idSensor, namaSensor, statusSensor FROM sensors WHERE idSensor = 1");
//         $lampuData = $stmtLampu->fetch(PDO::FETCH_ASSOC);
//         if ($lampuData) {
//             $response['Lampu'] = [
//                 "idSensor" => (string) $lampuData['idSensor'],
//                 "namaSensor" => $lampuData['namaSensor'],
//                 "statusSensor" => (string) $lampuData['statusSensor']
//             ];
//         }

//         // Ambil data Pompa (ID 2)
//         $stmtPompa = $conn->query("SELECT idSensor, namaSensor, statusSensor FROM sensors WHERE idSensor = 2");
//         $pompaData = $stmtPompa->fetch(PDO::FETCH_ASSOC);
//         if ($pompaData) {
//             $response['Pompa'] = [
//                 "idSensor" => (string) $pompaData['idSensor'],
//                 "namaSensor" => $pompaData['namaSensor'],
//                 "statusSensor" => (string) $pompaData['statusSensor']
//             ];
//         }
//     } catch (PDOException $e) {
//         http_response_code(500);
//         echo json_encode(['error' => "Database error: " . $e->getMessage()]);
//         exit();
//     }

// Tutup koneksi
//     $conn = null;

//     // Cetak response sebagai JSON
//     echo json_encode($response);
//     exit();
// }

// Jika bukan dari ESP, tampilkan halaman HTML
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Kontrol IoT</title>
    <meta http-equiv="refresh" content="5"> <!-- Refresh halaman setiap 5 detik -->
    <style>
        body {
            font-family: sans-serif;
            text-align: center;
            margin-top: 50px;
        }

        .container {
            max-width: 400px;
            margin: auto;
        }

        .sensor {
            border: 1px solid #ccc;
            padding: 20px;
            margin-bottom: 20px;
            border-radius: 8px;
        }

        .sensor h2 {
            margin-top: 0;
        }

        .btn {
            display: inline-block;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            text-decoration: none;
            color: white;
            border-radius: 5px;
            border: none;
        }

        .btn-on {
            background-color: #28a745;
        }

        .btn-off {
            background-color: #dc3545;
        }

        .status {
            font-size: 18px;
            font-weight: bold;
            margin: 10px 0;
        }

        .status-on {
            color: #28a745;
        }

        .status-off {
            color: #dc3545;
        }

        .btn.disabled {
            background-color: #6c757d;
            pointer-events: none;
            cursor: default;
        }
    </style>
</head>

<body>

    <div class="container">
        <h1>Kontrol Perangkat IoT</h1>
        <h1>Status Perangkat IoT</h1>

        <?php
        // Ambil status terakhir dari DB untuk ditampilkan di HTML
        try {
            $stmt = $conn->query("SELECT idSensor, statusSensor FROM sensors WHERE idSensor IN (1, 2)");
            $sensors = $stmt->fetchAll(PDO::FETCH_KEY_PAIR);
        } catch (PDOException $e) {
            die("Error fetching records: " . $e->getMessage());
        }

        // Data untuk Lampu (ID 1)
        $idLampu = 1;
        $statusLampu = isset($sensors[$idLampu]) ? $sensors[$idLampu] : 0;
        ?>
        <div class="sensor">
            <h2>Lampu</h2>
            <p class="status <?php echo $statusLampu ? 'status-on' : 'status-off'; ?>">
                Status: <?php echo $statusLampu ? 'HIDUP' : 'MATI'; ?>
            </p>
            <a href="?id=<?php echo $idLampu; ?>&status=1" class="btn btn-on<?php echo $statusLampu ? ' disabled' : ''; ?>">ON</a>
            <a href="?id=<?php echo $idLampu; ?>&status=0" class="btn btn-off<?php echo !$statusLampu ? ' disabled' : ''; ?>">OFF</a>
        </div>


        <?php
        // Data untuk Pompa (ID 2)
        $idPompa = 2;
        $statusPompa = isset($sensors[$idPompa]) ? $sensors[$idPompa] : 0;
        ?>
        <div class="sensor">
            <h2>Pompa</h2>
            <p class="status <?php echo $statusPompa ? 'status-on' : 'status-off'; ?>">
                Status: <?php echo $statusPompa ? 'HIDUP' : 'MATI'; ?>
            </p>
            <a href="?id=<?php echo $idPompa; ?>&status=1" class="btn btn-on<?php echo $statusPompa ? ' disabled' : ''; ?>">ON</a>
            <a href="?id=<?php echo $idPompa; ?>&status=0" class="btn btn-off<?php echo !$statusPompa ? ' disabled' : ''; ?>">OFF</a>
        </div>
    </div>

</body>

</html>
<?php
// Tutup koneksi setelah semua selesai
if ($conn) {
    $conn = null;
}
?>