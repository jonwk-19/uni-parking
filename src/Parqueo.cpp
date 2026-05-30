#include "../include/Parqueo.h"
#include "../include/Utilidades.h"
#include "../include/ArchivoBinario.h"
#include "../include/Ticket.h"
#include <iostream>
#include <climits>
#include <ctime>
#include <iomanip>
#include <set>

Parqueo::Parqueo(int cantCarriles, int capPorCarril, double tarifa)
    : cantidadCarriles(cantCarriles > 0 ? cantCarriles : 1),
      capacidadPorCarril(capPorCarril > 0 ? capPorCarril : 1),
      tarifa(tarifa > 0 ? tarifa : 1.0) {
    carriles.resize(cantidadCarriles);

    EstadoParqueoPersistido estado;
    bool estadoCargado = cargarEstadoParqueo(estado);
    if (estadoCargado &&
        estado.cantidadCarriles == cantidadCarriles &&
        estado.capacidadPorCarril == capacidadPorCarril &&
        (int)estado.carriles.size() == cantidadCarriles) {
        bool valido = true;
        for (int i = 0; i < cantidadCarriles; i++) {
            if ((int)estado.carriles[i].size() > capacidadPorCarril) {
                valido = false;
                break;
            }
            for (const Vehiculo& v : estado.carriles[i]) {
                carriles[i].push(v);
            }
        }
        if (valido) {
            for (const Vehiculo& v : estado.colaEspera) {
                colaEspera.push(v);
            }
            if (!validarIntegridad()) {
                for (int i = 0; i < cantidadCarriles; i++) {
                    while (!carriles[i].empty()) carriles[i].pop();
                }
                while (!colaEspera.empty()) colaEspera.pop();
                std::cerr << "Estado previo invalido. Se inicia parqueo vacio." << std::endl;
            } else {
                std::cout << "Estado del parqueo restaurado desde disco." << std::endl;
            }
        }
    } else if (estadoCargado) {
        std::cerr << "Estado persistido encontrado pero no aplicado por configuracion distinta. "
                  << "Esperado: carriles=" << estado.cantidadCarriles
                  << ", capacidadPorCarril=" << estado.capacidadPorCarril
                  << ". Recibido: carriles=" << cantidadCarriles
                  << ", capacidadPorCarril=" << capacidadPorCarril << "." << std::endl;
    }
}

bool Parqueo::hayEspacio() const {
    for (int i = 0; i < cantidadCarriles; i++) {
        if ((int)carriles[i].size() < capacidadPorCarril) return true;
    }
    return false;
}

int Parqueo::capacidadTotal() const {
    return cantidadCarriles * capacidadPorCarril;
}

int Parqueo::vehiculosEnParqueo() const {
    int total = 0;
    for (int i = 0; i < cantidadCarriles; i++) total += (int)carriles[i].size();
    return total;
}

int Parqueo::buscarCarrilConMenosVehiculos() const {
    int indiceMenor = 0;
    int menorTamano = INT_MAX;
    for (int i = 0; i < cantidadCarriles; i++) {
        if ((int)carriles[i].size() < capacidadPorCarril &&
            (int)carriles[i].size() < menorTamano) {
            menorTamano = (int)carriles[i].size();
            indiceMenor = i;
        }
    }
    return indiceMenor;
}

bool Parqueo::contienePlaca(const std::string& placa) const {
    for (int i = 0; i < cantidadCarriles; i++) {
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            if (std::string(temp.top().placa) == placa) return true;
            temp.pop();
        }
    }
    std::queue<Vehiculo> tempCola = colaEspera;
    while (!tempCola.empty()) {
        if (std::string(tempCola.front().placa) == placa) return true;
        tempCola.pop();
    }
    return false;
}

bool Parqueo::validarIntegridad() const {
    if (vehiculosEnParqueo() > capacidadTotal()) return false;
    std::set<std::string> placas;

    for (int i = 0; i < cantidadCarriles; i++) {
        if ((int)carriles[i].size() > capacidadPorCarril) return false;
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            std::string p = temp.top().placa;
            if (placas.count(p) > 0) return false;
            placas.insert(p);
            temp.pop();
        }
    }

    std::queue<Vehiculo> tempCola = colaEspera;
    while (!tempCola.empty()) {
        std::string p = tempCola.front().placa;
        if (placas.count(p) > 0) return false;
        placas.insert(p);
        tempCola.pop();
    }
    return true;
}

void Parqueo::guardarEstadoActual() const {
    EstadoParqueoPersistido estado;
    estado.cantidadCarriles = cantidadCarriles;
    estado.capacidadPorCarril = capacidadPorCarril;
    estado.carriles.resize(cantidadCarriles);
    for (int i = 0; i < cantidadCarriles; i++) {
        std::stack<Vehiculo> temp = carriles[i];
        std::vector<Vehiculo> topToBottom;
        while (!temp.empty()) {
            topToBottom.push_back(temp.top());
            temp.pop();
        }
        for (int j = (int)topToBottom.size() - 1; j >= 0; j--) {
            estado.carriles[i].push_back(topToBottom[j]);
        }
    }

    std::queue<Vehiculo> tempCola = colaEspera;
    while (!tempCola.empty()) {
        estado.colaEspera.push_back(tempCola.front());
        tempCola.pop();
    }

    if (!guardarEstadoParqueo(estado)) {
        std::cerr << "Advertencia: no se pudo persistir estado activo del parqueo." << std::endl;
    }
}

void Parqueo::ingresarVehiculo(const Vehiculo& vehiculo) {
    if (contienePlaca(vehiculo.placa)) {
        std::cout << "La placa " << vehiculo.placa
                  << " ya existe en el parqueo o cola de espera." << std::endl;
        return;
    }

    if (hayEspacio()) {
        int indiceMenor = buscarCarrilConMenosVehiculos();
        carriles[indiceMenor].push(vehiculo);
        std::cout << "Vehiculo " << vehiculo.placa
                  << " ingresado al carril " << (indiceMenor + 1) << "." << std::endl;
        guardarMovimiento(Movimiento(vehiculo.placa, MOV_ENTRADA));
        registrarPlacaSiEsNueva(vehiculo);
    } else {
        colaEspera.push(vehiculo);
        std::cout << "Parqueo lleno. Vehiculo " << vehiculo.placa
                  << " agregado a la cola de espera." << std::endl;
        guardarMovimiento(Movimiento(vehiculo.placa, MOV_COLA_ESPERA));
        registrarPlacaSiEsNueva(vehiculo);
    }

    if (!validarIntegridad()) {
        std::cerr << "Error de integridad detectado tras ingreso." << std::endl;
    }
    guardarEstadoActual();
}

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
        std::stack<Vehiculo> temp = carriles[i];
        while (!temp.empty()) {
            Vehiculo v = temp.top();
            temp.pop();
            std::cout << "  -> " << v.placa << " (" << v.marca << " " << v.modelo << ")" << std::endl;
        }
    }
}

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
        std::cout << pos << ". " << v.placa << " (" << v.marca << " " << v.modelo << ")" << std::endl;
        pos++;
    }
}

double Parqueo::getTarifa() const { return tarifa; }

void Parqueo::setTarifa(double nuevaTarifa) {
    if (nuevaTarifa > 0) tarifa = nuevaTarifa;
}

void Parqueo::retirarVehiculo(const std::string& placa) {
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
        std::cout << "Vehiculo con placa " << placa << " no encontrado en el parqueo." << std::endl;
        return;
    }

    std::stack<Vehiculo> auxiliar;
    while (std::string(carriles[carrilIndex].top().placa) != placa) {
        Vehiculo bloqueador = carriles[carrilIndex].top();
        carriles[carrilIndex].pop();
        auxiliar.push(bloqueador);
        std::cout << "Moviendo temporalmente: " << bloqueador.placa << std::endl;
        guardarMovimiento(Movimiento(bloqueador.placa, MOV_MOVIDO_TEMPORAL));
    }

    Vehiculo vehiculo = carriles[carrilIndex].top();
    carriles[carrilIndex].pop();
    vehiculo.horaSalida = time(nullptr);
    int segundos = calcularSegundos(vehiculo.horaEntrada, vehiculo.horaSalida);
    double monto = calcularMonto(segundos, tarifa);
    int horasCobradas = calcularHorasCobrables(segundos);

    int h, m, s;
    convertirSegundosAHMS(segundos, h, m, s);
    std::cout << "=== SALIDA DE VEHICULO ===" << std::endl;
    std::cout << "Placa:          " << vehiculo.placa << std::endl;
    std::cout << "Marca:          " << vehiculo.marca << std::endl;
    std::cout << "Modelo:         " << vehiculo.modelo << std::endl;
    std::cout << "Entrada:        " << convertirFechaHora(vehiculo.horaEntrada) << std::endl;
    std::cout << "Salida:         " << convertirFechaHora(vehiculo.horaSalida) << std::endl;
    std::cout << "Tiempo:         " << h << "h " << m << "m " << s << "s" << std::endl;
    std::cout << "Horas cobradas: " << horasCobradas << std::endl;
    std::ios_base::fmtflags flagsAntes = std::cout.flags();
    std::streamsize precisionAntes = std::cout.precision();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total a pagar:  $" << monto << std::endl;
    std::cout.flags(flagsAntes);
    std::cout.precision(precisionAntes);

    guardarMovimiento(Movimiento(vehiculo.placa, MOV_SALIDA, monto, segundos));
    generarTicket(vehiculo, segundos, tarifa, monto);

    while (!auxiliar.empty()) {
        Vehiculo regreso = auxiliar.top();
        auxiliar.pop();
        carriles[carrilIndex].push(regreso);
        std::cout << "Regresando al carril: " << regreso.placa << std::endl;
        guardarMovimiento(Movimiento(regreso.placa, MOV_REGRESADO));
    }

    ingresarDesdeColaSiHayEspacio();
    if (!validarIntegridad()) {
        std::cerr << "Error de integridad detectado tras retiro." << std::endl;
    }
    guardarEstadoActual();
}

void Parqueo::buscarVehiculo(const std::string& placa) const {
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
                std::cout << "Placa:    " << v.placa << std::endl;
                std::cout << "Marca:    " << v.marca << std::endl;
                std::cout << "Modelo:   " << v.modelo << std::endl;
                std::cout << "Entrada:  " << convertirFechaHora(v.horaEntrada) << std::endl;
                std::cout << "Tiempo estacionado: " << h << "h " << m << "m " << s << "s" << std::endl;
                return;
            }
        }
    }

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

void Parqueo::ingresarDesdeColaSiHayEspacio() {
    while (hayEspacio() && !colaEspera.empty()) {
        Vehiculo vehiculo = colaEspera.front();
        colaEspera.pop();
        vehiculo.horaEntrada = time(nullptr);
        int indiceMenor = buscarCarrilConMenosVehiculos();
        carriles[indiceMenor].push(vehiculo);
        std::cout << "Vehiculo " << vehiculo.placa
                  << " ingresado desde la cola al carril " << (indiceMenor + 1) << "." << std::endl;
        guardarMovimiento(Movimiento(vehiculo.placa, MOV_INGRESO_COLA));
    }
}
