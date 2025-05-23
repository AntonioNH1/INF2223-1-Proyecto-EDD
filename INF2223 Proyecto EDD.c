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
    nuevoNodo->sig = NULL;

    // Si la lista está vacía, se asigna como primer nodo
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

// Muestra todos los objetos investigativos de un tipo específico desde la carpeta investigativa
void mostrarObjetosPorTipo(struct CarpetaInvestigativa *carpeta, int tipoBuscado) {
    struct NodoObjetoInvestigativo *actual;
    char tipoTexto[25];

    // Validación: no mostrar si la carpeta o lista está vacía
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


