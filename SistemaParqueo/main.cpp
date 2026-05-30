#include <iostream>
#include <string>
#include "include/Parqueo.h"
#include "include/ArchivoBinario.h"
#include "include/Utilidades.h"
#include "include/Reportes.h"
#include "include/ConexionMySQL.h"

using namespace std;

int main() {
    // 1. Cargar tarifa guardada; si no existe o es invalida, pedirla al usuario
    double tarifa = cargarTarifa();
    auto pedirTarifa = [&]() {
        do {
            cout << "Ingrese la tarifa por hora (mayor a 0): ";
            cin >> tarifa;
        } while (tarifa <= 0);
        guardarTarifa(tarifa);
    };

    if (tarifa <= 0) {
        if (tarifa < 0) cout << "No se encontro tarifa previa." << endl;
        else cout << "Tarifa guardada invalida. Ingrese una nueva." << endl;
        pedirTarifa();
    } else {
        cout << "Tarifa anterior: $" << tarifa << " por hora." << endl;
        cout << "Desea cambiarla? (s/n): ";
        char respuesta;
        cin >> respuesta;
        if (respuesta == 's' || respuesta == 'S') {
            pedirTarifa();
        }
    }

    // 2. Configurar numero de carriles y capacidad
    int cantCarriles, capPorCarril;
    cout << "Numero de carriles: ";
    cin >> cantCarriles;
    cout << "Capacidad por carril: ";
    cin >> capPorCarril;

    Parqueo parqueo(cantCarriles, capPorCarril, tarifa);

    // 3. Menu principal
    int opcion;
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
        cout << "Ingrese opcion: ";
        cin >> opcion;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            opcion = 0;
        }

        switch(opcion) {
            case 1: {
                string placa, marca, modelo;
                cout << "Placa: ";
                cin.ignore();
                getline(cin, placa);
                cout << "Marca: ";
                getline(cin, marca);
                cout << "Modelo: ";
                getline(cin, modelo);
                placa = convertirAMayusculas(placa);
                Vehiculo v(placa.c_str(), marca.c_str(), modelo.c_str());
                parqueo.ingresarVehiculo(v);
                break;
            }
            case 2: {
                string placa;
                cout << "Placa a retirar: ";
                cin.ignore();
                getline(cin, placa);
                placa = convertirAMayusculas(placa);
                parqueo.retirarVehiculo(placa);
                break;
            }
            case 3: {
                string placa;
                cout << "Placa a buscar: ";
                cin.ignore();
                getline(cin, placa);
                placa = convertirAMayusculas(placa);
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
            case 8:
                cout << "Funcionalidad de historial en construccion." << endl;
                break;
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
