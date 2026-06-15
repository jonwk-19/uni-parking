#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>
#include <regex>
#include <cctype>
#include <algorithm>

class Validaciones {
public:
    // Valida placa: acepta formato A-Z, 0-9 y guion
    static bool esPlacaValida(const std::string& placa) {
        if (placa.empty() || placa.length() > 20) return false;
        for (char c : placa) {
            if (!std::isalnum(c) && c != '-') return false;
        }
        return true;
    }

    // Marca y modelo: solo letras, espacios y algunos caracteres
    static bool esNombreValido(const std::string& nombre) {
        if (nombre.empty() || nombre.length() > 50) return false;
        for (char c : nombre) {
            if (!std::isalnum(c) && c != ' ' && c != '-' && c != '.') return false;
        }
        return true;
    }

    // Capacidad: positivo, razonable (1-1000)
    static bool esCapacidadValida(int capacidad) {
        return capacidad > 0 && capacidad <= 1000;
    }

    // Tarifa: positiva, razonable (0.01-10000.00)
    static bool esTarifaValida(double tarifa) {
        return tarifa > 0.0 && tarifa < 10000.0;
    }

    // Sanitiza entrada de texto (trim, uppercase para placas)
    static std::string sanitizarPlaca(const std::string& placa) {
        std::string result = placa;
        // Trim espacios
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        // A mayúsculas
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    static std::string sanitizarNombre(const std::string& nombre) {
        std::string result = nombre;
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        return result;
    }
};

#endif
