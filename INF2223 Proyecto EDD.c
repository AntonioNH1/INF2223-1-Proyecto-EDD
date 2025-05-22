#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DENUNCIAS 1000


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
    struct NodoImputadoABB *imputadoAsociado; //referencia al imputado del caso
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

// Información de una denuncia
struct Denuncia {
    char *PersonaDenunciante; // Nombre de quien realiza la denuncia
    char *RutDenunciante;     // RUT del denunciante
    char *FechaDenuncia;      // Fecha en que se realiza la denuncia
};


// Información de una carpeta investigativa (asociada a una causa)
struct CarpetaInvestigativa {
    char *estadoCaso;                   // Estado actual del caso (en investigación, cerrado, etc.)
    struct Denuncia denuncias[MAX_DENUNCIAS]; // Arreglo de denuncias asociadas
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
    char *contenido; // Texto completo de la resolucion final
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

void leerCadena(char *buffer, int tam) {
    fgets(buffer, tam, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar salto de línea
}

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
    nuevoNodo->sig = ministerio->causas;
    ministerio->causas = nuevoNodo;
}

void mostrarCausas(struct MinisterioPublico *ministerio) {
    struct NodoCausa *nodoActual;
    struct Causa *causaActual;

    nodoActual = ministerio->causas;

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

struct CarpetaInvestigativa *crearCarpetaInvestigativa(const char *estado) {
    struct CarpetaInvestigativa *nueva;

    nueva = (struct CarpetaInvestigativa *) malloc(sizeof(struct CarpetaInvestigativa));
    nueva->estadoCaso = strdup(estado);
    nueva->CantDenuncias = 0;
    nueva->pruebas = NULL;
    nueva->diligencias = NULL;
    nueva->resoluciones = NULL;
    nueva->declaraciones = NULL;

    return nueva;
}



struct Denuncia crearDenuncia(const char *nombre, const char *rut, const char *fecha) {
    struct Denuncia nueva;

    nueva.PersonaDenunciante = strdup(nombre);
    nueva.RutDenunciante = strdup(rut);
    nueva.FechaDenuncia = strdup(fecha);

    return nueva;
}


void agregarDenunciaACarpeta(struct CarpetaInvestigativa *carpeta, struct Denuncia nuevaDenuncia) {
    if (carpeta->CantDenuncias >= MAX_DENUNCIAS) {
        printf("No se pueden agregar mas denuncias: se alcanzo el maximo de %d.\n", MAX_DENUNCIAS);
        return;
    }

    carpeta->denuncias[carpeta->CantDenuncias] = nuevaDenuncia;
    carpeta->CantDenuncias++;

    printf(" Denuncia agregada correctamente a la carpeta investigativa.\n");
}

void mostrarDenuncias(struct CarpetaInvestigativa *carpeta) {
    // Validar si la carpeta existe o si no hay denuncias
    if (carpeta == NULL || carpeta->CantDenuncias == 0) {
        printf("No hay denuncias registradas en la carpeta.\n");
        return;
    }

    printf("\n--- DENUNCIAS REGISTRADAS ---\n");

    int i;
    for (i = 0; i < carpeta->CantDenuncias; i++) {
        struct Denuncia d = carpeta->denuncias[i];  // acceder a la denuncia i
        printf("Denunciante: %s | RUT: %s | Fecha: %s\n", d.PersonaDenunciante, d.RutDenunciante, d.FechaDenuncia);
    }
}

struct Diligencia* crearDiligencia(const char *descripcion, const char *fecha, const char *prioridad) {
    struct Diligencia *nuevaDiligencia;

    nuevaDiligencia = (struct Diligencia*) malloc(sizeof(struct Diligencia));
    if (nuevaDiligencia == NULL) {
        printf("Error al asignar memoria para la diligencia.\n");
        return NULL;
    }

    nuevaDiligencia->descripcion = strdup(descripcion);
    nuevaDiligencia->fecha = strdup(fecha);
    nuevaDiligencia->prioridad = strdup(prioridad);

    return nuevaDiligencia;
}

void agregarDiligencia(struct Causa *causa) {
    struct Diligencia *diligenciaNueva;
    struct NodoDiligencia *nuevoNodo;
    struct NodoDiligencia *nodoActual;
    char descripcion[100];
    char fecha[30];
    char prioridad[30];

    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("Causa o carpeta inválida.\n");
        return;
    }

    printf("Ingrese descripción de la diligencia: ");
    leerCadena(descripcion, sizeof(descripcion));

    printf("Ingrese fecha (dd-mm-aaaa): ");
    leerCadena(fecha, sizeof(fecha));

    printf("Ingrese prioridad (alta, media, baja): ");
    leerCadena(prioridad, sizeof(prioridad));

    diligenciaNueva = crearDiligencia(descripcion, fecha, prioridad);
    if (diligenciaNueva == NULL) return;

    nuevoNodo = (struct NodoDiligencia*) malloc(sizeof(struct NodoDiligencia));
    if (nuevoNodo == NULL) {
        printf("Error al asignar memoria para el nodo de diligencia.\n");
        return;
    }

    nuevoNodo->diligencia = diligenciaNueva;
    nuevoNodo->sig = NULL;

    if (causa->carpetaInvestigativa->diligencias == NULL) {
        causa->carpetaInvestigativa->diligencias = nuevoNodo;
    } else {
        nodoActual = causa->carpetaInvestigativa->diligencias;
        while (nodoActual->sig != NULL) {
            nodoActual = nodoActual->sig;
        }
        nodoActual->sig = nuevoNodo;
    }

    printf("Diligencia agregada correctamente.\n");
}

void mostrarDiligencias(struct Causa *causa) {
    struct NodoDiligencia *nodoActual;

    if (causa == NULL || causa->carpetaInvestigativa == NULL || causa->carpetaInvestigativa->diligencias == NULL) {
        printf("No hay diligencias registradas.\n");
        return;
    }
    nodoActual = causa->carpetaInvestigativa->diligencias;

    printf("\n--- Lista de Diligencias ---\n");
    while (nodoActual != NULL) {
        printf("- Descripción: %s\n", nodoActual->diligencia->descripcion);
        printf("  Fecha: %s\n", nodoActual->diligencia->fecha);
        printf("  Prioridad: %s\n\n", nodoActual->diligencia->prioridad);
        nodoActual = nodoActual->sig;
    }
}

struct Prueba* crearPrueba(const char *tipo) {
    struct Prueba *nuevaPrueba;

    nuevaPrueba = (struct Prueba*) malloc(sizeof(struct Prueba));
    if (nuevaPrueba == NULL) {
        printf("Error al asignar memoria para la prueba.\n");
        return NULL;
    }

    nuevaPrueba->TipoPrueba = strdup(tipo);
    return nuevaPrueba;
}

void agregarPrueba(struct Causa *causa) {
    struct NodoPrueba *nuevoNodo;
    struct NodoPrueba *nodoActual;
    struct NodoPrueba *inicio;
    struct Prueba *pruebaNueva;
    char tipoPrueba[100];

    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("Causa o carpeta inválida.\n");
        return;
    }

    printf("Ingrese tipo de prueba (ej: Fotografía, Video, ADN): ");
    leerCadena(tipoPrueba, sizeof(tipoPrueba));

    pruebaNueva = crearPrueba(tipoPrueba);
    if (pruebaNueva == NULL) return;

    nuevoNodo = (struct NodoPrueba*) malloc(sizeof(struct NodoPrueba));
    if (nuevoNodo == NULL) {
        printf("Error al asignar memoria para nodo de prueba.\n");
        return;
    }

    nuevoNodo->prueba = pruebaNueva;

    inicio = causa->carpetaInvestigativa->pruebas;

    if (inicio == NULL) {
        nuevoNodo->sig = nuevoNodo;
        causa->carpetaInvestigativa->pruebas = nuevoNodo;
    } else {
        nodoActual = inicio;
        while (nodoActual->sig != inicio) {
            nodoActual = nodoActual->sig;
        }
        nodoActual->sig = nuevoNodo;
        nuevoNodo->sig = inicio;
    }

    printf("Prueba agregada correctamente.\n");
}

void mostrarPruebas(struct Causa *causa) {
    struct NodoPrueba *inicio;
    struct NodoPrueba *nodoActual;

    if (causa == NULL || causa->carpetaInvestigativa == NULL || causa->carpetaInvestigativa->pruebas == NULL) {
        printf("No hay pruebas registradas.\n");
        return;
    }

    inicio = causa->carpetaInvestigativa->pruebas;
    nodoActual = inicio;

    printf("\n--- Lista de Pruebas Registradas ---\n");
    do {
        printf("- Tipo de prueba: %s\n", nodoActual->prueba->TipoPrueba);
        nodoActual = nodoActual->sig;
    } while (nodoActual != inicio);
}



/*
███╗   ███╗███████╗███╗   ██╗██╗   ██╗
████╗ ████║██╔════╝████╗  ██║██║   ██║
██╔████╔██║█████╗  ██╔██╗ ██║██║   ██║
██║╚██╔╝██║██╔══╝  ██║╚██╗██║██║   ██║
██║ ╚═╝ ██║███████╗██║ ╚████║╚██████╔╝
╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝
*/

void menuMinisterioPublico(struct MinisterioPublico *ministerio) {
    int opcion;
    do {
        printf("\n--- MINISTERIO PUBLICO ---\n");
        printf("1. Agregar causa\n");
        printf("2. Mostrar causas\n");
        printf("3. Agregar fiscal\n");
        printf("4. Mostrar fiscales\n");
        printf("5. Salir\n");

        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);
        getchar(); // limpiar buffer

        switch (opcion) {
            case 1: {
                char categoria[100];
                char ruc[20];
                char comuna[50];
                struct Causa *nuevaCausa;

                printf("RUC: ");
                leerCadena(ruc, sizeof(ruc));

                printf("Categoria de la causa: ");
                leerCadena(categoria, sizeof(categoria));

                printf("Comuna: ");
                leerCadena(comuna, sizeof(comuna));

                nuevaCausa = crearCausa(categoria, ruc, comuna);
                agregarCausa(ministerio, nuevaCausa);

                printf("Causa agregada correctamente.\n");
                break;
            }

            case 2:
                mostrarCausas(ministerio);
                break;

            case 3: {
                int idFiscal;
                char nombre[100];
                char rut[20];
                struct Fiscal *nuevoFiscal;

                printf("ID Fiscal: ");
                scanf("%d", &idFiscal);
                getchar();

                printf("Nombre: ");
                leerCadena(nombre, sizeof(nombre));

                printf("RUT: ");
                leerCadena(rut, sizeof(rut));

                nuevoFiscal = crearFiscal(idFiscal, nombre, rut);
                agregarFiscal(ministerio, nuevoFiscal);

                printf("Fiscal agregado correctamente.\n");
                break;
            }

            case 4:
                mostrarFiscales(ministerio);
                break;

            case 5:
                printf("Saliendo del sistema...\n");
                exit(0);
                break;

            default:
                printf("Opcion no valida. Intente nuevamente.\n");
                break;
        }

    } while (opcion != 5);
}


int main() {
    struct MinisterioPublico ministerio;

    ministerio.causas = NULL;
    ministerio.fiscales = NULL;
    ministerio.raizImputados = NULL;

    menuMinisterioPublico(&ministerio);

    return 0;
}

