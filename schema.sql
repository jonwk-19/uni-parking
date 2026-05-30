-- Script de creacion de base de datos para Sistema de Parqueo
-- Ejecutar: mysql -u root -p < schema.sql

CREATE DATABASE IF NOT EXISTS sistema_parqueo;

USE sistema_parqueo;

CREATE TABLE IF NOT EXISTS vehiculos (
    id INT AUTO_INCREMENT PRIMARY KEY,
    placa VARCHAR(20) UNIQUE NOT NULL,
    marca VARCHAR(50),
    modelo VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS resumen_diario (
    id INT AUTO_INCREMENT PRIMARY KEY,
    fecha DATE NOT NULL UNIQUE,
    total_vehiculos INT NOT NULL DEFAULT 0,
    monto_total DECIMAL(10,2) NOT NULL DEFAULT 0.00,
    promedio_tiempo_segundos INT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS movimientos (
    id INT AUTO_INCREMENT PRIMARY KEY,
    placa VARCHAR(20),
    tipo_movimiento VARCHAR(50),
    fecha_hora DATETIME,
    monto_cobrado DECIMAL(10,2) DEFAULT 0.00,
    tiempo_segundos INT DEFAULT 0
);
