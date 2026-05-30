#ifndef ARCHIVOBINARIO_H
#define ARCHIVOBINARIO_H

// Modulo: ArchivoBinario
// Responsabilidad: Leer y escribir movimientos, tarifa y placas en archivos binarios.

#include <vector>
#include <string>
#include "Movimiento.h"
#include "Vehiculo.h"

// Rutas de los archivos de datos
#define ARCHIVO_MOVIMIENTOS "data/movimientos.dat"
#define ARCHIVO_TARIFA      "data/tarifa.dat"
#define ARCHIVO_PLACAS      "data/placas.dat"

// Persistencia de movimientos
void guardarMovimiento(const Movimiento& movimiento);
std::vector<Movimiento> leerMovimientos();

// Persistencia de tarifa
void guardarTarifa(double tarifa);
double cargarTarifa();  // retorna -1.0 si el archivo no existe

// Persistencia de placas nuevas
bool placaExiste(const std::string& placa);
void registrarPlacaSiEsNueva(const Vehiculo& vehiculo);

// Muestra el historial de visitas de una placa desde los movimientos registrados.
void mostrarHistorialPlaca(const std::string& placa);

#endif
