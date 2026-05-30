#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>
#include <ctime>

// Funciones auxiliares de tiempo, precio y formato de texto.

std::string convertirFechaHora(time_t fecha);
int calcularSegundos(time_t inicio, time_t fin);
void convertirSegundosAHMS(int totalSegundos, int& horas, int& minutos, int& segundos);
int calcularHorasCobrables(int segundos);
double calcularMonto(int segundos, double tarifa);
std::string convertirAMayusculas(std::string texto);
std::string limpiarTexto(std::string texto);
bool esPlacaValida(const std::string& placa);

#endif
