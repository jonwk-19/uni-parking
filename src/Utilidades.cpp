#include "../include/Utilidades.h"
#include <cmath>
#include <algorithm>
#include <cctype>

// Convierte un time_t a string con formato "YYYY-MM-DD HH:MM:SS"
std::string convertirFechaHora(time_t fecha) {
    char buffer[20];
    struct tm* info = localtime(&fecha);
    if (!info) return "fecha-invalida";
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", info);
    return std::string(buffer);
}

// Devuelve la diferencia en segundos entre inicio y fin (minimo 0)
int calcularSegundos(time_t inicio, time_t fin) {
    int diff = (int)difftime(fin, inicio);
    return diff < 0 ? 0 : diff;
}

// Descompone totalSegundos en horas, minutos y segundos
void convertirSegundosAHMS(int totalSegundos, int& horas, int& minutos, int& segundos) {
    if (totalSegundos < 0) totalSegundos = 0;
    horas   = totalSegundos / 3600;
    minutos = (totalSegundos % 3600) / 60;
    segundos = totalSegundos % 60;
}

// Devuelve la cantidad de horas cobrables (minimo 1, redondeo hacia arriba)
int calcularHorasCobrables(int segundos) {
    if (segundos <= 0) return 1;
    return (int)ceil(segundos / 3600.0);
}

// Devuelve el monto a cobrar segun los segundos estacionado y la tarifa por hora
double calcularMonto(int segundos, double tarifa) {
    return calcularHorasCobrables(segundos) * tarifa;
}

// Devuelve el texto convertido a mayusculas
std::string convertirAMayusculas(std::string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(),
                   [](unsigned char c) { return (char)toupper(c); });
    return texto;
}

// Elimina espacios al inicio y al final del texto
std::string limpiarTexto(std::string texto) {
    // Trim leading spaces
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    // Trim trailing spaces
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}
