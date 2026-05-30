#include "../include/ArchivoBinario.h"
#include "../include/Utilidades.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>

// Guarda un movimiento al final del archivo binario de movimientos.
void guardarMovimiento(const Movimiento& movimiento) {
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
