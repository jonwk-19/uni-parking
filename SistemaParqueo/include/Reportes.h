#ifndef REPORTES_H
#define REPORTES_H

// Genera reportes diarios desde los movimientos registrados en archivos binarios.
// Los reportes se guardan en la carpeta reportes/.

// Genera ambos reportes (CSV y HTML) leyendo el archivo binario una sola vez.
// Tambien intenta generar el PDF con wkhtmltopdf si esta disponible.
void generarReportes();

// Funciones individuales (leen el archivo cada una; usar generarReportes() para eficiencia).
void generarReporteCSV();
void generarReporteHTML();

// Genera el reporte PDF convirtiendo el HTML con wkhtmltopdf.
// Si wkhtmltopdf no esta disponible, muestra instrucciones alternativas.
void generarReportePDF();

#endif
