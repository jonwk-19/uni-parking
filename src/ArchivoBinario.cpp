#include "../include/ArchivoBinario.h"
#include "../include/Utilidades.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <filesystem>

static void asegurarCarpetaData() {
    std::filesystem::create_directories("data");
}

// Guarda un movimiento al final del archivo binario de movimientos.
void guardarMovimiento(const Movimiento& movimiento) {
    asegurarCarpetaData();
    FILE* fp = fopen(ARCHIVO_MOVIMIENTOS, "ab");
    if (!fp) {
        std::cerr << "Error: no se pudo abrir " << ARCHIVO_MOVIMIENTOS << " para escritura." << std::endl;
        return;
    }
    fwrite(&movimiento, sizeof(Movimiento), 1, fp);
    fclose(fp);
}

// Lee todos los movimientos almacenados en el archivo binario.
// Usado por Reportes (estadisticas diarias) e historial por placa (opcion 8).
std::vector<Movimiento> leerMovimientos() {
    std::vector<Movimiento> movimientos;
    FILE* fp = fopen(ARCHIVO_MOVIMIENTOS, "rb");
    if (!fp) {
        return movimientos;  // archivo no existe, retornar vector vacio
    }
    Movimiento mov;
    while (fread(&mov, sizeof(Movimiento), 1, fp) == 1) {
        movimientos.push_back(mov);
    }
    fclose(fp);
    return movimientos;
}

// Guarda la tarifa actual sobreescribiendo el archivo.
void guardarTarifa(double tarifa) {
    asegurarCarpetaData();
    FILE* fp = fopen(ARCHIVO_TARIFA, "wb");
    if (!fp) {
        std::cerr << "Error: no se pudo abrir " << ARCHIVO_TARIFA << " para escritura." << std::endl;
        return;
    }
    fwrite(&tarifa, sizeof(double), 1, fp);
    fclose(fp);
}

// Carga la tarifa desde el archivo. Retorna -1.0 si el archivo no existe.
double cargarTarifa() {
    FILE* fp = fopen(ARCHIVO_TARIFA, "rb");
    if (!fp) {
        return -1.0;
    }
    double tarifa = -1.0;
    fread(&tarifa, sizeof(double), 1, fp);
    fclose(fp);
    return tarifa;
}

// Verifica si una placa ya esta registrada en el archivo de placas.
bool placaExiste(const std::string& placa) {
    FILE* fp = fopen(ARCHIVO_PLACAS, "rb");
    if (!fp) {
        return false;
    }
    char buffer[20];
    while (fread(buffer, sizeof(char[20]), 1, fp) == 1) {
        if (placa == std::string(buffer)) {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

// Registra la placa del vehiculo si aun no existe en el archivo.
void registrarPlacaSiEsNueva(const Vehiculo& vehiculo) {
    if (!placaExiste(vehiculo.placa)) {
        asegurarCarpetaData();
        FILE* fp = fopen(ARCHIVO_PLACAS, "ab");
        if (!fp) {
            std::cerr << "Error: no se pudo abrir " << ARCHIVO_PLACAS << " para escritura." << std::endl;
            return;
        }
        char buffer[20];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, vehiculo.placa, sizeof(buffer) - 1);
        fwrite(buffer, sizeof(char[20]), 1, fp);
        fclose(fp);
    }
}

bool guardarEstadoParqueo(const EstadoParqueoPersistido& estado) {
    asegurarCarpetaData();
    FILE* fp = fopen(ARCHIVO_ESTADO, "wb");
    if (!fp) return false;

    if (fwrite(&estado.cantidadCarriles, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (fwrite(&estado.capacidadPorCarril, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }

    int cantCarriles = (int)estado.carriles.size();
    if (fwrite(&cantCarriles, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }

    for (int i = 0; i < cantCarriles; i++) {
        int cantVehiculos = (int)estado.carriles[i].size();
        if (fwrite(&cantVehiculos, sizeof(int), 1, fp) != 1) {
            fclose(fp);
            return false;
        }
        if (cantVehiculos > 0) {
            if (fwrite(estado.carriles[i].data(), sizeof(Vehiculo), cantVehiculos, fp) != (size_t)cantVehiculos) {
                fclose(fp);
                return false;
            }
        }
    }

    int cantCola = (int)estado.colaEspera.size();
    if (fwrite(&cantCola, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return false;
    }
    if (cantCola > 0) {
        if (fwrite(estado.colaEspera.data(), sizeof(Vehiculo), cantCola, fp) != (size_t)cantCola) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
    return true;
}

bool cargarEstadoParqueo(EstadoParqueoPersistido& estado) {
    estado.carriles.clear();
    estado.colaEspera.clear();

    FILE* fp = fopen(ARCHIVO_ESTADO, "rb");
    if (!fp) return false;

    int cantCarrilesConfig = 0;
    int capPorCarrilConfig = 0;
    if (fread(&cantCarrilesConfig, sizeof(int), 1, fp) != 1 ||
        fread(&capPorCarrilConfig, sizeof(int), 1, fp) != 1 ||
        cantCarrilesConfig <= 0 || capPorCarrilConfig <= 0) {
        fclose(fp);
        return false;
    }
    estado.cantidadCarriles = cantCarrilesConfig;
    estado.capacidadPorCarril = capPorCarrilConfig;

    int cantCarriles = 0;
    if (fread(&cantCarriles, sizeof(int), 1, fp) != 1 || cantCarriles < 0 || cantCarriles > 1000) {
        fclose(fp);
        return false;
    }
    estado.carriles.resize(cantCarriles);

    for (int i = 0; i < cantCarriles; i++) {
        int cantVehiculos = 0;
        if (fread(&cantVehiculos, sizeof(int), 1, fp) != 1 || cantVehiculos < 0 || cantVehiculos > 100000) {
            fclose(fp);
            return false;
        }
        if (cantVehiculos > 0) {
            estado.carriles[i].resize(cantVehiculos);
            if (fread(estado.carriles[i].data(), sizeof(Vehiculo), cantVehiculos, fp) != (size_t)cantVehiculos) {
                fclose(fp);
                return false;
            }
        }
    }

    int cantCola = 0;
    if (fread(&cantCola, sizeof(int), 1, fp) != 1 || cantCola < 0 || cantCola > 100000) {
        fclose(fp);
        return false;
    }
    if (cantCola > 0) {
        estado.colaEspera.resize(cantCola);
        if (fread(estado.colaEspera.data(), sizeof(Vehiculo), cantCola, fp) != (size_t)cantCola) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
    return true;
}

// Muestra el historial de visitas de una placa desde los movimientos registrados.
void mostrarHistorialPlaca(const std::string& placa) {
    std::vector<Movimiento> movimientos = leerMovimientos();

    int visitas = 0;
    double totalPagado = 0.0;
    std::string ultimaFecha = "";

    for (const Movimiento& m : movimientos) {
        if (std::string(m.placa) != placa) continue;
        if (strcmp(m.tipoMovimiento, MOV_SALIDA) != 0) continue;
        visitas++;
        totalPagado += m.montoCobrado;
        std::string fecha = convertirFechaHora(m.fechaHora);
        if (fecha > ultimaFecha) ultimaFecha = fecha;
    }

    std::cout << "=== HISTORIAL DE PLACA: " << placa << " ===" << std::endl;
    if (visitas == 0) {
        std::cout << "No se encontraron visitas registradas para esta placa." << std::endl;
        return;
    }
    std::cout << "Visitas registradas: " << visitas << std::endl;
    std::cout << "Ultima visita:       " << ultimaFecha << std::endl;
    std::ios_base::fmtflags flags = std::cout.flags();
    std::streamsize prec = std::cout.precision();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total pagado:        $" << totalPagado << std::endl;
    std::cout.flags(flags);
    std::cout.precision(prec);
}
