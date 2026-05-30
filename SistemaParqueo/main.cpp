#include <iostream>
#include "include/Parqueo.h"
#include "include/ArchivoBinario.h"
#include "include/Reportes.h"
#include "include/ConexionMySQL.h"

using namespace std;

int main() {
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

        switch(opcion) {
            case 1: cout << "Funcionalidad en construccion." << endl; break;
            case 2: cout << "Funcionalidad en construccion." << endl; break;
            case 3: cout << "Funcionalidad en construccion." << endl; break;
            case 4: cout << "Funcionalidad en construccion." << endl; break;
            case 5: cout << "Funcionalidad en construccion." << endl; break;
            case 6: cout << "Funcionalidad en construccion." << endl; break;
            case 7: cout << "Funcionalidad en construccion." << endl; break;
            case 8: cout << "Funcionalidad en construccion." << endl; break;
            case 9: cout << "Saliendo del sistema." << endl; break;
            default: cout << "Opcion invalida." << endl; break;
        }
    } while(opcion != 9);

    return 0;
}
