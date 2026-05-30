#include <iostream>
#include <string>
#include <limits>
#include "include/Parqueo.h"
#include "include/ArchivoBinario.h"
#include "include/Utilidades.h"
#include "include/Reportes.h"
#include "include/ConexionMySQL.h"

using namespace std;

static int leerEnteroPositivo(const string& mensaje) {
    int valor = 0;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (!cin.fail() && valor > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Valor invalido. Intente de nuevo." << endl;
    }
}

static double leerDoublePositivo(const string& mensaje) {
    double valor = 0.0;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (!cin.fail() && valor > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Valor invalido. Intente de nuevo." << endl;
    }
}

static bool leerPlacaValida(string& placa) {
    cout << "Placa: ";
    getline(cin, placa);
    placa = convertirAMayusculas(limpiarTexto(placa));
    if (!esPlacaValida(placa)) {
        cout << "Placa invalida. Use solo A-Z, 0-9 y guion (-)." << endl;
        return false;
    }
    return true;
}

int main() {
    double tarifa = cargarTarifa();
    if (tarifa <= 0) {
        cout << "No se encontro tarifa valida previa." << endl;
        tarifa = leerDoublePositivo("Ingrese la tarifa por hora (mayor a 0): ");
        guardarTarifa(tarifa);
    } else {
        cout << "Tarifa anterior: $" << tarifa << " por hora." << endl;
        cout << "Desea cambiarla? (s/n): ";
        char respuesta = 'n';
        cin >> respuesta;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (respuesta == 's' || respuesta == 'S') {
            tarifa = leerDoublePositivo("Ingrese la tarifa por hora (mayor a 0): ");
            guardarTarifa(tarifa);
        }
    }

    int cantCarriles = leerEnteroPositivo("Numero de carriles: ");
    int capPorCarril = leerEnteroPositivo("Capacidad por carril: ");
    Parqueo parqueo(cantCarriles, capPorCarril, tarifa);

    int opcion = 0;
    do {
        cout << "\n=== SISTEMA DE PARQUEO ===" << endl;
        cout << "1. Ingresar vehiculo" << endl;
        cout << "2. Retirar vehiculo" << endl;
        cout << "3. Buscar vehiculo por placa" << endl;
        cout << "4. Ver estado del parqueo" << endl;
        cout << "5. Ver cola de espera" << endl;
        cout << "6. Generar reportes" << endl;
        cout << "7. Sincronizar con MySQL" << endl;
        cout << "8. Consultar historial por placa" << endl;
        cout << "9. Salir" << endl;
        opcion = leerEnteroPositivo("Ingrese opcion: ");

        switch(opcion) {
            case 1: {
                string placa, marca, modelo;
                if (!leerPlacaValida(placa)) break;
                cout << "Marca: ";
                getline(cin, marca);
                marca = limpiarTexto(marca);
                cout << "Modelo: ";
                getline(cin, modelo);
                modelo = limpiarTexto(modelo);
                if (marca.empty() || modelo.empty()) {
                    cout << "Marca y modelo son obligatorios." << endl;
                    break;
                }
                Vehiculo v(placa.c_str(), marca.c_str(), modelo.c_str());
                parqueo.ingresarVehiculo(v);
                break;
            }
            case 2: {
                string placa;
                if (!leerPlacaValida(placa)) break;
                parqueo.retirarVehiculo(placa);
                break;
            }
            case 3: {
                string placa;
                if (!leerPlacaValida(placa)) break;
                parqueo.buscarVehiculo(placa);
                break;
            }
            case 4:
                parqueo.mostrarEstado();
                break;
            case 5:
                parqueo.mostrarColaEspera();
                break;
            case 6:
                generarReportes();
                break;
            case 7:
                sincronizarConMySQL();
                break;
            case 8: {
                string placa;
                if (!leerPlacaValida(placa)) break;
                mostrarHistorialPlaca(placa);
                break;
            }
            case 9:
                cout << "Saliendo del sistema." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
                break;
        }
    } while(opcion != 9);

    return 0;
}
