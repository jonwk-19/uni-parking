#include "../include/Vehiculo.h"
#include <cstring>
#include <ctime>

Vehiculo::Vehiculo() {
    placa[0] = '\0';
    marca[0] = '\0';
    modelo[0] = '\0';
    horaEntrada = 0;
    horaSalida = 0;
}

Vehiculo::Vehiculo(const char* p, const char* m, const char* mo) {
    strncpy(placa, p, sizeof(placa) - 1);
    placa[sizeof(placa) - 1] = '\0';
    strncpy(marca, m, sizeof(marca) - 1);
    marca[sizeof(marca) - 1] = '\0';
    strncpy(modelo, mo, sizeof(modelo) - 1);
    modelo[sizeof(modelo) - 1] = '\0';
    horaEntrada = time(nullptr);
    horaSalida = 0;
}
