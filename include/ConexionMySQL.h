#ifndef CONEXIONMYSQL_H
#define CONEXIONMYSQL_H

// Sincronizacion final de datos con MySQL.
// Para habilitar MySQL: compilar con -DUSAR_MYSQL y enlazar con -lmysqlclient
// Si MySQL no esta disponible, las funciones muestran un mensaje informativo.

bool conectarMySQL();
void sincronizarResumenDiario();
void sincronizarPlacasNuevas();
void cerrarConexionMySQL();

// Wrapper que ejecuta la sincronizacion completa
void sincronizarConMySQL();

#endif
