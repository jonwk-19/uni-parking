#include "../include/ConexionMySQL.h"
#include "../include/ArchivoBinario.h"
#include "../include/Movimiento.h"
#include "../include/Utilidades.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdio>

#ifdef USAR_MYSQL
#include <mysql/mysql.h>
static MYSQL* conexion = nullptr;
#endif

// Credenciales de conexion (ajustar segun entorno)
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS ""
#define DB_NAME "sistema_parqueo"
#define DB_PORT 3306

bool conectarMySQL() {
#ifdef USAR_MYSQL
    conexion = mysql_init(nullptr);
    if (!conexion) {
        std::cerr << "Error: no se pudo inicializar MySQL." << std::endl;
        return false;
    }
    if (!mysql_real_connect(conexion, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, nullptr, 0)) {
        std::cerr << "Error de conexion MySQL: " << mysql_error(conexion) << std::endl;
        mysql_close(conexion);
        conexion = nullptr;
        return false;
    }
    std::cout << "Conexion a MySQL exitosa." << std::endl;
    return true;
#else
    std::cout << "MySQL no disponible. Compile con -DUSAR_MYSQL para habilitar." << std::endl;
    return false;
#endif
}

void sincronizarResumenDiario() {
#ifdef USAR_MYSQL
    if (!conexion) {
        std::cerr << "No hay conexion activa con MySQL." << std::endl;
        return;
    }

    // Calcular resumen del dia actual
    std::vector<Movimiento> movimientos = leerMovimientos();
    time_t ahora = time(nullptr);
    std::string hoy = convertirFechaHora(ahora).substr(0, 10);

    int totalVehiculos = 0;
    double montoTotal = 0.0;
    int totalSegundos = 0;

    for (const Movimiento& m : movimientos) {
        if (strcmp(m.tipoMovimiento, MOV_SALIDA) != 0) continue;
        std::string fecha = convertirFechaHora(m.fechaHora).substr(0, 10);
        if (fecha != hoy) continue;
        totalVehiculos++;
        montoTotal += m.montoCobrado;
        totalSegundos += m.tiempoSegundos;
    }

    int promedioSegundos = totalVehiculos > 0 ? totalSegundos / totalVehiculos : 0;

    // INSERT OR UPDATE resumen diario
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO resumen_diario (fecha, total_vehiculos, monto_total, promedio_tiempo_segundos) "
        "VALUES ('%s', %d, %.2f, %d) "
        "ON DUPLICATE KEY UPDATE total_vehiculos=%d, monto_total=%.2f, promedio_tiempo_segundos=%d",
        hoy.c_str(), totalVehiculos, montoTotal, promedioSegundos,
        totalVehiculos, montoTotal, promedioSegundos);

    if (mysql_query(conexion, query)) {
        std::cerr << "Error al insertar resumen: " << mysql_error(conexion) << std::endl;
    } else {
        std::cout << "Resumen diario sincronizado para " << hoy << "." << std::endl;
    }
#else
    std::cout << "Sincronizacion de resumen diario no disponible (MySQL no compilado)." << std::endl;
#endif
}

void sincronizarPlacasNuevas() {
#ifdef USAR_MYSQL
    if (!conexion) {
        std::cerr << "No hay conexion activa con MySQL." << std::endl;
        return;
    }

    // Leer placas del archivo binario
    FILE* fp = fopen(ARCHIVO_PLACAS, "rb");
    if (!fp) {
        std::cout << "No hay placas registradas para sincronizar." << std::endl;
        return;
    }

    char placa[20];
    int insertadas = 0;
    while (fread(placa, sizeof(char[20]), 1, fp) == 1) {
        placa[19] = '\0'; // garantizar null-terminator
        char placaEscapada[41]; // 2*20 + 1 segun mysql_real_escape_string
        mysql_real_escape_string(conexion, placaEscapada, placa, (unsigned long)strlen(placa));
        char query[256];
        snprintf(query, sizeof(query),
            "INSERT IGNORE INTO vehiculos (placa) VALUES ('%s')", placaEscapada);
        if (mysql_query(conexion, query) == 0) {
            if (mysql_affected_rows(conexion) > 0) insertadas++;
        }
    }
    fclose(fp);
    std::cout << insertadas << " placa(s) nueva(s) sincronizada(s)." << std::endl;
#else
    std::cout << "Sincronizacion de placas no disponible (MySQL no compilado)." << std::endl;
#endif
}

void cerrarConexionMySQL() {
#ifdef USAR_MYSQL
    if (conexion) {
        mysql_close(conexion);
        conexion = nullptr;
        std::cout << "Conexion MySQL cerrada." << std::endl;
    }
#endif
}

void sincronizarConMySQL() {
    std::cout << "=== SINCRONIZACION CON MYSQL ===" << std::endl;
    if (!conectarMySQL()) {
        std::cout << "No se pudo conectar. Verifique que MySQL este corriendo y las credenciales sean correctas." << std::endl;
        return;
    }
    sincronizarResumenDiario();
    sincronizarPlacasNuevas();
    cerrarConexionMySQL();
    // Nota: cada funcion reporta su propio exito o error individualmente.
    std::cout << "Proceso de sincronizacion finalizado." << std::endl;
}
