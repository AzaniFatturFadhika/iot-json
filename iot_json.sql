-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Oct 08, 2025 at 03:51 PM
-- Server version: 8.0.30
-- PHP Version: 8.3.11

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `iot_json`
--

-- --------------------------------------------------------

--
-- Table structure for table `data`
--

CREATE TABLE `data` (
  `idSensor` int NOT NULL,
  `tglData` datetime NOT NULL,
  `nilaiData` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `data`
--

INSERT INTO `data` (`idSensor`, `tglData`, `nilaiData`) VALUES
(5, '2025-10-08 22:51:16', 1817),
(3, '2025-10-08 22:51:21', 32.9),
(4, '2025-10-08 22:51:21', 60.3),
(5, '2025-10-08 22:51:21', 1819),
(3, '2025-10-08 22:51:26', 32.9),
(4, '2025-10-08 22:51:26', 60.4),
(5, '2025-10-08 22:51:26', 1799),
(3, '2025-10-08 22:51:31', 32.9),
(4, '2025-10-08 22:51:31', 60.3),
(5, '2025-10-08 22:51:31', 1765);

-- --------------------------------------------------------

--
-- Table structure for table `sensors`
--

CREATE TABLE `sensors` (
  `idsensor` int NOT NULL,
  `namasensor` varchar(32) NOT NULL,
  `statussensor` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `sensors`
--

INSERT INTO `sensors` (`idsensor`, `namasensor`, `statussensor`) VALUES
(1, 'LED 1', 1),
(2, 'LED 2', 0),
(3, 'LUMEN', 1),
(4, 'SUHU', 1),
(5, 'TEMPERAURE', 1);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `data`
--
ALTER TABLE `data`
  ADD KEY `idSensor` (`idSensor`) USING BTREE;

--
-- Indexes for table `sensors`
--
ALTER TABLE `sensors`
  ADD PRIMARY KEY (`idsensor`);

--
-- Constraints for dumped tables
--

--
-- Constraints for table `data`
--
ALTER TABLE `data`
  ADD CONSTRAINT `data_ibfk_1` FOREIGN KEY (`idSensor`) REFERENCES `sensors` (`idsensor`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
