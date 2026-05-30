#ifndef TICKET_H
#define TICKET_H

#include "Vehiculo.h"

// Genera un ticket de salida en archivo .txt cuando un vehiculo abandona el parqueo.
void generarTicket(const Vehiculo& vehiculo, int tiempoSegundos, double tarifa, double total);

#endif
