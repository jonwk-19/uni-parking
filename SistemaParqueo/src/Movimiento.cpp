#include "../include/Movimiento.h"
#include <cstring>

Movimiento::Movimiento() {
    placa[0] = '\0';
    tipoMovimiento[0] = '\0';
    fechaHora = 0;
    montoCobrado = 0.0;
    tiempoSegundos = 0;
}

Movimiento::Movimiento(const char* p, const char* tipo, double monto, int segundos) {
    strncpy(placa, p, sizeof(placa) - 1);
    placa[sizeof(placa) - 1] = '\0';
    strncpy(tipoMovimiento, tipo, sizeof(tipoMovimiento) - 1);
    tipoMovimiento[sizeof(tipoMovimiento) - 1] = '\0';
    fechaHora = time(nullptr);
    montoCobrado = monto;
    tiempoSegundos = segundos;
}
