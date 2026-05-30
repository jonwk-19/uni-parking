#ifndef PARQUEO_H
#define PARQUEO_H

#include <vector>
#include <stack>
#include <queue>
#include <string>
#include "Vehiculo.h"

// Administra los carriles del parqueo y la cola de espera.
// Carriles modelados como pilas (LIFO): el ultimo en entrar es el primero en salir.
// Cola de espera modelada como queue (FIFO): el primero en esperar es el primero en entrar.

class Parqueo {
private:
    std::vector<std::stack<Vehiculo>> carriles;
    std::queue<Vehiculo> colaEspera;
    int cantidadCarriles;
    int capacidadPorCarril;
    double tarifa;

public:
    Parqueo(int cantidadCarriles, int capacidadPorCarril, double tarifa);

    bool hayEspacio() const;
    int capacidadTotal() const;
    int vehiculosEnParqueo() const;

    void ingresarVehiculo(const Vehiculo& vehiculo);
    void retirarVehiculo(const std::string& placa);
    void buscarVehiculo(const std::string& placa) const;
    void mostrarEstado() const;
    void mostrarColaEspera() const;

    double getTarifa() const;
    void setTarifa(double nuevaTarifa);

private:
    void ingresarDesdeColaSiHayEspacio();
    bool contienePlaca(const std::string& placa) const;
    bool validarIntegridad() const;
    void guardarEstadoActual() const;
    int buscarCarrilConMenosVehiculos() const;
};

#endif
