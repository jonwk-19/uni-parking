#include "catch.hpp"
#include "../include/Validaciones.h"

TEST_CASE("Validaciones - Placa valida", "[validaciones]") {
    REQUIRE(Validaciones::esPlacaValida("ABC123") == true);
    REQUIRE(Validaciones::esPlacaValida("ABC-123") == true);
    REQUIRE(Validaciones::esPlacaValida("") == false);
    REQUIRE(Validaciones::esPlacaValida("ABC@123") == false);
    REQUIRE(Validaciones::esPlacaValida("A" * 21) == false);
}

TEST_CASE("Validaciones - Nombre valido", "[validaciones]") {
    REQUIRE(Validaciones::esNombreValido("Toyota") == true);
    REQUIRE(Validaciones::esNombreValido("Honda Civic") == true);
    REQUIRE(Validaciones::esNombreValido("Ford-Focus") == true);
    REQUIRE(Validaciones::esNombreValido("") == false);
    REQUIRE(Validaciones::esNombreValido("Toyota@123") == false);
}

TEST_CASE("Validaciones - Capacidad valida", "[validaciones]") {
    REQUIRE(Validaciones::esCapacidadValida(1) == true);
    REQUIRE(Validaciones::esCapacidadValida(100) == true);
    REQUIRE(Validaciones::esCapacidadValida(1000) == true);
    REQUIRE(Validaciones::esCapacidadValida(0) == false);
    REQUIRE(Validaciones::esCapacidadValida(-1) == false);
    REQUIRE(Validaciones::esCapacidadValida(1001) == false);
}

TEST_CASE("Validaciones - Tarifa valida", "[validaciones]") {
    REQUIRE(Validaciones::esTarifaValida(0.01) == true);
    REQUIRE(Validaciones::esTarifaValida(5.0) == true);
    REQUIRE(Validaciones::esTarifaValida(9999.99) == true);
    REQUIRE(Validaciones::esTarifaValida(0.0) == false);
    REQUIRE(Validaciones::esTarifaValida(-1.0) == false);
    REQUIRE(Validaciones::esTarifaValida(10000.0) == false);
}

TEST_CASE("Validaciones - Sanitizar placa", "[validaciones]") {
    REQUIRE(Validaciones::sanitizarPlaca("  abc123  ") == "ABC123");
    REQUIRE(Validaciones::sanitizarPlaca("abc-123") == "ABC-123");
    REQUIRE(Validaciones::sanitizarPlaca("  XyZ  ") == "XYZ");
}

TEST_CASE("Validaciones - Sanitizar nombre", "[validaciones]") {
    REQUIRE(Validaciones::sanitizarNombre("  Toyota  ") == "Toyota");
    REQUIRE(Validaciones::sanitizarNombre("  Honda Civic  ") == "Honda Civic");
}
