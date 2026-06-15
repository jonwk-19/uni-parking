#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/Parqueo.h"
#include "../include/Vehiculo.h"

TEST_CASE("Parqueo - Ingreso y capacidad", "[parqueo]") {
    Parqueo parqueo(2, 2, 5.0);

    REQUIRE(parqueo.hayEspacio() == true);
    REQUIRE(parqueo.vehiculosEnParqueo() == 0);
    REQUIRE(parqueo.capacidadTotal() == 4);
}

TEST_CASE("Parqueo - Ingreso de vehiculos", "[parqueo]") {
    Parqueo parqueo(1, 2, 5.0);

    Vehiculo v1("ABC123", "Toyota", "Corolla");
    Vehiculo v2("XYZ789", "Honda", "Civic");

    parqueo.ingresarVehiculo(v1);
    REQUIRE(parqueo.vehiculosEnParqueo() == 1);

    parqueo.ingresarVehiculo(v2);
    REQUIRE(parqueo.vehiculosEnParqueo() == 2);

    REQUIRE(parqueo.hayEspacio() == false);
}

TEST_CASE("Parqueo - FIFO queue cuando esta lleno", "[parqueo]") {
    Parqueo parqueo(1, 1, 5.0);

    Vehiculo v1("A001", "Toyota", "Corolla");
    Vehiculo v2("A002", "Honda", "Civic");
    Vehiculo v3("A003", "Ford", "Focus");

    parqueo.ingresarVehiculo(v1);
    REQUIRE(parqueo.hayEspacio() == true);

    parqueo.ingresarVehiculo(v2);
    REQUIRE(parqueo.hayEspacio() == false);

    parqueo.ingresarVehiculo(v3);
    REQUIRE(parqueo.vehiculosEnParqueo() == 2);
}

TEST_CASE("Parqueo - Tarifa", "[parqueo]") {
    Parqueo parqueo(1, 1, 5.0);

    REQUIRE(parqueo.getTarifa() == 5.0);

    parqueo.setTarifa(7.5);
    REQUIRE(parqueo.getTarifa() == 7.5);
}
