# Validacion Funcional y Matriz de Trazabilidad (a-h)

## Checklist de regresion rapida

- [ ] Compila sin errores (`-std=c++17 -Wall`).
- [ ] Ingreso hasta llenar capacidad y encolado FIFO.
- [ ] Retiro de vehiculo bloqueado con movimientos temporales y restauracion.
- [ ] Cobro por hora/fraccion (minimo 1 hora).
- [ ] Reinicio del programa conserva carriles y cola.
- [ ] Reportes CSV/HTML/PDF coherentes entre si.
- [ ] Sincronizacion MySQL ejecutada 2 veces sin duplicados.

## Casos de prueba minimos

1. **Parqueo lleno + cola FIFO**
- Configurar 1 carril, capacidad 2.
- Ingresar A, B, C.
- Esperado: A y B estacionados, C en cola posicion 1.

2. **Retiro intermedio bloqueado**
- En el carril (top->bottom): C sobre B sobre A.
- Retirar A.
- Esperado: mover C y B temporalmente, retirar A, regresar B y C en orden original relativo.

3. **Persistencia operativa**
- Con vehiculos en carriles y cola, cerrar programa.
- Reabrir programa con misma config.
- Esperado: estado restaurado exactamente.

4. **Cobro por fraccion**
- Retirar vehiculo con permanencia menor a 1 hora.
- Esperado: 1 hora cobrada.
- Retirar con 1h + segundos.
- Esperado: 2 horas cobradas.

5. **Sincronizacion idempotente**
- Ejecutar sincronizacion MySQL.
- Ejecutarla de nuevo sin nuevos movimientos.
- Esperado: no duplicar registros en `movimientos`, ni placas, ni resumen inconsistente.

6. **Reportes consistentes**
- Generar reportes con movimientos de salida.
- Verificar que CSV/HTML/PDF tengan:
  - total vehiculos por dia,
  - total recaudado por dia,
  - promedio tiempo en `HH:MM:SS` y segundos.

## Trazabilidad requerimiento -> evidencia

- **a) Organizacion de espacios (LIFO + desbloqueo + cola)**  
  Evidencia: `Parqueo` usa `stack` y `queue`, retiro con auxiliar y reingreso desde cola.

- **b) Entidad vehiculo agrupada**  
  Evidencia: `struct Vehiculo` con placa, marca, modelo, horaEntrada, horaSalida.

- **c) Persistencia en tiempo real binaria**  
  Evidencia: `guardarMovimiento(...)` en cada evento + archivos `*.dat`.

- **d) Busqueda por placa y tiempo**  
  Evidencia: `buscarVehiculo(...)` muestra tiempo `h/m/s`.

- **e) Calculo de precio y tarifa persistida**  
  Evidencia: `calcularHorasCobrables` + `calcularMonto` + `cargarTarifa/guardarTarifa`.

- **f) Sincronizacion con MySQL**  
  Evidencia: sync de `movimientos`, `resumen_diario`, `vehiculos` (placas nuevas) con idempotencia.

- **g) Reportes CSV/HTML/PDF**  
  Evidencia: `generarReportes()` produce tres formatos con mismos KPIs.

- **h) Manual de usuario**  
  Evidencia: `manual_usuario.txt`.
