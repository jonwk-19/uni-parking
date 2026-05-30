#include "../include/Ticket.h"
#include "../include/Utilidades.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <filesystem>

void generarTicket(const Vehiculo& vehiculo, int tiempoSegundos, double tarifa, double total) {
    // Asegurar que la carpeta tickets/ exista
    std::filesystem::create_directories("tickets");

    // Construir nombre del archivo: tickets/ticket_PLACA_YYYYMMDD_HHMMSS.txt
    char dateBuf[16];
    struct tm* tm_info = localtime(&vehiculo.horaSalida);
    if (!tm_info) {
        std::cerr << "Error: horaSalida invalida, no se puede generar ticket." << std::endl;
        return;
    }
    strftime(dateBuf, sizeof(dateBuf), "%Y%m%d_%H%M%S", tm_info);

    std::string filename = std::string("tickets/ticket_") + vehiculo.placa + "_" + dateBuf + ".txt";

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo crear el ticket " << filename << std::endl;
        return;
    }

    int h, m, s;
    convertirSegundosAHMS(tiempoSegundos, h, m, s);
    int horasCobradas = calcularHorasCobrables(tiempoSegundos);

    file << "========================================"  << std::endl;
    file << "TICKET DE SALIDA - SISTEMA DE PARQUEO"    << std::endl;
    file << "========================================"  << std::endl;
    file << "Placa:          " << vehiculo.placa        << std::endl;
    file << "Marca:          " << vehiculo.marca        << std::endl;
    file << "Modelo:         " << vehiculo.modelo       << std::endl;
    file << "Entrada:        " << convertirFechaHora(vehiculo.horaEntrada) << std::endl;
    file << "Salida:         " << convertirFechaHora(vehiculo.horaSalida)  << std::endl;
    file << "Tiempo total:   " << h << "h " << m << "m " << s << "s" << std::endl;
    file << "Horas cobradas: " << horasCobradas         << std::endl;
    file << std::fixed << std::setprecision(2);
    file << "Tarifa/hora:    $" << tarifa               << std::endl;
    file << "Total a pagar:  $" << total                << std::endl;
    file << "========================================"  << std::endl;
    file << "Gracias por usar nuestro parqueo."         << std::endl;
    file << "========================================"  << std::endl;

    std::cout << "Ticket generado: " << filename << std::endl;
}
