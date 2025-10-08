<?php

/**
 * File untuk koneksi ke database.
 */

// Konfigurasi database
$host = 'localhost'; // atau '127.0.0.1'
$db_name = 'iot_json';
$username = 'root';
$password = '';

try {
    // Membuat objek PDO untuk koneksi
    $conn = new PDO("mysql:host=$host; dbname=$db_name", $username, $password);

    // Mengatur mode error PDO ke exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Mengatur mode fetch default ke associative array
    $conn->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);

    // Opsi tambahan (opsional): Mengatur character set ke utf8mb4
    $conn->exec("SET NAMES 'utf8mb4'");
} catch (PDOException $e) {
    // Menangkap dan menampilkan error jika koneksi gagal
    // die() akan menghentikan eksekusi skrip
    die("Koneksi ke database gagal: " . $e->getMessage());
}
