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

struct Causa* crearCausa(const char *categoria, const char *ruc, const char *comuna) {
    struct Causa *nuevaCausa;

    nuevaCausa = (struct Causa*) malloc(sizeof(struct Causa));
    nuevaCausa->CategoriaCausa = strdup(categoria);
    nuevaCausa->RUC = strdup(ruc);
    nuevaCausa->Comuna = strdup(comuna);
    nuevaCausa->carpetaInvestigativa = NULL;
    nuevaCausa->fiscalEncargado = NULL;

    return nuevaCausa;
}

void agregarCausa(struct MinisterioPublico *ministerio, struct Causa *causa) {
    struct NodoCausa *nuevoNodo;

    nuevoNodo = (struct NodoCausa*) malloc(sizeof(struct NodoCausa));
    nuevoNodo->datosCausa = causa;
    nuevoNodo->sig = ministerio->causas; // CAMBIADO
    ministerio->causas = nuevoNodo;      // CAMBIADO
}

void mostrarCausas(struct MinisterioPublico *ministerio) {
    struct NodoCausa *nodoActual;
    struct Causa *causaActual;

    nodoActual = ministerio->causas; // CAMBIADO

    printf("\nListado de causas:\n");
    while (nodoActual != NULL) {
        causaActual = nodoActual->datosCausa;
        printf("RUC: %s | Categoria: %s | Comuna: %s\n",
               causaActual->RUC, causaActual->CategoriaCausa, causaActual->Comuna);
        nodoActual = nodoActual->sig;
    }
}

struct Fiscal* crearFiscal(int id, const char *nombre, const char *rut) {
    struct Fiscal *nuevoFiscal;

    nuevoFiscal = (struct Fiscal*) malloc(sizeof(struct Fiscal));
    nuevoFiscal->idFiscal = id;
    nuevoFiscal->nombre = strdup(nombre);
    nuevoFiscal->rut = strdup(rut);
    nuevoFiscal->totalCausas = 0;

    return nuevoFiscal;
}

void agregarFiscal(struct MinisterioPublico *ministerio, struct Fiscal *fiscal) {
    struct NodoFiscal *nuevoNodo;

    nuevoNodo = (struct NodoFiscal*) malloc(sizeof(struct NodoFiscal));
    nuevoNodo->datosFiscal = fiscal;
    nuevoNodo->sig = ministerio->fiscales; // CAMBIADO
    ministerio->fiscales = nuevoNodo;      // CAMBIADO
}

void mostrarFiscales(struct MinisterioPublico *ministerio) {
    struct NodoFiscal *nodoActual;
    struct Fiscal *fiscalActual;

    nodoActual = ministerio->fiscales; // CAMBIADO

    printf("\nListado de fiscales:\n");
    while (nodoActual != NULL) {
        fiscalActual = nodoActual->datosFiscal;
        printf("ID: %d | Nombre: %s | RUT: %s | Total causas: %d\n",
               fiscalActual->idFiscal, fiscalActual->nombre,
               fiscalActual->rut, fiscalActual->totalCausas);
        nodoActual = nodoActual->sig;
    }
}

int menuMinisterioPublico() {
    struct MinisterioPublico ministerio;
    int opcion;

    ministerio.causas = NULL;
    ministerio.fiscales = NULL;
    ministerio.raizImputados = NULL;

    do {
        printf("\n--- MENU MINISTERIO PUBLICO ---\n");
        printf("1. Agregar causa\n");
        printf("2. Mostrar causas\n");
        printf("3. Agregar fiscal\n");
        printf("4. Mostrar fiscales\n");
        printf("5. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);
        getchar(); // limpiar buffer

        if (opcion == 1) {
            char categoriaIngresada[100];
            char rucIngresado[20];
            char comunaIngresada[50];
            struct Causa *nuevaCausa;

            printf("RUC: ");
            fgets(rucIngresado, sizeof(rucIngresado), stdin);
            rucIngresado[strcspn(rucIngresado, "\n")] = '\0';

            printf("Categoria de la causa: ");
            fgets(categoriaIngresada, sizeof(categoriaIngresada), stdin);
            categoriaIngresada[strcspn(categoriaIngresada, "\n")] = '\0';

            printf("Comuna: ");
            fgets(comunaIngresada, sizeof(comunaIngresada), stdin);
            comunaIngresada[strcspn(comunaIngresada, "\n")] = '\0';

            nuevaCausa = crearCausa(categoriaIngresada, rucIngresado, comunaIngresada);
            agregarCausa(&ministerio, nuevaCausa);

            printf("Causa agregada correctamente.\n");

        } else if (opcion == 2) {
            mostrarCausas(&ministerio);

        } else if (opcion == 3) {
            int idIngresado;
            char nombreIngresado[100];
            char rutIngresado[20];
            struct Fiscal *nuevoFiscal;

            printf("ID Fiscal: ");
            scanf("%d", &idIngresado);
            getchar();

            printf("Nombre: ");
            fgets(nombreIngresado, sizeof(nombreIngresado), stdin);
            nombreIngresado[strcspn(nombreIngresado, "\n")] = '\0';

            printf("RUT: ");
            fgets(rutIngresado, sizeof(rutIngresado), stdin);
            rutIngresado[strcspn(rutIngresado, "\n")] = '\0';

            nuevoFiscal = crearFiscal(idIngresado, nombreIngresado, rutIngresado);
            agregarFiscal(&ministerio, nuevoFiscal);

            printf("Fiscal agregado correctamente.\n");

        } else if (opcion == 4) {
            mostrarFiscales(&ministerio);
        }

    } while (opcion != 5);

    return 0;
}



int main() {
    int resultado;
    resultado = menuMinisterioPublico();
    return 0;
}
