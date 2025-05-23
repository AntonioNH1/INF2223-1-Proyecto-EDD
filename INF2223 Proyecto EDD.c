#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***
 *    ███████╗████████╗██████╗ ██╗   ██╗ ██████╗████████╗███████╗
 *    ██╔════╝╚══██╔══╝██╔══██╗██║   ██║██╔════╝╚══██╔══╝██╔════╝
 *    ███████╗   ██║   ██████╔╝██║   ██║██║        ██║   ███████╗
 *    ╚════██║   ██║   ██╔══██╗██║   ██║██║        ██║   ╚════██║
 *    ███████║   ██║   ██║  ██║╚██████╔╝╚██████╗   ██║   ███████║
 *    ╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝  ╚═════╝   ╚═╝   ╚══════╝
 *
 */

// Estructura principal del sistema: Ministerio Público
struct MinisterioPublico {
    struct NodoCausa *causas;                // Lista simple de causas
    struct NodoPersona *personas;            // Lista simple de todas las personas
    struct NodoImputadoABB *raizImputados;   // Árbol de imputados
};


// Nodo de árbol binario de búsqueda para imputados
struct NodoImputadoABB {
    struct Persona *datosImputado;         // Imputado (tipo = 2)
    struct NodoImputadoABB *izq;
    struct NodoImputadoABB *der;
};

// Lista simple de causas
struct NodoCausa {
    struct Causa *datosCausa;
    struct NodoCausa *sig;
};

// Información principal de una causa penal
struct Causa {
    char *RUC;                             // Rol Único de Causa
    char *CategoriaCausa;                 // Tipo de causa (ej: Robo, Estafa)
    char *Comuna;                         // Comuna donde ocurrió
    struct CarpetaInvestigativa *carpetaInvestigativa; // Antecedentes del caso
    struct Persona *fiscalEncargado;      // Fiscal asignado (tipo 5)
    struct NodoImputadoABB *imputadoAsociado; // Imputado relacionado (tipo 2)
};

// Carpeta investigativa asociada a una causa penal
struct CarpetaInvestigativa {
    char *estadoCaso;  // Estado actual del caso (ej: "en investigación")
    struct NodoObjetoInvestigativo *objetos;  // Lista de objetos vinculados a esta carpeta
};

// Lista simple de objetos investigativos
struct NodoObjetoInvestigativo {
    struct ObjetoInvestigativo *objeto;
    struct NodoObjetoInvestigativo *sig;
};

// Representa cualquier objeto en la carpeta investigativa
struct ObjetoInvestigativo {
    int id;         // Identificador único del objeto
    char *fecha;    // Fecha del objeto
    char *rut;      // RUT de quien generó el objeto (referencia lógica a Persona)
    char *detalle;  // Contenido del objeto
    int tipo;       // 1=Denuncia, 2=Prueba, 3=Diligencia, 4=Declaración, 5=Resolución judicial
};

// Lista simple de personas (genérica, ya no limitada a fiscales)
struct NodoPersona {
    struct Persona *datosPersona;
    struct NodoPersona *sig;
};

// Representa a una persona en el sistema: juez, denunciante, testigo, imputado, fiscal, etc.
struct Persona {
    char *nombre;  // Nombre completo
    char *rut;     // RUT único
    int tipo;      // 1=Denunciante, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal
};

/***
 *    ███████╗██╗   ██╗███╗   ██╗ ██████╗██╗ ██████╗ ███╗   ██╗███████╗███████╗
 *    ██╔════╝██║   ██║████╗  ██║██╔════╝██║██╔═══██╗████╗  ██║██╔════╝██╔════╝
 *    █████╗  ██║   ██║██╔██╗ ██║██║     ██║██║   ██║██╔██╗ ██║█████╗  ███████╗
 *    ██╔══╝  ██║   ██║██║╚██╗██║██║     ██║██║   ██║██║╚██╗██║██╔══╝  ╚════██║
 *    ██║     ╚██████╔╝██║ ╚████║╚██████╗██║╚██████╔╝██║ ╚████║███████╗███████║
 *    ╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚══════╝
 *
 */


void leerCadena(char *buffer, int tam) {
    fgets(buffer, tam, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar salto de línea
}

struct ObjetoInvestigativo* crearObjetoInvestigativo(int id, const char *fecha, const char *rut, const char *detalle, int tipo) {
    struct ObjetoInvestigativo *nuevo;
    nuevo = (struct ObjetoInvestigativo*) malloc(sizeof(struct ObjetoInvestigativo));

    nuevo->id = id;
    nuevo->fecha = strdup(fecha);
    nuevo->rut = strdup(rut);
    nuevo->detalle = strdup(detalle);
    nuevo->tipo = tipo;

    return nuevo;
}

void agregarObjeto(struct CarpetaInvestigativa *carpeta, struct ObjetoInvestigativo *objeto) {
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

    nuevoNodo = (struct NodoObjetoInvestigativo*) malloc(sizeof(struct NodoObjetoInvestigativo));
    nuevoNodo->objeto = objeto;
    nuevoNodo->sig = NULL;

    if (carpeta->objetos == NULL) {
        carpeta->objetos = nuevoNodo;
    } else {
        actual = carpeta->objetos;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
    }
}

void obtenerNombreTipoObjeto(int tipo, char *buffer) {
    if (tipo == 1) strcpy(buffer, "Denuncia");
    else if (tipo == 2) strcpy(buffer, "Prueba");
    else if (tipo == 3) strcpy(buffer, "Diligencia");
    else if (tipo == 4) strcpy(buffer, "Declaracion");
    else if (tipo == 5) strcpy(buffer, "Resolucion Judicial");
    else strcpy(buffer, "Desconocido");
}


void mostrarObjetosPorTipo(struct CarpetaInvestigativa *carpeta, int tipoBuscado) {
    struct NodoObjetoInvestigativo *actual;
    char tipoTexto[25];

    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("No hay objetos registrados.\n");
        return;
    }

    actual = carpeta->objetos;

    while (actual != NULL) {
        if (actual->objeto->tipo == tipoBuscado) {
            obtenerNombreTipoObjeto(actual->objeto->tipo, tipoTexto);
            printf("ID: %d | Tipo: %s | Fecha: %s | RUT: %s\nDetalle: %s\n\n",
                   actual->objeto->id,
                   tipoTexto,
                   actual->objeto->fecha,
                   actual->objeto->rut,
                   actual->objeto->detalle);
        }
        actual = actual->sig;
    }
}


// Crea una nueva persona con nombre, rut y tipo
struct Persona* crearPersona(const char *nombre, const char *rut, int tipo) {
    struct Persona *nueva;

    nueva = (struct Persona*) malloc(sizeof(struct Persona));
    nueva->nombre = strdup(nombre);
    nueva->rut = strdup(rut);
    nueva->tipo = tipo;

    return nueva;
}


// Agrega una persona a una lista enlazada (al inicio)
void agregarPersona(struct NodoPersona **head, struct Persona *persona) {
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona*) malloc(sizeof(struct NodoPersona));
    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = *head;
    *head = nuevoNodo;
}

void obtenerNombreTipoPersona(int tipo, char *buffer) {
    if (tipo == 1) strcpy(buffer, "Denunciante");
    else if (tipo == 2) strcpy(buffer, "Imputado");
    else if (tipo == 3) strcpy(buffer, "Testigo");
    else if (tipo == 4) strcpy(buffer, "Juez");
    else if (tipo == 5) strcpy(buffer, "Fiscal");
    else strcpy(buffer, "Desconocido");
}


void mostrarPersonas(struct NodoPersona *head) {
    struct NodoPersona *actual;
    char tipoTexto[20];

    actual = head;

    while (actual != NULL) {
        obtenerNombreTipoPersona(actual->datosPersona->tipo, tipoTexto);
        printf("Nombre: %s | RUT: %s | Tipo: %s\n",
               actual->datosPersona->nombre,
               actual->datosPersona->rut,
               tipoTexto);
        actual = actual->sig;
    }
}


/***
 *    ███╗   ███╗███████╗███╗   ██╗██╗   ██╗
 *    ████╗ ████║██╔════╝████╗  ██║██║   ██║
 *    ██╔████╔██║█████╗  ██╔██╗ ██║██║   ██║
 *    ██║╚██╔╝██║██╔══╝  ██║╚██╗██║██║   ██║
 *    ██║ ╚═╝ ██║███████╗██║ ╚████║╚██████╔╝
 *    ╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝
 *
 */
void menuMinisterioPublico(struct MinisterioPublico *ministerio) {
    int opcion;

    do {
        printf("\n--- MINISTERIO PUBLICO ---\n");
        printf("1. Agregar persona\n");
        printf("2. Mostrar personas\n");
        printf("3. Agregar objeto investigativo a una carpeta\n");
        printf("4. Mostrar objetos por tipo\n");
        printf("5. Salir\n");

        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);
        getchar(); // limpiar buffer

        switch (opcion) {

            case 1: {
                char nombre[100];
                char rut[20];
                int tipo;
                struct Persona *nueva;

                printf("Nombre: ");
                leerCadena(nombre, sizeof(nombre));

                printf("RUT: ");
                leerCadena(rut, sizeof(rut));

                printf("Tipo (1=Denunciante, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal): ");
                scanf("%d", &tipo);
                getchar();

                nueva = crearPersona(nombre, rut, tipo);
                agregarPersona(&(ministerio->personas), nueva);

                printf("Persona agregada correctamente.\n");
                break;
            }

            case 2:
                mostrarPersonas(ministerio->personas);
                break;

            case 3: {
                int id, tipo;
                char fecha[30];
                char rut[20];
                char detalle[200];
                char estado[30];
                struct ObjetoInvestigativo *nuevo;
                struct CarpetaInvestigativa *carpeta;

                printf("Ingrese ID del objeto: ");
                scanf("%d", &id);
                getchar();

                printf("Fecha (dd-mm-aaaa): ");
                leerCadena(fecha, sizeof(fecha));

                printf("RUT del creador: ");
                leerCadena(rut, sizeof(rut));

                printf("Detalle: ");
                leerCadena(detalle, sizeof(detalle));

                printf("Tipo de objeto (1=Denuncia, 2=Prueba, 3=Diligencia, 4=Declaracion, 5=Resolucion): ");
                scanf("%d", &tipo);
                getchar();

                printf("Estado de la carpeta investigativa: ");
                leerCadena(estado, sizeof(estado));

                nuevo = crearObjetoInvestigativo(id, fecha, rut, detalle, tipo);

                carpeta = (struct CarpetaInvestigativa*) malloc(sizeof(struct CarpetaInvestigativa));
                carpeta->estadoCaso = strdup(estado);
                carpeta->objetos = NULL;

                agregarObjeto(carpeta, nuevo);

                printf("Objeto investigativo agregado a nueva carpeta.\n");
                break;
            }

            case 4: {
                int tipo;
                struct CarpetaInvestigativa *carpeta;

                carpeta = (struct CarpetaInvestigativa*) malloc(sizeof(struct CarpetaInvestigativa));
                carpeta->estadoCaso = strdup("simulada");
                carpeta->objetos = NULL;

                printf("Tipo de objeto a mostrar (1-5): ");
                scanf("%d", &tipo);
                getchar();

                mostrarObjetosPorTipo(carpeta, tipo);
                break;
            }

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

/***
 *    ███╗   ███╗ █████╗ ██╗███╗   ██╗
 *    ████╗ ████║██╔══██╗██║████╗  ██║
 *    ██╔████╔██║███████║██║██╔██╗ ██║
 *    ██║╚██╔╝██║██╔══██║██║██║╚██╗██║
 *    ██║ ╚═╝ ██║██║  ██║██║██║ ╚████║
 *    ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝
 *
 */

int main() {
    struct MinisterioPublico ministerio;

    ministerio.causas = NULL;
    ministerio.personas = NULL;
    ministerio.raizImputados = NULL;

    menuMinisterioPublico(&ministerio);

    return 0;
}

