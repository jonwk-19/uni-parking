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
#include <cstdlib>

struct DiaReporte {
    std::string fecha;
    int cantidadSalidas;
    double montoTotal;
    int totalSegundos;
};

// Lee movimientos.dat y agrupa los registros SALIDA por fecha (YYYY-MM-DD).
static std::map<std::string, DiaReporte> cargarDiasReporte() {
    std::vector<Movimiento> movimientos = leerMovimientos();
    std::map<std::string, DiaReporte> dias;

    for (const Movimiento& m : movimientos) {
        if (strcmp(m.tipoMovimiento, MOV_SALIDA) != 0) continue;

        std::string fechaHora = convertirFechaHora(m.fechaHora);
        std::string fecha = fechaHora.substr(0, 10); // "YYYY-MM-DD"

        if (dias.find(fecha) == dias.end()) {
            DiaReporte d;
            d.fecha = fecha;
            d.cantidadSalidas = 0;
            d.montoTotal = 0.0;
            d.totalSegundos = 0;
            dias[fecha] = d;
        }

        dias[fecha].montoTotal += m.montoCobrado;
        dias[fecha].totalSegundos += m.tiempoSegundos;
        dias[fecha].cantidadSalidas++;
    }

    return dias;
}

// Escribe el reporte CSV usando datos ya cargados.
static void escribirCSV(const std::map<std::string, DiaReporte>& dias) {
    std::ofstream archivo("reportes/reporte_diario.csv", std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo crear reportes/reporte_diario.csv" << std::endl;
        return;
    }

    archivo << "Fecha,Total Vehiculos,Monto Total,Promedio Tiempo (seg)\n";

    if (dias.empty()) {
        archivo << "# Sin datos registrados\n";
    } else {
        archivo << std::fixed << std::setprecision(2);
        for (const auto& par : dias) {
            const DiaReporte& d = par.second;
            int promedio = d.cantidadSalidas > 0 ? d.totalSegundos / d.cantidadSalidas : 0;
            archivo << d.fecha << ","
                    << d.cantidadSalidas << ","
                    << d.montoTotal << ","
                    << promedio << "\n";
        }
    }

    archivo.close();
    std::cout << "Reporte CSV generado: reportes/reporte_diario.csv" << std::endl;
}

// Escribe el reporte HTML usando datos ya cargados.
static void escribirHTML(const std::map<std::string, DiaReporte>& dias) {
    time_t ahora = time(nullptr);
    std::string fechaGeneracion = convertirFechaHora(ahora);

    std::ofstream archivo("reportes/reporte_diario.html", std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo crear reportes/reporte_diario.html" << std::endl;
        return;
    }

    archivo << "<!DOCTYPE html>\n"
            << "<html lang=\"es\">\n"
            << "<head>\n"
            << "    <meta charset=\"UTF-8\">\n"
            << "    <title>Reporte Diario - Sistema de Parqueo</title>\n"
            << "    <style>\n"
            << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
            << "        h1 { color: #333; }\n"
            << "        table { border-collapse: collapse; width: 100%; }\n"
            << "        th { background-color: #4CAF50; color: white; padding: 8px; "
            << "text-align: left; border: 1px solid #aaa; }\n"
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

        for (const auto& par : dias) {
            const DiaReporte& d = par.second;
            totalVehiculos += d.cantidadSalidas;
            montoTotalGlobal += d.montoTotal;
            totalSegundosGlobal += d.totalSegundos;
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
                    << "            <td>" << d.cantidadSalidas << "</td>\n"
                    << "            <td>" << d.montoTotal << "</td>\n"
                    << "            <td>" << h << "h " << min << "m " << seg << "s</td>\n"
                    << "        </tr>\n";
        }

        int promedioGlobalSeg = totalVehiculos > 0 ? totalSegundosGlobal / totalVehiculos : 0;
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

// Comando wkhtmltopdf para conversion a PDF.
// Windows: 2>nul suprime stderr. En Linux/Mac cambiar a: 2>/dev/null
static const char* CMD_PDF =
    "wkhtmltopdf reportes/reporte_diario.html reportes/reporte_diario.pdf 2>nul";

static void intentarGenerarPDF() {
    int resultado = system(CMD_PDF);
    if (resultado == 0) {
        std::cout << "Reporte PDF generado: reportes/reporte_diario.pdf" << std::endl;
    } else {
        std::cout << "PDF no generado automaticamente. Opciones:" << std::endl;
        std::cout << "  1. wkhtmltopdf reportes/reporte_diario.html reportes/reporte_diario.pdf" << std::endl;
        std::cout << "  2. Abra el HTML en el navegador y use Ctrl+P" << std::endl;
    }
}

// Genera ambos reportes con una sola lectura del archivo binario.
// Tambien intenta generar el PDF con wkhtmltopdf si esta disponible.
void generarReportes() {
    std::map<std::string, DiaReporte> dias = cargarDiasReporte();
    escribirCSV(dias);
    escribirHTML(dias);
    intentarGenerarPDF();
}

// Genera solo el reporte CSV (lee el archivo independientemente).
void generarReporteCSV() {
    escribirCSV(cargarDiasReporte());
}

// Genera solo el reporte HTML (lee el archivo independientemente).
void generarReporteHTML() {
    escribirHTML(cargarDiasReporte());
}

// Genera el reporte PDF asegurando que el HTML este actualizado primero.
// Conversion via wkhtmltopdf; muestra instrucciones alternativas si no esta disponible.
void generarReportePDF() {
    generarReporteHTML();
    intentarGenerarPDF();
}
