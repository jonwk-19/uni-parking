#include "../include/Parqueo.h"
#include "../include/Utilidades.h"
#include <iostream>
#include <climits>
#include <ctime>
#include <iomanip>

// Constructor: inicializa los carriles como pilas vacias
Parqueo::Parqueo(int cantCarriles, int capPorCarril, double tarifa)
    : cantidadCarriles(cantCarriles > 0 ? cantCarriles : 1),
      capacidadPorCarril(capPorCarril > 0 ? capPorCarril : 1),
      tarifa(tarifa > 0 ? tarifa : 1.0) {
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

    for (int i = 0; i < cantidadCarriles; i++) {
        int enCarril = (int)carriles[i].size();
        if (enCarril == 0) {
            std::cout << "Carril " << (i + 1) << ": (vacio)" << std::endl;
            continue;
        }
        std::cout << "Carril " << (i + 1) << ": " << enCarril
                  << " vehiculos (el ultimo en entrar aparece primero)" << std::endl;

        // Copiar la pila para no modificar la original (LIFO: top = ultimo en entrar)
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

// Setter de tarifa (solo acepta valores positivos)
void Parqueo::setTarifa(double nuevaTarifa) {
    if (nuevaTarifa > 0) tarifa = nuevaTarifa;
}

// Retira un vehiculo del parqueo por placa.
// Si hay vehiculos encima en el carril, los mueve temporalmente y los regresa despues.
void Parqueo::retirarVehiculo(const std::string& placa) {
    // 1. Buscar el carril que contiene la placa
    int carrilIndex = -1;
    for (int i = 0; i < cantidadCarriles; i++) {
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            if (std::string(temp.top().placa) == placa) {
                carrilIndex = i;
                break;
            }
            temp.pop();
        }
        if (carrilIndex != -1) break;
    }

    if (carrilIndex == -1) {
        std::cout << "Vehiculo con placa " << placa
                  << " no encontrado en el parqueo." << std::endl;
        return;
    }

    // 2. Mover los vehiculos encima del objetivo a una pila auxiliar
    std::stack<Vehiculo> auxiliar;
    while (std::string(carriles[carrilIndex].top().placa) != placa) {
        Vehiculo bloqueador = carriles[carrilIndex].top();
        carriles[carrilIndex].pop();
        auxiliar.push(bloqueador);
        std::cout << "Moviendo temporalmente: " << bloqueador.placa << std::endl;
    }

    // 3. El vehiculo objetivo esta ahora en el tope; registrar salida y calcular cobro
    Vehiculo vehiculo = carriles[carrilIndex].top();
    carriles[carrilIndex].pop();

    vehiculo.horaSalida = time(nullptr);
    int segundos = calcularSegundos(vehiculo.horaEntrada, vehiculo.horaSalida);
    double monto  = calcularMonto(segundos, tarifa);
    int horasCobradas = calcularHorasCobrables(segundos);

    int h, m, s;
    convertirSegundosAHMS(segundos, h, m, s);

    std::cout << "=== SALIDA DE VEHICULO ===" << std::endl;
    std::cout << "Placa:          " << vehiculo.placa  << std::endl;
    std::cout << "Marca:          " << vehiculo.marca  << std::endl;
    std::cout << "Modelo:         " << vehiculo.modelo << std::endl;
    std::cout << "Entrada:        " << convertirFechaHora(vehiculo.horaEntrada) << std::endl;
    std::cout << "Salida:         " << convertirFechaHora(vehiculo.horaSalida)  << std::endl;
    std::cout << "Tiempo:         " << h << "h " << m << "m " << s << "s" << std::endl;
    std::cout << "Horas cobradas: " << horasCobradas << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total a pagar:  $" << monto << std::endl;

    // 4. Regresar los vehiculos bloqueadores al carril en el orden original
    while (!auxiliar.empty()) {
        Vehiculo regreso = auxiliar.top();
        auxiliar.pop();
        carriles[carrilIndex].push(regreso);
        std::cout << "Regresando al carril: " << regreso.placa << std::endl;
    }

    // 5. Si hay espacio, ingresar vehiculos desde la cola de espera
    ingresarDesdeColaSiHayEspacio();
}

// Busca un vehiculo por placa sin modificar las estructuras.
// Revisa los carriles y luego la cola de espera.
void Parqueo::buscarVehiculo(const std::string& placa) const {
    // Buscar en los carriles usando copias de las pilas
    for (int i = 0; i < cantidadCarriles; i++) {
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            Vehiculo v = temp.top();
            temp.pop();
            if (std::string(v.placa) == placa) {
                time_t ahora = time(nullptr);
                int segundos = calcularSegundos(v.horaEntrada, ahora);
                int h, m, s;
                convertirSegundosAHMS(segundos, h, m, s);

                std::cout << "=== VEHICULO ENCONTRADO ===" << std::endl;
                std::cout << "Placa:    " << v.placa  << std::endl;
                std::cout << "Marca:    " << v.marca  << std::endl;
                std::cout << "Modelo:   " << v.modelo << std::endl;
                std::cout << "Entrada:  " << convertirFechaHora(v.horaEntrada) << std::endl;
                std::cout << "Tiempo estacionado: "
                          << h << "h " << m << "m " << s << "s" << std::endl;
                return;
            }
        }
    }

    // Buscar en la cola de espera usando una copia
    std::queue<Vehiculo> tempCola = colaEspera;
    int posicion = 1;
    while (!tempCola.empty()) {
        Vehiculo v = tempCola.front();
        tempCola.pop();
        if (std::string(v.placa) == placa) {
            std::cout << "Vehiculo " << placa
                      << " esta en la cola de espera (posicion " << posicion << ")." << std::endl;
            return;
        }
        posicion++;
    }

    std::cout << "Vehiculo con placa " << placa << " no encontrado." << std::endl;
}

// Ingresa vehiculos desde la cola de espera mientras haya espacio disponible.
void Parqueo::ingresarDesdeColaSiHayEspacio() {
    while (hayEspacio() && !colaEspera.empty()) {
        Vehiculo vehiculo = colaEspera.front();
        colaEspera.pop();
        vehiculo.horaEntrada = time(nullptr);
        ingresarVehiculo(vehiculo);
    }
}
