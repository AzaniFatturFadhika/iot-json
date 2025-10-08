<?php
// Include file koneksi database
require_once 'conn.php';

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
    </style>
</head>

<body>

    <div class="container">
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