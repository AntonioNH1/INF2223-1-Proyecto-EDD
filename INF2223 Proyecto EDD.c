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

/* Estructura principal que representa el Ministerio Publico */
struct MinisterioPublico {
    struct NodoCausa *causas;                /* Lista de causas registradas */
    struct NodoPersona *personas;            /* Lista general de personas */
    struct NodoImputadoABB *raizImputados;   /* Arbol binario de imputados */
};

/* Nodo del arbol binario de busqueda que contiene imputados */
struct NodoImputadoABB {
    struct NodoPersona *datosImputados;     /* Puntero al imputado (persona tipo 2) */
    struct NodoImputadoABB *izq;            /* Subarbol izquierdo */
    struct NodoImputadoABB *der;            /* Subarbol derecho */
    struct Formalizacion *formalizacion;    /* Informacion de la formalizacion judicial */
};

/* Contiene los datos de formalizacion judicial de un imputado */
struct Formalizacion {
    char *delito;           /* Nombre del delito formalizado */
    char *antecedentes;     /* Antecedentes que sustentan el delito */
    char *fecha;            /* Fecha de la formalizacion */
    int medidaCautelar;     /* Medida cautelar (1 = prision, 2 = arraigo, etc.) */
    struct Persona *defensor;
};

/* Nodo de la lista enlazada de causas */
struct NodoCausa {
    struct Causa *datosCausa;     /* Puntero a los datos de la causa */
    struct NodoCausa *sig;        /* Siguiente nodo en la lista */
};

/* Informacion principal de una causa penal */
struct Causa {
    char *RUC;                             /* Rol Unico de Causa */
    char *CategoriaCausa;                 /* Tipo o categoria de delito */
    char *estado;                         /* Estado actual de la causa (abierta/cerrada) */
    struct CarpetaInvestigativa *carpetaInvestigativa; /* Carpeta con antecedentes del caso */
    struct Persona *fiscalEncargado;      /* Fiscal a cargo de la causa (tipo 5) */
    struct NodoPersona *testigos;         /* Lista de testigos asociados */
    struct NodoPersona *victimas;         /* Lista de victimas asociadas */
    struct NodoPersona *imputadosAsociados; /* Lista de imputados asociados a la causa */
};

/* Carpeta investigativa asociada a una causa penal */
struct CarpetaInvestigativa {
    char *estadoCaso;                                     /* Estado del caso en la carpeta */
    struct NodoObjetoInvestigativo *objetos;              /* Lista circular de objetos vinculados */
    struct ObjetoInvestigativo *denunciasRecientes[MAX_DENUNCIAS]; /* Arreglo estatico de denuncias */
    int totalDenuncias;                                   /* Numero actual de denuncias guardadas */
};

/* Nodo de la lista circular de objetos investigativos */
struct NodoObjetoInvestigativo {
    struct ObjetoInvestigativo *objeto;     /* Puntero al objeto investigativo */
    struct NodoObjetoInvestigativo *sig;    /* Siguiente objeto (circular) */
};

/* Representa cualquier elemento incluido en la investigacion */
struct ObjetoInvestigativo {
    int id;              /* Identificador unico */
    char *fecha;         /* Fecha de creacion o incorporacion */
    char *rut;           /* RUT de la persona que lo genero */
    char *detalle;       /* Descripcion del objeto */
    int tipo;            /* Tipo de objeto (1=Denuncia, 2=Prueba, etc.) */
    int sentenciaFinal;  /* 1 si es sentencia final, 0 si no lo es */
};

/* Nodo de lista simple que almacena personas */
struct NodoPersona {
    struct Persona *datosPersona;     /* Puntero a la persona */
    struct NodoPersona *sig;          /* Siguiente nodo */
};

/* Informacion de una persona involucrada en el sistema */
struct Persona {
    char *nombre;     /* Nombre completo */
    char *rut;        /* RUT unico */
    int tipo;         /* Tipo: 1=Denunciante, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal, 6=Defensor */
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

/* ======================== FUNCIONES EXTRA ======================== */

/* Lee una cadena desde stdin y elimina el salto de linea final */
void leerCadena(char *buffer, int tam) {
    int i;
    fgets(buffer, tam, stdin);
    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            break;
        }
    }
}

/* Copia una cadena manualmente, compatible con ANSI C */
char *copiarCadena(char *texto) {
    char *copia;
    copia = (char *)malloc(strlen(texto) + 1);
    if (copia != NULL) {
        strcpy(copia, texto);
    }
    return copia;
}

/* Marca la causa como resuelta mediante acuerdo reparatorio (Art. 241 CPP) */
void proponerAcuerdoReparatorio(struct Causa *causa) {
    if (causa == NULL) {
        return;
    }
    causa->estado = copiarCadena("acuerdo reparatorio");
    printf("La causa fue resuelta mediante un acuerdo reparatorio.\n");
}

int esInterviniente(int tipoPersona) {
    if (tipoPersona == 1 || tipoPersona == 2 || tipoPersona == 5) {
        return 1; // Denunciante, Imputado o Fiscal
    }
    return 0;
}

/* Busca una persona en la lista por su RUT y retorna un puntero a la persona */
struct Persona *buscarPersonaPorRut(struct NodoPersona *listaPersonas, char *rutBuscado) {
    struct NodoPersona *actual;

    /* Recorrer la lista */
    actual = listaPersonas;
    while (actual != NULL) {
        /* Comparar RUT */
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            return actual->datosPersona;
        }
        actual = actual->sig;
    }

    /* Si no se encontro coincidencia */
    return NULL;
}

/* ======================== PUNTO 1: RECEPCION DE LA DENUNCIA ======================== */

/* Crea una nueva causa penal con RUC, categoria y estado */
struct Causa *crearCausa(char *ruc, char *categoria, char *estado) {
    struct Causa *nuevaCausa;

    /* Reservar memoria para la nueva causa */
    nuevaCausa = (struct Causa *) malloc(sizeof(struct Causa));
    if (nuevaCausa == NULL) {
        return NULL;
    }

    /* Asignar los valores iniciales */
    nuevaCausa->RUC = copiarCadena(ruc);
    nuevaCausa->CategoriaCausa = copiarCadena(categoria);
    nuevaCausa->estado = copiarCadena(estado);

    /* Inicializar los punteros internos */
    nuevaCausa->carpetaInvestigativa = NULL;
    nuevaCausa->fiscalEncargado = NULL;
    nuevaCausa->victimas = NULL;
    nuevaCausa->testigos = NULL;
    nuevaCausa->imputadosAsociados = NULL;

    return nuevaCausa;
}

/* Agrega una causa a la lista de causas del Ministerio Publico */
void agregarCausa(struct NodoCausa **listaCausas, struct Causa *nuevaCausa) {
    struct NodoCausa *nuevoNodo;

    /* Reservar memoria para el nuevo nodo */
    nuevoNodo = (struct NodoCausa *) malloc(sizeof(struct NodoCausa));
    if (nuevoNodo == NULL) {
        return;
    }

    /* Enlazar la nueva causa al inicio de la lista */
    nuevoNodo->datosCausa = nuevaCausa;
    nuevoNodo->sig = *listaCausas;
    *listaCausas = nuevoNodo;
}
/* Muestra todas las causas registradas */
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

/* Muestra los datos de una causa que coincida con el RUC buscado */
void mostrarCausaPorRUC(struct NodoCausa *listaCausas, char *rucBuscado) {
    struct NodoCausa *nodoActual;

    nodoActual = listaCausas;

    /* Recorrer la lista de causas */
    while (nodoActual != NULL) {
        /* Comparar RUC */
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            /* Mostrar los datos si se encuentra */
            printf("Causa encontrada:\n");
            printf("RUC: %s\n", nodoActual->datosCausa->RUC);
            printf("Categoria: %s\n", nodoActual->datosCausa->CategoriaCausa);
            printf("Estado: %s\n", nodoActual->datosCausa->estado);
            return;
        }
        nodoActual = nodoActual->sig;
    }

    /* Si no se encontro coincidencia */
    printf("No se encontro una causa con ese RUC.\n");
}

/* Modifica la categoria y el estado de una causa buscandola por RUC */
void modificarCausa(struct NodoCausa *listaCausas, char *rucBuscado, char *nuevaCategoria, char *nuevoEstado) {
    struct NodoCausa *nodoActual;

    nodoActual = listaCausas;

    /* Buscar la causa con el RUC indicado */
    while (nodoActual != NULL) {
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            /* Asignar nuevos valores */
            nodoActual->datosCausa->CategoriaCausa = copiarCadena(nuevaCategoria);
            nodoActual->datosCausa->estado = copiarCadena(nuevoEstado);
            printf("Causa modificada correctamente.\n");
            return;
        }
        nodoActual = nodoActual->sig;
    }

    /* Si no se encontro la causa */
    printf("No se encontro una causa con ese RUC.\n");
}

/* Actualiza el estado de una causa según su RUC */
void cambiarEstadoCausa(struct NodoCausa *listaCausas, char *rucBuscado, char *nuevoEstado) {
    struct NodoCausa *actual;

    actual = listaCausas;
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            actual->datosCausa->estado = copiarCadena(nuevoEstado);
            printf("Estado de la causa actualizado a: %s\n", nuevoEstado);
            return;
        }
        actual = actual->sig;
    }

    printf("No se encontro una causa con ese RUC.\n");
}

/* Elimina una causa de la lista enlazada segun su RUC */
void eliminarCausaPorRUC(struct NodoCausa **listaCausas, char *rucBuscado) {
    struct NodoCausa *nodoActual;
    struct NodoCausa *nodoAnterior;

    /* Inicio de la lista */
    nodoActual = *listaCausas;
    nodoAnterior = NULL;

    while (nodoActual != NULL) {
        /* Comparar el RUC de la causa actual */
        if (strcmp(nodoActual->datosCausa->RUC, rucBuscado) == 0) {
            /* Si es el primer nodo */
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

    printf("No se encontro una causa con ese RUC.\n");
}

/* Crea una nueva carpeta investigativa y la asigna a la causa */
void crearCarpetaInvestigativa(struct Causa *causa) {
    struct CarpetaInvestigativa *nuevaCarpeta;

    /* Reservar memoria para la nueva carpeta */
    nuevaCarpeta = (struct CarpetaInvestigativa *) malloc(sizeof(struct CarpetaInvestigativa));
    if (nuevaCarpeta == NULL) {
        return;
    }

    /* Inicializar los campos de la carpeta */
    nuevaCarpeta->objetos = NULL;
    nuevaCarpeta->totalDenuncias = 0;

    /* Asociar la carpeta a la causa */
    causa->carpetaInvestigativa = nuevaCarpeta;
}

/* Registra una denuncia dentro del arreglo de la carpeta investigativa */
void registrarDenuncia(struct CarpetaInvestigativa *carpeta, char *rutDenunciante, char *descripcion, char *fecha, int id) {
    struct ObjetoInvestigativo *nuevaDenuncia;

    if (carpeta == NULL || carpeta->totalDenuncias >= MAX_DENUNCIAS) {
        printf("Error: Carpeta no valida o limite de denuncias alcanzado.\n");
        return;
    }

    /* Reservar memoria para la denuncia */
    nuevaDenuncia = (struct ObjetoInvestigativo *) malloc(sizeof(struct ObjetoInvestigativo));
    if (nuevaDenuncia == NULL) {
        return;
    }

    /* Asignar los datos de la denuncia */
    nuevaDenuncia->id = id;
    nuevaDenuncia->rut = copiarCadena(rutDenunciante);
    nuevaDenuncia->detalle = copiarCadena(descripcion);
    nuevaDenuncia->fecha = copiarCadena(fecha);
    nuevaDenuncia->tipo = 1;
    nuevaDenuncia->sentenciaFinal = 0;

    /* Agregar al arreglo de denuncias recientes */
    carpeta->denunciasRecientes[carpeta->totalDenuncias] = nuevaDenuncia;
    carpeta->totalDenuncias++;
}

/* Registra un nuevo objeto investigativo en la lista circular de la carpeta */
void agregarObjetoInvestigativo(struct CarpetaInvestigativa *carpeta, int tipo, char *rut, char *detalle, char *fecha) {
    struct ObjetoInvestigativo *nuevoObjeto;
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

    /* Validar carpeta */
    if (carpeta == NULL) {
        printf("Error: Carpeta investigativa no valida.\n");
        return;
    }

    /* Reservar memoria para el objeto y el nodo */
    nuevoObjeto = (struct ObjetoInvestigativo *) malloc(sizeof(struct ObjetoInvestigativo));
    nuevoNodo = (struct NodoObjetoInvestigativo *) malloc(sizeof(struct NodoObjetoInvestigativo));

    if (nuevoObjeto == NULL || nuevoNodo == NULL) {
        return;
    }

    /* Asignar datos al objeto */
    nuevoObjeto->id = carpeta->totalDenuncias + 1;
    nuevoObjeto->rut = copiarCadena(rut);
    nuevoObjeto->detalle = copiarCadena(detalle);
    nuevoObjeto->fecha = copiarCadena(fecha);
    nuevoObjeto->tipo = tipo;
    nuevoObjeto->sentenciaFinal = 0;

    /* Asociar el objeto al nodo */
    nuevoNodo->objeto = nuevoObjeto;

    /* Insertar en la lista circular */
    if (carpeta->objetos == NULL) {
        nuevoNodo->sig = nuevoNodo;
        carpeta->objetos = nuevoNodo;
    } else {
        actual = carpeta->objetos;
        while (actual->sig != carpeta->objetos) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->sig = carpeta->objetos;
    }
}

/* ======================== PUNTO 2: CARPETA INVESTIGATIVA ======================== */

/* Crea un nuevo objeto investigativo con los datos entregados */
struct ObjetoInvestigativo *crearObjetoInvestigativo(int id, char *fecha, char *rut, char *detalle, int tipo) {
    struct ObjetoInvestigativo *nuevo;

    /* Reservar memoria para el objeto investigativo */
    nuevo = (struct ObjetoInvestigativo *) malloc(sizeof(struct ObjetoInvestigativo));
    if (nuevo == NULL) {
        return NULL;
    }

    /* Asignar los valores al objeto */
    nuevo->id = id;
    nuevo->fecha = copiarCadena(fecha);
    nuevo->rut = copiarCadena(rut);
    nuevo->detalle = copiarCadena(detalle);
    nuevo->tipo = tipo;
    nuevo->sentenciaFinal = 0;

    return nuevo;
}

/* Agrega un objeto investigativo al final de la lista circular */
void agregarObjeto(struct CarpetaInvestigativa *carpeta, struct ObjetoInvestigativo *objeto) {
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

    /* Crear un nuevo nodo con el objeto entregado */
    nuevoNodo = (struct NodoObjetoInvestigativo *) malloc(sizeof(struct NodoObjetoInvestigativo));
    if (nuevoNodo == NULL) {
        return;
    }
    nuevoNodo->objeto = objeto;

    if (carpeta->objetos == NULL) {
        /* Si la lista esta vacia, el nuevo nodo apunta a si mismo */
        nuevoNodo->sig = nuevoNodo;
        carpeta->objetos = nuevoNodo;
    } else {
        /* Insertar al final de la lista circular */
        actual = carpeta->objetos;
        while (actual->sig != carpeta->objetos) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
        nuevoNodo->sig = carpeta->objetos;
    }
}

/* Solicita datos al usuario y agrega un objeto investigativo a la carpeta */
void agregarObjetoPorTipo(struct CarpetaInvestigativa *carpeta) {
    int id;
    int tipo;
    char fecha[30];
    char rut[20];
    char detalle[200];
    struct ObjetoInvestigativo *nuevo;

    /* Verifica que la carpeta investigativa sea valida */
    if (carpeta == NULL) {
        printf("La carpeta investigativa no es valida.\n");
        return;
    }

    /* Entrada de datos */
    printf("Ingrese ID del objeto: ");
    scanf("%d", &id);
    getchar(); /* Limpiar salto de linea */

    printf("Fecha (dd-mm-aaaa): ");
    leerCadena(fecha, sizeof(fecha));

    printf("RUT de la persona que genero el objeto: ");
    leerCadena(rut, sizeof(rut));

    printf("Detalle del objeto: ");
    leerCadena(detalle, sizeof(detalle));

    printf("Tipo de objeto:\n");
    printf("1 = Denuncia\n2 = Prueba\n3 = Diligencia\n4 = Declaracion\n5 = Resolucion Judicial\n");
    printf("Seleccione tipo: ");
    scanf("%d", &tipo);
    getchar(); /* Limpiar salto de linea */

    if (tipo < 1 || tipo > 5) {
        printf("Tipo invalido. Debe ser un numero entre 1 y 5.\n");
        return;
    }

    /* Crear y agregar objeto a la carpeta */
    nuevo = crearObjetoInvestigativo(id, fecha, rut, detalle, tipo);
    agregarObjeto(carpeta, nuevo);

    printf("Objeto agregado correctamente a la carpeta.\n");
}

/* Muestra los datos de un objeto investigativo segun su ID */
void listarObjetosPorID(struct NodoObjetoInvestigativo *head, int idBuscado) {
    int encontrado;

    encontrado = 0;

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

    if (encontrado == 0) {
        printf("No se encontro ningun objeto con el ID %d.\n", idBuscado);
    }
}

/* Busca un objeto investigativo por ID y retorna su puntero */
struct ObjetoInvestigativo *buscarObjetoPorId(struct NodoObjetoInvestigativo *listaObjetos, int idBuscado) {
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

/* Modifica el detalle y tipo de un objeto segun su ID */
void modificarDatosObjeto(struct NodoObjetoInvestigativo *listaObjetos, int idBuscado, char *nuevoDetalle, int nuevoTipo) {
    struct ObjetoInvestigativo *objeto;

    objeto = buscarObjetoPorId(listaObjetos, idBuscado);
    if (objeto != NULL) {
        objeto->detalle = copiarCadena(nuevoDetalle);
        objeto->tipo = nuevoTipo;
        printf("Objeto investigativo modificado correctamente.\n");
    } else {
        printf("No se encontro un objeto con ese ID.\n");
    }
}

/* Elimina un objeto investigativo de la lista circular segun su ID */
void eliminarObjetoPorId(struct NodoObjetoInvestigativo **listaObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *anterior;

    /* Validar si la lista esta vacia */
    if (listaObjetos == NULL || *listaObjetos == NULL) {
        printf("No hay objetos registrados.\n");
        return;
    }

    actual = *listaObjetos;
    anterior = NULL;

    do {
        if (actual->objeto->id == idBuscado) {
            /* Caso: unico nodo */
            if (actual->sig == actual) {
                *listaObjetos = NULL;
            }
            /* Caso: eliminar cabeza */
            else if (actual == *listaObjetos) {
                struct NodoObjetoInvestigativo *ultimo = *listaObjetos;
                while (ultimo->sig != *listaObjetos) {
                    ultimo = ultimo->sig;
                }
                ultimo->sig = actual->sig;
                *listaObjetos = actual->sig;
            }
            /* Caso general */
            else {
                anterior->sig = actual->sig;
            }

            printf("Objeto investigativo eliminado correctamente.\n");
            return;
        }

        anterior = actual;
        actual = actual->sig;
    } while (actual != *listaObjetos);

    printf("No se encontro un objeto con ese ID.\n");
}

/* Muestra todos los objetos investigativos registrados en la carpeta */
void revisarCarpetaInvestigativa(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta->objetos == NULL) {
        printf("\nNo hay objetos investigativos registrados.\n");
        return;
    }

    printf("\n--- Contenido de la carpeta investigativa ---\n");
    actual = carpeta->objetos;
    do {
        printf("ID: %d | Tipo: %d | Fecha: %s | RUT: %s\n",
               actual->objeto->id,
               actual->objeto->tipo,
               actual->objeto->fecha,
               actual->objeto->rut);
        printf("Detalle: %s\n\n", actual->objeto->detalle);
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

void solicitarRevisionCarpeta(struct Persona *solicitante, struct CarpetaInvestigativa *carpeta) {
    if (solicitante == NULL || carpeta == NULL) {
        printf("\nSolicitud invalida.\n");
        return;
    }

    if (esInterviniente(solicitante->tipo)) {
        printf("\nAcceso autorizado para %s (RUT: %s).\n", solicitante->nombre, solicitante->rut);
        revisarCarpetaInvestigativa(carpeta);
    } else {
        printf("\nAcceso denegado. Usted no tiene permisos para revisar la carpeta investigativa.\n");
    }
}

/* ======================== PUNTO 3: FORMALIZACION DE IMPUTADOS ======================== */

/* Crea una nueva persona con nombre, RUT y tipo */
struct Persona *crearPersona(char *nombre, char *rut, int tipo) {
    struct Persona *nueva;

    /* Validar entrada */
    if (nombre == NULL || rut == NULL) {
        printf("Datos de persona invalidos.\n");
        return NULL;
    }

    /* Reservar memoria para la persona */
    nueva = (struct Persona *) malloc(sizeof(struct Persona));
    if (nueva == NULL) {
        return NULL;
    }

    /* Asignar los datos */
    nueva->nombre = copiarCadena(nombre);
    nueva->rut = copiarCadena(rut);
    nueva->tipo = tipo;

    return nueva;
}

/* Agrega una persona al inicio de la lista enlazada */
void agregarPersona(struct NodoPersona **head, struct Persona *persona) {
    struct NodoPersona *nuevoNodo;

    if (persona == NULL) {
        printf("Persona invalida.\n");
        return;
    }

    nuevoNodo = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nuevoNodo == NULL) {
        return;
    }

    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = *head;
    *head = nuevoNodo;
}

/* Busca un imputado en el ABB por su RUT */
struct NodoImputadoABB *buscarImputadoPorRut(struct NodoImputadoABB *raiz, char *rutBuscado) {
    int comparacion;

    if (raiz == NULL || rutBuscado == NULL) {
        return NULL;
    }

    comparacion = strcmp(rutBuscado, raiz->datosImputados->datosPersona->rut);

    if (comparacion == 0) {
        return raiz;
    } else if (comparacion < 0) {
        return buscarImputadoPorRut(raiz->izq, rutBuscado);
    } else {
        return buscarImputadoPorRut(raiz->der, rutBuscado);
    }
}

/* Muestra los datos de una persona segun su RUT */
void listarPersonaPorRut(struct NodoPersona *lista, char *rut) {
    struct Persona *actual;

    /* Buscar persona en la lista */
    actual = buscarPersonaPorRut(lista, rut);

    if (actual == NULL) {
        printf("No se encontro una persona con ese RUT.\n");
        return;
    }

    /* Mostrar los datos de la persona encontrada */
    printf("Datos de la persona:\n");
    printf("Nombre: %s\n", actual->nombre);
    printf("RUT: %s\n", actual->rut);
    printf("Tipo: %d\n", actual->tipo);
}

/* Modifica el nombre y tipo de una persona segun su RUT */
void modificarDatosPersona(struct NodoPersona *listaPersonas, char *rutBuscado, char *nuevoNombre, int nuevoTipo) {
    struct Persona *persona;

    /* Buscar persona por RUT */
    persona = buscarPersonaPorRut(listaPersonas, rutBuscado);

    if (persona != NULL) {
        /* Asignar nuevo nombre y tipo */
        persona->nombre = copiarCadena(nuevoNombre);
        persona->tipo = nuevoTipo;
        printf("Persona modificada correctamente.\n");
    } else {
        printf("No se encontro una persona con ese RUT.\n");
    }
}

/* Elimina una persona de la lista enlazada segun su RUT */
void eliminarPersonaPorRut(struct NodoPersona **listaPersonas, char *rutBuscado) {
    struct NodoPersona *actual;
    struct NodoPersona *anterior;

    /* Iniciar desde el comienzo de la lista */
    actual = *listaPersonas;
    anterior = NULL;

    /* Recorrer la lista mientras queden nodos */
    while (actual != NULL) {
        /* Comparar el RUT del nodo actual con el buscado */
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            /* Si es el primer nodo */
            if (anterior == NULL) {
                *listaPersonas = actual->sig;
            } else {
                /* Si no es el primero, ajustar el enlace del anterior */
                anterior->sig = actual->sig;
            }

            /* No se libera memoria segun la politica del proyecto */
            printf("Persona eliminada correctamente.\n");
            return;
        }

        /* Avanzar en la lista */
        anterior = actual;
        actual = actual->sig;
    }

    /* Si no se encontro la persona */
    printf("No se encontro una persona con ese RUT.\n");
}

/* Escribe en el buffer una cadena segun el tipo de persona */
void obtenerNombreTipoPersona(int tipo, char *buffer) {
    if (tipo == 1) {
        strcpy(buffer, "Denunciante");
    } else if (tipo == 2) {
        strcpy(buffer, "Imputado");
    } else if (tipo == 3) {
        strcpy(buffer, "Testigo");
    } else if (tipo == 4) {
        strcpy(buffer, "Juez");
    } else if (tipo == 5) {
        strcpy(buffer, "Fiscal");
    } else {
        strcpy(buffer, "Desconocido");
    }
}

/* Asocia un imputado a una causa penal agregándolo a la lista de imputados */
void asociarImputadoACausa(struct Causa *causa, struct Persona *imputado) {
    struct NodoPersona *nuevoNodo;

    /* Reservar memoria para el nodo que contendrá al imputado */
    nuevoNodo = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nuevoNodo == NULL) return;

    /* Asignar el imputado al nodo y enlazarlo a la lista de la causa */
    nuevoNodo->datosPersona = imputado;
    nuevoNodo->sig = causa->imputadosAsociados;
    causa->imputadosAsociados = nuevoNodo;
}

/* Inserta un imputado en el arbol binario de busqueda segun su RUT */
struct NodoImputadoABB *insertarImputadoABB(struct NodoImputadoABB *raiz, struct Persona *personaImputado) {
    struct NodoImputadoABB *nuevoNodo;

    if (raiz == NULL) {
        /* Crear nuevo nodo si el arbol esta vacio */
        nuevoNodo = (struct NodoImputadoABB *) malloc(sizeof(struct NodoImputadoABB));
        if (nuevoNodo == NULL) {
            return NULL;
        }
        nuevoNodo->datosImputados = personaImputado;
        nuevoNodo->izq = NULL;
        nuevoNodo->der = NULL;
        nuevoNodo->formalizacion = NULL;
        return nuevoNodo;
    }

    /* Comparar RUT para insertar a la izquierda o derecha */
    if (strcmp(personaImputado->rut, raiz->datosImputados->datosPersona->rut) < 0) {
        raiz->izq = insertarImputadoABB(raiz->izq, personaImputado);
    } else {
        raiz->der = insertarImputadoABB(raiz->der, personaImputado);
    }

    return raiz;
}

/* Inserta un nuevo imputado en el arbol ABB segun su RUT */
struct NodoImputadoABB* insertarImputado(struct NodoImputadoABB *raizActual, struct NodoImputadoABB *nuevoImputado) {
    int comparacion;

    if (raizActual == NULL) {
        return nuevoImputado;
    }

    comparacion = strcmp(nuevoImputado->datosImputados->datosPersona->rut,
                         raizActual->datosImputados->datosPersona->rut);

    if (comparacion < 0) {
        raizActual->izq = insertarImputado(raizActual->izq, nuevoImputado);
    } else if (comparacion > 0) {
        raizActual->der = insertarImputado(raizActual->der, nuevoImputado);
    } else {
        printf("Ya existe un imputado con ese RUT.\n");
    }

    return raizActual;
}
/* Crea y asigna una formalizacion judicial al imputado del nodo */
void formalizarImputado(struct NodoImputadoABB *imputados, char *delito, char *antecedentes, char *fecha, int medidaCautelar) {
    struct Formalizacion *nuevaFormalizacion;

    /* Reservar memoria para la formalizacion */
    nuevaFormalizacion = (struct Formalizacion *) malloc(sizeof(struct Formalizacion));
    if (nuevaFormalizacion == NULL) {
        return;
    }

    /* Asignar los datos de la formalizacion */
    nuevaFormalizacion->delito = copiarCadena(delito);
    nuevaFormalizacion->antecedentes = copiarCadena(antecedentes);
    nuevaFormalizacion->fecha = copiarCadena(fecha);
    nuevaFormalizacion->medidaCautelar = medidaCautelar;

    /* Asociar formalizacion al nodo de imputado */
    imputados->formalizacion = nuevaFormalizacion;
}

/* Formaliza a una persona como imputado, asignándole un defensor y registrando la formalización en el ABB y carpeta */
void formalizarConDefensor(struct MinisterioPublico *ministerio,
                           struct Causa *causa,
                           char *nombreImputado, char *rutImputado,
                           char *nombreDefensor, char *rutDefensor,
                           char *delito, char *antecedentes,
                           char *fecha, int medidaCautelar) {
    struct Persona *imputado;
    struct Persona *defensor;
    struct Formalizacion *infoFormalizacion;
    struct NodoImputadoABB *nuevoNodo;
    struct NodoPersona *nuevoNodoPersona;

    /* Validar datos de entrada */
    if (ministerio == NULL || causa == NULL || causa->carpetaInvestigativa == NULL ||
        nombreImputado == NULL || rutImputado == NULL ||
        nombreDefensor == NULL || rutDefensor == NULL ||
        delito == NULL || antecedentes == NULL || fecha == NULL) {
        printf("Error: Datos insuficientes o nulos.\n");
        return;
    }

    /* Verificar si el imputado ya fue formalizado */
    if (buscarImputadoPorRut(ministerio->raizImputados, rutImputado) != NULL) {
        printf("El imputado ya fue formalizado anteriormente.\n");
        return;
    }

    /* Crear imputado y defensor */
    imputado = crearPersona(nombreImputado, rutImputado, 2);
    defensor = crearPersona(nombreDefensor, rutDefensor, 6);

    /* Reservar memoria para la formalizacion */
    infoFormalizacion = (struct Formalizacion *) malloc(sizeof(struct Formalizacion));
    if (infoFormalizacion == NULL) {
        printf("Error de memoria para formalizacion.\n");
        return;
    }

    /* Asignar los datos */
    infoFormalizacion->delito = copiarCadena(delito);
    infoFormalizacion->antecedentes = copiarCadena(antecedentes);
    infoFormalizacion->fecha = copiarCadena(fecha);
    infoFormalizacion->medidaCautelar = medidaCautelar;
    infoFormalizacion->defensor = defensor;

    /* Crear nodo ABB */
    nuevoNodo = (struct NodoImputadoABB *) malloc(sizeof(struct NodoImputadoABB));
    if (nuevoNodo == NULL) {
        printf("Error de memoria para nodo imputado.\n");
        return;
    }

    /* Crear nodo lista */
    nuevoNodoPersona = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nuevoNodoPersona == NULL) {
        printf("Error de memoria para nodo persona.\n");
        return;
    }

    /* Asignar valores a los nodos */
    nuevoNodoPersona->datosPersona = imputado;
    nuevoNodoPersona->sig = NULL;

    nuevoNodo->datosImputados = nuevoNodoPersona;
    nuevoNodo->formalizacion = infoFormalizacion;
    nuevoNodo->izq = NULL;
    nuevoNodo->der = NULL;

    /* Insertar en el arbol */
    ministerio->raizImputados = insertarImputado(ministerio->raizImputados, nuevoNodo);

    /* Asociar a la causa */
    nuevoNodoPersona->sig = causa->imputadosAsociados;
    causa->imputadosAsociados = nuevoNodoPersona;

    /* Registrar resolucion */
    agregarObjetoInvestigativo(causa->carpetaInvestigativa, 5, rutImputado, "Audiencia de formalizacion realizada.", fecha);

    printf("Formalizacion registrada y defensor asignado.\n");
}

/* Asigna un defensor a una formalización existente */
void asignarDefensor(struct Formalizacion *formalizacion, struct Persona *defensor) {
    if (formalizacion != NULL && defensor != NULL) {
        defensor->tipo = 6;
        formalizacion->defensor = defensor;
        printf("Defensor asignado correctamente.\n");
    } else {
        printf("Error: Formalizacion o defensor invalido.\n");
    }
}

/* Muestra los datos de la formalizacion judicial de un imputado */
void mostrarFormalizacion(struct NodoImputadoABB *imputados) {
    if (imputados == NULL || imputados->formalizacion == NULL) {
        printf("No hay formalizacion registrada para este imputado.\n");
        return;
    }

    struct Formalizacion *formalizacion = imputados->formalizacion;
    printf("\n--- Datos de Formalizacion ---\n");
    printf("Delito: %s\n", formalizacion->delito);
    printf("Fecha: %s\n", formalizacion->fecha);
    printf("Antecedentes: %s\n", formalizacion->antecedentes);
    printf("Medida Cautelar: %d\n", formalizacion->medidaCautelar);
    if (formalizacion->defensor != NULL) {
        printf("Defensor: %s (RUT: %s)\n", formalizacion->defensor->nombre, formalizacion->defensor->rut);
    }
    printf("-----------------------------\n");
}

/* Cambia la medida cautelar asignada en la formalización */
void cambiarMedidaCautelar(struct NodoImputadoABB *imputados, int nuevaMedida) {
    if (imputados == NULL || imputados->formalizacion == NULL) {
        printf("No se puede cambiar la medida cautelar. Falta formalizacion.\n");
        return;
    }

    imputados->formalizacion->medidaCautelar = nuevaMedida;
    printf("Medida cautelar actualizada a %d\n", nuevaMedida);
}

/* ======================== PUNTO 4: DESARROLLO DE LA INVESTIGACION ======================== */

/* Registra una diligencia adicional como objeto investigativo en la carpeta */
void solicitarDiligencia(struct CarpetaInvestigativa *carpeta, char *rutSolicitante, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 3, rutSolicitante, descripcion, fecha);
}

/* Registra una inspección como diligencia tipo 3 en la carpeta investigativa */
void registrarInspeccion(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 3, rut, descripcion, fecha);
    printf("Inspeccion registrada correctamente.\n");
}

/* Registra una citacion como declaracion (tipo 4) */
void registrarCitacion(struct CarpetaInvestigativa *carpeta, char *rut, char *fecha, char *detalle) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 4, rut, detalle, fecha);
    printf("Citacion registrada en la carpeta investigativa.\n");
}

/* Simula una citacion como declaracion en la carpeta */
void citarImputado(struct CarpetaInvestigativa *carpeta, char *rutImputado, char *fecha, char *detalle) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 4, rutImputado, detalle, fecha);
    printf("Citacion registrada para el imputado %s.\n", rutImputado);
}

/* Registra un peritaje como objeto tipo 2 en la carpeta investigativa */
void registrarPeritaje(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 2, rut, descripcion, fecha);
    printf("Peritaje registrado correctamente.\n");
}

/* Registra una medida de proteccion como objeto investigativo en la carpeta */
void aplicarMedidaProteccion(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 4, rut, descripcion, fecha);
}

/* Registra una sentencia como objeto investigativo en la carpeta */
void registrarSentencia(struct CarpetaInvestigativa *carpeta, char *rut, char *detalle, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 5, rut, detalle, fecha);
}

/* Registra una orden de detencion como resolucion judicial (tipo 5) */
void registrarOrdenDetencion(struct CarpetaInvestigativa *carpeta, char *rut, char *fecha, char *detalle) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 5, rut, detalle, fecha);
    printf("Orden de detencion registrada en la carpeta investigativa.\n");
}

/* Registra una orden de detencion como resolucion judicial */
void emitirOrdenDetencion(struct CarpetaInvestigativa *carpeta, char *rutImputado, char *fecha, char *detalle) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 5, rutImputado, detalle, fecha);
    printf("Orden de detencion registrada para el imputado %s.\n", rutImputado);
}

/* Registra una audiencia de formalizacion como resolucion judicial en la carpeta investigativa */
void registrarAudienciaFormalizacion(struct CarpetaInvestigativa *carpeta, char *rut, char *fecha, char *detalle) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 5, rut, detalle, fecha);
    printf("Audiencia de formalizacion registrada en la carpeta.\n");
}

/* Muestra todas las pruebas registradas en la carpeta investigativa */
void presentarPruebas(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("\nNo hay pruebas registradas.\n");
        return;
    }

    printf("\n--- Pruebas registradas ---\n");
    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 2) {
            printf("Fecha: %s | RUT: %s\n", actual->objeto->fecha, actual->objeto->rut);
            printf("Detalle: %s\n\n", actual->objeto->detalle);
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

/* ======================== PUNTO 5: CIERRE INVESTIGACION Y ACUSACION ======================== */

/* Cambia el estado de una causa según la decisión del usuario */
void cerrarInvestigacion(struct Causa *causa, char *nuevoEstado) {
    if (causa == NULL) {
        return;
    }
    causa->estado = copiarCadena(nuevoEstado);
    printf("La causa ha sido actualizada al estado: %s\n", nuevoEstado);
}

/* Cambia el estado de una causa a "no perseverar" según decisión del fiscal */
void marcarNoPerseverar(struct Causa *causa) {
    if (causa == NULL) {
        return;
    }
    causa->estado = copiarCadena("no perseverar");
    printf("La causa ha sido marcada como 'no perseverar'.\n");
}

/* Reabre una causa si su estado actual es "archivo" o "cerrada" */
void reAbrirCausa(struct NodoCausa *listaCausas, char *rucBuscado) {
    struct NodoCausa *actual;

    /* Validar entrada */
    if (listaCausas == NULL || rucBuscado == NULL) {
        return;
    }

    actual = listaCausas;

    while (actual != NULL) {
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            if (strcmp(actual->datosCausa->estado, "archivo") == 0 || strcmp(actual->datosCausa->estado, "cerrada") == 0) {
                actual->datosCausa->estado = copiarCadena("abierta");
                printf("La causa ha sido reabierta correctamente.\n");
                return;
            } else {
                printf("La causa no esta cerrada ni archivada, no se puede reabrir.\n");
                return;
            }
        }
        actual = actual->sig;
    }

    printf("No se encontro una causa con ese RUC.\n");
}

/* Marca la causa como sobreseída de forma definitiva (Art. 250 CPP) */
void proponerSobreseimientoDefinitivo(struct Causa *causa) {
    if (causa == NULL) {
        return;
    }
    causa->estado = copiarCadena("sobreseimiento definitivo");
    printf("La causa ha sido sobreseida de forma definitiva.\n");
}

/* Marca la causa como sobreseída de forma temporal (Art. 251 CPP) */
void proponerSobreseimientoTemporal(struct Causa *causa) {
    if (causa == NULL) {
        return;
    }
    causa->estado = copiarCadena("sobreseimiento temporal");
    printf("La causa ha sido sobreseida de forma temporal.\n");
}

/* ======================== PUNTO 6: JUICIO ORAL, SENTENCIAS Y RESOLUCION ======================== */

void registrarSentenciaFinal(struct CarpetaInvestigativa *carpeta, char *rutImputado, char *fecha, int tipoSentencia) {
    struct ObjetoInvestigativo *sentencia;
    char *detalle;
    int id;

    if (carpeta == NULL) {
        printf("Carpeta no valida.\n");
        return;
    }

    id = carpeta->totalDenuncias + 1;

    if (tipoSentencia == 1) {
        detalle = copiarCadena("Sentencia condenatoria emitida por el tribunal.");
    } else if (tipoSentencia == 2) {
        detalle = copiarCadena("Sentencia absolutoria emitida por el tribunal.");
    } else {
        printf("Tipo de sentencia no valido.\n");
        return;
    }

    sentencia = crearObjetoInvestigativo(id, fecha, rutImputado, detalle, 5);
    if (sentencia == NULL) {
        printf("Error al crear la sentencia.\n");
        return;
    }

    sentencia->sentenciaFinal = 1;
    agregarObjeto(carpeta, sentencia);

    printf("Sentencia registrada correctamente.\n");
}

/* Lista todas las resoluciones judiciales (tipo 5) contenidas en la carpeta investigativa */
void listarResolucionesJudiciales(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("No hay resoluciones judiciales registradas.\n");
        return;
    }

    printf("\n--- Resoluciones Judiciales ---\n");
    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 5) {
            printf("Fecha: %s | RUT: %s\n", actual->objeto->fecha, actual->objeto->rut);
            printf("Detalle: %s\n\n", actual->objeto->detalle);
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

/* Simula la realizacion de un juicio oral mostrando pruebas y testigos */
void realizarJuicioOral(struct Causa *causa) {
    struct NodoPersona *testigoActual;

    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("No hay causa o carpeta disponible para el juicio oral.\n");
        return;
    }

    printf("\n===== JUICIO ORAL =====\n");
    printf("Fiscal a cargo: %s\n", causa->fiscalEncargado != NULL ? causa->fiscalEncargado->nombre : "No asignado");

    /* Mostrar pruebas */
    presentarPruebas(causa->carpetaInvestigativa);

    /* Declaracion de testigos */
    printf("\n--- Testigos ---\n");
    testigoActual = causa->testigos;
    while (testigoActual != NULL) {
        printf("Nombre: %s | RUT: %s\n", testigoActual->datosPersona->nombre, testigoActual->datosPersona->rut);
        testigoActual = testigoActual->sig;
    }

    printf("\n===== FIN DEL JUICIO ORAL =====\n");
}

    /* Lista solo las sentencias finales registradas en la carpeta investigativa */
    void listarSentenciasFinales(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("No hay sentencias finales registradas.\n");
        return;
    }

    printf("\n--- Sentencias Finales Registradas ---\n");
    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 5 && actual->objeto->sentenciaFinal == 1) {
            printf("Fecha: %s | RUT: %s\n", actual->objeto->fecha, actual->objeto->rut);
            printf("Detalle: %s\n\n", actual->objeto->detalle);
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

/* ======================== PUNTO 7: PROTECCION DE VICTIMAS Y TESTIGOS ======================== */

/* Ejecuta la sentencia final de una causa si existe, y actualiza su estado */
void ejecutarSentenciaFinal(struct Causa *causa, char *fechaEjecucion) {
    struct NodoObjetoInvestigativo *actual;

    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("Causa o carpeta investigativa no valida.\n");
        return;
    }

    actual = causa->carpetaInvestigativa->objetos;
    if (actual == NULL) {
        printf("No hay resoluciones en la carpeta.\n");
        return;
    }

    do {
        if (actual->objeto->tipo == 5 && actual->objeto->sentenciaFinal == 1) {
            causa->estado = copiarCadena("cerrada");
            agregarObjetoInvestigativo(causa->carpetaInvestigativa, 5, actual->objeto->rut, "Sentencia ejecutada, causa cerrada.", fechaEjecucion);
            printf("Sentencia ejecutada. Causa marcada como cerrada.\n");
            return;
        }
        actual = actual->sig;
    } while (actual != causa->carpetaInvestigativa->objetos);

    printf("No se encontro una sentencia final para ejecutar.\n");
}

/* Archiva una causa ya cerrada, registrando la resolucion en la carpeta investigativa */
void archivarCausa(struct Causa *causa, char *fechaArchivo) {
    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("Causa o carpeta investigativa no valida.\n");
        return;
    }

    if (strcmp(causa->estado, "cerrada") != 0) {
        printf("La causa no esta cerrada. No se puede archivar.\n");
        return;
    }

    causa->estado = copiarCadena("archivo");
    agregarObjetoInvestigativo(causa->carpetaInvestigativa, 5, "-", "Causa archivada tras ejecucion de sentencia.", fechaArchivo);
    printf("La causa ha sido archivada correctamente.\n");
}

/* Verifica si la carpeta investigativa contiene al menos una sentencia final */
int tieneSentenciaFinal(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        return 0;
    }

    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 5 && actual->objeto->sentenciaFinal == 1) {
            return 1;
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);

    return 0;
}

/* Muestra el historial de resoluciones judiciales de la causa */
void mostrarHistorialResoluciones(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("No hay resoluciones judiciales registradas.\n");
        return;
    }

    printf("\n--- Historial de Resoluciones Judiciales ---\n");
    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 5) {
            printf("Fecha: %s | RUT: %s\n", actual->objeto->fecha, actual->objeto->rut);
            printf("Detalle: %s\n\n", actual->objeto->detalle);
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

/* ======================== FUNCIONES MENU ======================== */

/* Muestra a todas las personas del tipo especificado en la lista */
void declararPersonasPorTipo(struct NodoPersona *listaPersonas, int tipoBuscado, char *titulo) {
    struct NodoPersona *actual;

    if (listaPersonas == NULL) {
        printf("\nNo hay personas registradas.\n");
        return;
    }

    printf("\n--- %s ---\n", titulo);
    actual = listaPersonas;
    while (actual != NULL) {
        if (actual->datosPersona->tipo == tipoBuscado) {
            printf("Nombre: %s | RUT: %s\n", actual->datosPersona->nombre, actual->datosPersona->rut);
        }
        actual = actual->sig;
    }
}

/* Genera un reporte del estado de las causas registradas */
void generarReporteEstadoCausas(struct NodoCausa *listaCausas) {
    int abiertas = 0;
    int archivadas = 0;
    int acusaciones = 0;
    int cerradas = 0;
    int suspendidas = 0;
    struct NodoCausa *actual;

    actual = listaCausas;
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->estado, "abierta") == 0) {
            abiertas++;
        } else if (strcmp(actual->datosCausa->estado, "archivo") == 0) {
            archivadas++;
        } else if (strcmp(actual->datosCausa->estado, "acusacion") == 0) {
            acusaciones++;
        } else if (strcmp(actual->datosCausa->estado, "cerrada") == 0) {
            cerradas++;
        } else if (strcmp(actual->datosCausa->estado, "suspension condicional") == 0) {
            suspendidas++;
        }
        actual = actual->sig;
    }

    /* Mostrar los resultados */
    printf("\n===== REPORTE DE ESTADO DE CAUSAS =====\n");
    printf("Causas abiertas: %d\n", abiertas);
    printf("Causas archivadas: %d\n", archivadas);
    printf("Causas con acusacion: %d\n", acusaciones);
    printf("Causas cerradas: %d\n", cerradas);
    printf("Causas con suspension condicional: %d\n", suspendidas);
    printf("=======================================\n");
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


/* Menu para VICTIMA - solo consulta y agrega antecedentes */
void menuVictima(struct MinisterioPublico *ministerio, struct Persona *victima) {
    int opcion;
    do {
        printf("\n===== MENU VICTIMA =====\n");
        printf("1. Registrar denuncia\n");
        printf("2. Revisar carpeta investigativa\n");
        printf("3. Agregar prueba\n");
        printf("4. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                registrarDenunciaVictima(ministerio, victima);
                break;
            case 2:
                revisarCarpetaVictima(ministerio, victima);
                break;
            case 3:
                agregarPruebaVictima(ministerio, victima);
                break;
            case 4:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 4);
}

/* Menu para IMPUTADO - solo consulta de carpeta */
void menuImputado(struct MinisterioPublico *ministerio, struct Persona *imputado) {
    int opcion;
    do {
        printf("\n===== MENU IMPUTADO =====\n");
        printf("1. Ver carpeta asociada\n");
        printf("2. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                mostrarCarpetasDeImputado(ministerio, imputado);
                break;
            case 2:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 2);
}

/* Menu para TESTIGO - consulta basica (si se requiere) */
void menuTestigo(struct MinisterioPublico *ministerio, struct Persona *testigo) {
    int opcion;
    do {
        printf("\n===== MENU TESTIGO =====\n");
        printf("1. Ver citaciones personales\n");
        printf("2. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                mostrarCitacionesTestigo(ministerio, testigo);
                break;
            case 2:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 2);
}

/* Menu para FISCAL - acceso amplio, pero sin automatizar decisiones */
void menuFiscal(struct MinisterioPublico *ministerio, struct Persona *fiscal) {
    int opcion;
    do {
        printf("\n===== MENU FISCAL =====\n");
        printf("1. Ver causas asignadas\n");
        printf("2. Agregar diligencia\n");
        printf("3. Formalizar imputado\n");
        printf("4. Cambiar estado de causa\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                listarCausas(ministerio->causas);
                break;
            case 2:
                agregarDiligenciaFiscal(ministerio);
                break;
            case 3:
                ejecutarFormalizacionFiscal(ministerio);
                break;
            case 4:
                cambiarEstadoDesdeFiscal(ministerio);
                break;
            case 5:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 5);
}

/* Menu para DEFENSOR - acceso solo a formalizaciones del defendido */
void menuDefensor(struct MinisterioPublico *ministerio, struct Persona *defensor) {
    int opcion;
    do {
        printf("\n===== MENU DEFENSOR =====\n");
        printf("1. Ver formalizaciones asignadas\n");
        printf("2. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1:
                mostrarFormalizacionesDefensor(ministerio, defensor);
                break;
            case 2:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 2);
}

/* Menu principal que invoca los submenús según el tipo de usuario */
void menuPrincipal(struct MinisterioPublico *ministerio) {
    struct Persona *usuario;
    struct Persona *verificada;

    char nombre[100];
    char rut[20];
    int tipo;

    printf("--> Ingrese su nombre: ");
    leerCadena(nombre, sizeof(nombre));
    printf("--> Ingrese su RUT: ");
    leerCadena(rut, sizeof(rut));
    printf("--> Ingrese su tipo (1=Victima, 2=Imputado, 3=Testigo, 5=Fiscal, 6=Defensor): ");
    scanf("%d", &tipo);
    getchar();

    /* Validar existencia del usuario en la lista */
    verificada = buscarPersonaPorRut(ministerio->personas, rut);
    if (verificada == NULL || strcmp(verificada->nombre, nombre) != 0 || verificada->tipo != tipo) {
        if (verificada == NULL) {
            printf("--> Usuario no registrado o datos incorrectos.\n");
            return;
        }

        usuario = (struct Persona *) malloc(sizeof(struct Persona));
        if (usuario == NULL) {
            printf("No se pudo asignar memoria para el usuario.");
            return;
        }
        usuario->nombre = copiarCadena(nombre);
        usuario->rut = copiarCadena(rut);
        usuario->tipo = tipo;

        switch (usuario->tipo) {
            case 1:
                menuVictima(ministerio, usuario);
            break;
            case 2:
                menuImputado(ministerio, usuario);
            break;
            case 3:
                menuTestigo(ministerio, usuario);
            break;
            case 5:
                menuFiscal(ministerio, usuario);
            break;
            case 6:
                menuDefensor(ministerio, usuario);
            break;
            default:
                printf("--> Tipo de usuario no valido o no implementado.\n");
            break;
        }
    }
}

/* Funcion principal del sistema */
int main() {
    struct MinisterioPublico ministerio;

    ministerio.causas = NULL;
    ministerio.personas = NULL;
    ministerio.raizImputados = NULL;

    menuPrincipal(&ministerio);

    return 0;
}



