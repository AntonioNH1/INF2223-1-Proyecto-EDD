#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_DENUNCIAS 1000

/* Estructura principal que representa el Ministerio Publico */
struct MinisterioPublico {
    struct NodoCausa *causas;                /* Lista de causas registradas */
    struct NodoPersona *personas;            /* Lista general de personas */
    struct NodoImputadoABB *raizImputados;   /* Arbol binario de imputados */
};

/* Contiene los datos de formalizacion judicial de un imputado */
struct Formalizacion {
    char *delito;
    char *antecedentes;
    char *fecha;
    int medidaCautelar;
    struct Persona *defensor;
    char *rutImputado;
};

/* Nodo del arbol binario de busqueda que contiene imputados */
struct NodoImputadoABB {
    struct NodoPersona *datosImputados;     /* Puntero al imputado (persona tipo 2) */
    struct NodoImputadoABB *izq;            /* Subarbol izquierdo */
    struct NodoImputadoABB *der;            /* Subarbol derecho */
    struct Formalizacion *formalizacion;    /* Informacion de la formalizacion judicial */
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
    int contadorObjetos;
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
    int tipo;         /* Tipo: 1=victima, 2=Imputado, 3=Testigo, 4=Juez, 5=Fiscal, 6=Defensor */
    char *clave;
};

/* ======================== FUNCIONES EXTRA ======================== */

void limpiarCadena(char *cadena) {
    int i;
    i = 0;
    while (cadena[i] != '\0') {
        if (cadena[i] == '\n' || cadena[i] == '\r') {
            cadena[i] = '\0';
            break;
        }
        i++;
    }
}

/* Lee una cadena desde stdin y elimina el salto de linea final */
void leerCadena(char *cadena, int longitud) {
    int i;
    char c;
    c = '\0';

    i = 0;
    while (i < longitud - 1) {
        c = getchar();
        if (c == '\n' || c == '\r') {
            break;
        }
        cadena[i] = c;
        i++;
    }

    cadena[i] = '\0';
    while (c != '\n' && c != '\r' && c != EOF) {
        c = getchar();
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

int esInterviniente(int tipoPersona) {
    if (tipoPersona == 1 || tipoPersona == 2 || tipoPersona == 5) {
        return 1; /*Victima, Imputado o Fiscal*/
    }
    return 0;
}

/* Busca una persona en la lista por su RUT y retorna un puntero a la persona */
struct Persona *buscarPersonaPorRut(struct NodoPersona *headPersonas, char *rutBuscado) {
    struct NodoPersona *actual;

    /* Recorrer la lista */
    actual = headPersonas;
    while (actual != NULL) {
        /* Comparar RUT */
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            if (actual != NULL && actual->datosPersona != NULL)
                return actual->datosPersona;
        }
        actual = actual->sig;
    }
    /* Si no se encontro coincidencia */
    return NULL;
}

/* Busca manualmente si una subcadena existe dentro de otra */
int contieneSubcadena(char *cadena, char *subcadena) {
    int i, j, longitudCadena, longitudSub;

    if (cadena == NULL || subcadena == NULL) {
        return 0;
    }

    longitudCadena = strlen(cadena);
    longitudSub = strlen(subcadena);

    for (i = 0; i <= longitudCadena - longitudSub; i++) {
        for (j = 0; j < longitudSub; j++) {
            if (cadena[i + j] != subcadena[j]) {
                break;
            }
        }
        if (j == longitudSub) {
            return 1;
        }
    }

    return 0;
}

int estaAsociadoALaCausa(struct Causa *causa, struct Persona *persona) {
    struct NodoPersona *nodo;

    if (causa == NULL || persona == NULL) {
        return 0;
    }

    if (persona->tipo == 1) {
        if (causa->victimas == NULL) return 0;
        nodo = causa->victimas;
    } else if (persona->tipo == 2) {
        if (causa->imputadosAsociados == NULL) return 0;
        nodo = causa->imputadosAsociados;
    } else if (persona->tipo == 5) {
        return (causa->fiscalEncargado != NULL &&
                strcmp(causa->fiscalEncargado->rut, persona->rut) == 0);
    } else {
        return 0;
    }

    while (nodo != NULL) {
        if (nodo->datosPersona != NULL &&
            strcmp(nodo->datosPersona->rut, persona->rut) == 0) {
            return 1;
            }
        nodo = nodo->sig;
    }

    return 0;
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
void agregarCausa(struct NodoCausa **headCausas, struct Causa *nuevaCausa) {
    struct NodoCausa *nuevoNodo;

    /* Reservar memoria para el nuevo nodo */
    nuevoNodo = (struct NodoCausa *) malloc(sizeof(struct NodoCausa));
    if (nuevoNodo == NULL) {
        return;
    }

    /* Enlazar la nueva causa al inicio de la lista */
    nuevoNodo->datosCausa = nuevaCausa;
    nuevoNodo->sig = *headCausas;
    *headCausas = nuevoNodo;
}
/* Muestra todas las causas registradas */
void listarCausas(struct NodoCausa *headCausas) {
    struct NodoCausa *actual;

    actual = headCausas;
    while (actual != NULL) {
        printf("RUC: %s | Categoria: %s | Estado: %s\n",
               actual->datosCausa->RUC,
               actual->datosCausa->CategoriaCausa,
               actual->datosCausa->estado);

        actual = actual->sig;
    }
}

/* Muestra los datos de una causa que coincida con el RUC buscado */
void mostrarCausaPorRUC(struct NodoCausa *headCausas, char *rucBuscado) {
    struct NodoCausa *actual;

    actual = headCausas;

    /* Recorrer la lista de causas */
    while (actual != NULL) {
        /* Comparar RUC */
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            /* Mostrar los datos si se encuentra */
            printf("Causa encontrada:\n");
            printf("RUC: %s\n", actual->datosCausa->RUC);
            printf("Categoria: %s\n", actual->datosCausa->CategoriaCausa);
            printf("Estado: %s\n", actual->datosCausa->estado);
            return;
        }
        actual = actual->sig;
    }
    /* Si no se encontro coincidencia */
    printf("No se encontro una causa con ese RUC.\n");
}

/* Modifica la categoria y el estado de una causa buscandola por RUC */
void modificarCausa(struct NodoCausa *headCausas, char *rucBuscado, char *nuevaCategoria, char *nuevoEstado) {
    struct NodoCausa *actual;

    actual = headCausas;

    /* Buscar la causa con el RUC indicado */
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            /* Asignar nuevos valores */
            actual->datosCausa->CategoriaCausa = copiarCadena(nuevaCategoria);
            actual->datosCausa->estado = copiarCadena(nuevoEstado);
            printf("Causa modificada correctamente.\n");
            return;
        }
        actual = actual->sig;
    }

    /* Si no se encontro la causa */
    printf("No se encontro una causa con ese RUC.\n");
}

/* Elimina una causa de la lista enlazada segun su RUC */
void eliminarCausaPorRUC(struct NodoCausa **headCausas, char *rucBuscado) {
    struct NodoCausa *actual;
    struct NodoCausa *anterior;

    /* Inicio de la lista */
    actual = *headCausas;
    anterior = NULL;

    while (actual != NULL) {
        /* Comparar el RUC de la causa actual */
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            /* Si es el primer nodo */
            if (anterior == NULL) {
                *headCausas = actual->sig;
            } else {
                anterior->sig = actual->sig;
            }
            printf("Causa eliminada de la lista.\n");
            return;
        }
        anterior = actual;
        actual = actual->sig;
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
    nuevaCarpeta->contadorObjetos = 1;
    nuevaCarpeta->objetos = NULL;
    nuevaCarpeta->totalDenuncias = 0;

    /* Asociar la carpeta a la causa */
    causa->carpetaInvestigativa = nuevaCarpeta;
}

/* Registra una denuncia dentro del arreglo de la carpeta investigativa */
void registrarDenuncia(struct CarpetaInvestigativa *carpeta, char *rutDenunciante, char *descripcion, char *fecha) {
    struct ObjetoInvestigativo *nuevaDenuncia;

    if (carpeta == NULL || carpeta->totalDenuncias >= MAX_DENUNCIAS) {
        printf("Error: Carpeta no valida o limite de denuncias alcanzado.\n");
        return;
    }

    nuevaDenuncia = (struct ObjetoInvestigativo *) malloc(sizeof(struct ObjetoInvestigativo));
    if (nuevaDenuncia == NULL) {
        return;
    }

    nuevaDenuncia->id = carpeta->contadorObjetos;
    carpeta->contadorObjetos++;

    nuevaDenuncia->rut = copiarCadena(rutDenunciante);
    nuevaDenuncia->detalle = copiarCadena(descripcion);
    nuevaDenuncia->fecha = copiarCadena(fecha);
    nuevaDenuncia->tipo = 1;
    nuevaDenuncia->sentenciaFinal = 0;

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
    nuevoObjeto->id = carpeta->contadorObjetos;
    carpeta->contadorObjetos++;
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
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *inicio;

    /* Verifica que la carpeta investigativa sea valida */
    if (carpeta == NULL) {
        printf("La carpeta investigativa no es valida.\n");
        return;
    }

    /* Entrada de datos */
    printf("Ingrese ID del objeto: ");
    scanf("%d", &id);
    while (getchar() != '\n');
    inicio = carpeta->objetos;
    actual = inicio;

    do {
        if (actual->objeto != NULL) {
            if (actual->objeto->id == id) {
                printf("Ya existe un objeto con ese ID en la carpeta investigativa.\n");
                return;
            }
        }
        actual = actual->sig;
    } while (actual != inicio);

    printf("Fecha (dd-mm-aaaa): ");
    leerCadena(fecha, sizeof(fecha));
    limpiarCadena(fecha);

    printf("RUT de la persona que genero el objeto: ");
    leerCadena(rut, sizeof(rut));
    limpiarCadena(rut);

    printf("Detalle del objeto: ");
    leerCadena(detalle, sizeof(detalle));
    limpiarCadena(detalle);

    printf("Tipo de objeto:\n");
    printf("1 = Prueba\n2 = Diligencia\n3 = Declaracion\n4 = Resolucion Judicial\n");
    printf("Seleccione tipo: ");
    scanf("%d", &tipo);
    while (getchar() != '\n');

    if (tipo < 1 || tipo > 4) {
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
struct ObjetoInvestigativo *buscarObjetoPorId(struct NodoObjetoInvestigativo *headObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;

    actual = headObjetos;
    while (actual != NULL) {
        if (actual->objeto->id == idBuscado) {
            return actual->objeto;
        }
        actual = actual->sig;
    }

    return NULL;
}

/* Modifica el detalle y tipo de un objeto segun su ID */
void modificarDatosObjeto(struct NodoObjetoInvestigativo *headObjetos, int idBuscado, char *nuevoDetalle, int nuevoTipo) {
    struct ObjetoInvestigativo *objeto;

    objeto = buscarObjetoPorId(headObjetos, idBuscado);
    if (objeto != NULL) {
        objeto->detalle = copiarCadena(nuevoDetalle);
        objeto->tipo = nuevoTipo;
        printf("Objeto investigativo modificado correctamente.\n");
    } else {
        printf("No se encontro un objeto con ese ID.\n");
    }
}

/* Elimina un objeto investigativo de la lista circular segun su ID */
void eliminarObjetoPorId(struct NodoObjetoInvestigativo **headObjetos, int idBuscado) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoObjetoInvestigativo *anterior;

    /* Validar si la lista esta vacia */
    if (headObjetos == NULL || *headObjetos == NULL) {
        printf("No hay objetos registrados.\n");
        return;
    }

    actual = *headObjetos;
    anterior = NULL;

    do {
        if (actual->objeto->id == idBuscado) {
            /* Caso: unico nodo */
            if (actual->sig == actual) {
                *headObjetos = NULL;
            }
            /* Caso: eliminar cabeza */
            else if (actual == *headObjetos) {
                struct NodoObjetoInvestigativo *ultimo = *headObjetos;
                while (ultimo->sig != *headObjetos) {
                    ultimo = ultimo->sig;
                }
                ultimo->sig = actual->sig;
                *headObjetos = actual->sig;
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
    } while (actual != *headObjetos);

    printf("No se encontro un objeto con ese ID.\n");
}

/* Devuelve el nombre textual de un tipo de objeto investigativo */
char *obtenerNombreTipoObjeto(int tipo) {

    if (tipo == 2) return "Prueba";
    if (tipo == 3) return "Diligencia";
    if (tipo == 4) return "Declaracion";
    if (tipo == 5) return "Resolucion Judicial";
    if (tipo == 6) return "Sentencia Final";
    return "Desconocido";
}

/* Muestra todos los objetos investigativos registrados en la carpeta */
void revisarCarpetaInvestigativa(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;

    if (carpeta->objetos == NULL) {
        printf("\nNo hay objetos investigativos registrados.\n");
        return;
    }

    actual = carpeta->objetos;
    do {
        printf("\nID: %d | Tipo: %s | Fecha: %s | RUT: %s\n",
               actual->objeto->id,
               obtenerNombreTipoObjeto(actual->objeto->tipo),
               actual->objeto->fecha,
               actual->objeto->rut);
        printf("Detalle: %s\n", actual->objeto->detalle);
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

/* Permite a un interviniente revisar la carpeta investigativa de una causa */
void solicitarRevisionCarpeta(struct Persona *solicitante, struct Causa *causa) {
    if (solicitante == NULL || causa == NULL || causa->carpetaInvestigativa == NULL) {
        if (causa == NULL) {
            printf("No se encontro una causa con ese RUC.\n");
        }
        printf("\nSolicitud invalida.\n");
        return;
    }

    if (esInterviniente(solicitante->tipo) && estaAsociadoALaCausa(causa, solicitante)) {
        printf("\nAcceso autorizado para %s (RUT: %s).\n", solicitante->nombre, solicitante->rut);
        revisarCarpetaInvestigativa(causa->carpetaInvestigativa);
    } else {
        printf("\nAcceso denegado. Usted no tiene permisos para revisar esta carpeta investigativa.\n");
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
void agregarPersona(struct NodoPersona **headPersonas, struct Persona *persona) {
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
    nuevoNodo->sig = *headPersonas;
    *headPersonas = nuevoNodo;
}

/* Busca un imputado por su RUT en el arbol ABB */
struct NodoImputadoABB *buscarImputadoPorRut(struct NodoImputadoABB *raiz, char *rut) {
    int comparacion;

    if (raiz == NULL || rut == NULL) {
        return NULL;
    }

    comparacion = strcmp(rut, raiz->datosImputados->datosPersona->rut);

    if (comparacion == 0) {
        return raiz;
    }

    if (comparacion < 0) {
        return buscarImputadoPorRut(raiz->izq, rut);
    } else {
        return buscarImputadoPorRut(raiz->der, rut);
    }
}

/* Muestra los datos de una persona segun su RUT */
void listarPersonaPorRut(struct NodoPersona *headPersonas, char *rut) {
    struct Persona *actual;

    /* Buscar persona en la lista */
    actual = buscarPersonaPorRut(headPersonas, rut);

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
void modificarDatosPersona(struct NodoPersona *headPersonas, char *rutBuscado, char *nuevoNombre, int nuevoTipo) {
    struct Persona *persona;

    /* Buscar persona por RUT */
    persona = buscarPersonaPorRut(headPersonas, rutBuscado);

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
void eliminarPersonaPorRut(struct NodoPersona **headPersonas, char *rutBuscado) {
    struct NodoPersona *actual;
    struct NodoPersona *anterior;

    /* Iniciar desde el comienzo de la lista */
    actual = *headPersonas;
    anterior = NULL;

    /* Recorrer la lista mientras queden nodos */
    while (actual != NULL) {
        /* Comparar el RUT del nodo actual con el buscado */
        if (strcmp(actual->datosPersona->rut, rutBuscado) == 0) {
            /* Si es el primer nodo */
            if (anterior == NULL) {
                *headPersonas = actual->sig;
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

/* Inserta un nuevo imputado en el arbol de imputados del Ministerio Publico */
struct NodoImputadoABB *insertarImputado(struct NodoImputadoABB *raiz, struct NodoImputadoABB *nuevo) {
    int comparacion;

    /* Verificar datos de entrada */
    if (nuevo == NULL) {
        return raiz;
    }

    /* Si el arbol esta vacio, este nodo se convierte en la nueva raiz */
    if (raiz == NULL) {
        return nuevo;
    }

    /* Comparar los RUT para decidir la posicion */
    comparacion = strcmp(nuevo->datosImputados->datosPersona->rut, raiz->datosImputados->datosPersona->rut);

    if (comparacion < 0) {
        raiz->izq = insertarImputado(raiz->izq, nuevo);
    } else if (comparacion > 0) {
        raiz->der = insertarImputado(raiz->der, nuevo);
    } else {
        /* Ya existe un imputado con el mismo RUT */
        printf("Ya existe un imputado con ese RUT.\n");
    }

    return raiz;
}

/* Crea y retorna una nueva formalización */
struct Formalizacion *crearFormalizacion(char *delito, char *antecedentes, char *fecha, int medida, struct Persona *defensor, char *rutImputado) {
    struct Formalizacion *nuevaFormalizacion;

    nuevaFormalizacion = (struct Formalizacion *) malloc(sizeof(struct Formalizacion));
    if (nuevaFormalizacion == NULL) {
        printf("Error de memoria para formalizacion.\n");
        return NULL;
    }
    nuevaFormalizacion->delito = copiarCadena(delito);
    nuevaFormalizacion->antecedentes = copiarCadena(antecedentes);
    nuevaFormalizacion->fecha = copiarCadena(fecha);
    nuevaFormalizacion->medidaCautelar = medida;
    nuevaFormalizacion->defensor = defensor;
    nuevaFormalizacion->rutImputado = copiarCadena(rutImputado);

    return nuevaFormalizacion;
}


/* Crea y retorna un nuevo nodo de imputado en ABB */
struct NodoImputadoABB *crearNodoImputado(struct Persona *imputado, struct Formalizacion *formalizacion) {
    struct NodoImputadoABB *nuevoNodoABB;
    struct NodoPersona *nodoPersona;

    nodoPersona = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nodoPersona == NULL) {
        printf("Error de memoria para nodo persona.\n");
        return NULL;
    }
    nodoPersona->datosPersona = imputado;
    nodoPersona->sig = NULL;

    nuevoNodoABB = (struct NodoImputadoABB *) malloc(sizeof(struct NodoImputadoABB));
    if (nuevoNodoABB == NULL) {
        printf("Error de memoria para nodo ABB.\n");
        return NULL;
    }

    nuevoNodoABB->datosImputados = nodoPersona;
    nuevoNodoABB->formalizacion = formalizacion;
    nuevoNodoABB->izq = NULL;
    nuevoNodoABB->der = NULL;

    return nuevoNodoABB;
}
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

    if (ministerio == NULL || causa == NULL || causa->carpetaInvestigativa == NULL ||
        nombreImputado == NULL || rutImputado == NULL ||
        nombreDefensor == NULL || rutDefensor == NULL ||
        delito == NULL || antecedentes == NULL || fecha == NULL) {
        printf("Error: Datos insuficientes o nulos.\n");
        return;
        }

    if (buscarImputadoPorRut(ministerio->raizImputados, rutImputado) != NULL) {
        printf("El imputado ya fue formalizado anteriormente.\n");
        return;
    }

    imputado = crearPersona(nombreImputado, rutImputado, 2);
    defensor = crearPersona(nombreDefensor, rutDefensor, 6);
    infoFormalizacion = crearFormalizacion(delito, antecedentes, fecha, medidaCautelar, defensor, rutImputado);

    if (infoFormalizacion == NULL) return;

    nuevoNodo = crearNodoImputado(imputado, infoFormalizacion);
    if (nuevoNodo == NULL) return;

    ministerio->raizImputados = insertarImputado(ministerio->raizImputados, nuevoNodo);

    nuevoNodo->datosImputados->sig = causa->imputadosAsociados;
    causa->imputadosAsociados = nuevoNodo->datosImputados;

    agregarObjetoInvestigativo(causa->carpetaInvestigativa, 5, rutImputado, "Audiencia de formalizacion realizada.", fecha);
}

/* Muestra los datos de la formalizacion judicial de un imputado */
void mostrarFormalizacion(struct NodoImputadoABB *imputados) {

    struct Formalizacion *formalizacion = NULL;
    if (imputados == NULL || imputados->formalizacion == NULL) {
        printf("No hay formalizacion registrada para este imputado.\n");
        return;
    }
    formalizacion = imputados->formalizacion;
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
void registrarDiligencia(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 3, rut, descripcion, fecha);
}

/* Registra un peritaje como objeto tipo 2 en la carpeta investigativa */
void registrarPeritaje(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 2, rut, descripcion, fecha);
}

/* Registra una medida de proteccion como objeto investigativo en la carpeta */
void aplicarMedidaProteccion(struct CarpetaInvestigativa *carpeta, char *rut, char *descripcion, char *fecha) {
    if (carpeta == NULL) {
        printf("Carpeta investigativa no valida.\n");
        return;
    }
    agregarObjetoInvestigativo(carpeta, 4, rut, descripcion, fecha);
}

/* Cuenta la cantidad de medidas de proteccion activas registradas en la carpeta investigativa */
int contarMedidasProteccionActivas(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;
    int contador;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        return 0;
    }

    contador = 0;

    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 4) {
            if (contieneSubcadena(actual->objeto->detalle, "proteccion")) {
                contador++;
            }
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);

    return contador;
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

/* ======================== PUNTO 5: CIERRE INVESTIGACION  ======================== */

/* Actualiza el estado de una causa según su RUC */
void cambiarEstadoCausa(struct NodoCausa *headCausas, char *rucBuscado, char *nuevoEstado) {
    struct NodoCausa *actual;

    if (headCausas == NULL) {
        return;
    }
    actual = headCausas;
    while (actual != NULL) {
        limpiarCadena(actual->datosCausa->RUC);
        limpiarCadena(rucBuscado);
        if (strcmp(actual->datosCausa->RUC, rucBuscado) == 0) {
            actual->datosCausa->estado = copiarCadena(nuevoEstado);
            printf("Estado de la causa actualizado a: %s\n", nuevoEstado);
            return;
        }
        actual = actual->sig;
    }

    printf("No se encontro una causa con ese RUC.\n");
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

    id = carpeta->contadorObjetos;
    carpeta->contadorObjetos++;

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

/* Muestra el juicio oral con fiscal, pruebas y testigos */
void realizarJuicioOral(struct Causa *causa) {
    struct NodoPersona *testigoActual;

    if (causa == NULL || causa->carpetaInvestigativa == NULL) {
        printf("No hay causa o carpeta disponible para el juicio oral.\n");
        return;
    }

    printf("\n===== JUICIO ORAL =====\n");

    if (causa->fiscalEncargado != NULL) {
        printf("Fiscal a cargo: %s\n", causa->fiscalEncargado->nombre);
    } else {
        printf("Fiscal a cargo: No asignado\n");
    }

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

/* Cuenta la cantidad de sentencias finales de tipo condenatoria en la carpeta investigativa */
int contarSentenciasCondenatorias(struct CarpetaInvestigativa *carpeta) {
    struct NodoObjetoInvestigativo *actual;
    int contador;

    if (carpeta == NULL || carpeta->objetos == NULL) {
        return 0;
    }
    contador = 0;

    actual = carpeta->objetos;

    do {
        if (actual->objeto->tipo == 5 && actual->objeto->sentenciaFinal == 1) {
            if (contieneSubcadena(actual->objeto->detalle, "condenatoria")) {
                contador++;
            }
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);

    return contador;
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
}

/* ======================== FUNCIONES ORDENAMIENTO Y BUSQUEDA ======================== */

/* Ordena el arreglo de denuncias por fecha (formato AAAA-MM-DD o similar) */
void ordenarDenunciasPorFecha(struct CarpetaInvestigativa *carpeta) {
    int i, j;
    struct ObjetoInvestigativo *aux;

    if (carpeta == NULL) {
        return;
    }

    for (i = 0; i < carpeta->totalDenuncias - 1; i++) {
        for (j = 0; j < carpeta->totalDenuncias - 1 - i; j++) {
            if (strcmp(carpeta->denunciasRecientes[j]->fecha,
                       carpeta->denunciasRecientes[j + 1]->fecha) > 0) {
                aux = carpeta->denunciasRecientes[j];
                carpeta->denunciasRecientes[j] = carpeta->denunciasRecientes[j + 1];
                carpeta->denunciasRecientes[j + 1] = aux;
                       }
        }
    }
}

void buscarDenunciaPorFecha(struct CarpetaInvestigativa *carpeta, char *fechaBuscada) {
    int inicio, fin, centro, encontrada;
    int i;
    struct ObjetoInvestigativo **denuncias;

    if (carpeta == NULL || carpeta->totalDenuncias <= 0) {
        printf("No hay denuncias registradas.\n");
        return;
    }

    ordenarDenunciasPorFecha(carpeta);

    denuncias = carpeta->denunciasRecientes;
    inicio = 0;
    fin = carpeta->totalDenuncias - 1;
    encontrada = 0;
    centro = 0;

    while ((inicio <= fin) && (!encontrada)) {
        centro = (inicio + fin) / 2;
        if (strcmp(denuncias[centro]->fecha, fechaBuscada) == 0) {
            encontrada = 1;
        } else {
            if (strcmp(denuncias[centro]->fecha, fechaBuscada) > 0) {
                fin = centro - 1;
            } else {
                inicio = centro + 1;
            }
        }
    }

    if (!encontrada) {
        printf("No se encontro una denuncia con la fecha %s.\n", fechaBuscada);
        return;
    }

    printf("Denuncias con fecha %s:\n", fechaBuscada);

    i = centro;
    while ((i >= 0) && (strcmp(denuncias[i]->fecha, fechaBuscada) == 0)) {
        i--;
    }
    i++;

    while ((i < carpeta->totalDenuncias) &&
           (strcmp(denuncias[i]->fecha, fechaBuscada) == 0)) {
        printf("ID: %d | RUT: %s\nDetalle: %s\n\n",
               denuncias[i]->id,
               denuncias[i]->rut,
               denuncias[i]->detalle);
        i++;
           }
}

/* ======================== FUNCIONES MENU ======================== */

/* Muestra a todas las personas del tipo especificado en la lista */
void listarPersonaPorTipo(struct NodoPersona *head, int tipoBuscado) {
    struct NodoPersona *actual;

    if (head == NULL) {
        printf("\nNo hay personas registradas.\n");
        return;
    }

    printf("\n--- Personas ---\n");
    actual = head;
    while (actual != NULL) {
        if (actual->datosPersona->tipo == tipoBuscado) {
            printf("Nombre: %s | RUT: %s\n", actual->datosPersona->nombre, actual->datosPersona->rut);
        }
        actual = actual->sig;
    }
}

/* Genera un reporte del estado de las causas registradas */
void generarReporteEstadoCausas(struct NodoCausa *headCausas) {
    int abiertas = 0;
    int archivadas = 0;
    int acusaciones = 0;
    int cerradas = 0;
    int suspendidas = 0;
    struct NodoCausa *actual;

    actual = headCausas;
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

/* Cuenta cuántas causas están archivadas provisionalmente */
int contarArchivosProvisionales(struct NodoCausa *headCausas) {
    int contador;
    struct NodoCausa *actual;

    contador = 0;
    actual = headCausas;
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->estado, "archivo provisional") == 0) {
            contador++;
        }
        actual = actual->sig;
    }
    return contador;
}

void consultarDenunciasPorRUC(struct MinisterioPublico *ministerio, char *rucCausa) {
    struct NodoCausa *actual;
    int i;

    actual = ministerio->causas;
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->RUC, rucCausa) == 0) {
            struct CarpetaInvestigativa *carpeta = actual->datosCausa->carpetaInvestigativa;
            if (carpeta == NULL || carpeta->totalDenuncias == 0) {
                printf("No hay denuncias registradas en esta causa.\n");
                return;
            }
            printf("\nDenuncias asociadas a la causa %s:\n", rucCausa);
            for (i = 0; i < carpeta->totalDenuncias; i++) {
                struct ObjetoInvestigativo *denuncia = carpeta->denunciasRecientes[i];
                printf("ID: %d | Fecha: %s | RUT: %s\nDetalle: %s\n\n",
                       denuncia->id, denuncia->fecha, denuncia->rut, denuncia->detalle);
            }
            return;
        }
        actual = actual->sig;
    }

    printf("No se encontro una causa con ese RUC.\n");
}

void mostrarCausasAsignadasAFiscal(struct NodoCausa *headCausas, struct Persona *fiscal) {
    struct NodoCausa *actual;

    if (headCausas == NULL || fiscal == NULL) {
        printf("No hay causas o fiscal invalido.\n");
        return;
    }

    actual = headCausas;
    while (actual != NULL) {
        if (actual->datosCausa->fiscalEncargado != NULL && strcmp(actual->datosCausa->fiscalEncargado->rut, fiscal->rut) == 0) {
            printf("RUC: %s | Categoria: %s | Estado: %s\n",
                   actual->datosCausa->RUC,
                   actual->datosCausa->CategoriaCausa,
                   actual->datosCausa->estado);
        }
        actual = actual->sig;
    }
}

void mostrarFormalizacionesDeDefensor(struct NodoImputadoABB *raiz, struct Persona *defensor) {
    if (raiz == NULL || defensor == NULL) {
        return;
    }

    mostrarFormalizacionesDeDefensor(raiz->izq, defensor);

    if (raiz->formalizacion != NULL && raiz->formalizacion->defensor != NULL) {
        if (strcmp(raiz->formalizacion->defensor->rut, defensor->rut) == 0) {
            printf("\n--- Formalizacion asociada ---\n");
            printf("Imputado: %s\n", raiz->datosImputados->datosPersona->nombre);
            printf("RUT: %s\n", raiz->datosImputados->datosPersona->rut);
            printf("Delito: %s\n", raiz->formalizacion->delito);
            printf("Fecha: %s\n", raiz->formalizacion->fecha);
            printf("Antecedentes: %s\n", raiz->formalizacion->antecedentes);
            printf("Medida Cautelar: %d\n", raiz->formalizacion->medidaCautelar);
        }
    }

    mostrarFormalizacionesDeDefensor(raiz->der, defensor);
}

void mostrarCausasAsociadasAVictima(struct NodoCausa *headCausas, struct Persona *victima) {
    struct NodoCausa *actual;
    struct NodoPersona *nodoVictima;

    if (headCausas == NULL || victima == NULL) {
        printf("No hay causas o victima invalida.\n");
        return;
    }

    actual = headCausas;
    while (actual != NULL) {
        nodoVictima = actual->datosCausa->victimas;
        while (nodoVictima != NULL) {
            if (strcmp(nodoVictima->datosPersona->rut, victima->rut) == 0) {
                printf("RUC: %s | Categoria: %s | Estado: %s\n",
                       actual->datosCausa->RUC,
                       actual->datosCausa->CategoriaCausa,
                       actual->datosCausa->estado);
                break;
            }
            nodoVictima = nodoVictima->sig;
        }
        actual = actual->sig;
    }
}

void mostrarCausasAsociadasAImputado(struct NodoCausa *headCausas, struct Persona *imputado) {
    struct NodoCausa *actual;
    struct NodoPersona *nodoImputado;

    if (headCausas == NULL || imputado == NULL) {
        printf("No hay causas o imputado invalido.");
        return;
    }

    actual = headCausas;
    while (actual != NULL) {
        nodoImputado = actual->datosCausa->imputadosAsociados;
        while (nodoImputado != NULL) {
            if (strcmp(nodoImputado->datosPersona->rut, imputado->rut) == 0) {
                printf("RUC: %s | Categoria: %s | Estado: %s\n",
                       actual->datosCausa->RUC,
                       actual->datosCausa->CategoriaCausa,
                       actual->datosCausa->estado);
                break;
            }
            nodoImputado = nodoImputado->sig;
        }
        actual = actual->sig;
    }
}

void consultarEstadoCausaAsociada(struct NodoCausa *headCausas, struct Persona *persona, char *ruc) {
    struct NodoCausa *actual;
    struct NodoPersona *nodo;

    if (headCausas == NULL || persona == NULL || ruc == NULL) {
        printf("Parametros invalidos.\n");
        return;
    }

    actual = headCausas;
    while (actual != NULL) {
        if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
            /* Verificar segun tipo */
            if (persona->tipo == 1) {
                nodo = actual->datosCausa->victimas;
            } else if (persona->tipo == 2) {
                nodo = actual->datosCausa->imputadosAsociados;
            } else if (persona->tipo == 5) {
                if (actual->datosCausa->fiscalEncargado != NULL &&
                    strcmp(actual->datosCausa->fiscalEncargado->rut, persona->rut) == 0) {
                    printf("Estado actual de la causa %s: %s\n", ruc, actual->datosCausa->estado);
                    return;
                    } else {
                        printf("La causa existe pero no esta asignada a este fiscal.\n");
                        return;
                    }
            } else {
                printf("Tipo de persona no valida para consulta.\n");
                return;
            }

            while (nodo != NULL) {
                if (strcmp(nodo->datosPersona->rut, persona->rut) == 0) {
                    printf("Estado actual de la causa %s: %s\n", ruc, actual->datosCausa->estado);
                    return;
                }
                nodo = nodo->sig;
            }
            printf("La causa existe pero no esta asociada a esta persona.\n");
            return;
        }
        actual = actual->sig;
    }

}
void revisarMedidaCautelar(struct NodoImputadoABB *raiz, struct Persona *imputado) {
    struct NodoImputadoABB *nodo;

    if (raiz == NULL || imputado == NULL) {
        printf("Parametros invalidos.");
        return;
    }

    nodo = buscarImputadoPorRut(raiz, imputado->rut);
    if (nodo != NULL && nodo->formalizacion != NULL) {
        printf("Medida cautelar vigente: %d", nodo->formalizacion->medidaCautelar);
    } else {
        printf("No se encontro una formalizacion con medida cautelar para este imputado.");
    }
}

void verFormalizacionPropia(struct NodoImputadoABB *raiz, struct Persona *imputado) {
    struct NodoImputadoABB *nodo;

    if (raiz == NULL || imputado == NULL) {
        printf("Parametros invalidos.");
        return;
    }

    nodo = buscarImputadoPorRut(raiz, imputado->rut);
    if (nodo != NULL && nodo->formalizacion != NULL) {
        mostrarFormalizacion(nodo);
    } else {
        printf("No hay formalizacion registrada para este imputado.");
    }
}

void verCarpetasDeDefendidos(struct MinisterioPublico *ministerio, struct NodoImputadoABB *raiz, struct Persona *defensor) {
    struct NodoCausa *actual;
    struct NodoPersona *nodo;

    if (raiz == NULL || defensor == NULL || ministerio == NULL) {
        return;
    }

    verCarpetasDeDefendidos(ministerio, raiz->izq, defensor);

    if (raiz->formalizacion != NULL && raiz->formalizacion->defensor != NULL) {
        if (strcmp(raiz->formalizacion->defensor->rut, defensor->rut) == 0) {
            actual = ministerio->causas;
            while (actual != NULL) {
                nodo = actual->datosCausa->imputadosAsociados;
                while (nodo != NULL) {
                    if (strcmp(nodo->datosPersona->rut, raiz->datosImputados->datosPersona->rut) == 0) {
                        printf("Carpeta del imputado: %s (%s)", nodo->datosPersona->nombre, nodo->datosPersona->rut);
                        solicitarRevisionCarpeta(defensor, actual->datosCausa);
                        break;
                    }
                    nodo = nodo->sig;
                }
                actual = actual->sig;
            }
        }
    }

    verCarpetasDeDefendidos(ministerio, raiz->der, defensor);
}

void listarResolucionesPorImputado(struct CarpetaInvestigativa *carpeta, char *rutBuscado) {
    struct NodoObjetoInvestigativo *actual;
    if (carpeta == NULL || carpeta->objetos == NULL) {
        printf("Carpeta no valida o vacia.\n");
        return;
    }

    actual = carpeta->objetos;
    do {
        if (actual->objeto->tipo == 5 && strcmp(actual->objeto->rut, rutBuscado) == 0) {
            printf("Fecha: %s | Detalle: %s\n", actual->objeto->fecha, actual->objeto->detalle);
        }
        actual = actual->sig;
    } while (actual != carpeta->objetos);
}

void listarDeclaracionesDeTestigos(struct CarpetaInvestigativa *carpeta, struct NodoPersona *testigos) {
    struct NodoObjetoInvestigativo *actual;
    struct NodoPersona *testigo;

    if (carpeta == NULL || carpeta->objetos == NULL || testigos == NULL) {
        printf("No hay declaraciones o testigos disponibles.\n");
        return;
    }

    printf("=== Declaraciones de testigos ===\n");
    testigo = testigos;
    while (testigo != NULL) {
        actual = carpeta->objetos;
        do {
            if (actual->objeto->tipo == 4 &&
                strcmp(actual->objeto->rut, testigo->datosPersona->rut) == 0) {
                printf("Testigo: %s | Fecha: %s\n", testigo->datosPersona->nombre, actual->objeto->fecha);
                printf("Detalle: %s\n\n", actual->objeto->detalle);
                }
            actual = actual->sig;
        } while (actual != carpeta->objetos);
        testigo = testigo->sig;
    }
}

/* Asocia una persona a una causa dependiendo de su tipo */
void asociarPersona(struct Causa *causa, struct Persona *persona) {
    struct NodoPersona *nuevoNodo;

    /* Verificar entradas validas */
    if (causa == NULL || persona == NULL) {
        printf("Parametros invalidos.\n");
        return;
    }


    /* Reservar memoria para el nodo */
    nuevoNodo = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    if (nuevoNodo == NULL) {
        printf("Error de memoria al asociar persona.\n");
        return;
    }

    nuevoNodo->datosPersona = persona;
    nuevoNodo->sig = NULL;

    /* Asociar segun tipo de persona */
    if (persona->tipo == 1) {
        /* Victima */
        nuevoNodo->sig = causa->victimas;
        causa->victimas = nuevoNodo;

    } else if (persona->tipo == 2) {
        /* Imputado */
        nuevoNodo->sig = causa->imputadosAsociados;
        causa->imputadosAsociados = nuevoNodo;

    } else if (persona->tipo == 3) {
        /* Testigo */
        nuevoNodo->sig = causa->testigos;
        causa->testigos = nuevoNodo;

    } else {
        printf("Tipo de persona no valido para asociar a causa.\n");
    }
}

/* Busca el nodo con el RUT minimo en el subarbol (el más a la izquierda) */
struct NodoImputadoABB *minimoNodo(struct NodoImputadoABB *nodo) {
    while (nodo != NULL && nodo->izq != NULL) {
        nodo = nodo->izq;
    }
    return nodo;
}

/* Elimina un imputado del arbol ABB segun su RUT  */
struct NodoImputadoABB *eliminarImputadoPorRut(struct NodoImputadoABB *raiz, char *rutBuscado) {
    int comparacion;
    struct NodoImputadoABB *temp;
    struct NodoImputadoABB *sucesor;

    if (raiz == NULL || rutBuscado == NULL) {
        return NULL;
    }

    /* Comparamos el RUT que buscamos con el RUT del nodo actual */
    comparacion = strcmp(rutBuscado, raiz->datosImputados->datosPersona->rut);

    if (comparacion < 0) {
        /* El RUT es menor, vamos a la izquierda */
        raiz->izq = eliminarImputadoPorRut(raiz->izq, rutBuscado);
    } else if (comparacion > 0) {
        /* El RUT es mayor, vamos a la derecha */
        raiz->der = eliminarImputadoPorRut(raiz->der, rutBuscado);
    } else {
        /* Encontramos el nodo que debe ser eliminado */

        /* Caso 1: no tiene hijos */
        if (raiz->izq == NULL && raiz->der == NULL) {
            return NULL;
        }

        /* Caso 2: solo tiene hijo derecho */
        else if (raiz->izq == NULL) {
            temp = raiz->der;
            return temp;
        }

        /* Caso 2: solo tiene hijo izquierdo */
        else if (raiz->der == NULL) {
            temp = raiz->izq;
            return temp;
        }

        /* Caso 3: tiene dos hijos */
        else {
            /* Buscamos el sucesor (el menor del subarbol derecho) */
            sucesor = minimoNodo(raiz->der);

            /* Copiamos los datos del sucesor al nodo actual */
            raiz->datosImputados = sucesor->datosImputados;
            raiz->formalizacion = sucesor->formalizacion;

            /* Eliminamos el sucesor */
            raiz->der = eliminarImputadoPorRut(raiz->der, sucesor->datosImputados->datosPersona->rut);
        }
    }

    return raiz;
}

/* Lista todas las personas registradas, sin filtrar por tipo */
void listarTodasLasPersonas(struct NodoPersona *head) {
    struct NodoPersona *actual;

    actual = head;

    if (actual == NULL) {
        printf("No hay personas registradas.\n");
        return;
    }

    printf("\n--- Lista completa de personas ---\n");

    while (actual != NULL) {
        printf("Nombre: %s | RUT: %s | Tipo: %d\n", actual->datosPersona->nombre, actual->datosPersona->rut, actual->datosPersona->tipo);
        actual = actual->sig;
    }
}

/* Recorre y muestra los imputados formalizados en orden alfabético por RUT */
void listarImputadosInorden(struct NodoImputadoABB *raiz) {
    if (raiz == NULL) {
        return;
    }

    /* Lado izquierdo */
    listarImputadosInorden(raiz->izq);

    /* Nodo actual */
    printf("Nombre: %s | RUT: %s\n", raiz->datosImputados->datosPersona->nombre, raiz->datosImputados->datosPersona->rut);

    if (raiz->formalizacion != NULL) {
        printf("  - Delito: %s\n", raiz->formalizacion->delito);
        printf("  - Fecha: %s\n", raiz->formalizacion->fecha);
        printf("  - Medida Cautelar: %d\n", raiz->formalizacion->medidaCautelar);
    }

    /* Lado derecho */
    listarImputadosInorden(raiz->der);
}

/* ======================== MENUS ADAPTADOS ======================== */

void menuVictima(struct MinisterioPublico *ministerio, struct Persona *victima) {
    int opcion;
    int cantidad;
    char ruc[30];
    struct NodoCausa *actual;

    do {
        printf("\n===== MENU VICTIMA =====\n");
        printf("1. Ver causas asociadas\n");
        printf("2. Ver carpeta investigativa\n");
        printf("3. Consultar estado de causa\n");
        printf("4. Ver medidas de proteccion activas\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                mostrarCausasAsociadasAVictima(ministerio->causas, victima);
                break;
            case 2:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        solicitarRevisionCarpeta(victima, actual->datosCausa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;
            case 3:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                consultarEstadoCausaAsociada(ministerio->causas, victima, ruc);
                break;
            case 4:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                if (ministerio->causas == NULL) {
                    printf("causa no encontrada.");
                    break;
                }
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {

                        if (actual->datosCausa != NULL && actual->datosCausa->carpetaInvestigativa != NULL) {
                            cantidad = contarMedidasProteccionActivas(actual->datosCausa->carpetaInvestigativa);
                            printf("Cantidad de medidas de proteccion activas: %d", cantidad);
                        } else {
                            printf("Datos de causa o carpeta no disponibles.\n");
                        }
                        break;
                    }
                    actual = actual->sig;
                }
                break;
            case 5:
                break;
            default:
                printf("Opcion invalida.");
        }
    } while (opcion != 5);
}


void menuImputado(struct MinisterioPublico *ministerio, struct Persona *imputado) {
    int opcion;
    int encontrada;
    char ruc[30];
    struct NodoCausa *actual;

    do {
        printf("\n===== MENU IMPUTADO =====\n");
        printf("1. Ver causas asociadas\n");
        printf("2. Ver carpeta investigativa\n");
        printf("3. Consultar estado de causa\n");
        printf("4. Revisar medida cautelar\n");
        printf("5. Ver formalizacion propia\n");
        printf("6. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        while (getchar() != '\n');
        switch (opcion) {
            case 1:
                mostrarCausasAsociadasAImputado(ministerio->causas, imputado);
            break;
            case 2:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                actual = ministerio->causas;
                encontrada = 0;
                while (actual != NULL) {
                    if (actual->datosCausa != NULL && strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        solicitarRevisionCarpeta(imputado, actual->datosCausa);
                        encontrada = 1;
                        break;
                    }
                    actual = actual->sig;
                }
                if (!encontrada) {
                    printf("No se encontro una causa con ese RUC.\n");
                }
                break;
            case 3:
                printf("Ingrese RUC de la causa: ");
            leerCadena(ruc, sizeof(ruc));
            consultarEstadoCausaAsociada(ministerio->causas, imputado, ruc);
            break;
            case 4:
                revisarMedidaCautelar(ministerio->raizImputados, imputado);
            break;
            case 5:
                verFormalizacionPropia(ministerio->raizImputados, imputado);
            break;
            case 6:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 6);
}


/* Menu para el usuario fiscal */
void menuFiscal(struct MinisterioPublico *ministerio, struct Persona *fiscal) {
    int opcion;
    char ruc[30];
    char rut[20];
    char nombre[100];
    char categoria[100];
    char estado[50];
    char detalle[200];
    char fechaBuscada[11];
    int tipo;
    int id;
    struct NodoCausa *actual;
    struct Persona *persona;
    struct NodoImputadoABB *imputadoNodo = NULL;

    int i;
    int encontrada;

    do {
        printf("\n===== MENU FISCAL =====\n");

        /* CAUSAS */
        printf("1. Ver causas asignadas\n");
        printf("2. Cambiar estado de causa\n");
        printf("3. Modificar datos de causa\n");
        printf("4. Asociar persona a causa\n");

        /* DENUNCIAS Y DILIGENCIAS */
        printf("5. Registrar nueva denuncia\n");
        printf("6. Consultar denuncias por RUC\n");
        printf("7. Agregar diligencia o prueba\n");
        printf("8. Ver denuncias recientes ordenadas por fecha\n");
        printf("9. Buscar denuncia por fecha\n");

        /* OBJETOS INVESTIGATIVOS */
        printf("10. Modificar objeto investigativo\n");
        printf("11. Eliminar objeto investigativo\n");

        /* RESOLUCIONES Y DECLARACIONES */
        printf("12. Registrar resolucion judicial\n");
        printf("13. Ver resoluciones por RUC\n");
        printf("14. Ver declaraciones de testigos\n");

        /* FORMALIZACION Y SENTENCIA */
        printf("15. Formalizar imputado\n");
        printf("16. Registrar sentencia final\n");

        /* MEDIDAS DE PROTECCION */
        printf("17. Aplicar medida de proteccion a victima\n");

        /* REVISION Y JUICIO ORAL */
        printf("18. Revisar carpeta investigativa por RUC\n");
        printf("19. Realizar juicio oral\n");
        printf("20. Cambiar medida cautelar\n");

        /* SALIDA */
        printf("21. Salir\n");


        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
            case 1:
                mostrarCausasAsignadasAFiscal(ministerio->causas, fiscal);
                break;

            case 2:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("Ingrese nuevo estado: ");
                leerCadena(estado, sizeof(estado));
                limpiarCadena(ruc);
                cambiarEstadoCausa(ministerio->causas, ruc, estado);
                break;

            case 3:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        if (actual->datosCausa->fiscalEncargado != NULL &&
                            strcmp(actual->datosCausa->fiscalEncargado->rut, fiscal->rut) == 0) {
                            printf("Ingrese nueva categoria: ");
                            leerCadena(categoria, sizeof(categoria));
                            printf("Ingrese nuevo estado: ");
                            leerCadena(estado, sizeof(estado));
                            modificarCausa(ministerio->causas, ruc, categoria, estado);
                            } else {
                                printf("Usted no es el fiscal asignado a esta causa.\n");
                            }
                        break;
                    }
                    actual = actual->sig;
                }
                break;


            case 4:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0 && actual->datosCausa->fiscalEncargado != NULL &&
                        strcmp(actual->datosCausa->fiscalEncargado->rut, fiscal->rut) == 0) {
                        printf("Ingrese RUT de la persona a asociar: ");
                        leerCadena(rut, sizeof(rut));
                        limpiarCadena(rut);
                        persona = buscarPersonaPorRut(ministerio->personas, rut);
                        if (persona != NULL) {
                            asociarPersona(actual->datosCausa, persona);
                        } else {
                            printf("No se encontro persona con ese RUT.\n");
                        }
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 5:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                encontrada = 0;

                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0 &&
                        actual->datosCausa->fiscalEncargado != NULL &&
                        strcmp(actual->datosCausa->fiscalEncargado->rut, fiscal->rut) == 0) {

                        encontrada = 1;

                        printf("Ingrese RUT del denunciante: ");
                        leerCadena(rut, sizeof(rut));

                        printf("Ingrese descripcion: ");
                        leerCadena(detalle, sizeof(detalle));

                        printf("Ingrese fecha (dd-mm-aaaa): ");
                        leerCadena(estado, sizeof(estado));

                        registrarDenuncia(actual->datosCausa->carpetaInvestigativa, rut, detalle, estado);
                        break;
                        }

                    actual = actual->sig;
                }

                if (!encontrada) {
                    printf("No tiene permiso para registrar denuncia en esta causa.\n");
                }

                break;

            case 6:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                while (getchar() != '\n');
                consultarDenunciasPorRUC(ministerio, ruc);
                break;

            case 7:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        agregarObjetoPorTipo(actual->datosCausa->carpetaInvestigativa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 8:
                while (getchar() != '\n'); /* Limpiar buffer */
                printf("Ingrese RUC de la causa para ver sus denuncias: ");
                leerCadena(ruc, sizeof(ruc));

                actual = ministerio->causas;
                while (actual != NULL) {
                    if (actual->datosCausa != NULL &&
                        actual->datosCausa->RUC != NULL &&
                        strcmp(actual->datosCausa->RUC, ruc) == 0) {

                        if (actual->datosCausa->carpetaInvestigativa != NULL &&
                            actual->datosCausa->carpetaInvestigativa->denunciasRecientes != NULL) {

                            ordenarDenunciasPorFecha(actual->datosCausa->carpetaInvestigativa);

                            printf("\n===== Denuncias ordenadas por fecha =====\n");
                            for (i = 0; i < actual->datosCausa->carpetaInvestigativa->totalDenuncias; i++) {
                                if (actual->datosCausa->carpetaInvestigativa->denunciasRecientes[i] != NULL) {
                                    printf("Fecha: %s | RUT: %s\n",
                                        actual->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->fecha,
                                        actual->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->rut);
                                    printf("Detalle: %s\n\n",
                                        actual->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->detalle);
                                }
                            }
                        } else {
                            printf("La causa no tiene carpeta investigativa o no hay denuncias.\n");
                        }
                        break;
                    }
                    actual = actual->sig;
                }

                if (actual == NULL) {
                    printf("No se encontro una causa con ese RUC.\n");
                }
                break;
            case 9:
                while (getchar() != '\n');
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (actual->datosCausa != NULL &&
                        strcmp(actual->datosCausa->RUC, ruc) == 0 &&
                        actual->datosCausa->fiscalEncargado != NULL &&
                        strcmp(actual->datosCausa->fiscalEncargado->rut, fiscal->rut) == 0) {
                        if (actual->datosCausa->carpetaInvestigativa != NULL) {
                            printf("Ingrese fecha a buscar (AAAA-MM-DD): ");
                            leerCadena(fechaBuscada, sizeof(fechaBuscada));
                            buscarDenunciaPorFecha(actual->datosCausa->carpetaInvestigativa, fechaBuscada);
                        } else {
                            printf("La causa no tiene carpeta investigativa.\n");
                        }
                        break;
                        }
                    actual = actual->sig;
                }

                if (actual == NULL) {
                    printf("No se encontro una causa con ese RUC o no esta asignada a este fiscal.\n");
                }
                break;

            case 10:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese ID del objeto a modificar: ");
                        scanf("%d", &id);
                        while (getchar() != '\n');
                        printf("Ingrese nuevo detalle: ");
                        leerCadena(detalle, sizeof(detalle));
                        limpiarCadena(detalle);
                        printf("Ingrese nuevo tipo: ");
                        scanf("%d", &tipo);
                        while (getchar() != '\n');
                        modificarDatosObjeto(actual->datosCausa->carpetaInvestigativa->objetos, id, detalle, tipo);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 11:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese ID del objeto a eliminar: ");
                        scanf("%d", &id);
                        while (getchar() != '\n');
                        eliminarObjetoPorId(&actual->datosCausa->carpetaInvestigativa->objetos, id);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 12:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese RUT asociado: ");
                        leerCadena(rut, sizeof(rut));
                        limpiarCadena(rut);
                        printf("Ingrese detalle de la resolucion: ");
                        leerCadena(detalle, sizeof(detalle));
                        limpiarCadena(detalle);
                        printf("Ingrese fecha (dd-mm-aaaa): ");
                        leerCadena(estado, sizeof(estado));
                        limpiarCadena(estado);
                        agregarObjetoInvestigativo(actual->datosCausa->carpetaInvestigativa, 5, rut, detalle, estado);

                        break;
                    }
                    actual = actual->sig;
                }
                break;
            case 13:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        listarResolucionesJudiciales(actual->datosCausa->carpetaInvestigativa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 14:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        listarDeclaracionesDeTestigos(actual->datosCausa->carpetaInvestigativa, actual->datosCausa->testigos);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 15:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese nombre del imputado: ");
                        leerCadena(nombre, sizeof(nombre));
                        printf("Ingrese RUT del imputado: ");
                        leerCadena(rut, sizeof(rut));
                        printf("Ingrese nombre del defensor: ");
                        leerCadena(categoria, sizeof(categoria));
                        printf("Ingrese RUT del defensor: ");
                        leerCadena(estado, sizeof(estado));
                        printf("Ingrese delito formalizado: ");
                        leerCadena(detalle, sizeof(detalle));
                        printf("Ingrese medios probatorios: ");
                        leerCadena(nombre, sizeof(nombre));
                        printf("Ingrese fecha: ");
                        leerCadena(ruc, sizeof(ruc));
                        printf("Ingrese ID formalizacion: ");
                        scanf("%d", &id);
                        formalizarConDefensor(ministerio, actual->datosCausa, nombre, rut, categoria, estado, detalle, nombre, ruc, id);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 16:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese detalle de la sentencia: ");
                        leerCadena(detalle, sizeof(detalle));
                        printf("Ingrese fecha (dd-mm-aaaa): ");
                        leerCadena(estado, sizeof(estado));
                        agregarObjetoInvestigativo(actual->datosCausa->carpetaInvestigativa, 6, "", detalle, estado);

                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 17:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("Ingrese RUT de la victima: ");
                        leerCadena(rut, sizeof(rut));
                        printf("Ingrese detalle de la medida: ");
                        leerCadena(detalle, sizeof(detalle));
                        printf("Ingrese fecha: ");
                        leerCadena(estado, sizeof(estado));
                        aplicarMedidaProteccion(actual->datosCausa->carpetaInvestigativa, rut, detalle, estado);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 18:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        printf("\n--- Contenido de la carpeta investigativa RUC: %s ---\n", actual->datosCausa->RUC);
                        revisarCarpetaInvestigativa(actual->datosCausa->carpetaInvestigativa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 19:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        realizarJuicioOral(actual->datosCausa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 20:
                printf("Ingrese RUT del imputado: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                imputadoNodo = buscarImputadoPorRut(ministerio->raizImputados, rut);
                if (imputadoNodo == NULL) {
                    printf("No se encontro imputado con ese RUT.\n");
                } else {
                    printf("Ingrese nueva medida cautelar:\n");
                    printf("1 = Prision preventiva\n");
                    printf("2 = Arraigo nacional\n");
                    printf("3 = Firma mensual\n");
                    printf("4 = Prohibicion de acercarse a la victima\n");
                    printf("5 = Arresto domiciliario total\n");
                    printf("6 = Arresto domiciliario nocturno\n");
                    printf("Seleccione opcion: ");
                    scanf("%d", &tipo);
                    cambiarMedidaCautelar(imputadoNodo, tipo);
                }
                break;
            case 21:
                break;
            default:
                printf("Opcion invalida.\n");
            break;
        }

    } while (opcion != 21);
}

void menuAdministrador(struct MinisterioPublico *ministerio) {
    int opcion;
    char rut[20];
    char ruc[30];
    char nombre[100];
    char categoria[100];
    char estado[50];
    char detalle[200];
    char fechaBuscada[11];
    int tipo;
    int id;
    struct Causa *nuevaCausa;
    struct Persona *nuevaPersona;
    struct NodoCausa *actual;
    struct Persona *persona;
    struct NodoCausa *actualCausa;
    struct NodoImputadoABB *resultado;
    int i;

    do {
        printf("\n===== MENU ADMINISTRADOR =====\n");

        /* CAUSAS */
        printf("1. Listar causas\n");
        printf("2. Agregar nueva causa\n");
        printf("3. Modificar causa\n");
        printf("4. Eliminar causa\n");
        printf("5. Buscar causa por RUC\n");

        /* PERSONAS */
        printf("6. Listar todas las personas\n");
        printf("7. Agregar nueva persona\n");
        printf("8. Modificar persona\n");
        printf("9. Eliminar persona\n");
        printf("10. Buscar persona por RUT\n");
        printf("11. Asociar persona a causa\n");

        /* OBJETOS INVESTIGATIVOS */
        printf("12. Listar objetos investigativos\n");
        printf("13. Agregar objeto a carpeta\n");
        printf("14. Modificar objeto investigativo\n");
        printf("15. Eliminar objeto investigativo\n");
        printf("16. Buscar objeto investigativo por ID\n");

        /* RESOLUCIONES Y SENTENCIAS */
        printf("17. Ver resoluciones judiciales\n");
        printf("18. Ver sentencias finales\n");

        /* REPORTES */
        printf("19. Ver reporte de estado de causas\n");
        printf("20. Ver total de sentencias condenatorias\n");
        printf("21. Ver total de causas archivadas provisionalmente\n");
        printf("22. Ver medidas de proteccion activas por RUC\n");

        /* IMPUTADOS */
        printf("23. Buscar imputado formalizado por RUT\n");
        printf("24. Eliminar imputado del sistema\n");
        printf("25. Listar todos los imputados formalizados\n");

        /* DENUNCIAS */
        printf("26. Ver denuncias recientes ordenadas por fecha\n");
        printf("27. Buscar denuncia por fecha\n");



        /* SALIDA */
        printf("28. Salir\n");

        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
            case 1:
                listarCausas(ministerio->causas);
                break;

            case 2:
                printf("RUC: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("Categoria: ");
                leerCadena(categoria, sizeof(categoria));
                limpiarCadena(categoria);
                printf("Estado: ");
                leerCadena(estado, sizeof(estado));
                limpiarCadena(estado);
                nuevaCausa = crearCausa(ruc, categoria, estado);
                agregarCausa(&ministerio->causas, nuevaCausa);
                crearCarpetaInvestigativa(nuevaCausa);
                break;

            case 3:
                printf("RUC a modificar: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("Nueva categoria: ");
                leerCadena(categoria, sizeof(categoria));
                limpiarCadena(categoria);
                printf("Nuevo estado: ");
                leerCadena(estado, sizeof(estado));
                limpiarCadena(estado);
                modificarCausa(ministerio->causas, ruc, categoria, estado);
                break;

            case 4:
                printf("RUC a eliminar: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                eliminarCausaPorRUC(&ministerio->causas, ruc);
                break;

            case 5:
                printf("Ingrese RUC a buscar: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                mostrarCausaPorRUC(ministerio->causas, ruc);
                break;

            case 6:
                listarTodasLasPersonas(ministerio->personas);
                break;

            case 7:
                printf("Nombre: ");
                leerCadena(nombre, sizeof(nombre));
                limpiarCadena(nombre);
                printf("RUT: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                printf("Tipo (1=victima, 2=imputado, 3=testigo, 5=fiscal): ");
                scanf("%d", &tipo);
                while (getchar() != '\n');
                nuevaPersona = crearPersona(nombre, rut, tipo);
                agregarPersona(&ministerio->personas, nuevaPersona);
                break;

            case 8:
                printf("Ingrese RUT a modificar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                printf("Nuevo nombre: ");
                leerCadena(nombre, sizeof(nombre));
                limpiarCadena(nombre);
                printf("Nuevo tipo: ");
                scanf("%d", &tipo);
                while (getchar() != '\n');
                modificarDatosPersona(ministerio->personas, rut, nombre, tipo);
                break;

            case 9:
                printf("Ingrese RUT de la persona a eliminar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                eliminarPersonaPorRut(&ministerio->personas, rut);
                break;

            case 10:
                printf("Ingrese RUT a buscar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                listarPersonaPorRut(ministerio->personas, rut);
                break;

            case 11:
                printf("Ingrese RUT de la persona a asociar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                printf("Ingrese RUC de la causa destino: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        persona = buscarPersonaPorRut(ministerio->personas, rut);
                        if (persona != NULL) {
                            asociarPersona(actual->datosCausa, persona);
                        } else {
                            printf("No se encontro persona con ese RUT.\n");
                        }
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 12:
                actual = ministerio->causas;
                while (actual != NULL) {
                    printf("\n--- Contenido de la carpeta investigativa RUC: %s ---\n", actual->datosCausa->RUC);
                    revisarCarpetaInvestigativa(actual->datosCausa->carpetaInvestigativa);
                    actual = actual->sig;
                }
                break;

            case 13:
                printf("RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        agregarObjetoPorTipo(actual->datosCausa->carpetaInvestigativa);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 14:
                printf("RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("ID objeto: ");
                scanf("%d", &id);
                while (getchar() != '\n');
                printf("Nuevo detalle: ");
                leerCadena(detalle, sizeof(detalle));
                limpiarCadena(detalle);
                printf("Nuevo tipo: ");
                scanf("%d", &tipo);
                while (getchar() != '\n');
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        modificarDatosObjeto(actual->datosCausa->carpetaInvestigativa->objetos, id, detalle, tipo);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 15:
                printf("RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("ID objeto a eliminar: ");
                scanf("%d", &id);
                while (getchar() != '\n');
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        eliminarObjetoPorId(&actual->datosCausa->carpetaInvestigativa->objetos, id);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 16:
                printf("RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                printf("ID del objeto a buscar: ");
                scanf("%d", &id);
                while (getchar() != '\n');
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        listarObjetosPorID(actual->datosCausa->carpetaInvestigativa->objetos, id);
                        break;
                    }
                    actual = actual->sig;
                }
                break;

            case 17:
                actual = ministerio->causas;
                while (actual != NULL) {
                    listarResolucionesJudiciales(actual->datosCausa->carpetaInvestigativa);
                    actual = actual->sig;
                }
                break;

            case 18:
                actual = ministerio->causas;
                while (actual != NULL) {
                    listarSentenciasFinales(actual->datosCausa->carpetaInvestigativa);
                    actual = actual->sig;
                }
                break;

            case 19:
                generarReporteEstadoCausas(ministerio->causas);
                break;

            case 20:
                {
                    int total;
                    total = 0;
                    actualCausa = ministerio->causas;
                    while (actualCausa != NULL) {
                        total += contarSentenciasCondenatorias(actualCausa->datosCausa->carpetaInvestigativa);
                        actualCausa = actualCausa->sig;
                    }
                    printf("Total de sentencias condenatorias registradas: %d\n", total);
                }
                break;

            case 21:
                {
                    int cantidad;
                    cantidad = contarArchivosProvisionales(ministerio->causas);
                    printf("Total de causas archivadas provisionalmente: %d\n", cantidad);
                }
                break;

            case 22:
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                limpiarCadena(ruc);
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (strcmp(actual->datosCausa->RUC, ruc) == 0) {
                        int cantidad = contarMedidasProteccionActivas(actual->datosCausa->carpetaInvestigativa);
                        printf("Medidas de proteccion activas en la causa: %d\n", cantidad);
                        break;
                    }
                    actual = actual->sig;
                }
                if (actual == NULL) {
                    printf("No se encontro una causa con ese RUC.\n");
                }
                break;

            case 23:
                printf("Ingrese RUT del imputado a buscar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                resultado = buscarImputadoPorRut(ministerio->raizImputados, rut);
                if (resultado != NULL) {
                    printf("Imputado encontrado: %s | RUT: %s\n",
                           resultado->datosImputados->datosPersona->nombre,
                           resultado->datosImputados->datosPersona->rut);
                } else {
                    printf("No se encontro un imputado formalizado con ese RUT.\n");
                }
                break;

            case 24:
                printf("Ingrese RUT del imputado a eliminar: ");
                leerCadena(rut, sizeof(rut));
                limpiarCadena(rut);
                ministerio->raizImputados = eliminarImputadoPorRut(ministerio->raizImputados, rut);
                printf("Intento de eliminacion completado.\n");
                break;

            case 25:
                printf("===== IMPUTADOS FORMALIZADOS =====\n");
                listarImputadosInorden(ministerio->raizImputados);
                break;
            case 26:
                while (getchar() != '\n'); /* Limpia el buffer antes de leer */
                printf("Ingrese RUC de la causa para ver sus denuncias: ");
                leerCadena(ruc, sizeof(ruc));

                actualCausa = ministerio->causas;

                while (actualCausa != NULL) {
                    if (actualCausa->datosCausa != NULL &&
                        actualCausa->datosCausa->RUC != NULL &&
                        strcmp(actualCausa->datosCausa->RUC, ruc) == 0) {

                        if (actualCausa->datosCausa->carpetaInvestigativa != NULL &&
                            actualCausa->datosCausa->carpetaInvestigativa->denunciasRecientes != NULL) {

                            ordenarDenunciasPorFecha(actualCausa->datosCausa->carpetaInvestigativa);

                            printf("\n===== Denuncias ordenadas por fecha =====\n");
                            for (i = 0; i < actualCausa->datosCausa->carpetaInvestigativa->totalDenuncias; i++) {
                                if (actualCausa->datosCausa->carpetaInvestigativa->denunciasRecientes[i] != NULL) {
                                    printf("Fecha: %s | RUT: %s\n",
                                        actualCausa->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->fecha,
                                        actualCausa->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->rut);
                                    printf("Detalle: %s\n\n",
                                        actualCausa->datosCausa->carpetaInvestigativa->denunciasRecientes[i]->detalle);
                                }
                            }

                        } else {
                            printf("La causa no tiene carpeta investigativa o no hay denuncias.\n");
                        }

                        break;
                    }

                    actualCausa = actualCausa->sig;
                }

                if (actualCausa == NULL) {
                    printf("No se encontro una causa con ese RUC.\n");
                }
                break;
            case 27:
                while (getchar() != '\n');
                printf("Ingrese RUC de la causa: ");
                leerCadena(ruc, sizeof(ruc));
                actual = ministerio->causas;
                while (actual != NULL) {
                    if (actual->datosCausa != NULL &&
                        actual->datosCausa->RUC != NULL &&
                        strcmp(actual->datosCausa->RUC, ruc) == 0) {

                        if (actual->datosCausa->carpetaInvestigativa != NULL) {
                            printf("Ingrese fecha a buscar (AAAA-MM-DD): ");
                            leerCadena(fechaBuscada, sizeof(fechaBuscada));
                            buscarDenunciaPorFecha(actual->datosCausa->carpetaInvestigativa, fechaBuscada);
                        } else {
                            printf("La causa no tiene carpeta investigativa.\n");
                        }
                        break;
                        }
                    actual = actual->sig;
                }

                if (actual == NULL) {
                    printf("No se encontro una causa con ese RUC.\n");
                }
                break;
            case 28:
                break;

            default:
                printf("Opcion invalida.\n");
                break;
        }

    } while (opcion != 28);
}

void menuPrincipal(struct MinisterioPublico *ministerio) {
    char nombre[100];
    char rut[20];
    char clave[50];
    int tipo;
    struct Persona *usuario;

    printf("Ingrese su nombre: ");
    leerCadena(nombre, sizeof(nombre));

    printf("Ingrese su RUT (sin puntos y con guion): ");
    leerCadena(rut, sizeof(rut));

    printf("Ingrese su tipo (0=Administrador, 1=Victima, 2=Imputado, 5=Fiscal): ");
    scanf("%d", &tipo);
    while (getchar() != '\n');

    usuario = buscarPersonaPorRut(ministerio->personas, rut);

    if (usuario == NULL || strcmp(usuario->nombre, nombre) != 0 || usuario->tipo != tipo) {
        printf("Usuario no registrado o datos incorrectos.\n");
        return;
    }

    if (strcmp(rut, "99999999-9") == 0 && tipo == 0) {
        printf("Ingrese la contrasena de administrador: ");
        leerCadena(clave, sizeof(clave));

        if (strcmp(clave, "admin123") == 0) {
            menuAdministrador(ministerio);
        } else {
            printf("Contrasena incorrecta. Acceso denegado.\n");
        }
        return;
    }

    switch (usuario->tipo) {
        case 1:
            menuVictima(ministerio, usuario);
            break;

        case 2:
            menuImputado(ministerio, usuario);
            break;

        case 5:
            printf("Ingrese la contrasena del fiscal: ");
            leerCadena(clave, sizeof(clave));
            if (strcmp(clave, "fiscal123") == 0) {
                menuFiscal(ministerio, usuario);
            } else {
                printf("Contrasena incorrecta para el fiscal.\n");
            }
            break;

        default:
            printf("Tipo de usuario no valido.\n");
            break;
    }
}


int main() {
    struct MinisterioPublico *ministerio;
    struct Causa *c1;
    struct Causa *c2;
    struct Causa *c3;
    struct Causa *c4;
    struct NodoPersona *testigo1;
    struct NodoPersona *testigo2;
    struct NodoPersona *victimaC3;
    struct Persona *p1;
    struct Persona *p2;
    struct Persona *p3;
    struct Persona *p4;
    struct Persona *p5;
    struct Persona *p6;
    struct Persona *p7;
    struct Persona *p8;
    struct Persona *p9;
    struct Persona *p10;
    struct Persona *admin;


    ministerio = (struct MinisterioPublico *) malloc(sizeof(struct MinisterioPublico));

    if (ministerio == NULL) {
        printf("No se pudo asignar memoria para el sistema.\n");
        return 1;
    }

    ministerio->causas = NULL;
    ministerio->personas = NULL;
    ministerio->raizImputados = NULL;

    /* Personas */
    admin = crearPersona("admin", "99999999-9", 0);
    p1 = crearPersona("Juan Perez", "12345678-9", 1);  /* Victima*/
    p2 = crearPersona("Maria Lopez", "98765432-1", 2); /* Imputado*/
    p3 = crearPersona("Pedro Gonzalez", "11222333-4", 5); /*Fiscal*/
    p3->clave = copiarCadena("fiscal123");  /* Asigna clave al fiscal Pedro Gonzalez*/
    p4 = crearPersona("Ana Torres", "55667788-9", 6); /* Defensor*/
    p5 = crearPersona("Laura Rivas", "10101010-1", 1); /* Victima*/
    p6 = crearPersona("Cristiano Ronaldo", "20202020-2", 2); /* Imputado*/
    p7 = crearPersona("Valeria Ruiz", "30303030-3", 5); /* Fiscal*/
    p7->clave = copiarCadena("fiscal123"); /* Asigna clave al fiscal Valeria Ruiz*/
    p8 = crearPersona("Diego Herrera", "40404040-4", 6); /* Defensor*/
    p9 = crearPersona("Felipe Nunez", "50505050-5", 3); /* Testigo*/
    p10 = crearPersona("Alexis Sanchez", "51285850-5", 3); /* Testigo*/

    agregarPersona(&ministerio->personas, admin);
    agregarPersona(&ministerio->personas, p1);
    agregarPersona(&ministerio->personas, p2);
    agregarPersona(&ministerio->personas, p3);
    agregarPersona(&ministerio->personas, p4);
    agregarPersona(&ministerio->personas, p5);
    agregarPersona(&ministerio->personas, p6);
    agregarPersona(&ministerio->personas, p7);
    agregarPersona(&ministerio->personas, p8);
    agregarPersona(&ministerio->personas, p9);
    agregarPersona(&ministerio->personas, p10);

    /* Causas */
    c1 = crearCausa("2403-2025-1342", "robo", "abierta");
    c2 = crearCausa("0834-2024-0578", "estafa", "cerrada");
    c3 = crearCausa("3322-2023-9981", "lesiones", "abierta");
    c4 = crearCausa("5533-2022-7788", "homicidio frustrado", "archivo");

    agregarCausa(&ministerio->causas, c1);
    agregarCausa(&ministerio->causas, c2);
    agregarCausa(&ministerio->causas, c3);
    agregarCausa(&ministerio->causas, c4);

    crearCarpetaInvestigativa(c1);
    crearCarpetaInvestigativa(c2);
    crearCarpetaInvestigativa(c3);
    crearCarpetaInvestigativa(c4);

    /* Fiscales asignados */
    c1->fiscalEncargado = p3;
    c2->fiscalEncargado = p3;
    c3->fiscalEncargado = p7;
    c4->fiscalEncargado = p3;

    /* Asociar testigos y victima a c3 */
    testigo1 = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    testigo2 = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));
    victimaC3 = (struct NodoPersona *) malloc(sizeof(struct NodoPersona));

    testigo1->datosPersona = p9;
    testigo1->sig = testigo2;

    testigo2->datosPersona = p10;
    testigo2->sig = NULL;

    victimaC3->datosPersona = p5;
    victimaC3->sig = NULL;

    c3->testigos = testigo1;
    c3->victimas = victimaC3;

    /* Declaraciones de testigos */
    agregarObjetoInvestigativo(c3->carpetaInvestigativa, 4, "50505050-5", "El testigo vio al imputado en el lugar de los hechos.", "07-06-2025");
    agregarObjetoInvestigativo(c3->carpetaInvestigativa, 4, "51285850-5", "El testigo escucho gritos y llamo a carabineros.", "07-06-2025");

    /* Datos en c1 */
    registrarDenuncia(c1->carpetaInvestigativa, "12345678-9", "Denuncia por robo con violencia", "01-06-2025");
    registrarPeritaje(c1->carpetaInvestigativa, "98765432-1", "Huella encontrada", "02-06-2025");
    registrarDiligencia(c1->carpetaInvestigativa, "12345678-9", "Revision de camaras", "03-06-2025");
    aplicarMedidaProteccion(c1->carpetaInvestigativa, "12345678-9", "Proteccion otorgada", "04-06-2025");

    /* Datos en c3 */
    registrarDenuncia(c3->carpetaInvestigativa, "10101010-1", "Agresion callejera", "05-06-2025");
    registrarPeritaje(c3->carpetaInvestigativa, "20202020-2", "Lesiones comprobadas", "06-06-2025");
    aplicarMedidaProteccion(c3->carpetaInvestigativa, "10101010-1", "Amenazas previas", "07-06-2025");

    /* Formalizacion del imputado p2 */
    formalizarConDefensor(ministerio, c2, "Maria Lopez", "98765432-1", "Diego Herrera", "40404040-4",
                              "Lesiones Graves", "Testigos y pericias", "23-03-2025", 3);

    /* Formalizacion del imputado p6 */
    formalizarConDefensor(ministerio, c3, "Cristiano Ronaldo", "20202020-2", "Antonio Navarro", "22149473-3",
                            "Lesiones leves", "Testigos y pericias", "07-06-2025", 2);

    /* --- CAUSA c1: robo --- */
    registrarDenuncia(c1->carpetaInvestigativa, p1->rut, "Robo con violencia en la via publica", "01-06-2025");
    registrarPeritaje(c1->carpetaInvestigativa, p2->rut, "Video de camara de vigilancia", "02-06-2025");
    registrarOrdenDetencion(c1->carpetaInvestigativa, p2->rut, "03-06-2025", "Orden emitida por juez de garantia");
    registrarSentenciaFinal(c1->carpetaInvestigativa, p2->rut, "05-06-2025", 2);  /* Absolutoria */

    /* --- CAUSA c2: estafa --- */
    registrarSentenciaFinal(c2->carpetaInvestigativa, p2->rut, "10-05-2024", 1);  /* Condenatoria */

    /* --- CAUSA c3: lesiones --- */
    agregarObjetoInvestigativo(c3->carpetaInvestigativa, 3, p5->rut, "Solicitud de antecedentes clinicos", "06-06-2025");
    registrarOrdenDetencion(c3->carpetaInvestigativa, p6->rut, "07-06-2025", "Detencion por riesgo de fuga");
    registrarSentenciaFinal(c3->carpetaInvestigativa, p6->rut, "08-06-2025", 1);  /* Condenatoria */
    ejecutarSentenciaFinal(c3, "09-06-2025");
    archivarCausa(c3, "10-06-2025");

    /* Asociaciones de personas a causas */
    asociarPersona(c1, p1);  /* Victima*/
    asociarPersona(c1, p2);  /* Imputado*/
    asociarPersona(c1, p9);  /* Testigo*/
    asociarPersona(c1, p10); /* Testigo*/

    asociarPersona(c2, p1);  /* Victima*/
    asociarPersona(c2, p2);  /* Imputado*/

    asociarPersona(c3, p5);  /* Victima*/
    asociarPersona(c3, p6);  /* Imputado*/
    asociarPersona(c3, p9);  /* Testigo*/
    asociarPersona(c3, p10); /* Testigo*/

    asociarPersona(c4, p1);  /* Victima*/
    asociarPersona(c4, p2);  /* Imputado*/

     /*ordena las denuncias de las causas por fecha*/
    ordenarDenunciasPorFecha(c1->carpetaInvestigativa);
    ordenarDenunciasPorFecha(c2->carpetaInvestigativa);
    ordenarDenunciasPorFecha(c3->carpetaInvestigativa);
    ordenarDenunciasPorFecha(c4->carpetaInvestigativa);
    /* Iniciar el menu */
    menuPrincipal(ministerio);

    return 0;
}