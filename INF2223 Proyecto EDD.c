#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct MinisterioPublico {
  struct NodoCausas *causas;

};

struct NodoCausas { //Lista simple doblemente enlazada
  struct Causa *datosCausa;
  struct NodoCausas *sig, *ant;
};

struct Causa {
  struct Carpetas *carpetaInvestigativa;
  char *CategoriaCausa; //De que tipo fue la denuncia
  char *RUC;
  char *Comuna;
  struct Fiscal *fiscalEncargado;
};

struct Fiscal {
  int idFiscal;
  char nombre;
  char rut;
  int totalCausas;
};

struct CarpetaInvestigativa {
  char *estadoCaso;
  struct Denuncia Denuncia[CantDenuncias]; //Arreglo
  int CantDenuncias;
  struct NodoPrueba *pruebas;
  struct Diligencia *diligencia;
  struct Resolucion *resolucion;
  struct declaracion *declaracion;
};

struct Denuncia {
  char *PersonaDenunciante;
  char *RutDenunciante;
  char *FechaDenuncia;
};

struct NodoPrueba { //Lista circular
  struct prueba *prueba;
  struct NodoPrueba *sig;
};

struct prueba {
  char *TipoPrueba; //Si es una grabacion, foto, etc...
};

struct NodoTestigos {
  struct Testigo *datosTestigos;
  struct NodoTestigos *sig;
};

