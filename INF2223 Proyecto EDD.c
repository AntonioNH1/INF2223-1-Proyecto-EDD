#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DENUNCIAS 1000

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
    struct NodoPersona *datosImputados;         // Imputado (tipo = 2)
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
    char *estado;                        //cerrado o abierto
    struct CarpetaInvestigativa *carpetaInvestigativa; // Antecedentes del caso
    struct Persona *fiscalEncargado;      // Fiscal asignado (tipo 5)
    struct NodoPersona *testigos;
    struct NodoPersonas *victimas;
    struct NodoImputadoABB *imputadosAsociado; // Imputado relacionado (tipo 2)
};

// Carpeta investigativa asociada a una causa penal
struct CarpetaInvestigativa {
    char *estadoCaso;
    struct NodoObjetoInvestigativo *objetos;  // lista circular
    struct ObjetoInvestigativo *denunciasRecientes[MAX_DENUNCIAS]; // arreglo estático
    int totalDenuncias; // cantidad actual
};

// Lista circular de objetos investigativos
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


// Lee una cadena desde la entrada estándar (stdin) y elimina el salto de línea final
void leerCadena(char *buffer, int tam) {
    fgets(buffer, tam, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Reemplaza '\n' con terminador nulo
}

// Crea y retorna un puntero a un nuevo ObjetoInvestigativo con los datos recibidos
struct ObjetoInvestigativo* crearObjetoInvestigativo(int id, char *fecha, char *rut, char *detalle, int tipo) {
    struct ObjetoInvestigativo *nuevo;

    nuevo = (struct ObjetoInvestigativo*) malloc(sizeof(struct ObjetoInvestigativo));
    nuevo->id = id;
    nuevo->fecha = strdup(fecha);     // Duplica la fecha
    nuevo->rut = strdup(rut);         // Duplica el RUT del autor
    nuevo->detalle = strdup(detalle); // Duplica el contenido del objeto
    nuevo->tipo = tipo;

    return nuevo;
}

// Agrega un objeto investigativo al final de la lista de objetos en una carpeta investigativa
void agregarObjeto(struct CarpetaInvestigativa *carpeta, struct ObjetoInvestigativo *objeto) {
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

    nuevoNodo = (struct NodoObjetoInvestigativo*) malloc(sizeof(struct NodoObjetoInvestigativo));
    nuevoNodo->objeto = objeto;

    if (carpeta->objetos == NULL) {
        nuevoNodo->sig = nuevoNodo;  // se apunta a sí mismo
        carpeta->objetos = nuevoNodo;
    } else {
        actual = carpeta->objetos;
        while (actual->sig != carpeta->objetos) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->sig = carpeta->objetos;  // cierre circular
    }
}


void agregarObjetoPorTipo(struct CarpetaInvestigativa *carpeta) {
    // --- Declaración de variables ---
    int id, tipo;
    char fecha[30];
    char rut[20];
    char detalle[200];
    struct ObjetoInvestigativo *nuevo;

    // --- Verificación de carpeta válida ---
    if (carpeta == NULL) {
        printf("La carpeta investigativa no es válida.\n");
        return;
    }

    // --- Entrada de datos ---
    printf("Ingrese ID del objeto: ");
    scanf("%d", &id);
    getchar(); // limpiar buffer

    printf("Fecha (dd-mm-aaaa): ");
    leerCadena(fecha, sizeof(fecha));

    printf("RUT de la persona que generó el objeto: ");
    leerCadena(rut, sizeof(rut));

    printf("Detalle del objeto: ");
    leerCadena(detalle, sizeof(detalle));

    printf("Tipo de objeto:\n");
    printf("1 = Denuncia\n2 = Prueba\n3 = Diligencia\n4 = Declaración\n5 = Resolución Judicial\n");
    printf("Seleccione tipo: ");
    scanf("%d", &tipo);
    getchar(); // limpiar buffer

    if (tipo < 1 || tipo > 5) {
        printf("Tipo inválido. Debe ser un número entre 1 y 5.\n");
        return;
    }

    // --- Crear y agregar objeto ---
    nuevo = crearObjetoInvestigativo(id, fecha, rut, detalle, tipo);
    agregarObjeto(carpeta, nuevo);

    printf("Objeto agregado correctamente a la carpeta.\n");
}

// Traduce el valor entero del tipo de objeto a un texto representativo
void obtenerNombreTipoObjeto(int tipo, char *buffer) {
    if (tipo == 1) strcpy(buffer, "Denuncia");
    else if (tipo == 2) strcpy(buffer, "Prueba");
    else if (tipo == 3) strcpy(buffer, "Diligencia");
    else if (tipo == 4) strcpy(buffer, "Declaracion");
    else if (tipo == 5) strcpy(buffer, "Resolucion Judicial");
    else strcpy(buffer, "Desconocido");
}

void listarObjetosPorID(struct NodoObjetoInvestigativo *head, int idBuscado) {
    int encontrado = 0;

    while (head != NULL) {
        if (head->objeto->id == idBuscado) {
            printf("Objeto investigativo encontrado:\n");
            printf("- ID: %d\n", head->objeto->id);
            printf("  Fecha: %s\n", head->objeto->fecha);
            printf("  RUT relacionado: %s\n", head->objeto->rut);
            printf("  Detalle: %s\n", head->objeto->detalle);
            printf("  Tipo: %d\n\n", head->objeto->tipo);
            encontrado = 1;
            break; // Si solo deseas mostrar uno, puedes salir aquí
        }
        head = head->sig;
    }

    if (!encontrado) {
        printf("No se encontró ningún objeto con el ID %d.\n", idBuscado);
    }
}

// Busca un objeto investigativo por su ID y retorna un puntero al objeto encontrado.
// Si no se encuentra, retorna NULL.
struct ObjetoInvestigativo* buscarObjetoPorId(struct NodoObjetoInvestigativo *listaObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;

    actual = listaObjetos;
    while (actual != NULL) {
        if (actual->objeto->id == idBuscado) {
            return actual->objeto;
        }
        actual = actual->sig;
    }
    return NULL;
}

// Modifica el detalle y tipo de un objeto investigativo según su ID.
void modificarDatosObjeto(struct NodoObjetoInvestigativo *listaObjetos, int idBuscado, char *nuevoDetalle, int nuevoTipo) {
    struct ObjetoInvestigativo *objeto;

    objeto = buscarObjetoPorId(listaObjetos, idBuscado);
    if (objeto != NULL) {
        objeto->detalle = strdup(nuevoDetalle);
        objeto->tipo = nuevoTipo;
        printf("Objeto investigativo modificado correctamente.\n");
    } else {
        printf("No se encontró un objeto con ese ID.\n");
    }
}

// Elimina un objeto investigativo de la lista enlazada según su ID.
void eliminarObjetoPorId(struct NodoObjetoInvestigativo **listaObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *anterior;

    actual = *listaObjetos;
    anterior = NULL;

    while (actual != NULL) {
        if (actual->objeto->id == idBuscado) {
            if (anterior == NULL) {
                *listaObjetos = actual->sig;
            } else {
                anterior->sig = actual->sig;
            }
            printf("Objeto investigativo eliminado correctamente.\n");
            return;
        }
        anterior = actual;
        actual = actual->sig;
    }

    printf("No se encontró un objeto con ese ID.\n");
}

// Crea y retorna un puntero a una nueva persona con los datos recibidos
struct Persona* crearPersona(char *nombre, char *rut, int tipo) {
    struct Persona *nueva;

    nueva = (struct Persona*) malloc(sizeof(struct Persona));
    nueva->nombre = strdup(nombre); // Duplica el nombre
    nueva->rut = strdup(rut);       // Duplica el RUT
    nueva->tipo = tipo;

    return nueva;
}

// Agrega una persona al inicio de la lista enlazada
void agregarPersona(struct NodoPersona **head, struct Persona *persona) {
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona*) malloc(sizeof(struct NodoPersona));
    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = *head;  // Inserta al inicio
    *head = nuevoNodo;
}

// Traduce el valor entero del tipo de persona a una cadena representativa
void obtenerNombreTipoPersona(int tipo, char *buffer) {
    if (tipo == 1) strcpy(buffer, "Denunciante");
    else if (tipo == 2) strcpy(buffer, "Imputado");
    else if (tipo == 3) strcpy(buffer, "Testigo");
    else if (tipo == 4) strcpy(buffer, "Juez");
    else if (tipo == 5) strcpy(buffer, "Fiscal");
    else strcpy(buffer, "Desconocido");
}

// Busca una persona en la lista por su RUT y retorna el puntero a la persona encontrada.
// Si no la encuentra, retorna NULL.
struct Persona* buscarPersonaPorRut(struct NodoPersona *listaPersonas, char *rutBuscado) {
    struct NodoPersona *actual;

    actual = listaPersonas;
    while (actual != NULL) {
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            return actual->datosPersona;
        }
        actual = actual->sig;
    }
    return NULL;
}

 void listarPersonaPorRut(struct NodoPersona *lista, char *rut) {
    struct Persona *actual;

    actual=buscarPersonaPorRut(lista, rut);

    if (actual == NULL) {
        printf("No se encontró una persona con ese RUT.\n");
        return;
    }

    printf("Datos de la persona:\n");
    printf("Nombre: %s\n", actual->nombre);
    printf("RUT: %s\n", actual->rut);
    printf("Tipo: %d\n", actual->tipo);
}

// Modifica el nombre y tipo de una persona existente según su RUT.
void modificarDatosPersona(struct NodoPersona *listaPersonas, char *rutBuscado, char *nuevoNombre, int nuevoTipo) {
    struct Persona *persona;

    persona = buscarPersonaPorRut(listaPersonas, rutBuscado);
    if (persona != NULL) {
        persona->nombre = strdup(nuevoNombre);
        persona->tipo = nuevoTipo;
        printf("Persona modificada correctamente.\n");
    } else {
        printf("No se encontró una persona con ese RUT.\n");
    }
}

// Elimina una persona de la lista enlazada según su RUT.
void eliminarPersonaPorRut(struct NodoPersona **listaPersonas, char *rutBuscado) {
    struct NodoPersona *actual;
    struct NodoPersona *anterior;

    actual = *listaPersonas;
    anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            if (anterior == NULL) {
                *listaPersonas = actual->sig;
            } else {
                anterior->sig = actual->sig;
            }
            printf("Persona eliminada correctamente.\n");
            return;
        }
        anterior = actual;
        actual = actual->sig;
    }

    printf("No se encontró una persona con ese RUT.\n");
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


/*
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
                mostrarPersonasPorTipo(ministerio->personas, ministerio->personas->datosPersona->tipo);
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

/*
int main() {
    struct MinisterioPublico ministerio;

    ministerio.causas = NULL;
    ministerio.personas = NULL;
    ministerio.raizImputados = NULL;

    menuMinisterioPublico(&ministerio);

    return 0;
}

