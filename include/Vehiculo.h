#ifndef VEHICULO_H
#define VEHICULO_H

#include <ctime>

// Representa un vehiculo estacionado en el parqueo.
// Usa char[] para compatibilidad con escritura binaria directa.
struct Vehiculo {
    char placa[20];
    char marca[30];
    char modelo[30];
    time_t horaEntrada;
    time_t horaSalida;

    Vehiculo();
    Vehiculo(const char* placa, const char* marca, const char* modelo);
};

#endif
