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
#include <cstdlib>

#ifdef USAR_MYSQL
#include <mysql/mysql.h>
static MYSQL* conexion = nullptr;
static bool huboErrorSync = false;
#endif

#define DB_HOST_DEFAULT "localhost"
#define DB_USER_DEFAULT "root"
#define DB_PASS_DEFAULT ""
#define DB_NAME_DEFAULT "sistema_parqueo"
#define DB_PORT_DEFAULT 3306

#ifdef USAR_MYSQL
static std::string envOrDefault(const char* key, const char* fallback) {
    const char* val = std::getenv(key);
    return (val && *val) ? std::string(val) : std::string(fallback);
}

static int envPortOrDefault(const char* key, int fallback) {
    const char* val = std::getenv(key);
    if (!val || !*val) return fallback;
    int parsed = std::atoi(val);
    return parsed > 0 ? parsed : fallback;
}
#endif

bool conectarMySQL() {
#ifdef USAR_MYSQL
    std::string host = envOrDefault("PARQUEO_DB_HOST", DB_HOST_DEFAULT);
    std::string user = envOrDefault("PARQUEO_DB_USER", DB_USER_DEFAULT);
    std::string pass = envOrDefault("PARQUEO_DB_PASS", DB_PASS_DEFAULT);
    std::string name = envOrDefault("PARQUEO_DB_NAME", DB_NAME_DEFAULT);
    int port = envPortOrDefault("PARQUEO_DB_PORT", DB_PORT_DEFAULT);

    conexion = mysql_init(nullptr);
    if (!conexion) {
        std::cerr << "Error: no se pudo inicializar MySQL." << std::endl;
        return false;
    }
    if (!mysql_real_connect(conexion, host.c_str(), user.c_str(), pass.c_str(), name.c_str(), port, nullptr, 0)) {
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

#ifdef USAR_MYSQL
static bool ejecutarDDL(const char* query, const char* contexto) {
    if (mysql_query(conexion, query) != 0) {
        std::cerr << "Error en " << contexto << ": " << mysql_error(conexion) << std::endl;
        return false;
    }
    return true;
}

static bool consultaRetornaFilas(const std::string& query, bool& existe) {
    existe = false;
    if (mysql_query(conexion, query.c_str()) != 0) {
        std::cerr << "Error ejecutando consulta de esquema: " << mysql_error(conexion) << std::endl;
        return false;
    }
    MYSQL_RES* res = mysql_store_result(conexion);
    if (!res) return false;
    existe = mysql_num_rows(res) > 0;
    mysql_free_result(res);
    return true;
}

static bool asegurarEsquemaSync() {
    if (!conexion) return false;

    bool columnaExiste = false;
    if (!consultaRetornaFilas(
            "SELECT 1 FROM information_schema.COLUMNS "
            "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'movimientos' AND COLUMN_NAME = 'sync_uid' "
            "LIMIT 1",
            columnaExiste)) {
        return false;
    }

    if (!columnaExiste) {
        if (!ejecutarDDL(
                "ALTER TABLE movimientos ADD COLUMN sync_uid VARCHAR(64) NULL",
                "migracion sync_uid")) {
            return false;
        }
    }

    // Backfill seguro para filas legacy. Incluye id para evitar colisiones de duplicados historicos.
    if (!ejecutarDDL(
            "UPDATE movimientos "
            "SET sync_uid = SHA2(CONCAT('legacy|', id, '|', COALESCE(placa,''), '|', COALESCE(tipo_movimiento,''), '|', "
            "COALESCE(DATE_FORMAT(fecha_hora, '%Y-%m-%d %H:%i:%s'), ''), '|', "
            "COALESCE(CAST(tiempo_segundos AS CHAR), ''), '|', COALESCE(CAST(monto_cobrado AS CHAR), '')), 256) "
            "WHERE sync_uid IS NULL OR sync_uid = ''",
            "backfill sync_uid")) {
        return false;
    }

    bool indiceExiste = false;
    if (!consultaRetornaFilas(
            "SELECT 1 FROM information_schema.STATISTICS "
            "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'movimientos' AND INDEX_NAME = 'uq_movimiento_sync_uid' "
            "LIMIT 1",
            indiceExiste)) {
        return false;
    }
    if (!indiceExiste) {
        if (!ejecutarDDL(
                "CREATE UNIQUE INDEX uq_movimiento_sync_uid ON movimientos(sync_uid)",
                "indice uq_movimiento_sync_uid")) {
            return false;
        }
    }

    if (!ejecutarDDL(
            "ALTER TABLE movimientos MODIFY COLUMN sync_uid VARCHAR(64) NOT NULL",
            "sync_uid not null")) {
        return false;
    }
    return true;
}
#endif

void sincronizarResumenDiario() {
#ifdef USAR_MYSQL
    if (!conexion) {
        std::cerr << "No hay conexion activa con MySQL." << std::endl;
        return;
    }

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
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO resumen_diario (fecha, total_vehiculos, monto_total, promedio_tiempo_segundos) "
        "VALUES ('%s', %d, %.2f, %d) "
        "ON DUPLICATE KEY UPDATE total_vehiculos=%d, monto_total=%.2f, promedio_tiempo_segundos=%d",
        hoy.c_str(), totalVehiculos, montoTotal, promedioSegundos,
        totalVehiculos, montoTotal, promedioSegundos);

    if (mysql_query(conexion, query)) {
        std::cerr << "Error al insertar resumen: " << mysql_error(conexion) << std::endl;
        huboErrorSync = true;
    } else {
        std::cout << "Resumen diario sincronizado para " << hoy << "." << std::endl;
    }
#else
    std::cout << "Sincronizacion de resumen diario no disponible (MySQL no compilado)." << std::endl;
#endif
}

static void sincronizarMovimientos() {
#ifdef USAR_MYSQL
    if (!conexion) return;
    std::vector<Movimiento> movimientos = leerMovimientos();
    int insertados = 0;

    for (const Movimiento& m : movimientos) {
        std::string fechaHora = convertirFechaHora(m.fechaHora);
        char placaEsc[41];
        char tipoEsc[81];
        mysql_real_escape_string(conexion, placaEsc, m.placa, (unsigned long)strlen(m.placa));
        mysql_real_escape_string(conexion, tipoEsc, m.tipoMovimiento, (unsigned long)strlen(m.tipoMovimiento));

        char query[1024];
        snprintf(query, sizeof(query),
            "INSERT IGNORE INTO movimientos "
            "(sync_uid, placa, tipo_movimiento, fecha_hora, monto_cobrado, tiempo_segundos) "
            "VALUES (SHA2(CONCAT('%s','|','%s','|','%s','|',%d,'|',%.2f), 256), '%s', '%s', '%s', %.2f, %d)",
            placaEsc, tipoEsc, fechaHora.c_str(), m.tiempoSegundos, m.montoCobrado,
            placaEsc, tipoEsc, fechaHora.c_str(), m.montoCobrado, m.tiempoSegundos);

        if (mysql_query(conexion, query) == 0) {
            if (mysql_affected_rows(conexion) > 0) insertados++;
        } else {
            std::cerr << "Error sincronizando movimiento de placa " << m.placa
                      << ": " << mysql_error(conexion) << std::endl;
            huboErrorSync = true;
        }
    }
    std::cout << insertados << " movimiento(s) nuevo(s) sincronizado(s)." << std::endl;
#endif
}

void sincronizarPlacasNuevas() {
#ifdef USAR_MYSQL
    if (!conexion) {
        std::cerr << "No hay conexion activa con MySQL." << std::endl;
        return;
    }

    FILE* fp = fopen(ARCHIVO_PLACAS, "rb");
    if (!fp) {
        std::cout << "No hay placas registradas para sincronizar." << std::endl;
        return;
    }

    char placa[20];
    int insertadas = 0;
    while (fread(placa, sizeof(char[20]), 1, fp) == 1) {
        placa[19] = '\0';
        char placaEscapada[41];
        mysql_real_escape_string(conexion, placaEscapada, placa, (unsigned long)strlen(placa));
        char query[256];
        snprintf(query, sizeof(query), "INSERT IGNORE INTO vehiculos (placa) VALUES ('%s')", placaEscapada);
        if (mysql_query(conexion, query) == 0) {
            if (mysql_affected_rows(conexion) > 0) insertadas++;
        } else {
            std::cerr << "Error sincronizando placa " << placa << ": " << mysql_error(conexion) << std::endl;
            huboErrorSync = true;
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
        std::cout << "No se pudo conectar. Verifique conexion/credenciales y variables PARQUEO_DB_*." << std::endl;
        return;
    }

    bool transaccionOk = true;
#ifdef USAR_MYSQL
    huboErrorSync = false;
    if (!asegurarEsquemaSync()) {
        cerrarConexionMySQL();
        std::cerr << "No se pudo garantizar esquema de sincronizacion." << std::endl;
        return;
    }
    if (mysql_query(conexion, "START TRANSACTION") != 0) {
        std::cerr << "No se pudo iniciar transaccion: " << mysql_error(conexion) << std::endl;
        transaccionOk = false;
    }
#endif

    if (transaccionOk) sincronizarMovimientos();
    if (transaccionOk) sincronizarResumenDiario();
    if (transaccionOk) sincronizarPlacasNuevas();

#ifdef USAR_MYSQL
    bool syncOk = transaccionOk && !huboErrorSync;
    if (syncOk) {
        if (mysql_query(conexion, "COMMIT") != 0) {
            std::cerr << "Error al confirmar transaccion: " << mysql_error(conexion) << std::endl;
            mysql_query(conexion, "ROLLBACK");
            syncOk = false;
        }
    } else {
        mysql_query(conexion, "ROLLBACK");
    }
    transaccionOk = syncOk;
#endif

    if (!transaccionOk) {
        std::cerr << "Sincronizacion finalizo con errores y se aplico rollback cuando fue posible." << std::endl;
        std::cout << "Proceso de sincronizacion FALLIDO." << std::endl;
    } else {
        std::cout << "Proceso de sincronizacion EXITOSO." << std::endl;
    }
    cerrarConexionMySQL();
}
