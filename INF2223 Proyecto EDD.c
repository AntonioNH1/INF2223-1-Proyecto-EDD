#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Estructura principal que modela el Ministerio Público
struct MinisterioPublico {
    struct NodoCausa *causas;              // Lista simple de causas
    struct NodoFiscal *fiscales;           // Lista simple de fiscales
    struct NodoImputadoABB *raizImputados; // Árbol binario de búsqueda (ABB) de imputados
};

// Nodo de lista simple de causas
struct NodoCausa { // Lista simple
    struct Causa *datosCausa;             // Puntero a los datos de la causa
    struct NodoCausa *sig;               // Puntero al siguiente nodo
};

// Información asociada a una causa penal
struct Causa {
    struct CarpetaInvestigativa *carpetaInvestigativa; // Carpeta con antecedentes del caso
    char *CategoriaCausa;   // Tipo de delito o denuncia
    char *RUC;              // Rol Único de Causa (identificador)
    char *Comuna;           // Comuna donde se originó la causa
    struct Fiscal *fiscalEncargado; // Fiscal responsable de la causa
};

// Nodo de lista simple de fiscales
struct NodoFiscal { // Lista simple
    struct Fiscal *datosFiscal; // Información del fiscal
    struct NodoFiscal *sig;     // Siguiente nodo
};

// Información de un fiscal del Ministerio Público
struct Fiscal {
    int idFiscal;       // Identificador único del fiscal
    char *nombre;       // Nombre completo
    char *rut;          // RUT del fiscal
    int totalCausas;    // Número de causas asignadas
};

// Información de una carpeta investigativa (asociada a una causa)
struct CarpetaInvestigativa {
    char *estadoCaso;                   // Estado actual del caso (en investigación, cerrado, etc.)
    struct Denuncia *denuncias;        // Arreglo de denuncias asociadas
    int CantDenuncias;                 // Cantidad de denuncias en la carpeta
    struct NodoPrueba *pruebas;        // Lista circular de pruebas
    struct NodoDiligencia *diligencias; // Lista simple de diligencias realizadas
    struct NodoResolucion *resoluciones; // Lista simple de resoluciones judiciales
    struct NodoDeclaracion *declaraciones; // Lista simple de declaraciones tomadas
};

// Nodo de lista circular de pruebas
struct NodoPrueba { // Lista circular
    struct Prueba *prueba;        // Información de la prueba
    struct NodoPrueba *sig;       // Siguiente prueba en la lista
};

// Información de una prueba recolectada
struct Prueba {
    char *TipoPrueba; // Descripción: grabación, fotografía, evidencia física, etc.
};

// Nodo de lista simple de diligencias
struct NodoDiligencia { // Lista simple
    struct Diligencia *diligencia;     // Información de la diligencia
    struct NodoDiligencia *sig;        // Siguiente diligencia
};

// Información de una diligencia (acción investigativa)
struct Diligencia {
    char *descripcion; // Qué se realizó (allanamiento, citación, etc.)
    char *fecha;       // Fecha en que se realizó
    char *prioridad;   // Nivel de prioridad: alta, media, baja
};

// Nodo de lista simple de resoluciones judiciales
struct NodoResolucion { // Lista simple
    struct Resolucion *resolucion; // Información de la resolución
    struct NodoResolucion *sig;    // Siguiente resolución
};

// Información de una resolución judicial
struct Resolucion {
    char *tipo;      // Tipo de resolución: sentencia, sobreseimiento, etc.
    char *contenido; // Contenido o fundamentos
    char *fecha;     // Fecha de emisión
};

// Nodo de lista simple de declaraciones
struct NodoDeclaracion { // Lista simple
    struct Declaracion *declaracion; // Información de la declaración
    struct NodoDeclaracion *sig;     // Siguiente declaración
};

// Declaración realizada por un testigo, víctima o imputado
struct Declaracion {
    char *tipoDeclarante; // Tipo de declarante: víctima, testigo, imputado
    char *nombre;         // Nombre del declarante
    char *contenido;      // Contenido de la declaración
    char *fecha;          // Fecha en que se tomó la declaración
};

// Estructura de árbol binario de búsqueda de imputados (ABB)
struct NodoImputadoABB { // Árbol binario de búsqueda
    struct Imputado *datosImputado; // Datos del imputado
    struct NodoImputadoABB *izq;    // Subárbol izquierdo
    struct NodoImputadoABB *der;    // Subárbol derecho
};

// Información del imputado en una causa
struct Imputado {
    char *nombre;              // Nombre completo
    char *rut;                 // RUT del imputado (clave para el ABB)
    char *estadoProcesal;      // Estado del imputado: formalizado, condenado, etc.
    char *medidasCautelares;   // Medidas cautelares impuestas
    struct NodoDeclaracion *declaraciones; // Lista de declaraciones del imputado
};

// Información de una denuncia
struct Denuncia {
    char *PersonaDenunciante; // Nombre de quien realiza la denuncia
    char *RutDenunciante;     // RUT del denunciante
    char *FechaDenuncia;      // Fecha en que se realiza la denuncia
};
