#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main(){
	 char comando[LONGITUD_COMANDO];
	 char orden[LONGITUD_COMANDO];
	 char argumento1[LONGITUD_COMANDO];
	 char argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
   EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     
     //...
     
     fent = fopen("particion.bin","r+b");

     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
    

     // Bucle de tratamiento de comandos
     for (;;){
		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) != 0);
	     if (strcmp(orden,"dir\n") == 0) {
            Directorio(&directorio,&ext_blq_inodos);
            continue;
            }
            
        if(strcmp(orden, "info\n") == 0) {
          LeeSuperBloque(&ext_superblock); 
          continue;
        }

        if(strcmp(orden, "bytemaps\n") == 0) {
          Printbytemaps(&ext_bytemaps);
        } 

        if(strcmp(orden, "rename") == 0) {
          Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
          continue;
        }

        if(strcmp(orden, "imprimir") == 0) {
          Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
        }

        if(strcmp(orden, "remove") == 0) {
          Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
        }

        if(strcmp(orden, "copy") == 0) {
          Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
        }

         /*//...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         */
         if (strcmp(orden,"salir\n")==0){
            //GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
 
    char *a = strtok(strcomando, " ");
    strcpy(orden, a);

    char *b = strtok(NULL, " ");

    if(b != NULL) {
      strcpy(argumento1, b);
    }
    char *c = strtok(NULL, " ");
        
    if(c != NULL) {
      strcpy(argumento2, c);
    }

    return 0;
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre) {
  for(int i = 1; i < MAX_FICHEROS; i++) {
    if(strcmp(directorio[i].dir_nfich, nombre) == 0 && directorio[i].dir_inodo != NULL_INODO) {
      return directorio[i].dir_inodo;
    }
  }

  return -1;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup) {

    printf("Bloque %d bytes\n", psup->s_block_size);
    printf("Inodos particion = %d\n", psup->s_inodes_count);
    printf("Inodos libres = %d\n", psup->s_free_inodes_count);
    printf("Bloques particion = %d\n", psup->s_blocks_count);
    printf("Bloques libres = %d\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos = %d\n", psup->s_first_data_block);

}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
   
    printf("Inodos: ");
 
    for(int i = 0; i < MAX_INODOS; i++) {
      printf("%d ", ext_bytemaps->bmap_inodos[i]);
    }
 
    printf("\nBloques [0-25]: ");
   
    for(int i = 0; i < 25; i++) {
      printf("%d ", ext_bytemaps->bmap_bloques[i]);
    }
 
    printf("\n");
 
  }

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {

    int inodo_fichero;
    int n_bloque;

    for(int i = 1; i < MAX_FICHEROS; i++) {

      inodo_fichero = directorio[i].dir_inodo;
      if(inodo_fichero != NULL_INODO){
        printf("Nombre: %s\tTamano: %d\tInodo: %d\tBloque: ", directorio[i].dir_nfich, inodos->blq_inodos[inodo_fichero].size_fichero, inodo_fichero);

        for(int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
          
          n_bloque = inodos->blq_inodos[inodo_fichero].i_nbloque[j];
          
          if(n_bloque != NULL_BLOQUE){
          printf("%d ", inodos->blq_inodos[inodo_fichero].i_nbloque[j]);
        }
      }
        printf("\n");

      }  
    }
}


int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
  
  nombrenuevo[strcspn(nombrenuevo, "\n")] = 0;
  nombreantiguo[strcspn(nombreantiguo, "\n")] = 0;
  
  int inodo_fichero = BuscaFich(directorio, inodos, nombrenuevo);

  if(inodo_fichero != -1) {
    printf("ERROR: El nombre de fichero ya existe\n");
    return 1;
  }

  inodo_fichero = BuscaFich(directorio, inodos, nombreantiguo);

  if(inodo_fichero == -1) {
    printf("ERROR: El fichero de origen no se ha encontrado\n");
    return 1;
  }

  // Encontramos el fichero a través del inodo

  for(int i = 0; i < MAX_FICHEROS; i++) {
    if(directorio[i].dir_inodo == inodo_fichero) {
      strcpy(directorio[i].dir_nfich, nombrenuevo);
    }
  }

}


int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {

  nombre[strcspn(nombre, "\n")] = 0;

  int inodo_fichero = BuscaFich(directorio, inodos, nombre);
  int bloque;

  char datos[SIZE_BLOQUE * MAX_NUMS_BLOQUE_INODO];

  if(inodo_fichero == -1) {
    printf("ERROR: Fichero no encontrado\n");
    return 1;
  }

  for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {

    bloque = inodos->blq_inodos[inodo_fichero].i_nbloque[i];

    // Recorremos cada bloque caracter a caracter o bien hasta que se termine o hasta que encuentre el null byte

    if(bloque != NULL_BLOQUE) {
      for(int j = 0; j < SIZE_BLOQUE; j++) {

        datos[i * SIZE_BLOQUE + j] = memdatos[bloque - PRIM_BLOQUE_DATOS].dato[j];
        if(memdatos[bloque - PRIM_BLOQUE_DATOS].dato[j] == '\0') {
          break;
        }
      }
    }
  }

  printf("%s\n", datos);

  return 0;
}


int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich) {

  nombre[strcspn(nombre, "\n")] = 0;

  int inodo = BuscaFich(directorio, inodos, nombre);
  int bloque;

  if(inodo == -1) {
    printf("ERROR: Fichero no encontrado\n");
    return 1;
  }

  // Marcamos los bloques como libres en el bytemap de bloques y el inodo en el bytemap de inodos


  ext_bytemaps->bmap_inodos[inodo] = 0;
  ext_superblock->s_free_inodes_count++;

  for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {

    bloque = inodos->blq_inodos[inodo].i_nbloque[i];

    if(bloque != NULL_BLOQUE) {
      ext_bytemaps->bmap_bloques[bloque] = 0;
      ext_superblock->s_free_blocks_count++;
    }
  }

  // Liberamos el inodo

  inodos->blq_inodos[inodo].size_fichero = 0;

  for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
    inodos->blq_inodos[inodo].i_nbloque[i] = NULL_BLOQUE;
  }

  // Liberamos el archivo

  for(int i = 0; i < MAX_FICHEROS; i++) {
    
    if(directorio[i].dir_inodo == inodo) {
      strcpy(directorio[i].dir_nfich, "");
      directorio[i].dir_inodo = NULL_INODO;
    }
  }

}


int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich) {

  
  nombredestino[strcspn(nombredestino, "\n")] = 0;

  int inodo = BuscaFich(directorio, inodos, nombreorigen);
  int inodo_libre = -1;

  if(inodo == -1) {
    printf("ERROR: Fichero no encontrado\n");
  }

  // Encontramos el proximo inodo libre (el 0 y 1 estan reservados y el 2 es el directorio)

  for(int i = 3; i < MAX_INODOS; i++) {

    if(ext_bytemaps->bmap_inodos[i] == 0) {
      inodo_libre = i;

      ext_bytemaps->bmap_inodos[i] = 1;
      ext_superblock->s_free_inodes_count--;
      break;
    }
  }

  if(inodo_libre == -1) {
    printf("ERROR: No quedan inodos libres\n");
    return 1;
  }


// Encontramos la cantidad de bloques que nos sean necesarios

  int bloques_necesarios = 0;

  for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
    if(inodos->blq_inodos[inodo].i_nbloque[i] != NULL_BLOQUE) {
      bloques_necesarios++;
    }
  }


  // Vemos que tengamos n bloques vacios

  int n = 0;
  int bloques_nuevos[MAX_NUMS_BLOQUE_INODO];

  for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
    bloques_nuevos[i] = NULL_BLOQUE;
  }

  int hay_bloques_vacios = 0;

  for(int i = PRIM_BLOQUE_DATOS; i < MAX_BLOQUES_PARTICION; i++) {
    
    if(ext_bytemaps->bmap_bloques[i] == 0) {
      bloques_nuevos[n] = i;
      ext_bytemaps->bmap_bloques[i] = 1;
      n++;
    }

    if(n == bloques_necesarios) {
      hay_bloques_vacios = 1;
      break;
    }
  }

  if(!hay_bloques_vacios) {
    printf("ERROR: No quedan suficientes bloques vacios\n");
  }


// Copiamos los bloques en los primeros bloques vacios que tengamos


  int bloque_a_copiar;
  int bloque_destino;

for(int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {

  bloque_a_copiar = inodos->blq_inodos[inodo].i_nbloque[i];
  bloque_destino = bloques_nuevos[i];


  if(bloque_a_copiar != NULL_BLOQUE && bloque_destino != NULL_BLOQUE) {
    memcpy(&memdatos[bloque_destino - PRIM_BLOQUE_DATOS].dato, &memdatos[bloque_a_copiar - PRIM_BLOQUE_DATOS].dato, SIZE_BLOQUE);
    ext_superblock->s_free_blocks_count--;
  }
  
}


// Creamos la entrada del directorio

  for(int i = 1; i < MAX_FICHEROS; i++) {
    if(directorio[i].dir_inodo == NULL_INODO) {
      strcpy(directorio[i].dir_nfich, nombredestino);
      directorio[i].dir_inodo = inodo_libre;

      inodos->blq_inodos[inodo_libre].size_fichero = inodos->blq_inodos[inodo].size_fichero;

      for(int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
        inodos->blq_inodos[inodo_libre].i_nbloque[j] = bloques_nuevos[j];
      }
      break;
    }
  }


}
