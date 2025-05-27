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
    int tipo;         /* Tipo: 1=Denunciante, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal */
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

/* Elimina un objeto investigativo de la lista segun su ID */
void eliminarObjetoPorId(struct NodoObjetoInvestigativo **listaObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *anterior;

    actual = *listaObjetos;
    anterior = NULL;

    /* Recorrer la lista */
    while (actual != NULL) {
        /* Comparar ID */
        if (actual->objeto->id == idBuscado) {
            /* Si es el primer nodo */
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

    printf("No se encontro un objeto con ese ID.\n");
}

/* Crea una nueva persona con nombre, RUT y tipo */
struct Persona *crearPersona(char *nombre, char *rut, int tipo) {
    struct Persona *nueva;

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

    /* Reservar memoria para el nuevo nodo */
    nuevoNodo = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nuevoNodo == NULL) {
        return;
    }

    /* Enlazar la nueva persona al inicio de la lista */
    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = *head;
    *head = nuevoNodo;
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

/* Registra una denuncia dentro del arreglo de la carpeta investigativa */
void registrarDenuncia(struct CarpetaInvestigativa *carpeta, char *rutDenunciante, char *descripcion, char *fecha, int id) {
    struct ObjetoInvestigativo *nuevaDenuncia;

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

    /* Agregar al arreglo de denuncias recientes */
    carpeta->denunciasRecientes[carpeta->totalDenuncias] = nuevaDenuncia;
    carpeta->totalDenuncias++;
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

/* Crea y asigna una formalizacion judicial al imputado del nodo */
void formalizarImputado(struct NodoImputadoABB *nodo, char *delito, char *antecedentes, char *fecha, int medidaCautelar) {
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
    nodo->formalizacion = nuevaFormalizacion;
}

/* Registra un nuevo objeto investigativo en la lista circular de la carpeta */
void agregarObjetoInvestigativo(struct CarpetaInvestigativa *carpeta, int tipo, char *rut, char *detalle, char *fecha) {
    struct ObjetoInvestigativo *nuevoObjeto;
    struct NodoObjetoInvestigativo *nuevoNodo;
    struct NodoObjetoInvestigativo *actual;

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

/* Cambia el estado de una causa segun la decision del fiscal */
void cerrarInvestigacion(struct Causa *causa, int decision) {
    if (decision == 1) {
        causa->estado = copiarCadena("acusacion");
    } else if (decision == 2) {
        causa->estado = copiarCadena("archivo");
    } else if (decision == 3) {
        causa->estado = copiarCadena("suspension condicional");
    } else {
        causa->estado = copiarCadena("cerrada");
    }
}

/* Registra una sentencia como objeto investigativo en la carpeta */
void registrarSentencia(struct CarpetaInvestigativa *carpeta, char *rut, char *detalle, char *fecha) {
    agregarObjetoInvestigativo(carpeta, 5, rut, detalle, fecha);
}

/* Registra una medida de proteccion como objeto investigativo en la carpeta */
void aplicarMedidaProteccion(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    agregarObjetoInvestigativo(carpeta, 4, rut, descripcion, fecha);
}

/* Registra una diligencia adicional como objeto investigativo en la carpeta */
void solicitarDiligencia(struct CarpetaInvestigativa *carpeta, char *rutSolicitante, char *descripcion, char *fecha) {
    agregarObjetoInvestigativo(carpeta, 3, rutSolicitante, descripcion, fecha);
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

/***
 *    ███╗   ███╗███████╗███╗   ██╗██╗   ██╗
 *    ████╗ ████║██╔════╝████╗  ██║██║   ██║
 *    ██╔████╔██║█████╗  ██╔██╗ ██║██║   ██║
 *    ██║╚██╔╝██║██╔══╝  ██║╚██╗██║██║   ██║
 *    ██║ ╚═╝ ██║███████╗██║ ╚████║╚██████╔╝
 *    ╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝
 *
 */




