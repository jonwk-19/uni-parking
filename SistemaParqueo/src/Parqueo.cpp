#include "../include/Parqueo.h"
#include <iostream>
#include <climits>

// Constructor: inicializa los carriles como pilas vacias
Parqueo::Parqueo(int cantCarriles, int capPorCarril, double tarifa)
    : cantidadCarriles(cantCarriles), capacidadPorCarril(capPorCarril), tarifa(tarifa) {
    carriles.resize(cantidadCarriles);
}

// Devuelve true si al menos un carril tiene espacio disponible
bool Parqueo::hayEspacio() const {
    for (int i = 0; i < cantidadCarriles; i++) {
        if ((int)carriles[i].size() < capacidadPorCarril) {
            return true;
        }
    }
    return false;
}

// Devuelve la capacidad total del parqueo
int Parqueo::capacidadTotal() const {
    return cantidadCarriles * capacidadPorCarril;
}

// Devuelve la cantidad de vehiculos actualmente estacionados
int Parqueo::vehiculosEnParqueo() const {
    int total = 0;
    for (int i = 0; i < cantidadCarriles; i++) {
        total += (int)carriles[i].size();
    }
    return total;
}

// Ingresa un vehiculo al carril con menos vehiculos, o a la cola si el parqueo esta lleno
void Parqueo::ingresarVehiculo(const Vehiculo& vehiculo) {
    if (hayEspacio()) {
        // Buscar el carril con menos vehiculos
        int indiceMenor = 0;
        int menorTamano = INT_MAX;
        for (int i = 0; i < cantidadCarriles; i++) {
            if ((int)carriles[i].size() < capacidadPorCarril &&
                (int)carriles[i].size() < menorTamano) {
                menorTamano = (int)carriles[i].size();
                indiceMenor = i;
            }
        }
        carriles[indiceMenor].push(vehiculo);
        std::cout << "Vehiculo " << vehiculo.placa
                  << " ingresado al carril " << (indiceMenor + 1) << "." << std::endl;
    } else {
        colaEspera.push(vehiculo);
        std::cout << "Parqueo lleno. Vehiculo " << vehiculo.placa
                  << " agregado a la cola de espera." << std::endl;
    }
}

// Muestra el estado actual de todos los carriles
void Parqueo::mostrarEstado() const {
    int estacionados = vehiculosEnParqueo();
    int disponibles  = capacidadTotal() - estacionados;

    std::cout << "=== ESTADO DEL PARQUEO ===" << std::endl;
    std::cout << "Capacidad total: " << capacidadTotal()
              << " | Vehiculos estacionados: " << estacionados
              << " | Disponibles: " << disponibles << std::endl;

    if (estacionados == 0) {
        std::cout << "El parqueo esta vacio." << std::endl;
        return;
    }

    for (int i = 0; i < cantidadCarriles; i++) {
        std::cout << "Carril " << (i + 1) << ": " << carriles[i].size()
                  << " vehiculos" << std::endl;

        // Copiar la pila para no modificar la original
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            Vehiculo v = temp.top();
            temp.pop();
            std::cout << "  -> " << v.placa
                      << " (" << v.marca << " " << v.modelo << ")" << std::endl;
        }
    }
}

// Muestra los vehiculos en la cola de espera
void Parqueo::mostrarColaEspera() const {
    std::cout << "=== COLA DE ESPERA ===" << std::endl;

    if (colaEspera.empty()) {
        std::cout << "No hay vehiculos en espera." << std::endl;
        return;
    }

    std::queue<Vehiculo> temp = colaEspera;
    int pos = 1;
    while (!temp.empty()) {
        Vehiculo v = temp.front();
        temp.pop();
        std::cout << pos << ". " << v.placa
                  << " (" << v.marca << " " << v.modelo << ")" << std::endl;
        pos++;
    }
}

// Getter de tarifa
double Parqueo::getTarifa() const {
    return tarifa;
}

// Setter de tarifa
void Parqueo::setTarifa(double nuevaTarifa) {
    tarifa = nuevaTarifa;
}
