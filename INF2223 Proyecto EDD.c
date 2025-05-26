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

// Estructura principal que representa el Ministerio Público
struct MinisterioPublico {
    struct NodoCausa *causas;                // Lista de causas registradas
    struct NodoPersona *personas;            // Lista general de personas
    struct NodoImputadoABB *raizImputados;   // Árbol binario de imputados
};

// Nodo del árbol binario de búsqueda que contiene imputados
struct NodoImputadoABB {
    struct NodoPersona *datosImputados;     // Puntero al imputado (persona tipo 2)
    struct NodoImputadoABB *izq;            // Subárbol izquierdo
    struct NodoImputadoABB *der;            // Subárbol derecho
    struct Formalizacion *formalizacion;    // Información de la formalización judicial
};

// Contiene los datos de formalización judicial de un imputado
struct Formalizacion {
    char *delito;           // Nombre del delito formalizado
    char *antecedentes;     // Antecedentes que sustentan el delito
    char *fecha;            // Fecha de la formalización
    int medidaCautelar;     // Medida cautelar (1 = prisión, 2 = arraigo, etc.)
};

// Nodo de la lista enlazada de causas
struct NodoCausa {
    struct Causa *datosCausa;     // Puntero a los datos de la causa
    struct NodoCausa *sig;        // Siguiente nodo en la lista
};

// Información principal de una causa penal
struct Causa {
    char *RUC;                             // Rol Único de Causa
    char *CategoriaCausa;                 // Tipo o categoría de delito
    char *estado;                         // Estado actual de la causa (abierta/cerrada)
    struct CarpetaInvestigativa *carpetaInvestigativa; // Carpeta con antecedentes del caso
    struct Persona *fiscalEncargado;      // Fiscal a cargo de la causa (tipo 5)
    struct NodoPersona *testigos;         // Lista de testigos asociados
    struct NodoPersona *victimas;         // Lista de víctimas asociadas
    struct NodoPersona *imputadosAsociados; // Lista de imputados asociados a la causa
};

// Carpeta investigativa asociada a una causa penal
struct CarpetaInvestigativa {
    char *estadoCaso;                                     // Estado del caso en la carpeta
    struct NodoObjetoInvestigativo *objetos;              // Lista circular de objetos vinculados
    struct ObjetoInvestigativo *denunciasRecientes[MAX_DENUNCIAS]; // Arreglo estático de denuncias
    int totalDenuncias;                                   // Número actual de denuncias guardadas
};

// Nodo de la lista circular de objetos investigativos
struct NodoObjetoInvestigativo {
    struct ObjetoInvestigativo *objeto;     // Puntero al objeto investigativo
    struct NodoObjetoInvestigativo *sig;    // Siguiente objeto (circular)
};

// Representa cualquier elemento incluido en la investigación
struct ObjetoInvestigativo {
    int id;              // Identificador único
    char *fecha;         // Fecha de creación o incorporación
    char *rut;           // RUT de la persona que lo generó
    char *detalle;       // Descripción del objeto
    int tipo;            // Tipo de objeto (1=Denuncia, 2=Prueba, etc.)
};

// Nodo de lista simple que almacena personas
struct NodoPersona {
    struct Persona *datosPersona;     // Puntero a la persona
    struct NodoPersona *sig;          // Siguiente nodo
};

// Información de una persona involucrada en el sistema
struct Persona {
    char *nombre;     // Nombre completo
    char *rut;        // RUT único
    int tipo;         // Tipo: 1=Denunciante, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal
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

// Crea un nuevo objeto investigativo con los datos entregados
struct ObjetoInvestigativo* crearObjetoInvestigativo(int id, char *fecha, char *rut, char *detalle, int tipo) {
    struct ObjetoInvestigativo *nuevo;

    nuevo = (struct ObjetoInvestigativo*) malloc(sizeof(struct ObjetoInvestigativo));
    nuevo->id = id;
    nuevo->fecha = strdup(fecha);
    nuevo->rut = strdup(rut);
    nuevo->detalle = strdup(detalle);
    nuevo->tipo = tipo;

    return nuevo;
}

// Agrega un objeto investigativo al final de la lista circular
void agregarObjeto(struct CarpetaInvestigativa *carpeta, struct ObjetoInvestigativo *objeto) {
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

    // Crear nuevo nodo con el objeto
    nuevoNodo = (struct NodoObjetoInvestigativo*) malloc(sizeof(struct NodoObjetoInvestigativo));
    nuevoNodo->objeto = objeto;

    if (carpeta->objetos == NULL) {
        // Primer nodo: apunta a sí mismo
        nuevoNodo->sig = nuevoNodo;
        carpeta->objetos = nuevoNodo;
    } else {
        // Insertar al final de la lista circular
        actual = carpeta->objetos;
        while (actual->sig != carpeta->objetos) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->sig = carpeta->objetos;
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

// Muestra los datos de un objeto investigativo según su ID
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
            break;
        }
        head = head->sig;
    }

    if (!encontrado) {
        printf("No se encontró ningún objeto con el ID %d.\n", idBuscado);
    }
}

// Busca un objeto investigativo por ID y retorna su puntero
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

// Modifica el detalle y tipo de un objeto según su ID
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

// Elimina un objeto investigativo de la lista según su ID
void eliminarObjetoPorId(struct NodoObjetoInvestigativo **listaObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *anterior;

    actual = *listaObjetos;
    anterior = NULL;

    // Recorrer la lista
    while (actual != NULL) {
        // Comparar ID
        if (actual->objeto->id == idBuscado) {
            // Si es el primer nodo
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

// Crea una nueva persona con nombre, RUT y tipo
struct Persona* crearPersona(char *nombre, char *rut, int tipo) {
    struct Persona *nueva;

    nueva = (struct Persona*) malloc(sizeof(struct Persona));
    nueva->nombre = strdup(nombre);
    nueva->rut = strdup(rut);
    nueva->tipo = tipo;

    return nueva;
}

// Agrega una persona al inicio de la lista enlazada
void agregarPersona(struct NodoPersona **head, struct Persona *persona) {
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona*) malloc(sizeof(struct NodoPersona));

    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = *head;
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

// Busca una persona en la lista por su RUT y retorna un puntero a la persona
struct Persona* buscarPersonaPorRut(struct NodoPersona *listaPersonas, char *rutBuscado) {
    struct NodoPersona *actual;

    // Recorrer la lista
    actual = listaPersonas;
    while (actual != NULL) {
        // Comparar RUT
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            return actual->datosPersona;
        }
        actual = actual->sig;
    }

    // No se encontró
    return NULL;
}

// Muestra los datos de una persona según su RUT
void listarPersonaPorRut(struct NodoPersona *lista, char *rut) {
    struct Persona *actual;

    // Buscar persona en la lista
    actual = buscarPersonaPorRut(lista, rut);

    if (actual == NULL) {
        printf("No se encontró una persona con ese RUT.\n");
        return;
    }

    // Mostrar datos si se encontró
    printf("Datos de la persona:\n");
    printf("Nombre: %s\n", actual->nombre);
    printf("RUT: %s\n", actual->rut);
    printf("Tipo: %d\n", actual->tipo);
}

// Modifica nombre y tipo de una persona según su RUT
void modificarDatosPersona(struct NodoPersona *listaPersonas, char *rutBuscado, char *nuevoNombre, int nuevoTipo) {
    struct Persona *persona;

    // Buscar persona por RUT
    persona = buscarPersonaPorRut(listaPersonas, rutBuscado);

    if (persona != NULL) {
        // Asignar nuevo nombre y tipo
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

    // Se inicia apuntando al comienzo de la lista
    actual = *listaPersonas;
    anterior = NULL;

    // Recorre la lista mientras queden nodos
    while (actual != NULL) {
        // Compara si el RUT del nodo actual coincide con el que se quiere eliminar
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            // Si el nodo a eliminar es el primero de la lista
            if (anterior == NULL) {
                *listaPersonas = actual->sig; // Se actualiza el inicio de la lista
            } else {
                // Si el nodo no es el primero, se salta el nodo actual
                anterior->sig = actual->sig;
            }

            // En este punto no se libera memoria porque trabajas sin free()
            printf("Persona eliminada correctamente.\n");
            return; // Sale de la función después de eliminar
        }

        // Avanza los punteros
        anterior = actual;
        actual = actual->sig;
    }

    // Si se recorrió toda la lista y no se encontró la persona
    printf("No se encontró una persona con ese RUT.\n");
}

// Crea una nueva causa penal con RUC, categoría y estado
struct Causa* crearCausa(char *ruc, char *categoria, char *estado) {
    struct Causa *nuevaCausa;

    // Reservar memoria para la nueva causa
    nuevaCausa = (struct Causa*) malloc(sizeof(struct Causa));

    // Asignar valores básicos
    nuevaCausa->RUC = strdup(ruc);
    nuevaCausa->CategoriaCausa = strdup(categoria);
    nuevaCausa->estado = strdup(estado);

    // Inicializar campos con punteros vacíos
    nuevaCausa->carpetaInvestigativa = NULL;
    nuevaCausa->fiscalEncargado = NULL;
    nuevaCausa->victimas = NULL;
    nuevaCausa->testigos = NULL;
    nuevaCausa->imputadosAsociados = NULL;

    return nuevaCausa;
}

// Agregar una causa a la lista de causas del Ministerio Público
void agregarCausa(struct NodoCausa **listaCausas, struct Causa *nuevaCausa) {
    struct NodoCausa *nuevoNodo;

    nuevoNodo = (struct NodoCausa*) malloc(sizeof(struct NodoCausa));
    nuevoNodo->datosCausa = nuevaCausa;
    nuevoNodo->sig = *listaCausas;
    *listaCausas = nuevoNodo;
}

// Mostrar todas las causas registradas
void listarCausas(struct NodoCausa *listaCausas) {
    struct NodoCausa *nodoActual;

    nodoActual = listaCausas;
    while (nodoActual != NULL) {
        printf("RUC: %s | Categoria: %s | Estado: %s\n",
               nodoActual->datosCausa->RUC,
               nodoActual->datosCausa->CategoriaCausa,
               nodoActual->datosCausa->estado);
        nodoActual = nodoActual->sig;
    }
}

// Muestra los datos de una causa que coincida con el RUC buscado
void mostrarCausaPorRUC(struct NodoCausa *listaCausas, char *rucBuscado) {
    struct NodoCausa *nodoActual;

    nodoActual = listaCausas;

    // Recorrer la lista de causas
    while (nodoActual != NULL) {
        // Comparar RUC
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            // Mostrar los datos si se encuentra
            printf("Causa encontrada:\n");
            printf("RUC: %s\n", nodoActual->datosCausa->RUC);
            printf("Categoria: %s\n", nodoActual->datosCausa->CategoriaCausa);
            printf("Estado: %s\n", nodoActual->datosCausa->estado);
            return;
        }
        nodoActual = nodoActual->sig;
    }

    // Si no se encuentra coincidencia
    printf("No se encontró una causa con ese RUC.\n");
}

// Modifica la categoría y estado de una causa buscándola por RUC
void modificarCausa(struct NodoCausa *listaCausas, char *rucBuscado, char *nuevaCategoria, char *nuevoEstado) {
    struct NodoCausa *nodoActual;

    nodoActual = listaCausas;

    // Buscar la causa con el RUC indicado
    while (nodoActual != NULL) {
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            // Asignar nuevos valores
            nodoActual->datosCausa->CategoriaCausa = strdup(nuevaCategoria);
            nodoActual->datosCausa->estado = strdup(nuevoEstado);
            printf("Causa modificada correctamente.\n");
            return;
        }
        nodoActual = nodoActual->sig;
    }

    // Si no se encontró la causa
    printf("No se encontró una causa con ese RUC.\n");
}

// Elimina una causa de la lista enlazada según su RUC
void eliminarCausaPorRUC(struct NodoCausa **listaCausas, char *rucBuscado) {
    struct NodoCausa *nodoActual;
    struct NodoCausa *nodoAnterior;

    nodoActual = *listaCausas; // Inicio de la lista
    nodoAnterior = NULL;

    while (nodoActual != NULL) {
        // Comparar el RUC de la causa actual
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            // Si es el primer nodo
            if (nodoAnterior == NULL) {
                *listaCausas = nodoActual->sig;
            } else {
                nodoAnterior->sig = nodoActual->sig;
            }
            printf("Causa eliminada de la lista.\n");
            return;
        }
        nodoAnterior = nodoActual;
        nodoActual = nodoActual->sig;
    }

    printf("No se encontró una causa con ese RUC.\n");
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


