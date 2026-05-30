#include "../include/Reportes.h"
#include "../include/ArchivoBinario.h"
#include "../include/Movimiento.h"
#include "../include/Utilidades.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <cstring>
#include <ctime>

struct DiaReporte {
    std::string fecha;
    int totalVehiculos;
    double montoTotal;
    int totalSegundos;
    int cantidadSalidas;
};

static std::map<std::string, DiaReporte> agregarMovimientos() {
    std::vector<Movimiento> movimientos = leerMovimientos();
    std::map<std::string, DiaReporte> dias;

    for (const Movimiento& m : movimientos) {
        if (strcmp(m.tipoMovimiento, MOV_SALIDA) != 0) continue;

        std::string fechaHora = convertirFechaHora(m.fechaHora);
        std::string fecha = fechaHora.substr(0, 10); // "YYYY-MM-DD"

        if (dias.find(fecha) == dias.end()) {
            DiaReporte d;
            d.fecha = fecha;
            d.totalVehiculos = 0;
            d.montoTotal = 0.0;
            d.totalSegundos = 0;
            d.cantidadSalidas = 0;
            dias[fecha] = d;
        }

        dias[fecha].montoTotal += m.montoCobrado;
        dias[fecha].totalSegundos += m.tiempoSegundos;
        dias[fecha].cantidadSalidas++;
        dias[fecha].totalVehiculos++;
    }

    return dias;
}

void generarReporteCSV() {
    std::map<std::string, DiaReporte> dias = agregarMovimientos();

    std::ofstream archivo("reportes/reporte_diario.csv", std::ios::trunc);
    archivo << "Fecha,Total Vehiculos,Monto Total,Promedio Tiempo (seg)\n";

    if (dias.empty()) {
        archivo << "Sin datos,0,0.00,0\n";
    } else {
        archivo << std::fixed << std::setprecision(2);
        for (const auto& par : dias) {
            const DiaReporte& d = par.second;
            int promedio = d.cantidadSalidas > 0 ? d.totalSegundos / d.cantidadSalidas : 0;
            archivo << d.fecha << ","
                    << d.totalVehiculos << ","
                    << d.montoTotal << ","
                    << promedio << "\n";
        }
    }

    archivo.close();
    std::cout << "Reporte CSV generado: reportes/reporte_diario.csv" << std::endl;
}

void generarReporteHTML() {
    std::map<std::string, DiaReporte> dias = agregarMovimientos();

    // Get current date/time for the "Generado:" line
    time_t ahora = time(nullptr);
    std::string fechaGeneracion = convertirFechaHora(ahora);

    std::ofstream archivo("reportes/reporte_diario.html", std::ios::trunc);

    archivo << "<!DOCTYPE html>\n"
            << "<html lang=\"es\">\n"
            << "<head>\n"
            << "    <meta charset=\"UTF-8\">\n"
            << "    <title>Reporte Diario - Sistema de Parqueo</title>\n"
            << "    <style>\n"
            << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
            << "        h1 { color: #333; }\n"
            << "        table { border-collapse: collapse; width: 100%; }\n"
            << "        th { background-color: #4CAF50; color: white; padding: 8px; text-align: left; }\n"
            << "        td { border: 1px solid #ddd; padding: 8px; }\n"
            << "        tr:nth-child(even) { background-color: #f2f2f2; }\n"
            << "        .total-row { font-weight: bold; background-color: #e8f5e9; }\n"
            << "    </style>\n"
            << "</head>\n"
            << "<body>\n"
            << "    <h1>Reporte Diario - Sistema de Parqueo</h1>\n"
            << "    <p>Generado: " << fechaGeneracion << "</p>\n";

    if (dias.empty()) {
        archivo << "    <p>No hay datos registrados aun.</p>\n";
    } else {
        int totalVehiculos = 0;
        double montoTotalGlobal = 0.0;
        int totalSegundosGlobal = 0;
        int totalSalidasGlobal = 0;

        for (const auto& par : dias) {
            const DiaReporte& d = par.second;
            totalVehiculos += d.totalVehiculos;
            montoTotalGlobal += d.montoTotal;
            totalSegundosGlobal += d.totalSegundos;
            totalSalidasGlobal += d.cantidadSalidas;
        }

        archivo << "    <table>\n"
                << "        <tr>\n"
                << "            <th>Fecha</th>\n"
                << "            <th>Total Vehiculos</th>\n"
                << "            <th>Monto Total ($)</th>\n"
                << "            <th>Promedio Tiempo</th>\n"
                << "        </tr>\n";

        archivo << std::fixed << std::setprecision(2);

        for (const auto& par : dias) {
            const DiaReporte& d = par.second;
            int promedioSeg = d.cantidadSalidas > 0 ? d.totalSegundos / d.cantidadSalidas : 0;
            int h = 0, min = 0, seg = 0;
            convertirSegundosAHMS(promedioSeg, h, min, seg);

            archivo << "        <tr>\n"
                    << "            <td>" << d.fecha << "</td>\n"
                    << "            <td>" << d.totalVehiculos << "</td>\n"
                    << "            <td>" << d.montoTotal << "</td>\n"
                    << "            <td>" << h << "h " << min << "m " << seg << "s</td>\n"
                    << "        </tr>\n";
        }

        // Totals row
        int promedioGlobalSeg = totalSalidasGlobal > 0 ? totalSegundosGlobal / totalSalidasGlobal : 0;
        int hG = 0, minG = 0, segG = 0;
        convertirSegundosAHMS(promedioGlobalSeg, hG, minG, segG);

        archivo << "        <tr class=\"total-row\">\n"
                << "            <td>TOTAL</td>\n"
                << "            <td>" << totalVehiculos << "</td>\n"
                << "            <td>" << montoTotalGlobal << "</td>\n"
                << "            <td>" << hG << "h " << minG << "m " << segG << "s</td>\n"
                << "        </tr>\n"
                << "    </table>\n";
    }

    archivo << "</body>\n"
            << "</html>\n";

    archivo.close();
    std::cout << "Reporte HTML generado: reportes/reporte_diario.html" << std::endl;
}
