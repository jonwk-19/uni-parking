#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include <ctime>

// Tipos de movimiento registrados en el sistema.
#define MOV_ENTRADA          "ENTRADA"
#define MOV_SALIDA           "SALIDA"
#define MOV_COLA_ESPERA      "COLA_ESPERA"
#define MOV_MOVIDO_TEMPORAL  "MOVIDO_TEMPORAL"
#define MOV_REGRESADO        "REGRESADO"
#define MOV_INGRESO_COLA     "INGRESO_DESDE_COLA"

// Registro de un movimiento en el parqueo, guardado en binario.
struct Movimiento {
    char placa[20];
    char tipoMovimiento[40];
    time_t fechaHora;
    double montoCobrado;
    int tiempoSegundos;

    Movimiento();
    Movimiento(const char* placa, const char* tipo, double monto = 0.0, int segundos = 0);
};

#endif
