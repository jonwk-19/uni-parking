#include <iostream>

int main() {
    int opcion;

    do {
        std::cout << "\n=== SISTEMA DE PARQUEO ===" << std::endl;
        std::cout << "1. Ingresar vehiculo" << std::endl;
        std::cout << "2. Retirar vehiculo" << std::endl;
        std::cout << "3. Buscar vehiculo por placa" << std::endl;
        std::cout << "4. Ver estado del parqueo" << std::endl;
        std::cout << "5. Ver cola de espera" << std::endl;
        std::cout << "6. Generar reportes" << std::endl;
        std::cout << "7. Sincronizar con MySQL" << std::endl;
        std::cout << "8. Consultar historial por placa" << std::endl;
        std::cout << "9. Salir" << std::endl;
        std::cout << "Ingrese una opcion: ";

        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Opcion invalida." << std::endl;
            continue;
        }

        switch (opcion) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                std::cout << "Funcionalidad en construccion." << std::endl;
                break;
            case 9:
                std::cout << "Saliendo del sistema..." << std::endl;
                break;
            default:
                std::cout << "Opcion invalida." << std::endl;
                break;
        }
    } while (opcion != 9);

    return 0;
}
